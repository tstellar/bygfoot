#include "cup.h"
#include "free.h"
#include "main.h"
#include "maths.h"
#include "misc.h"
#include "team.h"
#include "variables.h"
#include "xml_league.h"

/**
   Create and return a new cup with default values.
   @return The new cup.
   @see #Cup
*/
Cup
cup_new(void)
{
    Cup new;

    new.name = g_string_new("");
    new.short_name = g_string_new("");
    new.symbol = g_string_new("");
    new.sid = g_string_new("");

    new.id = cup_new_id();
    new.type = CUP_TYPE_NATIONAL;
    new.last_week = new.week_gap = -1;
    new.yellow_red = 1000;
    new.skill_diff = 0;

    new.choose_teams = g_array_new(FALSE, FALSE, sizeof(CupChooseTeam));
    new.choose_team_user = cup_choose_team_new();
    new.rounds = g_array_new(FALSE, FALSE, sizeof(CupRound));
    new.teams = g_array_new(FALSE, FALSE, sizeof(Team));
    new.user_teams = g_ptr_array_new();
    new.tables = g_array_new(FALSE, FALSE, sizeof(Table));
    new.fixtures = g_array_new(FALSE, FALSE, sizeof(Fixture));
    new.bye = g_ptr_array_new();

    new.next_fixture_update_week = -1;
    new.next_fixture_update_week_round = -1;
    
    return new;
}

/** Return a new numerical id for a cup.
    We browse through the existing cups and take the
    first free id.
*/
gint
cup_new_id(void)
{
    gint i, j;

    if(cps == NULL)
	return ID_CUP_START;

    for(i=ID_CUP_START;i<ID_CUP_START+1000;i++)
    {
	for(j=0;j<cps->len;j++)
	    if(cp(j).id == i)
		break;

	if(j == cps->len)
	    return i;
    }

    g_warning("cup_new_id: didn't find a free numerical id.");
    return -1;
}

/** Return a CupChooseTeam with default values. */
CupChooseTeam
cup_choose_team_new(void)
{
    CupChooseTeam new;

    new.sid = g_string_new("");
    new.number_of_teams = -1;
    new.start_idx = new.end_idx = -1;
    new.randomly = FALSE;

    return new;
}

/** Return a CupRound with default values. */
CupRound
cup_round_new(void)
{
    CupRound new;

    new.home_away = TRUE;
    new.replay = 0;
    new.neutral = FALSE;
    new.round_robin_number_of_groups = 0;
    new.round_robin_number_of_advance = -1;
    new.round_robin_number_of_best_advance = 0;

    return new;
}

/** Fill the teams array of a cup with teams according to the choose_teams of
    the cup. Only called for international cups.
    @param cup The pointer to the cup. */
void
cup_load_choose_teams(Cup *cup)
{
    gint i, j, k, number_of_teams, end_idx = -1;
    CupChooseTeam *choose_team = NULL;
    GArray *teams = NULL;
    GArray *leagues = NULL;
    GPtrArray *sids = NULL;

    for(i=0;i<cup->choose_teams->len;i++)
    {
	choose_team = &g_array_index(cup->choose_teams, CupChooseTeam, i);
	leagues = g_array_new(FALSE, FALSE, sizeof(League));
	teams = g_array_new(FALSE, FALSE, sizeof(Team));
	sids = misc_separate_strings(choose_team->sid->str);

	for(j=0;j<sids->len;j++)
	{
	    xml_league_read(((GString*)g_ptr_array_index(sids, j))->str, leagues);
	    for(k=0;k<g_array_index(leagues, League, leagues->len - 1).teams->len;k++)
		team_append_to_array(
		    &g_array_index(g_array_index(leagues, League, leagues->len - 1).teams, Team,  k),
		    teams);
	}

	gint permutation[teams->len];
	for(j=0;j<teams->len;j++)
	    permutation[j] = j;

	if(choose_team->randomly)
	{
	    if(choose_team->start_idx == -1)
		math_generate_permutation(permutation, 0, teams->len - 1);
	    else
		math_generate_permutation(permutation,
					  choose_team->start_idx - 1,
					  choose_team->end_idx - 1);
	}

	number_of_teams = 0;
	if(choose_team->start_idx == -1)
	    end_idx = teams->len;
	else
	    end_idx = choose_team->end_idx -
		choose_team->start_idx + 1;

	if(teams->len < end_idx)
	    cup_choose_team_abort(cup, choose_team, FALSE);

	for(j = 0; j < end_idx; j++)
	{
	    if(!query_is_in_international_cups(&g_array_index(teams, Team, permutation[j])))
	    {
		team_append_to_array_with_ids(&g_array_index(teams, Team, permutation[j]), cup->teams,
					      cup->id, cup->teams->len);
		number_of_teams++;
	    }

	    if(number_of_teams == choose_team->number_of_teams)
		break;
	}

	if(number_of_teams != choose_team->number_of_teams)
	    cup_choose_team_abort(cup, choose_team, FALSE);
	
	free_g_string_array(&sids);
	free_teams_array(&teams);
	free_leagues_array(&leagues);
    }

    for(i=0;i<cup->teams->len;i++)
	team_generate_players_stadium(&g_array_index(cup->teams, Team, i));

    /*d*/
/*     printf("\n%s\n", cup->name->str); */
/*     for(i=0;i<cup->teams->len;i++) */
/* 	printf("%d %s\n", i, g_array_index(cup->teams, Team, i).name->str); */
}

/** Add the teams specified by the choose_team_user rule to the teams
    array of the cup.
    @param cup The pointer to the cup. */
void
cup_load_choose_team_user(Cup *cup)
{
    gint i;
    gchar type[SMALL];
    gint number, number_of_teams = 0;
    CupChooseTeam *choose_team = &cup->choose_team_user;

    sscanf(choose_team->sid->str, "%[^0-9]%d", type, &number);

    if(strcmp(type, "league") == 0)
    {
	if(ligs->len < number ||
	   lig(number - 1).teams->len < choose_team->end_idx)
	    cup_choose_team_abort(cup, choose_team, TRUE);
	
	for(i = choose_team->start_idx - 1; i <= choose_team->end_idx - 1; i++)
	{
	    if(!query_is_in_international_cups(&g_array_index(lig(number - 1).teams, Team, i)))
	    {
		g_ptr_array_add(cup->user_teams, (gpointer)&g_array_index(lig(number - 1).teams, Team, i));
		number_of_teams++;
	    }
	    
	    if(number_of_teams == choose_team->number_of_teams)
		break;
	}

	if(number_of_teams != choose_team->number_of_teams)
	    cup_choose_team_abort(cup, choose_team, TRUE);

	return;
    }

    if(strcmp(type, "cup") == 0)
    {
	if(season == 1)
	{
	    if(lig(0).teams->len < choose_team->number_of_teams)
		cup_choose_team_abort(cup, choose_team, TRUE);

	    gint permutation[lig(0).teams->len];
	    math_generate_permutation(permutation, 0, lig(0).teams->len - 1);

	    for(i = choose_team->start_idx - 1; i <= choose_team->end_idx - 1; i++)
	    {
		if(!query_is_in_international_cups(
		       &g_array_index(lig(number - 1).teams,
				      Team, permutation[i - choose_team->start_idx + 1])))
		{
		    g_ptr_array_add(cup->user_teams,
				    (gpointer)&g_array_index(lig(number - 1).teams,
							     Team, permutation[i - choose_team->start_idx + 1]));
		    number_of_teams++;
		}
		
		if(number_of_teams == choose_team->number_of_teams)
		    break;
	    }
	    
	    if(number_of_teams != choose_team->number_of_teams)
		cup_choose_team_abort(cup, choose_team, TRUE);

	    return;
	}

	/** todo: load teams from a cup */
    }
}

/** Print an error and exit the program if there's a problem loading
    the CupChooseTeam.
    @param *cup The cup the choose_team is from.
    @param choose_team The choose_team that caused the error.
    @param user Whether the function is called from cup_load_choose_team_user()
    or cup_load_choose_teams(). */
void
cup_choose_team_abort(const Cup *cup, const CupChooseTeam *choose_team, gboolean user)
{
    if(user)
	g_warning("cup_load_choose_team_user: there was an error loading choose_team %s from cup %s. maybe there aren't enough teams in the specified league or cup.\n", choose_team->sid->str, cup->sid->str);
    else
	g_warning("cup_load_choose_teams: there was an error loading choose_team %s from cup %s. maybe there aren't enough teams in the specified league or cup.\n", choose_team->sid->str, cup->sid->str);
    
    main_exit_program(EXIT_CHOOSE_TEAM_ERROR, NULL);
}

/** Calculate the week number the first matchday of the given
    cup round takes place.
    @param cup The cup we examine.
    @param cup_round The index of the cup round in the cup.rounds array.
    @return A week number. */
gint
cup_get_first_week_of_cup_round(const Cup *cup, gint cup_round)
{
    gint week_number;

    if(cup_round == cup->rounds->len - 1)
	week_number = cup->last_week - 
	    (cup_get_matchdays_in_cup_round(cup, cup_round) - 1) * cup->week_gap;
    else
	week_number = cup_get_first_week_of_cup_round(cup, cup_round + 1) -
	    cup_get_matchdays_in_cup_round(cup, cup_round) * cup->week_gap;

    return week_number;
}

/** Return the number of matchdays for a given cup round.
    @param cup The cup we examine.
    @param cup_round The index of the cup round.
    @return The number of matchdays, mostly 1 or 2. */
gint
cup_get_matchdays_in_cup_round(const Cup *cup, gint cup_round)
{
    gint number_of_teams = -1;
    gint number_of_matchdays = -1;
    
    if(g_array_index(cup->rounds, CupRound, cup_round).
       round_robin_number_of_groups > 0)
    {
	number_of_teams = cup_round_get_number_of_teams(cup, cup_round);
	number_of_matchdays = 
	    2 * (number_of_teams / g_array_index(cup->rounds, CupRound, cup_round).
		 round_robin_number_of_groups);
    }
    else if(g_array_index(cup->rounds, CupRound, cup_round).home_away)
	number_of_matchdays = 2;
    else
	number_of_matchdays = 1;

    return number_of_matchdays;
}

/** Return the number of teams playing in the given cup round.
    @param cup The cup we examine.
    @param cup_round The index of the cup round.
    @return The number teams. */
gint
cup_round_get_number_of_teams(const Cup *cup, gint cup_round)
{
    gint number_of_teams = -1;

    if(cup_round == 0)
	number_of_teams = cup->teams->len;
    else if(g_array_index(cup->rounds, CupRound, cup_round - 1).round_robin_number_of_groups > 0)
    {
	number_of_teams = 
	    g_array_index(cup->rounds, CupRound, cup_round - 1).round_robin_number_of_groups *
	    g_array_index(cup->rounds, CupRound, cup_round - 1).round_robin_number_of_advance +
	    g_array_index(cup->rounds, CupRound, cup_round - 1).round_robin_number_of_best_advance;
    }
    else
	number_of_teams = cup_round_get_number_of_teams(cup, cup_round - 1) / 2;

    return number_of_teams;
}

/** Return the teams participating in the cup as a pointer array.
    @param cup The cup pointer.
    @return An array of team pointers. */
GPtrArray*
cup_get_team_pointers(const Cup *cup)
{
    gint i;
    GPtrArray *teams = team_get_pointers_from_array(cup->teams);

    for(i=0;i<cup->user_teams->len;i++)
	g_ptr_array_add(teams, g_ptr_array_index(cup->user_teams, i));

    return teams;
}

/** Return the cup pointer belonging to the id.
    @param clid The id we look for.
    @return The cup pointer or NULL if failed. */
Cup*
cup_from_clid(gint clid)
{
    gint i;

    for(i=0;i<cps->len;i++)
	if(cp(i).id == clid)
	    return &cp(i);

    g_warning("cup_from_clid: didn't find cup with id %d\n", clid);
    return NULL;
}

/** Return the name of a cup round, e.g. 'round robin'
    or 'final' or so.
    @param fix A fixture belonging to the round.
    @param buf The buffer we write the name into*/
void
cup_round_name(const Fixture *fix, gchar *buf)
{
    const Cup *cup = cup_from_clid(fix->clid);
    const CupRound *cup_round = 
	&g_array_index(cup->rounds, CupRound, fix->round);

    if(cup_round->round_robin_number_of_groups > 0)
	strcpy(buf, "Round robin");
    else
    {
	if(fix->round == cup->rounds->len - 1)
	    strcpy(buf, _("Final"));
	else if(fix->round == cup->rounds->len - 2)
	    strcpy(buf, _("Semi-final"));
	else if(fix->round == cup->rounds->len - 3)
	    strcpy(buf, _("Quarter-final"));
	else
	    sprintf(buf, _("Last %d"), (gint)rint(powf(2, cup->rounds->len - fix->round)));
	    
	if(cup_round->home_away)
	{
	    if(fix->second_leg)
		strcat(buf, " -- Second leg");
	    else
		strcat(buf, " -- First leg");
	}
	else if(fix->replay_number > 0)
	    strcat(buf, " -- Replay match");
    }	
}
