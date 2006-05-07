/*
   start_end.c

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

#include "bet.h"
#include "cup.h"
#include "file.h"
#include "finance.h"
#include "fixture.h"
#include "free.h"
#include "game_gui.h"
#include "gui.h"
#include "job.h"
#include "league.h"
#include "live_game.h"
#include "load_save.h"
#include "main.h"
#include "maths.h"
#include "misc.h"
#include "name.h"
#include "option.h"
#include "player.h"
#include "start_end.h"
#include "stat.h"
#include "table.h"
#include "team.h"
#include "transfer.h"
#include "user.h"
#include "variables.h"
#include "xml_name.h"
#include "youth_academy.h"

/** Prototype of a function called at the start or
    end of a week round. */
typedef void(*WeekFunc)(void);

/** Array of functions called when a week round
    is ended. */
WeekFunc end_week_round_funcs[] =
{end_week_round_results, end_week_round_sort_tables,
 end_week_round_update_fixtures, NULL};

/** Array of functions called when a week round
    is started. */
WeekFunc start_week_round_funcs[] =
{start_week_round_update_teams, bet_update, NULL};

/** Array of functions called when a week
    is started. */
WeekFunc start_week_funcs[] = 
{start_week_add_cups, start_week_update_users,
 start_week_update_teams, start_week_update_user_finances,
 youth_academy_update_weekly, transfer_update, job_update, NULL};

WeekFunc end_week_funcs[] = {stat_update_leagues, end_week_hide_cups, NULL};

/** Generate the teams etc. */
void
start_new_game(void)
{
    start_write_variables();

    start_generate_league_teams();
    start_new_season();
}

/** Make new fixtures, nullify things etc. */
void
start_new_season(void)
{
    gint i, j;
    gchar buf[SMALL];

    week = week_round = 1;
    free_names(TRUE);
    stat5 = STATUS_GENERATE_TEAMS;

    for(i = acps->len - 1; i >= 0; i--)
    {
	g_ptr_array_free(acp(i)->team_names, TRUE);
	acp(i)->team_names = g_ptr_array_new();

	if(acp(i)->add_week > 0)
	    g_ptr_array_remove_index(acps, i);
    }

    for(i=cps->len - 1; i >= 0; i--)
	if(cp(i).add_week == -1)
	{
	    cup_reset(&cp(i));
	    fixture_write_cup_fixtures(&cp(i));
	}

    if(season > 1)
    {
	for(i=0;i<users->len;i++)
	{
	    sprintf(buf, "%d", team_get_league_rank(usr(i).tm));
	    user_history_add(&usr(i), USER_HISTORY_END_SEASON,
			     usr(i).tm->name, 
			     league_cup_get_name_string(usr(i).tm->clid), 
			     buf, NULL);
	}

	start_new_season_league_changes();

	for(i=0;i<users->len;i++)
	{
	    usr(i).tm = team_of_id(usr(i).team_id);

	    usr(i).youth_academy.tm = usr(i).tm;
	    for(j=0;j<usr(i).youth_academy.players->len;j++)
	    {
		g_array_index(usr(i).youth_academy.players, Player, j).team = usr(i).tm;
		player_season_start(
		    &g_array_index(usr(i).youth_academy.players, Player, j), 0);
	    }

	    live_game_reset(&usr(i).live_game, NULL, TRUE);
	}

	start_new_season_reset_ids();
    }
    else
    {
	for(i=0;i<cps->len;i++)
	    if(cp(i).add_week <= 0)
		g_ptr_array_add(acps, &cp(i));
    }
		
    for(i=0;i<ligs->len;i++)
	if(lig(i).active)
	    fixture_write_league_fixtures(&lig(i));

    for(i=cps->len - 1; i >= 0; i--)
    {
	if(cp(i).add_week >= 0)
	{
	    cup_reset(&cp(i));
	    
	    if(cp(i).add_week == 0)
		fixture_write_cup_fixtures(&cp(i));
	}
	else if(cp(i).add_week == -1)
	{
	    for(j=0;j<cp(i).fixtures->len;j++)
	    {
		g_array_index(cp(i).fixtures, Fixture, j).teams[0] =
		    team_of_id(g_array_index(cp(i).fixtures, Fixture, j).team_ids[0]);
		g_array_index(cp(i).fixtures, Fixture, j).teams[1] =
		    team_of_id(g_array_index(cp(i).fixtures, Fixture, j).team_ids[1]);
	    }
	}
    }
    
    stat5 = -1;

    for(i=0;i<name_lists->len;i++)
	name_shorten_list(&nli(i));

    if(season == 1)
	bet_update();
}

/** Fill some global variables with default values at the
    beginning of a new game. */
void
start_write_variables(void)
{
    gint i;

    season = 1;
    cur_user = 0;

    for(i=0;i<users->len;i++)
	file_load_user_conf_file(&usr(i));
}

/** Generate the teams in the leagues. */
void
start_generate_league_teams(void)
{
    gint i, j;    

    stat5 = STATUS_GENERATE_TEAMS;

    if(ligs->len == 0)
	main_exit_program(EXIT_NO_LEAGUES,
			  "start_generate_league_teams: no leagues found. There must be at least one league in the game.\n");

    for(i=0;i<ligs->len;i++)
	for(j=0;j<lig(i).teams->len;j++)
	    team_generate_players_stadium(&g_array_index(lig(i).teams, Team, j), 0);

    stat5 = -1;
}

/** Reset the cup and league ids to the smallest possible
    value to avoid an overflow (gotta admit, the id system
    isn't perfect). */
void
start_new_season_reset_ids(void)
{
    gint i, max;

    max = -1;
    for(i=0;i<ligs->len;i++)
	if(lig(i).id > max)
	    max = lig(i).id;

    counters[COUNT_LEAGUE_ID] = max + 1;

    max = -1;
    for(i=0;i<cps->len;i++)
	if(cp(i).id > max)
	    max = cp(i).id;

    counters[COUNT_CUP_ID] = max + 1;
}

/** End a week round. */
void
end_week_round(void)
{
    gint i = 0;
    gboolean new_week = TRUE;
    WeekFunc *end_func = end_week_round_funcs;

    if(debug > 100)
	g_print("End w %d r %d \n", week, week_round);

    while(*end_func != NULL)
    {
	(*end_func)();
	end_func++;
    }

    week_round++;

    for(i=0;i<ligs->len;i++)
	if(query_fixture_in_week_round(lig(i).id, week, week_round))
	{
	    new_week = FALSE;
	    break;
	}

    for(i=0;i<acps->len;i++)
	if(query_fixture_in_week_round(acp(i)->id, week, week_round))
	{
	    new_week = FALSE;
	    break;
	}

    if(new_week)
    {
	end_week();

	if(query_start_end_season_end())
	{
	    end_season();

	    season++;
	    week = 1;
	    
	    start_new_season();
	}
	else
	    week++;

	week_round = 1;
	start_week();
    }

    start_week_round();
}

/** Calculate the match results of a week round. */
void
end_week_round_results(void)
{
    gint i, j, done = 0;
    gchar buf[SMALL], buf2[SMALL];
    gfloat num_matches =
	(gfloat)fixture_get_number_of_matches(week, week_round);
    
    for(i=0;i<ligs->len;i++)
    {
	for(j=0;j<lig(i).fixtures->len;j++)
	    if(g_array_index(lig(i).fixtures, Fixture, j).week_number == week &&
	       g_array_index(lig(i).fixtures, Fixture, j).week_round_number == week_round &&
	       g_array_index(lig(i).fixtures, Fixture, j).attendance == -1)
	    {
		live_game_calculate_fixture(&g_array_index(lig(i).fixtures, Fixture, j));

		done++;
		fixture_result_to_buf(&g_array_index(lig(i).fixtures, Fixture, j),
				      buf, FALSE);
		sprintf(buf2, "%s %s %s",
			g_array_index(lig(i).fixtures, Fixture, j).teams[0]->name,
			buf,
			g_array_index(lig(i).fixtures, Fixture, j).teams[1]->name);
		gui_show_progress((gfloat)done / num_matches, buf2,
				  PIC_TYPE_MATCHPIC);

		if(debug > 120)
		    g_print("%s \n", buf2);
	    }
    }

    for(i=0;i<acps->len;i++)
	for(j=0;j<acp(i)->fixtures->len;j++)
	{
	    if(g_array_index(acp(i)->fixtures, Fixture, j).week_number == week &&
	       g_array_index(acp(i)->fixtures, Fixture, j).week_round_number == week_round &&
	       g_array_index(acp(i)->fixtures, Fixture, j).attendance == -1)
	    {
		live_game_calculate_fixture(&g_array_index(acp(i)->fixtures, Fixture, j));

		done++;
		fixture_result_to_buf(&g_array_index(acp(i)->fixtures, Fixture, j), 
				      buf, FALSE);
		sprintf(buf2, "%s %s %s",
			g_array_index(acp(i)->fixtures, Fixture, j).teams[0]->name,
			buf,
			g_array_index(acp(i)->fixtures, Fixture, j).teams[1]->name);
		gui_show_progress((gfloat)done / num_matches, buf2,
				  PIC_TYPE_MATCHPIC);
		if(debug > 120)
		    g_print("%s \n", buf2);
	    }
	}

    gui_show_progress(-1, "", PIC_TYPE_MATCHPIC);
}

/** Sort league and cup tables. */
void
end_week_round_sort_tables(void)
{
    gint i, j, k;

    for(i=0;i<ligs->len;i++)
	if(query_fixture_in_week_round(lig(i).id, week, week_round))
	{
	    for(j=0;j<lig(i).table.elements->len;j++)
		g_array_index(lig(i).table.elements, TableElement, j).old_rank = j;

	    g_array_sort_with_data(lig(i).table.elements,
				   (GCompareDataFunc)table_element_compare_func,
				   GINT_TO_POINTER(lig(i).id));
	}

    for(i=0;i<acps->len;i++)
	if(query_fixture_in_week_round(acp(i)->id, week, week_round) &&
	   g_array_index(acp(i)->fixtures, Fixture, acp(i)->fixtures->len - 1).round ==
	   cup_has_tables(acp(i)->id))
	    for(j=0;j<cup_get_last_tables(acp(i)->id)->len;j++)
	    {
		for(k=0;k<g_array_index(cup_get_last_tables(acp(i)->id), Table, j).elements->len;k++)
		    g_array_index(
			g_array_index(cup_get_last_tables(acp(i)->id), Table, j).elements,
			TableElement, k).old_rank = k;

		g_array_sort_with_data(
		    g_array_index(cup_get_last_tables(acp(i)->id), Table, j).elements,
		    (GCompareDataFunc)table_element_compare_func,
		    GINT_TO_POINTER(acp(i)->id));
	    }
}

/** Update cup fixtures. */
void
end_week_round_update_fixtures(void)
{
    gint i;

    for(i=0;i<acps->len;i++)
	if(acp(i)->next_fixture_update_week == week &&
	   acp(i)->next_fixture_update_week_round == week_round)
	    fixture_update(acp(i));

    for(i=0;i<ligs->len;i++)
	if(lig(i).active && week == 
	   g_array_index(
	       lig(i).fixtures, Fixture, lig(i).fixtures->len - 1).week_number && 
	   week_round == 1 &&
	   team_is_user(g_array_index(lig(i).table.elements, TableElement, 0).team) != -1)
	    user_history_add(&usr(team_is_user(g_array_index(lig(i).table.elements, TableElement, 0).team)),
			     USER_HISTORY_CHAMPION, 
			     g_array_index(lig(i).table.elements, 
					   TableElement, 0).team->name,
			     league_cup_get_name_string(lig(i).id),
			     NULL, NULL);
    
    for(i=0;i<cps->len;i++)
    {
	if(cp(i).add_week == 1000 && 
	   cp(i).fixtures->len == 0 &&
	   query_cup_begins(&cp(i)))
	{
	    cp(i).last_week = cup_get_last_week_from_first(&cp(i), week + 1);
	    fixture_write_cup_fixtures(&cp(i));
	    g_ptr_array_add(acps, &cp(i));
	}
    }
}

/** Start a new week round. */
void
start_week_round(void)
{
    WeekFunc *start_func = start_week_round_funcs;

    if(debug > 100)
	g_print("Start w %d r %d \n", week, week_round);

    while(*start_func != NULL)
    {
	(*start_func)();
	start_func++;
    }

    if(!query_start_end_season_end() &&
	opt_int("int_opt_skip") &&
       !query_user_games_this_week_round() &&
       ((week_round == 1 && 
	 !query_user_games_in_week_round(week - 1, fixture_get_last_week_round(week - 1))) ||
	(week_round > 1 && 
	 !query_user_games_in_week_round(week, week_round - 1))))
	end_week_round();
    else
    {
        cur_user = 0;
        game_gui_show_main();

	user_event_show_next();
    }
}

/** Start a new week. */
void
start_week(void)
{
    WeekFunc *start_func = start_week_funcs;

    while(*start_func != NULL)
    {
	(*start_func)();
	start_func++;
    }
}

/** Finish a week. */
void
end_week(void)
{
    WeekFunc *end_func = end_week_funcs;

    while(*end_func != NULL)
    {
	(*end_func)();
	end_func++;
    }

    if(debug > 150)
	stat_show_av_league_goals();
}

/** Hide some not-so-important cups that
    are already finished. */
void
end_week_hide_cups(void)
{
    gint i;

    for(i=acps->len - 1; i >= 0; i--)
	if(query_cup_hide(acp(i)->id) &&
	   g_array_index(acp(i)->fixtures, Fixture, acp(i)->fixtures->len - 1).attendance > 0)
	    g_ptr_array_remove_index(acps, i);
}

/** Add the cups that begin later in the season to the acps array. */
void
start_week_add_cups(void)
{
    gint i;

    for(i=0;i<cps->len;i++)
	if(cp(i).add_week == week)
	{
	    fixture_write_cup_fixtures(&cp(i));
	    g_ptr_array_add(acps, &cp(i));
	}
}

/** Age increase etc. of players.
    CPU teams get updated at the end of their matches
    (to avoid cup teams getting updated too often). */
void
start_week_update_teams(void)
{
    gint i, j;
    
    for(i=0;i<ligs->len;i++)
	for(j=0;j<lig(i).teams->len;j++)
	    team_update_team_weekly(&g_array_index(lig(i).teams, Team, j));

    for(i=0;i<cps->len;i++)
	for(j=0;j<cp(i).teams->len;j++)
	    team_update_team_weekly((Team*)g_ptr_array_index(cp(i).teams, j));
}

/** Do some things at the beginning of each new round for
    the user teams. */
void
start_week_round_update_teams(void)
{
    gint i, j;
    
    for(i=0;i<ligs->len;i++)
	for(j=0;j<lig(i).teams->len;j++)
	    team_update_team_week_roundly(&g_array_index(lig(i).teams, Team, j));

    for(i=0;i<cps->len;i++)
	for(j=0;j<cp(i).teams->len;j++)
	    team_update_team_week_roundly((Team*)g_ptr_array_index(cp(i).teams, j));
}

/** Deduce wages etc. */
void
start_week_update_user_finances(void)
{
    gint i;

    if(sett_int("int_opt_disable_finances"))
	return;

    for(i=0;i<users->len;i++)
	finance_update_user_weekly(&usr(i));
}

/** Some general user update. */
void
start_week_update_users(void)
{
    gint i;

    for(i=0;i<users->len;i++)
    {
	if(usr(i).scout >= 100)
	    usr(i).scout = math_get_place(usr(i).scout, 2);
	
	if(usr(i).physio >= 100)
	    usr(i).physio = math_get_place(usr(i).physio, 2);

	if(usr(i).youth_academy.coach >= 100)
	    usr(i).youth_academy.coach = math_get_place(usr(i).youth_academy.coach, 2);

	user_weekly_update_counters(&usr(i));
    }
}

/** Check whether the season has ended. */
gboolean
query_start_end_season_end(void)
{
    gint i, j;

    for(i=0;i<ligs->len;i++)
	for(j=0;j<lig(i).fixtures->len;j++)
	    if(g_array_index(lig(i).fixtures, Fixture, j).week_number > week - 1)
		return FALSE;

    for(i=0;i<acps->len;i++)
	for(j=0;j<acp(i)->fixtures->len;j++)
	    if(g_array_index(acp(i)->fixtures, Fixture, j).week_number > week - 1)
		return FALSE;

    return TRUE;
}

/** Manage promotions and relegations at the beginning of a new season. */
void
start_new_season_league_changes(void)
{
    gint i, j, k;
    gint league_size[ligs->len];
    GArray *team_movements = g_array_new(FALSE, FALSE, sizeof(TeamMove));

    for(i=0;i<ligs->len;i++)
	league_get_team_movements(&lig(i), team_movements);

    for(i=0;i<ligs->len;i++)
	league_size[i] = lig(i).teams->len;

    for(i=0;i<team_movements->len;i++)
	league_remove_team_with_id(
	    league_from_clid(g_array_index(team_movements, TeamMove, i).tm.clid),
	    g_array_index(team_movements, TeamMove, i).tm.id);

    league_team_movements_destinations(team_movements, league_size);

    for(i = team_movements->len - 1; i >= 0; i--)
	if(g_array_index(team_movements, TeamMove, i).prom_rel_type == 
	   PROM_REL_RELEGATION)
	    g_array_prepend_val(
		lig(g_array_index(
			g_array_index(team_movements, TeamMove, i).dest_idcs, 
			gint, 0)).teams,
		g_array_index(team_movements, TeamMove, i).tm);
    
    for(i=1;i<team_movements->len;i++)
	if(g_array_index(team_movements, TeamMove, i).prom_rel_type != 
	   PROM_REL_RELEGATION)
	    g_array_append_val(
		lig(g_array_index(
			g_array_index(team_movements, TeamMove, i).dest_idcs, 
			gint, 0)).teams,
		g_array_index(team_movements, TeamMove, i).tm);
    
    for(i=0;i<team_movements->len;i++)
	g_array_free(g_array_index(team_movements, TeamMove, i).dest_idcs, TRUE);
    g_array_free(team_movements, TRUE);
    
    for(i=0;i<ligs->len;i++)
    {
	for(j=0;j<lig(i).teams->len;j++)
	{
	    g_array_index(lig(i).teams, Team, j).clid = lig(i).id;
	    for(k=0;k<g_array_index(lig(i).teams, Team, j).players->len;k++)
		g_array_index(g_array_index(lig(i).teams, Team, j).players, 
			      Player, k).team =
		    &g_array_index(lig(i).teams, Team, j);
	}

	league_season_start(&lig(i));
    }
}


/** End a season (store stats etc.) */
void
end_season(void)
{
    stat_create_season_stat();
}
