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
    new.average_capacity = -1;

    new.choose_teams = g_array_new(FALSE, FALSE, sizeof(CupChooseTeam));
    new.choose_team_user = cup_choose_team_new();
    new.rounds = g_array_new(FALSE, FALSE, sizeof(CupRound));
    new.teams = g_array_new(FALSE, FALSE, sizeof(Team));

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
	    if(!is_in_international_cups(&g_array_index(teams, Team, permutation[j])))
	    {
		team_append_to_array_with_ids(&g_array_index(teams, Team, permutation[j]), cup->teams,
					      cup->id, cup->teams->len);
		team_generate_players(&g_array_index(cup->teams, Team, cup->teams->len - 1));
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
	    if(!is_in_international_cups(&g_array_index(lig(number - 1).teams, Team, i)))
	    {
		team_append_to_array(&g_array_index(lig(number - 1).teams, Team, i),
				     cup->teams);
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
	    
	    for(i = choose_team->start_idx - 1; i < choose_team->end_idx - 1; i++)
	    {
		if(!is_in_international_cups(&g_array_index(lig(number - 1).teams, Team, permutation[i])))
		{
		    team_append_to_array(&g_array_index(lig(number - 1).teams,
							Team, permutation[i - choose_team->start_idx + 1]),
					 cup->teams);
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
    
    main_exit_program(EXIT_CHOOSE_TEAM_ERROR);
}
