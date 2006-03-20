/*
   team.c

   Bygfoot Football Manager -- a small and simple GTK2-based
   football management game.

   http://bygfoot.sourceforge.net

   Copyright (C) 2005  Gyözö Both (gyboth@bygfoot.com)

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "cup.h"
#include "file.h"
#include "finance.h"
#include "fixture.h"
#include "game.h"
#include "game_gui.h"
#include "league.h"
#include "main.h"
#include "maths.h"
#include "misc.h"
#include "option.h"
#include "player.h"
#include "strategy.h"
#include "team.h"
#include "transfer.h"
#include "user.h"
#include "xml_team.h"

/**
   Generate a team with default values, e.g. 
   random playing structure and an empty string
   as name.
   @return A new team.
*/
Team
team_new(gboolean new_id)
{
    Team new;

    new.name = new.names_file = 
	new.symbol = new.def_file = 
	new.stadium.name = new.strategy_sid = NULL;
    
    new.clid = -1;
    new.id = (new_id) ? team_id_new : -1;
    new.structure = 442;
    new.style = 0;
    new.boost = 0;
    new.average_talent = 0;

    new.players = g_array_new(FALSE, FALSE, sizeof(Player));

    return new;
}

/* Fill the players array of the team and the stadium.
   @param tm The team that gets filled. */
void
team_generate_players_stadium(Team *tm, gfloat av_talent)
{
    gint i;    
    gfloat skill_factor = math_rnd(1 - const_float("float_team_skill_variance"),
				   1 + const_float("float_team_skill_variance"));
    Player new;
    gfloat wages = 0, average_talent, league_av_talent;
    gchar *def_file = team_has_def_file(tm);    

    tm->strategy_sid = strategy_get_random();

    tm->stadium.average_attendance = tm->stadium.possible_attendance =
	tm->stadium.games = 0;
    tm->stadium.safety = 
	math_rnd(const_float("float_team_stadium_safety_lower"),
		 const_float("float_team_stadium_safety_upper"));

    if(tm->clid < ID_CUP_START)
    {
	league_av_talent = (av_talent > 0) ?
	    av_talent : league_from_clid(tm->clid)->average_talent;
	average_talent = (tm->average_talent == 0) ?
	    skill_factor * league_av_talent :
	    tm->average_talent;
    }
    else
	average_talent = 
	    skill_factor * team_get_average_talents(lig(0).teams) *
	    (1 + cup_from_clid(tm->clid)->talent_diff);
	
    average_talent = CLAMP(average_talent, 0, const_float("float_player_max_skill"));
    tm->average_talent = average_talent;

    if(def_file == NULL)
    {
	for(i=0;i<const_int("int_team_cpu_players");i++)
	{
	    new = player_new(tm, average_talent, TRUE);
	    g_array_append_val(tm->players, new);
	}
    }
    else
    {
	xml_team_read(tm, def_file);
	g_free(def_file);
    }

    for(i=0;i<const_int("int_team_cpu_players") - 2;i++)
	wages += g_array_index(tm->players, Player, i).wage;

    tm->stadium.capacity = 
	math_round_integer((gint)rint((wages / (gfloat)const_int("int_team_stadium_ticket_price")) *
				      const_float("float_team_stadium_size_wage_factor")), 2);
}

/** Check whether the team is already part of an
    international cup. We'd like to avoid having Real Madrid
    both in the Champions' League and in the CWC.
    @param tm The team we check (by comparing names).
    @param group The cup group the team shouldn't be in.
    @return TRUE if the team's already participating in a cup,
    FALSE otherwise. */
gboolean
query_team_is_in_cups(const Team *tm, gint group)
{
    gint i, j;

    if(group == -1)
	return FALSE;

    for(i=0;i<cps->len;i++)
	if(cp(i).group == group)
	    for(j=0;j<cp(i).team_names->len;j++)
		if(strcmp(tm->name, 
			  (gchar*)g_ptr_array_index(cp(i).team_names, j)) == 0)
		{
		    if(debug > 90)
			g_print("team %s group %d found in %s \n", tm->name,
				group, cp(i).name);
		    return TRUE;
		}
    
    return FALSE;
}

/** Check whether a team participates in a cup.
    @param tm The team.
    @param cup The cup.
    @return TRUE or FALSE. */
gboolean
query_team_is_in_cup(const Team *tm, const Cup *cup)
{
    gint i;

    for(i=0;i<cup->team_names->len;i++)
	if(strcmp(tm->name, 
		  (gchar*)g_ptr_array_index(cup->team_names, i)) == 0)
	    return TRUE;

    return FALSE;
}

/** Return a GPtrArray containing the pointers
    to the teams from the teams array.
    @param teams The teams array we use.
    @return A GPtrArray containing pointers to the teams. */
GPtrArray*
team_get_pointers_from_array(const GArray *teams)
{
    gint i;
    GPtrArray *team_pointers = g_ptr_array_new();

    for(i=0;i<teams->len;i++)
	g_ptr_array_add(team_pointers, (gpointer)&g_array_index(teams, Team, i));

    return team_pointers;
}

/** Return the pointer to the team belonging to
    the id. */
Team*
team_of_id(gint id)
{    
    gint i, j;

    for(i=0;i<ligs->len;i++)
	for(j=0;j<lig(i).teams->len;j++)
	    if(g_array_index(lig(i).teams, Team, j).id == id)
		return &g_array_index(lig(i).teams, Team, j);

    for(i=0;i<cps->len;i++)
	for(j=0;j<cp(i).teams->len;j++)
	    if(((Team*)g_ptr_array_index(cp(i).teams, j))->id == id)
		return (Team*)g_ptr_array_index(cp(i).teams, j);

    main_exit_program(EXIT_POINTER_NOT_FOUND, 
		      "team_of_id: team with id %d not found.", id);

    return NULL;
}

/** Return a pointer to the next or last fixture the team participates in.
    @param tm The team we examine.
    @return The pointer to the fixture or NULL if none is found. */
Fixture*
team_get_fixture(const Team *tm, gboolean last_fixture)
{
    gint i, j;
    Fixture *fix = NULL;

    if(!last_fixture && 
       (stat0 == STATUS_LIVE_GAME_PAUSE ||
	stat0 == STATUS_SHOW_LIVE_GAME) &&
       (tm == ((LiveGame*)statp)->fix->teams[0] ||
	tm == ((LiveGame*)statp)->fix->teams[1]))
	return ((LiveGame*)statp)->fix;
    
    if(!last_fixture)
    {
	if(tm->clid < ID_CUP_START)
	    for(i=0;i<ligs->len;i++)
	    {
		if(lig(i).active && lig(i).id == tm->clid)
		{
		    for(j=0;j<lig(i).fixtures->len;j++)
			if(g_array_index(lig(i).fixtures, Fixture, j).attendance == -1 &&
			   query_fixture_team_involved((&g_array_index(lig(i).fixtures, Fixture, j)), tm->id))
			{
			    fix = &g_array_index(lig(i).fixtures, Fixture, j);
			    break;
			}

		    break;
		}
	    }

	for(i=0;i<acps->len;i++)
	    if(fix == NULL ||
	       fix->week_number != week ||
	       fix->week_round_number != week_round)
	    {
		if(query_cup_is_national(acp(i)->id) ||
		   query_team_is_in_cup(tm, acp(i)))
		{
		    for(j=0;j<acp(i)->fixtures->len;j++)
			if(g_array_index(acp(i)->fixtures, Fixture, j).attendance == -1 &&
			   query_fixture_team_involved((&g_array_index(acp(i)->fixtures, Fixture, j)), tm->id) &&
			   (fix == NULL ||
			    query_fixture_is_earlier(&g_array_index(acp(i)->fixtures, Fixture, j), fix)))
			{
			    fix = &g_array_index(acp(i)->fixtures, Fixture, j);
			    break;
			}
		}
	    }
    }
    else
    {
	if(tm->clid < ID_CUP_START)
	    for(i=0;i<ligs->len;i++)
	    {
		if(lig(i).active && lig(i).id == tm->clid)
		{
		    for(j=lig(i).fixtures->len - 1;j>=0;j--)
			if(g_array_index(lig(i).fixtures, Fixture, j).attendance != -1 &&
			   query_fixture_team_involved((&g_array_index(lig(i).fixtures, Fixture, j)), tm->id))
			{
			    fix = &g_array_index(lig(i).fixtures, Fixture, j);
			    break;
			}
		
		    break;
		}
	    }

	for(i=0;i<acps->len;i++)
	    if(fix == NULL ||
	       fix->week_number != week ||
	       fix->week_round_number != week_round - 1)
	    {
		if(query_cup_is_national(acp(i)->id) ||
		   query_team_is_in_cup(tm, acp(i)))
		{
		    for(j=acp(i)->fixtures->len - 1;j>=0;j--)
			if(g_array_index(acp(i)->fixtures, Fixture, j).attendance != -1 &&
			   query_fixture_team_involved((&g_array_index(acp(i)->fixtures, Fixture, j)), tm->id) &&
			   (fix == NULL ||
			    query_fixture_is_later(&g_array_index(acp(i)->fixtures, Fixture, j), fix)))
			{
			    fix = &g_array_index(acp(i)->fixtures, Fixture, j);
			    break;
			}
		}
	    }
    }

    return fix;
}

/** Check whether the team is a user-managed team.
    @param tm The team we examine.
    @return The user's index in the #users array or -1.*/
gint
team_is_user(const Team *tm)
{
    gint i;

    for(i=0;i<users->len;i++)
	if(usr(i).team_id == tm->id)
	    return i;

    return -1;
}

/** Check whether the team with given name is a user-managed team.
    @param team_name The team name we examine.
    @return The user's index in the #users array or -1.*/
gint
team_name_is_user(const gchar *team_name)
{
    gint i;

    for(i=0;i<users->len;i++)
	if(strcmp(team_name, usr(i).tm->name) == 0)
	    return i;

    return -1;
}

/** Return the overall average skill or
    the cskill of the first 11 players.
    @param tm The team we examine.
    @param cskill Whether to take into account all players. */
gfloat
team_get_average_skill(const Team *tm, gboolean cskill)
{
    gint i, counter = 0;
    gfloat sum = 0;
    
    if(!cskill)
    {
	for(i=0;i<tm->players->len;i++)
	    if(player_of_idx_team(tm, i)->cskill != 0)
	    {
		sum += player_of_idx_team(tm, i)->skill;
		counter++;
	    }
    }
    else
	for(i=0;i<11;i++)
	{
	    sum += (player_get_game_skill(player_of_idx_team(tm, i), FALSE, FALSE));
	    counter++;
	}

    return (counter > 0) ? sum / (gfloat)counter : 0;
}

/** Return the overall average talent of the team's players.
    @param tm The team we examine. */
gfloat
team_get_average_talent(const Team *tm)
{
    gint i;
    gfloat sum = 0;

    for(i=0;i<tm->players->len;i++)
	sum += player_of_idx_team(tm, i)->talent;

    return (tm->players->len > 0) ? sum / (gfloat)tm->players->len : 0;
}


/** Return the rank of the team in the league tables. */
gint
team_get_league_rank(const Team *tm)
{
    gint i, clid = team_get_table_clid(tm), rank = 0;
    GArray *elements = NULL;
    
    if(clid < ID_CUP_START)
    {
	if(!league_from_clid(clid)->active)
	    return 0;

	elements = league_from_clid(tm->clid)->table.elements;
    }
    else
    {
	rank = team_get_cup_rank(
	    tm, &g_array_index(cup_from_clid(clid)->rounds, CupRound,
			       cup_has_tables(clid)), FALSE);
	return (rank == -1) ? 0 : rank;
    }

    for(i=0;i<elements->len;i++)
	if(g_array_index(elements, TableElement, i).team_id == tm->id)
	    return i + 1;
    
    main_exit_program(EXIT_INT_NOT_FOUND, 
		      "team_get_league_rank: no rank found for team %s in league %s. \n",
		      tm->name, league_cup_get_name_string(tm->clid));

    return -1;
}

/** Return the rank of the team in the round robin stage.
    @param abort Whether to exit if no corresponding entry can be found. */
gint
team_get_cup_rank(const Team *tm, const CupRound *cupround, gboolean abort)
{
    gint i, j;

    for(i=0;i<cupround->tables->len;i++)
    {
	for(j=0;j<g_array_index(cupround->tables, Table, i).elements->len;j++)
	    if(g_array_index(g_array_index(cupround->tables, Table, i).elements, TableElement, j).team_id == tm->id)
		return j + 1;
    }

    if(abort)
	main_exit_program(EXIT_INT_NOT_FOUND, 
			  "team_get_cup_rank: no rank found for team %s. \n ", 
			  tm->name);

    return -1;
}

/** Return the structure that fits the positions of
    the first 11 players.
    @param tm The team we examine.
    @return A new structure. */
gint
team_find_appropriate_structure(const Team *tm)
{
  gint i;
  gint structure = 0;

  for(i=0;i<11;i++)
      if(player_of_idx_team(tm, i)->cskill > 0 && player_of_idx_team(tm, i)->cpos != 0)
      {
	  if(player_of_idx_team(tm, i)->pos != 0)
	      structure += (gint)rint(powf(10, PLAYER_POS_FORWARD - player_of_idx_team(tm, i)->pos));
	  else
	      structure += (gint)rint(powf(10, PLAYER_POS_FORWARD - player_of_idx_team(tm, i)->cpos));
      }

  return structure;
}

/** Change the structure of a team and the appropriate
    cpos and cskill values.
    @param tm The team.
    @param new_structure The new structure value, e.g. 442. */
void
team_change_structure(Team *tm, gint new_structure)
{
  gint i;

  tm->structure = new_structure;

  for(i=1;i<11;i++)
    {
	player_of_idx_team(tm, i)->cpos =
	    player_get_position_from_structure(new_structure, i);

	player_of_idx_team(tm, i)->cskill =
	    player_get_cskill(player_of_idx_team(tm, i), 
			      player_of_idx_team(tm, i)->cpos, TRUE);
    }
}

/* Try to set each of the first 11 players on his
   favoured position and sort the substitutes by position.
   @param tm The team we rearrange. */
void
team_rearrange(Team *tm)
{
    gint i;

    g_array_sort_with_data(tm->players, (GCompareDataFunc)player_compare_func,
			   GINT_TO_POINTER(100 + PLAYER_COMPARE_ATTRIBUTE_POS));

    for(i=0;i<tm->players->len;i++)
    {
	player_of_idx_team(tm, i)->cpos = (i < 11) ?
	    player_get_position_from_structure(tm->structure, i) : player_of_idx_team(tm, i)->pos;
	if(player_of_idx_team(tm, i)->cskill > 0)
	    player_of_idx_team(tm, i)->cskill = (i < 11) ?
		player_get_cskill(player_of_idx_team(tm, i), 
				  player_of_idx_team(tm, i)->cpos, TRUE) : player_of_idx_team(tm, i)->skill;
    }
}

/** Return the name of the current setting of a team attribute, e.g. style.
    @param  tm The team.
    @param attribute The attribute. */
gchar*
team_attribute_to_char(gint attribute, gint value)
{
    switch(attribute)
    {
	default:
	    main_exit_program(EXIT_INT_NOT_FOUND, 
			      "team_attribute_to_char: unknown attribute %d\n", 
			      attribute);
	    break;
	case TEAM_ATTRIBUTE_STYLE:
	    switch(value)
	    {
		case -2:
		    return _("ALL OUT DEFEND");
		case -1:
		    return _("DEFEND");
		case 0:
		    return _("BALANCED");
		case 1:
		    return _("ATTACK");
		case 2:
		    return _("ALL OUT ATTACK");
	    }
	    break;
	case TEAM_ATTRIBUTE_BOOST:
	    switch(value)
	    {
		case -1:
		    /* Boost value. */
		    return _("ANTI");
		case 0:
		    /* Boost value. */
		    return _("OFF");
		case 1:
		    /* Boost value. */
		    return _("ON");
	    }
	    break;
    }

    return NULL;
}

/** Change a team attribute of the current user and print a message.
    @param attribute The attribute.
    @param new_value The new value. */
void
team_change_attribute_with_message(Team *tm, gint attribute, gint new_value)
{
    switch(attribute)
    {
	default:
	    g_warning("team_attribute_to_char: unknown attribute %d\n", attribute);
	    break;
	case TEAM_ATTRIBUTE_STYLE:
	    current_user.tm->style = new_value;
	    game_gui_print_message(_("Team style changed to %s."), 
				   team_attribute_to_char(attribute, new_value));
	    break;
	case TEAM_ATTRIBUTE_BOOST:
	    current_user.tm->boost = new_value;
	    if(new_value == 1)
		game_gui_print_message(
		    _("Boost changed to %s (costs %d per minute)."), 
		    team_attribute_to_char(attribute, new_value),
		    (gint)rint(finance_wage_unit(current_user.tm) *
			       const_float("float_boost_cost_factor")));
	    else
		game_gui_print_message(_("Boost changed to %s."), 
				       team_attribute_to_char(attribute, new_value));
	    break;
    }
}

/** Replace some players by new ones in a team. */
void
team_update_cpu_new_players(Team *tm)
{
    gint i;
    gint number_of_new = math_rndi(const_int("int_team_new_players_lower"),
				   const_int("int_team_new_players_upper"));
    gint player_numbers[tm->players->len];

    math_generate_permutation(player_numbers, 0, tm->players->len - 1);

    for(i=0;i<number_of_new;i++)
    {
	if(!query_transfer_player_is_on_list(player_of_idx_team(tm, player_numbers[i])))
	    player_replace_by_new(player_of_idx_team(tm, player_numbers[i]), TRUE);
    }
}

/** Increase player ages etc.
    @param tm The user team we examine. */
void
team_update_team_weekly(Team *tm)
{
    gint i;

    for(i=tm->players->len - 1;i>=0;i--)
	player_update_weekly(&g_array_index(tm->players, Player, i));

    if(team_is_user(tm) == -1 &&
       math_rnd(0, 1) < const_float("float_team_new_player_probability"))
	team_update_cpu_new_players(tm);
}

/** Regenerate player fitness etc. after a match. 
    @param tm The user team we examine.
    @param clid The fixture clid. */
void
team_update_post_match(Team *tm, const Fixture *fix)
{
    gint i;

    for(i=0;i<tm->players->len;i++)
	player_update_post_match(player_of_idx_team(tm, i), fix);
}

/** Some updates each round.
    @param tm The user team we examine. */
void
team_update_team_week_roundly(Team *tm)
{
    gint i;

    for(i=0;i<tm->players->len;i++)
	player_update_week_roundly(tm, i);

    if(team_is_user(tm) == -1)
	strategy_update_team_pre_match(tm);
}

/** Return a value from the league table element going with the team.
    @param type The type of the value. */
gint
team_get_table_value(const Team *tm, gint type)
{
    gint i;
    const GArray *elements = NULL;

    if(tm->clid >= ID_CUP_START)
	main_exit_program(EXIT_INT_NOT_FOUND, 
			  "team_get_table_value: team is not a league team: %s \n", 
			  tm->name);
    
    elements = league_from_clid(tm->clid)->table.elements;

    for(i=0;i<elements->len;i++)
	if(g_array_index(elements, TableElement, i).team_id == tm->id)
	    break;

    if(i == elements->len)
	main_exit_program(EXIT_INT_NOT_FOUND, 
			  "team_get_table_value: table entry not found for team %s \n", 
			  tm->name);

    return g_array_index(elements, TableElement, i).values[type];
}

/** Compare function for team arrays or pointer arrays. */
gint
team_compare_func(gconstpointer a, gconstpointer b, gpointer data)
{
    gint type = GPOINTER_TO_INT(data) % 100;
    const Team *tm1 = (GPOINTER_TO_INT(data) < 100) ? 
	*(const Team**)a : (const Team*)a;
    const Team *tm2 = (GPOINTER_TO_INT(data) < 100) ? 
	*(const Team**)b : (const Team*)b;
    gint return_value = 0;

    if(type == TEAM_COMPARE_LEAGUE_RANK)
    {
	if(tm1->clid == tm2->clid)
	    return_value = misc_int_compare(team_get_league_rank(tm2), team_get_league_rank(tm1));
	else
	    return_value = misc_int_compare(league_from_clid(tm2->clid)->layer,
					    league_from_clid(tm1->clid)->layer);
    }
    else if(type == TEAM_COMPARE_LEAGUE_LAYER)
	return_value = 
	    (tm1->clid >= ID_CUP_START || tm2->clid >= ID_CUP_START) ?
	    0 : misc_int_compare(league_from_clid(tm2->clid)->layer,
				 league_from_clid(tm1->clid)->layer);
    else if(type == TEAM_COMPARE_OFFENSIVE)
    {
	gint gf1 = team_get_table_value(tm1, TABLE_GF),
	    gf2 = team_get_table_value(tm2, TABLE_GF),
	    ga1 = team_get_table_value(tm1, TABLE_GA),
	    ga2 = team_get_table_value(tm2, TABLE_GA);

	if(gf1 > gf2)
	    return_value = -1;
	else if(gf1 < gf2)
	    return_value = 1;
	else if(ga1 < ga2)
	    return_value =  -1;
	else if(ga1 > ga2)
	    return_value = 1;
	else
	    return_value = 0;
    }
    else if(type == TEAM_COMPARE_DEFENSE)
    {
	gint gf1 = team_get_table_value(tm1, TABLE_GF),
	    gf2 = team_get_table_value(tm2, TABLE_GF),
	    ga1 = team_get_table_value(tm1, TABLE_GA),
	    ga2 = team_get_table_value(tm2, TABLE_GA);

	if(ga1 > ga2)
	    return_value = 1;
	else if(ga1 < ga2)
	    return_value = -1;
	else if(gf1 > gf2)
	    return_value =  -1;
	else if(gf1 < gf2)
	    return_value = 1;
	else
	    return_value = 0;
    }
    else if(type == TEAM_COMPARE_UNSORTED)
	return_value = 0;
    else if(type == TEAM_COMPARE_AV_SKILL)
	return_value = misc_float_compare(team_get_average_skill(tm1, FALSE),
					  team_get_average_skill(tm2, FALSE));

    return return_value;
}

/** Return the teams from all leagues sorted by the
    specified function. 
    @param type The integer to pass to the compare function.
    @param cup Whether we return the international cup teams or league teams. */
GPtrArray*
team_get_sorted(GCompareDataFunc compare_function, gint type, gboolean cup)
{ 
    gint i, j;
    GPtrArray *teams = g_ptr_array_new();

    if(!cup)
    {
	for(i=0;i<ligs->len;i++)
	    for(j=0;j<lig(i).teams->len;j++)
		g_ptr_array_add(teams, &g_array_index(lig(i).teams, Team, j));
    }
    else
    {
	for(i=0;i<acps->len;i++)
	    for(j=0;j<acp(i)->teams->len;j++)
		g_ptr_array_add(teams, g_ptr_array_index(acp(i)->teams, j));
    }

    g_ptr_array_sort_with_data(teams, compare_function, GINT_TO_POINTER(type));

    return teams;
}

/** Find a new team for a user, depending on whether he's been
    fired or very successful. */
Team*
team_get_new(const Team *tm, gboolean fire)
{
    gint i;
    gint lower = 0, upper = 0;
    gint bound1 = (fire) ? const_int("int_team_new_bound_upper") :
	const_int("int_team_new_bound_lower"),
	bound2 = (fire) ? const_int("int_team_new_bound_lower") :
	const_int("int_team_new_bound_upper");
    gint idx = -1;
    GPtrArray *teams = 
	team_get_sorted(team_compare_func, TEAM_COMPARE_LEAGUE_RANK, FALSE);
    Team *return_value;

    for(i=0;i<teams->len;i++)
	if((Team*)g_ptr_array_index(teams, i) != tm)
	    upper++;
	else
	{
	    idx = i;
	    break;
	}

    for(i=teams->len - 1; i >= 0; i--)
	if((Team*)g_ptr_array_index(teams, i) != tm)
	    lower++;
	else
	    break;
    
    bound1 = MIN(bound1, upper);
    bound2 = MIN(bound2, lower);

    return_value = (Team*)g_ptr_array_index(teams, math_rndi(i - bound1, i + bound2));
    while(return_value == tm)
	return_value = (Team*)g_ptr_array_index(teams, math_rndi(i - bound1, i + bound2));

    g_ptr_array_free(teams, TRUE);

    return return_value;
}

/** Return the index of the team in the teams array. */
gint
team_get_index(const Team *tm)
{
    gint i;
    gpointer *teams = league_cup_get_teams(tm->clid);

    if(tm->clid < ID_CUP_START)
    {
	for(i=0;i<((GArray*)teams)->len;i++)
	    if(&g_array_index((GArray*)teams, Team, i) == tm)
		return i;
    }
    else
    {
	for(i=0;i<((GPtrArray*)teams)->len;i++)
	    if((Team*)g_ptr_array_index((GPtrArray*)teams, i) == tm)
		return i;
    }

    main_exit_program(EXIT_INT_NOT_FOUND, 
		      "team_get_index: team %s not found.\n", tm->name);

    return -1;
}

/** Return the average of the average talents of the teams in the array. */
gfloat
team_get_average_talents(const GArray *teams)
{
    gint i, j, cnt = 0;
    gfloat sum = 0;

    if(teams->len == 0)
	return 0;

    for(i=0;i<teams->len;i++)
	for(j=0;j<g_array_index(teams, Team, i).players->len;j++)
	{
	    sum += g_array_index(g_array_index(teams, Team, i).players,
				 Player, j).talent;
	    cnt++;
	}

    return sum / (gfloat)cnt;
}

/** Find out whether a team plays at a given date. */
gboolean
query_team_plays(const Team *tm, gint week_number, gint week_round_number)
{
    gint i, j;

    if(tm->clid < ID_CUP_START)
	for(i=0;i<ligs->len;i++)
	    if(lig(i).id == tm->clid)
		for(j=0;j<lig(i).fixtures->len;j++)
		    if(g_array_index(lig(i).fixtures, Fixture, j).week_number == week_number && 
		       g_array_index(lig(i).fixtures, Fixture, j).week_round_number == week_round_number &&
		       (g_array_index(lig(i).fixtures, Fixture, j).teams[0] == tm ||
			g_array_index(lig(i).fixtures, Fixture, j).teams[1] == tm))
			return TRUE;
    
    for(i=0;i<acps->len;i++)
	if(query_cup_is_national(acp(i)->id) ||
	   query_team_is_in_cup(tm, acp(i)))
	    for(j=0;j<acp(i)->fixtures->len;j++)
		if(g_array_index(acp(i)->fixtures, Fixture, j).week_number == week_number && 
		   g_array_index(acp(i)->fixtures, Fixture, j).week_round_number == week_round_number &&
		   (g_array_index(acp(i)->fixtures, Fixture, j).teams[0] == tm ||
		    g_array_index(acp(i)->fixtures, Fixture, j).teams[1] == tm))
		    return TRUE;
    
    return FALSE;
}

/** Show the results of the user team against the specified team. 
    @param buf The buffer to fill with the results.
    @param sort Whether to sort the results according to home/away.
    @param cup_matches Whether to take cup matches into account as well. */
void
team_write_own_results(const Team *tm, gchar *buf, gboolean sort, gboolean cup_matches)
{
    gint i, res[2];
    gchar buf2[SMALL], buf3[SMALL], buf4[SMALL], neutral[SMALL];
    gint place;
    GPtrArray *matches = fixture_get_matches(current_user.tm, tm);        

    strcpy(buf4, "");
    strcpy(neutral, "");

    for(i=0;i<matches->len;i++)
	if(cup_matches || ((Fixture*)g_ptr_array_index(matches, i))->clid < ID_CUP_START)
	{
	    res[0] = math_sum_int_array(((Fixture*)g_ptr_array_index(matches, i))->result[0], 2);
	    res[1] = math_sum_int_array(((Fixture*)g_ptr_array_index(matches, i))->result[1], 2);
	
	    if(res[((Fixture*)g_ptr_array_index(matches, i))->teams[0] != current_user.tm] >
	       res[((Fixture*)g_ptr_array_index(matches, i))->teams[0] == current_user.tm])
		/* a won match */
		sprintf(buf2, _("W  %d : %d"), 
			res[((Fixture*)g_ptr_array_index(matches, i))->teams[0] != current_user.tm],
			res[((Fixture*)g_ptr_array_index(matches, i))->teams[0] == current_user.tm]);
	    else if(res[((Fixture*)g_ptr_array_index(matches, i))->teams[0] != current_user.tm] <
		    res[((Fixture*)g_ptr_array_index(matches, i))->teams[0] == current_user.tm])
		/* a lost match */
		sprintf(buf2, _("L  %d : %d"), 
			res[((Fixture*)g_ptr_array_index(matches, i))->teams[0] != current_user.tm],
			res[((Fixture*)g_ptr_array_index(matches, i))->teams[0] == current_user.tm]);
	    else
		/* a drawn match */
		sprintf(buf2, _("Dw %d : %d"),
			res[((Fixture*)g_ptr_array_index(matches, i))->teams[0] != current_user.tm],
			res[((Fixture*)g_ptr_array_index(matches, i))->teams[0] == current_user.tm]);
	
	    if(((Fixture*)g_ptr_array_index(matches, i))->home_advantage)
	    {
	    
		if(((Fixture*)g_ptr_array_index(matches, i))->teams[0] == current_user.tm)
		{
		    /* a match at home */
		    sprintf(buf3, _("%s (H) "), buf2);
		    place = 0;
		}
		else
		{
		    /* a match away */
		    sprintf(buf3, _("<span background='%s' foreground='%s'>%s (A) </span> "), 
			    const_app("string_treeview_live_game_commentary_away_bg"),
			    const_app("string_treeview_live_game_commentary_away_fg"),
			    buf2);
		    place = 1;
		}
	    }
	    else
	    {
		/* a match on neutral ground */
		sprintf(buf3, _("%s (N) "), buf2);
		place = -1;
	    }

	    if(!sort || place == 1)
		strcat(buf4, buf3);
	    else
	    {
		if(place == 0)
		{
		    strcpy(buf2, buf4);
		    sprintf(buf4, "%s%s", buf3, buf2);
		}
		else if(place == -1)
		    strcat(neutral, buf3);
	    }
	}

    sprintf(buf, "%s%s", buf4, neutral);

    g_ptr_array_free(matches, TRUE);
}

/** Show a row of WDWWLL type results and the goals for and against.
    @param tm The team we find the results for.
    @param buf The buffer we print the results into. */
void
team_write_results(const Team *tm, gchar *result_buf, gchar *goals_buf)
{
    gint i;
    GPtrArray *latest_fixtures = fixture_get_latest(tm);
    gint res[2], goals[2] = {0, 0};
    gint end_idx = latest_fixtures->len - const_int("int_treeview_latest_results");

    strcpy(result_buf, "");
    end_idx = MAX(0, end_idx);
    for(i=latest_fixtures->len - 1;i>=end_idx;i--)
    {
	res[0] = math_sum_int_array(((Fixture*)g_ptr_array_index(latest_fixtures, i))->result[0], 3);
	res[1] = math_sum_int_array(((Fixture*)g_ptr_array_index(latest_fixtures, i))->result[1], 3);
	goals[0] += 
	    math_sum_int_array(((Fixture*)
				g_ptr_array_index(latest_fixtures, i))->
			       result[(((Fixture*)g_ptr_array_index(latest_fixtures, i))->teams[0] != tm)], 2);
	goals[1] += 
	    math_sum_int_array(((Fixture*)
				g_ptr_array_index(latest_fixtures, i))->
			       result[(((Fixture*)g_ptr_array_index(latest_fixtures, i))->teams[0] == tm)], 2);
	if(res[0] == res[1])
	    /* draw */
	    strcat(result_buf, _("Dw "));
	else if(res[(((Fixture*)g_ptr_array_index(latest_fixtures, i))->teams[0] == tm)] >
		res[(((Fixture*)g_ptr_array_index(latest_fixtures, i))->teams[0] != tm)])
	    /* lost */
	    strcat(result_buf, _("L  "));
	else
	    /* won */
	    strcat(result_buf, _("W  "));
    }

    sprintf(goals_buf, "%d : %d", goals[0], goals[1]);
    g_ptr_array_free(latest_fixtures, TRUE);
}

/** Find out whether the team is in the given pointer array. */
gboolean
query_team_is_in_teams_array(const Team *tm, const GPtrArray *teams)
{
    gint i;

    for(i=0;i<teams->len;i++)
	if((Team*)g_ptr_array_index(teams, i) == tm)
	    return TRUE;

    return FALSE;
}

/** Check whether we find a definition file for the
    given team. */
gchar*
team_has_def_file(const Team *tm)
{
    gchar *return_value = NULL;
    gchar buf[SMALL];

    if(tm->def_file != NULL && opt_int("int_opt_load_defs") != 0)
    {
	sprintf(buf, "team_%s.xml", tm->def_file);
	return_value = file_find_support_file(buf, FALSE);
    }

    return return_value;
}

/** Complete the definition of the team (add players,
    calculate wages etc. Called after reading a team def file. */
void
team_complete_def(Team *tm)
{
    gint i, new_pos, pos_sum;
    gint positions[4] = {0, 0, 0, 0};
    Player new_player;
    gint add = const_int("int_team_cpu_players") - tm->players->len;
    gboolean is_user = (team_is_user(tm) != -1);

    for(i=0;i<tm->players->len;i++)
    {
	player_complete_def(&g_array_index(tm->players, Player, i), 
			    tm->average_talent);
	positions[g_array_index(tm->players, Player, i).pos]++;

	/** This is so we don't remove loaded players
	    from the team at startup. */
	if(is_user)
	    g_array_index(tm->players, Player, i).recovery = 1;
    }

    for(i=0;i<add;i++)
    {
	pos_sum = math_sum_int_array(positions, 4);
	new_player = player_new(tm, tm->average_talent, TRUE);
	
	if(positions[0] < 2)
	    new_pos = 0;
	else if((gfloat)positions[1] / (gfloat)pos_sum <
		const_float("float_player_pos_bound1"))
	    new_pos = 1;
	else if((gfloat)positions[2] / (gfloat)pos_sum <
		const_float("float_player_pos_bound1"))
	    new_pos = 2;
	else
	    new_pos = 3;

	new_player.pos = new_player.cpos = new_pos;
	positions[new_pos]++;

	g_array_append_val(tm->players, new_player);
    }

    team_complete_def_sort(tm);
}

/** Sort the players in the team according to the team structure
    and the player positions. */
void
team_complete_def_sort(Team *tm)
{
    gint i, j;
    gint positions[4] = {0, 0, 0, 0};
    gint structure[4] = {1, 
			 math_get_place(tm->structure, 3),
			 math_get_place(tm->structure, 2),
			 math_get_place(tm->structure, 1)};
    Player player_tmp, player_tmp2;
    
    for(i=0;i<11;i++)
	positions[g_array_index(tm->players, Player, i).pos]++;

    for(i=0;i<4;i++)
    {
	while(positions[i] > structure[i])
	{
	    for(j=0;j<11;j++)
		if(g_array_index(tm->players, Player, j).pos == i)
		{
		    player_tmp = g_array_index(tm->players, Player, j);
		    g_array_remove_index(tm->players, j);
		    break;
		}

	    for(j=10;j<tm->players->len;j++)
	    {
		if(g_array_index(tm->players, Player, j).pos != i)
		{
		    player_tmp2 = g_array_index(tm->players, Player, j);
		    g_array_remove_index(tm->players, j);
		    break;
		}
	    }

	    if(j == tm->players->len)
		main_exit_program(EXIT_DEF_SORT, 
				  "team_complete_def_sort (1): cannot sort according to structure %d (team %s).",
				  tm->structure, tm->name);
	    
	    positions[i]--;
	    positions[player_tmp2.pos]++;

	    g_array_append_val(tm->players, player_tmp);
	    g_array_prepend_val(tm->players, player_tmp2);	    
	}

	while(positions[i] < structure[i])
	{
	    for(j=0;j<11;j++)
		if(g_array_index(tm->players, Player, j).pos > i)
		{
		    player_tmp = g_array_index(tm->players, Player, j);
		    g_array_remove_index(tm->players, j);
		    break;
		}

	    if(j == 11)
		main_exit_program(EXIT_DEF_SORT, 
				  "team_complete_def_sort (2): cannot sort according to structure %d (team %s).",
				  tm->structure, tm->name);

	    for(j=10;j<tm->players->len;j++)
		if(g_array_index(tm->players, Player, j).pos == i)
		{
		    player_tmp2 = g_array_index(tm->players, Player, j);
		    g_array_remove_index(tm->players, j);
		    break;
		}

	    if(j == tm->players->len)
		main_exit_program(EXIT_DEF_SORT, 
				  "team_complete_def_sort (3): cannot sort according to structure %d (team %s).",
				  tm->structure, tm->name);
	    
	    positions[i]++;
	    positions[player_tmp.pos]--;

	    g_array_append_val(tm->players, player_tmp);
	    g_array_prepend_val(tm->players, player_tmp2);
	}
    }

    team_rearrange(tm);
}

/** Find out which cup or league table the team
    belongs to. */
gint
team_get_table_clid(const Team *tm)
{
    gint i;

    if(tm->clid >= ID_CUP_START ||
       (tm->clid < ID_CUP_START && !league_from_clid(tm->clid)->active))
    {
	for(i = acps->len - 1; i >= 0; i--)
	    if(cup_has_tables(acp(i)->id) != -1 && 
	       query_team_is_in_cup(tm, acp(i)))
		return acp(i)->id;
    }

    return tm->clid;
}

const Team*
team_get_next_opponent(const Team *tm)
{
    const Fixture *fix = team_get_fixture(tm, FALSE);
    
    return (fix == NULL) ? NULL :
	fix->teams[fix->teams[0] == tm];
}
