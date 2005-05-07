#include "cup.h"
#include "league.h"
#include "maths.h"
#include "misc.h"
#include "option.h"
#include "player.h"
#include "stat.h"
#include "table.h"
#include "team.h"
#include "user.h"
#include "variables.h"

/**
   Create a new league with some default values.
   @return The league we created.
   @see League
*/
League
league_new(gboolean new_id)
{
    League new;

    new.name = g_string_new("");
    new.sid = g_string_new("");
    new.short_name = g_string_new("");
    new.symbol = g_string_new("");
    
    new.id = (new_id) ? league_id_new : -1;

    new.average_skill = 7000;

    new.prom_rel.prom_games_dest_sid = g_string_new("");
    new.prom_rel.prom_games_loser_sid = g_string_new("");
    new.prom_rel.prom_games_cup_sid = g_string_new("");
    new.prom_rel.prom_games_number_of_advance = 1;

    new.prom_rel.elements = g_array_new(FALSE, FALSE, sizeof(PromRelElement));

    new.teams = g_array_new(FALSE, FALSE, sizeof(Team));
    
    new.fixtures = g_array_new(FALSE, FALSE, sizeof(Fixture));

    new.table = table_new();
    new.table.clid = new.id;

    new.first_week = new.week_gap = 1;
    new.round_robins = 1;
    new.yellow_red = 1000;

    new.stats = stat_league_new(new.id);

    return new;
}

/**
   Create a new PromRelElement with default values.
   @return The PromRelElement we created.
   @see PromRelElement
*/
PromRelElement
prom_rel_element_new(void)
{
    PromRelElement new;

    new.ranks[0] = new.ranks[1] = 0;
    new.dest_sid = g_string_new("");
    new.type = PROM_REL_NONE;

    return new;
}

/** Get the array index of the given league or cup id.
    @param clid The id of the league or cup.
    @return The index in the leagues or cups array. */
gint
league_cup_get_index_from_clid(gint clid)
{
    gint i;
    gint index = -1;

    if(clid < ID_CUP_START)
    {
	for(i=0;i<ligs->len;i++)
	    if(lig(i).id == clid)
	    {
		index = i;
		break;
	    }
    }
    else
	for(i=0;i<cps->len;i++)
	    if(cp(i).id == clid)
	    {
		index = i;
		break;
	    }

    if(index == -1)
	g_warning("league_cup_get_index_from_clid: couldn't find league or cup with id %d\n", clid);

    return index;
}

/** Return the league pointer belonging to the id.
    @param clid The id we look for.
    @return The league pointer or NULL if failed. */
League*
league_from_clid(gint clid)
{
    gint i;

    for(i=0;i<ligs->len;i++)
	if(lig(i).id == clid)
	    return &lig(i);

    g_warning("league_from_clid: didn't find league with id %d\n", clid);
    return NULL;
}

/** Return the id of the next league (or cup if we are
    at the end of the leagues array).
    @param clid The id of the current league or cup. */
gint
league_cup_get_next_clid(gint clid)
{
    gint i, return_value = -1;

    if(clid < ID_CUP_START)
    {
	for(i=0;i<ligs->len;i++)
	    if(lig(i).id == clid)
		break;

	if(i != ligs->len - 1)
	    return_value = lig(i + 1).id;
	else if(acps->len > 0)
	    return_value = acp(0)->id;
	else
	    return_value = lig(0).id;
    }
    else
    {
	for(i=0;i<acps->len;i++)
	    if(acp(i)->id == clid)
		break;	

	if(i != acps->len - 1)
	    return_value = acp(i + 1)->id;
	else
	    return_value = lig(0).id;
    }

    return return_value;
}


/** Return the id of the previous league or cup.
    @param clid The id of the current league or cup. */
gint
league_cup_get_previous_clid(gint clid)
{
    gint i, return_value = -1;

    if(clid < ID_CUP_START)
    {
	for(i=ligs->len - 1;i>=0;i--)
	    if(lig(i).id == clid)
		break;

	if(i != 0)
	    return_value = lig(i - 1).id;
	else if(acps->len > 0)
	    return_value = acp(acps->len - 1)->id;
	else
	    return_value = lig(ligs->len - 1).id;
    }
    else
    {
	for(i=acps->len - 1;i>=0;i--)
	    if(acp(i)->id == clid)
		break;

	if(i != 0)
	    return_value = acp(i - 1)->id;
	else
	    return_value = lig(ligs->len - 1).id;
    }

    return return_value;
}

/** Return the fixture coming after the specified week and round. 
    @param league The league the fixtures of which we examine. 
    @return A fixture pointer or NULL. */
Fixture*
league_cup_get_next_fixture(gint clid, gint week_number, gint week_round_number)
{
    gint i;
    GArray *fixtures = league_cup_get_fixtures(clid);

    for(i=0;i<fixtures->len;i++)
	if(g_array_index(fixtures, Fixture, i).week_number > week_number ||
	   (g_array_index(fixtures, Fixture, i).week_number == week_number &&
	    g_array_index(fixtures, Fixture, i).week_round_number >= week_round_number))
	    return &g_array_index(fixtures, Fixture, i);

    return NULL;
}

/** Return the fixture coming just before the specified week and round. 
    @param league The league the fixtures of which we examine. 
    @return A fixture pointer or NULL. */
Fixture*
league_cup_get_previous_fixture(gint clid, gint week_number, gint week_round_number)
{
    gint i;
    GArray *fixtures = league_cup_get_fixtures(clid);

    for(i=fixtures->len - 1;i>=0;i--)
	if(g_array_index(fixtures, Fixture, i).week_number < week_number ||
	   (g_array_index(fixtures, Fixture, i).week_number == week_number &&
	    g_array_index(fixtures, Fixture, i).week_round_number < week_round_number))
	    return &g_array_index(fixtures, Fixture, i);

    return NULL;
}


/** Return the average stadium capacity of cpu teams
    in the specified league or cup. */
gint
league_cup_average_capacity(gint clid)
{
    gint i;
    gfloat sum = 0;
    const GArray *teams = league_cup_get_teams(clid);

    for(i=0;i<teams->len;i++)
	if(team_is_user(&g_array_index(teams, Team, i)) == -1)
	    sum += g_array_index(teams, Team, i).stadium.capacity;

    return sum / (gfloat)teams->len;
}

/** Get the index of the league with the specified string id. */
gint
league_index_from_sid(const gchar *sid)
{
    gint i;

    for(i=0;i<ligs->len;i++)
	if(strcmp(lig(i).sid->str, sid) == 0)
	    return i;

    g_warning("league_index_from_sid: no index found for sid '%s'.\n", sid);

    return -1;
}

/** Remove the team with the specified id from the teams
    array without freeing the memory (used in promotion/relegation). */
void
league_remove_team_with_id(League *league, gint id)
{
    gint i;

    for(i=0;i<league->teams->len;i++)
	if(g_array_index(league->teams, Team, i).id == id)
	{
	    g_array_remove_index(league->teams, i);
	    return;
	}

    g_warning("league_remove_team_with_id: team with id %d in league %s not found\n",
	      id, league->name->str);
}

/** Add the teams to promote/relegate from the league to the array
    and remove them from the leagues. */
void
league_get_team_movements(League *league, GArray *team_movements)
{
    gint i, j, k;
    gint dest_idx;
    gint move_len = team_movements->len;
    TeamMove new_move;
    const GArray *elements = league->prom_rel.elements;
    GPtrArray *prom_games_teams = NULL;
    const Cup *prom_cup = NULL;

    for(i=0;i<elements->len;i++)
    {
	GPtrArray *dest_sids =
	    misc_separate_strings(g_array_index(elements, PromRelElement, i).dest_sid->str);
	gint order[dest_sids->len];

	k = 0;
	math_generate_permutation(order, 0, dest_sids->len - 1);

	for(j=g_array_index(elements, PromRelElement, i).ranks[0];
	    j<=g_array_index(elements, PromRelElement, i).ranks[1]; j++)
	{
	    new_move.tm = *(g_array_index(league->table.elements, TableElement, j - 1).team);
	    new_move.league_idx = 
		league_index_from_sid(((GString*)g_ptr_array_index(dest_sids, order[k++ % dest_sids->len]))->str);

	    g_array_append_val(team_movements, new_move);

	    if(team_is_user(g_array_index(league->table.elements, TableElement, j - 1).team) != -1)
	    {
		if(g_array_index(elements, PromRelElement, i).type == PROM_REL_PROMOTION)
		    user_history_add(&usr(team_is_user(
					      g_array_index(league->table.elements, TableElement, j - 1).team)),
				     USER_HISTORY_PROMOTED, new_move.tm.id, lig(new_move.league_idx).id, -1, "");
		else
		    user_history_add(&usr(team_is_user(
					      g_array_index(league->table.elements, TableElement, j - 1).team)),
				     USER_HISTORY_RELEGATED, new_move.tm.id, lig(new_move.league_idx).id, -1, "");
	    }
	}
	
	g_ptr_array_free(dest_sids, TRUE);
    }

    if(league_has_prom_games(league))
    {
	prom_cup = cup_from_sid(league->prom_rel.prom_games_cup_sid->str);
    
	if(prom_cup == NULL)
	{
	    g_warning("league_get_team_movements: promotion games cup not found for league %s (cup sid %s).\n",
		      league->name->str, league->prom_rel.prom_games_cup_sid->str);
	    return;
	}

	prom_games_teams = cup_get_teams_sorted(prom_cup);
	dest_idx = league_index_from_sid(league->prom_rel.prom_games_dest_sid->str);

	for(i=0;i<league->prom_rel.prom_games_number_of_advance;i++)
	{
	    new_move.tm = *((Team*)g_ptr_array_index(prom_games_teams, i));
	    new_move.league_idx = dest_idx;
	    g_array_append_val(team_movements, new_move);

	    if(team_is_user((Team*)g_ptr_array_index(prom_games_teams, i)) != -1)
		user_history_add(&usr(team_is_user(
					  (Team*)g_ptr_array_index(prom_games_teams, i))),
				 USER_HISTORY_PROMOTED, new_move.tm.id, lig(dest_idx).id, -1, "");
	}

	if(strlen(league->prom_rel.prom_games_loser_sid->str) > 0)
	{
	    dest_idx = league_index_from_sid(league->prom_rel.prom_games_loser_sid->str);

	    for(i=league->prom_rel.prom_games_number_of_advance;i<prom_games_teams->len;i++)
	    {
		new_move.tm = *((Team*)g_ptr_array_index(prom_games_teams, i));
		new_move.league_idx = dest_idx;
		g_array_append_val(team_movements, new_move);

		if(team_is_user((Team*)g_ptr_array_index(prom_games_teams, i)) != -1)
		    user_history_add(&usr(team_is_user(
					      (Team*)g_ptr_array_index(prom_games_teams, i))),
				     USER_HISTORY_RELEGATED, new_move.tm.id, lig(dest_idx).id, -1, "");
	    }
	}

	g_ptr_array_free(prom_games_teams, TRUE);
    }

    for(i=move_len;i<team_movements->len;i++)
	league_remove_team_with_id(league_from_clid(g_array_index(team_movements, TeamMove, i).tm.clid),
				   g_array_index(team_movements, TeamMove, i).tm.id);
}


/** Nullify league stuff at the beginning of a season. */
void
league_season_start(League *league)
{
    gint i, j;
    gint idx = league_index_from_sid(league->sid->str);
    gboolean user_champ = 
	(team_is_user(
	    team_of_id(g_array_index(lig(0).table.elements, TableElement, 0).team_id)) != -1);
    gfloat team_change_factor = 0;

    for(i=0;i<league->table.elements->len;i++)
    {
	g_array_index(league->table.elements, TableElement, i).team = 
	    &g_array_index(league->teams, Team, i);
	g_array_index(league->table.elements, TableElement, i).team_id = 
	    g_array_index(league->teams, Team, i).id;
	g_array_index(league->table.elements, TableElement, i).old_rank = i;

	for(j=0;j<TABLE_END;j++)
	    g_array_index(league->table.elements, TableElement, i).values[j] = 0;
    }

    for(i=0;i<league->teams->len;i++)
    {
	team_change_factor = 
	    (team_is_user(&g_array_index(league->teams, Team, i)) == -1) *
	    math_rnd(const_float("float_season_end_team_change_lower") +
		     (user_champ && idx == 0) * const_float("float_season_end_user_champ_addition"),
		     const_float("float_season_end_team_change_upper") +
		     (user_champ && idx == 0) * const_float("float_season_end_user_champ_addition"));

	for(j=0;j<g_array_index(league->teams, Team, i).players->len;j++)
	    player_season_start(
		&g_array_index(g_array_index(league->teams, Team, i).players, Player, j), team_change_factor);

	if(team_is_user(&g_array_index(league->teams, Team, i)) == -1)
	    team_update_cpu_structure(&g_array_index(league->teams, Team, i));
    }
}


/** Find out whether the team with specified rank in the league
    would participate in promotion games were the season to end. */
gboolean
query_league_rank_in_prom_games(const League *league, gint rank)
{
    gint i, j;
    const Cup *cup = NULL;

    for(i=0;i<ligs->len;i++)
	if(league_has_prom_games((&lig(i))))
	{
	    cup = cup_from_sid(lig(i).prom_rel.prom_games_cup_sid->str);
	    for(j=0;j<cup->choose_teams->len;j++)
	    {
		if(strcmp(g_array_index(cup->choose_teams, CupChooseTeam, j).sid->str,
			  league->sid->str) == 0 &&
		   ((rank >= g_array_index(cup->choose_teams,
					   CupChooseTeam, j).start_idx &&
		     rank <= g_array_index(cup->choose_teams, 
					   CupChooseTeam, j).end_idx && 
		     g_array_index(cup->choose_teams, 
				   CupChooseTeam, j).randomly) ||
		    (rank >= g_array_index(cup->choose_teams, 
					   CupChooseTeam, j).start_idx &&
		     rank < g_array_index(cup->choose_teams, 
					  CupChooseTeam, j).start_idx + 
		     g_array_index(cup->choose_teams, 
				   CupChooseTeam, j).number_of_teams &&
		     !g_array_index(cup->choose_teams, 
				    CupChooseTeam, j).randomly)))
		    return TRUE;
	    }
	}

    return FALSE;
}

/** Find out whether there are/were league matches. */
gboolean
query_league_matches_in_week(const League *league, gint week_number)
{
    gint i;

    for(i=0;i<league->fixtures->len;i++)
	if(g_array_index(league->fixtures, Fixture, i).week_number == week_number)
	    return TRUE;

    return FALSE;
}
