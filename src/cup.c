#include "cup.h"
#include "fixture.h"
#include "free.h"
#include "main.h"
#include "maths.h"
#include "misc.h"
#include "table.h"
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

    new.id = cup_new_id(FALSE);
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
    new.bye = NULL;

    new.next_fixture_update_week = -1;
    new.next_fixture_update_week_round = -1;
    
    return new;
}

/** Return a new numerical id for a cup.
    We browse through the existing cups and take the
    first free id.
    @param prom_cup Whether we need a promotion games cup id.
*/
gint
cup_new_id(gboolean prom_cup)
{
    gint i, j;
    gint start = (prom_cup) ? ID_PROM_CUP_START : ID_CUP_START;

    if(cps->len == 0)
	return start;

    for(i=start;i<start+1000;i++)
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

    free_teams_array(&cup->teams, TRUE);

    for(i=0;i<cup->choose_teams->len;i++)
    {
	choose_team = &g_array_index(cup->choose_teams, CupChooseTeam, i);
	leagues = g_array_new(FALSE, FALSE, sizeof(League));
	teams = g_array_new(FALSE, FALSE, sizeof(Team));
	sids = misc_separate_strings(choose_team->sid->str);

	for(j=0;j<sids->len;j++)
	{
	    xml_league_read(((GString*)g_ptr_array_index(sids, j))->str, leagues);

	    for(k=0; k < g_array_index(leagues, League, leagues->len - 1).teams->len; k++)
		g_array_append_val(teams, g_array_index(
				       g_array_index(leagues, League, leagues->len - 1).teams, Team,  k));

	    g_array_free(g_array_index(leagues, League, leagues->len - 1).teams, TRUE);
	}

	g_array_free(leagues, TRUE);

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
		g_array_append_val(cup->teams, g_array_index(teams, Team, permutation[j]));
		g_array_index(cup->teams, Team, cup->teams->len - 1).clid = cup->id;

		number_of_teams++;
	    }

	    if(number_of_teams == choose_team->number_of_teams)
		break;
	}

	if(number_of_teams != choose_team->number_of_teams)
	    cup_choose_team_abort(cup, choose_team, FALSE);

	for(j=teams->len - 1; j>=0;j--)
	    if(query_is_in_international_cups(&g_array_index(teams, Team, j)))
		g_array_remove_index(teams, j);
	
	free_g_string_array(&sids);
	free_teams_array(&teams, FALSE);
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
    GPtrArray *user_teams_sorted = NULL;
    Team *table_team = NULL;

    g_ptr_array_free(cup->user_teams, TRUE);
    cup->user_teams = g_ptr_array_new();

    sscanf(choose_team->sid->str, "%[^0-9]%d", type, &number);

    if(strcmp(type, "league") != 0 &&
       strcmp(type, "cup") != 0)
	cup_choose_team_abort(cup, choose_team, TRUE);
	
    /** Teams from a league. */
    if(strcmp(type, "league") == 0)
    {
	if(ligs->len < number ||
	   lig(number - 1).teams->len < choose_team->end_idx)
	    cup_choose_team_abort(cup, choose_team, TRUE);

	for(i = choose_team->start_idx - 1; i <= choose_team->end_idx - 1; i++)
	{
	    table_team = team_of_id(
		g_array_index(lig(number - 1).table.elements, TableElement, i).team_id);

	    if(!query_is_in_international_cups(table_team))
	    {
		g_ptr_array_add(cup->user_teams, table_team);
		number_of_teams++;
	    }
	    
	    if(number_of_teams == choose_team->number_of_teams)
		break;
	}

	if(number_of_teams != choose_team->number_of_teams)
	    cup_choose_team_abort(cup, choose_team, TRUE);

	return;
    }

    /** Teams from a cup; special case: in the first season,
	we load random teams from the first league. */
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

    /** Teams from a cup: normal case. We sort the teams that
	participated in the cup. */
    user_teams_sorted = cup_get_teams_sorted(&cp(number - 1));

    for(i = choose_team->start_idx - 1; i <= choose_team->end_idx - 1; i++)
    {
	if(!query_is_in_international_cups((Team*)g_ptr_array_index(user_teams_sorted, i)))
	{
	    g_ptr_array_add(cup->user_teams, g_ptr_array_index(user_teams_sorted, i));
	    number_of_teams++;
	}
		
	if(number_of_teams == choose_team->number_of_teams)
	    break;
    }
	    
    if(number_of_teams != choose_team->number_of_teams)
	cup_choose_team_abort(cup, choose_team, TRUE);    

    g_ptr_array_free(user_teams_sorted, TRUE);
}

/** Return a pointer array of teams ordered corresponding to
    their success in the cup. A bit tricky because we have to
    fetch the team pointers corresponding to their name because
    the team pointers in the fixtures are partially invalid because
    of promotion relegation. */
GPtrArray*
cup_get_teams_sorted(const Cup *cup)
{
    gint i, j;
    GPtrArray *teams = g_ptr_array_new();
    GArray *team_ids = g_array_new(FALSE, FALSE, sizeof(gint));
    
    for(i=0;i<cup->fixtures->len;i++)
	for(j=0;j<2;j++)
	    if(!query_misc_integer_is_in_g_array(
		   g_array_index(cup->fixtures, Fixture, i).team_ids[j], team_ids))
	    {
		g_array_append_val(team_ids, g_array_index(cup->fixtures, Fixture, i).team_ids[j]);
		g_ptr_array_add(teams, team_of_id(g_array_index(cup->fixtures, Fixture, i).team_ids[j]));
	    }

    g_ptr_array_sort_with_data(teams, cup_compare_success, (gpointer)cup->fixtures);

    g_array_free(team_ids, TRUE);

    return teams;
}

/** Compare two teams in a pointer array taking into account
    their success in a cup. */
gint
cup_compare_success(gconstpointer a, gconstpointer b, gpointer data)
{
    const GArray *fixtures = (GArray*)data;
    const Team *team1 = *(const Team**)a;
    const Team *team2 = *(const Team**)b;
    const Fixture *last_fix = &g_array_index(fixtures, Fixture, fixtures->len - 1);
    gint round_reached1 = cup_get_round_reached(team1, fixtures),
	round_reached2 = cup_get_round_reached(team2, fixtures);
    gint return_value = 0;

    if(round_reached1 < round_reached2)
	return_value = 1;
    else if(round_reached1 > round_reached2)
	return_value = -1;
    else
    {
	if(round_reached1 != last_fix->round)
	    return_value = 0;
	else
	{
	    if(GPOINTER_TO_INT(fixture_winner_of(last_fix, TRUE)) ==  team1->id)
		return_value = -1;
	    else
		return_value = 1;
	}
    }

    return return_value;
}

/** Return the cup round that the team reached in the cup.
    @param fixtures The fixtures array of the cup. */
gint
cup_get_round_reached(const Team *tm, const GArray *fixtures)
{
    gint round = -1;
    gint i;

    for(i=0;i<fixtures->len;i++)
	if(g_array_index(fixtures, Fixture, i).team_ids[0] == tm->id ||
	   g_array_index(fixtures, Fixture, i).team_ids[1] == tm->id)
	    round = MAX(round, g_array_index(fixtures, Fixture, i).round);

    return round;
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
