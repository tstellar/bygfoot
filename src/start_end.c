#include "cup.h"
#include "file.h"
#include "finance.h"
#include "fixture.h"
#include "free.h"
#include "game_gui.h"
#include "gui.h"
#include "league.h"
#include "live_game.h"
#include "load_save.h"
#include "main.h"
#include "maths.h"
#include "option.h"
#include "start_end.h"
#include "table.h"
#include "team.h"
#include "transfer.h"
#include "user.h"
#include "variables.h"
#include "xml_name.h"

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
{start_week_round_update_user_teams , NULL};

/** Array of functions called when a week
    is started. */
WeekFunc start_week_funcs[] = 
{start_week_update_users, start_week_update_user_teams,
 start_week_update_user_finances, transfer_update, NULL};

WeekFunc end_week_funcs[] = {NULL};

/** Generate the teams etc. */
void
start_new_game(void)
{
    xml_name_read(opt_str("string_opt_player_names_file"), -1);
    start_write_variables();
    start_generate_league_teams();
    start_new_season();
    xml_name_read(opt_str("string_opt_player_names_file"), 1000);
}

/** Make new fixtures, nullify things etc. */
void
start_new_season(void)
{
    gint i;

    xml_name_read(opt_str("string_opt_player_names_file"), 1000);

    week = week_round = 1;

    /*todo: nullify, promotion/relegation*/
    if(season > 1)
    {
	for(i=0;i<users->len;i++)
	    user_history_add(&usr(i), USER_HISTORY_END_SEASON,
			     usr(i).team_id, usr(i).tm->clid, 
			     team_get_league_rank(usr(i).tm), "");

	start_new_season_team_movements();

	for(i=0;i<users->len;i++)
	    usr(i).tm = team_of_id(usr(i).team_id);
    }

    start_load_cup_teams();

    for(i=acps->len - 1;i >= 0;i--)
    {
	if(acp(i)->id >= ID_PROM_CUP_START)
	    g_ptr_array_remove_index(acps, i);
	else
	    fixture_write_cup_fixtures(&cp(i));
    }

    for(i=0;i<ligs->len;i++)
    {
	if(season > 1)
	    league_season_start(&lig(i));
	fixture_write_league_fixtures(&lig(i));
    }

}

/** Fill some global variables with default values at the
    beginning of a new game. */
void
start_write_variables(void)
{
    gint i;

    season = 1;
    cur_user = 0;

    /*d*/
/*     week=24; week_round = 2; */

    for(i=0;i<users->len;i++)
	file_load_user_conf_file(&usr(i));
}

/** Generate the teams in the leagues. */
void
start_generate_league_teams(void)
{
    gint i, j;    

    if(ligs->len == 0)
	main_exit_program(EXIT_NO_LEAGUES,
			  "start_generate_league_teams: no leagues found. there must be at least one league in the game.\n");

    for(i=0;i<ligs->len;i++)
	for(j=0;j<lig(i).teams->len;j++)
	    team_generate_players_stadium(&g_array_index(lig(i).teams, Team, j));
}

/** Load the names from the xml files for the cups
    and generate the teams. */
void
start_load_cup_teams(void)
{
    gint i;

    for(i=0;i<cps->len;i++)
	if(cp(i).type == CUP_TYPE_INTERNATIONAL)
	{
	    free_teams_array(&cp(i).teams, TRUE);
	    g_ptr_array_free(cp(i).user_teams, TRUE);
	    cp(i).user_teams = g_ptr_array_new();
	}

    for(i=0;i<cps->len;i++)
	if(cp(i).type == CUP_TYPE_INTERNATIONAL)
	{
	    printf("nam %s \n ", cp(i).name->str);

	    cup_load_choose_teams(&cp(i));
	    cup_load_choose_team_user(&cp(i));
	}
}

/** End a week round. */
void
end_week_round(void)
{
    gint i;
    gboolean new_week = TRUE;
    WeekFunc *end_func = end_week_round_funcs;

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
	if(query_start_end_season_end())
	{
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
	if(week_round == 1)
	{
	    for(j=0;j<lig(i).fixtures->len;j++)
		if(g_array_index(lig(i).fixtures, Fixture, j).week_number == week &&
		   g_array_index(lig(i).fixtures, Fixture, j).week_round_number == week_round &&
		   g_array_index(lig(i).fixtures, Fixture, j).attendance == -1)
		{
		    live_game_calculate_fixture(&g_array_index(lig(i).fixtures, Fixture, j));

		    done++;
		    fixture_result_to_buf(&g_array_index(lig(i).fixtures, Fixture, j), buf);
		    sprintf(buf2, "%s %s %s",
			    g_array_index(lig(i).fixtures, Fixture, j).teams[0]->name->str,
			    buf,
			    g_array_index(lig(i).fixtures, Fixture, j).teams[1]->name->str);
		    gui_show_progress((gfloat)done / num_matches, buf2);
		}
	}

    for(i=0;i<acps->len;i++)
	if(week_round > 1 || query_cup_is_prom(acp(i)->id))
	{
	    for(j=0;j<acp(i)->fixtures->len;j++)
	    {
		if(g_array_index(acp(i)->fixtures, Fixture, j).week_number == week &&
		   g_array_index(acp(i)->fixtures, Fixture, j).week_round_number == week_round &&
		   g_array_index(acp(i)->fixtures, Fixture, j).attendance == -1)
		{
		    live_game_calculate_fixture(&g_array_index(acp(i)->fixtures, Fixture, j));

		    done++;
		    fixture_result_to_buf(&g_array_index(acp(i)->fixtures, Fixture, j), buf);
		    sprintf(buf2, "%s %s %s",
			    g_array_index(acp(i)->fixtures, Fixture, j).teams[0]->name->str,
			    buf,
			    g_array_index(acp(i)->fixtures, Fixture, j).teams[1]->name->str);
		    gui_show_progress((gfloat)done / num_matches, buf2);
		}
	    }
	}

    gui_show_progress(-1, "");
}

/** Sort league and cup tables. */
void
end_week_round_sort_tables(void)
{
    gint i, j;

    for(i=0;i<ligs->len;i++)
	if(query_fixture_in_week_round(lig(i).id, week, week_round))
	    g_array_sort_with_data(lig(i).table.elements,
				   (GCompareDataFunc)table_element_compare_func,
				   GINT_TO_POINTER(lig(i).id));

    for(i=0;i<acps->len;i++)
	if(query_fixture_in_week_round(acp(i)->id, week, week_round) &&
	   g_array_index(acp(i)->fixtures, Fixture, acp(i)->fixtures->len - 1).round ==
	   cup_has_tables(acp(i)->id))
	    for(j=0;j<cup_get_last_tables(acp(i)->id)->len;j++)
		g_array_sort_with_data(
		    g_array_index(cup_get_last_tables(acp(i)->id), Table, j).elements,
		    (GCompareDataFunc)table_element_compare_func,
		    GINT_TO_POINTER(acp(i)->id));
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
	if(league_has_prom_games((&lig(i))) &&
	   query_league_prom_games_begin(&lig(i)))
	{
	    if(season == 1)
		lig(i).prom_rel.prom_games_cup.last_week = 
		    cup_get_last_week_from_first(&lig(i).prom_rel.prom_games_cup, week + 1);

	    fixture_write_cup_fixtures(&lig(i).prom_rel.prom_games_cup);
	    g_ptr_array_add(acps, &lig(i).prom_rel.prom_games_cup);
	}
	else if(week == (lig(i).teams->len - 1) * 2 && week_round == 1 &&
		team_is_user(g_array_index(lig(i).table.elements, TableElement, 0).team) != -1)
	    user_history_add(&usr(team_is_user(g_array_index(lig(i).table.elements, TableElement, 0).team)),
			     USER_HISTORY_CHAMPION, g_array_index(lig(i).table.elements, TableElement, 0).team_id,
			     lig(i).id, -1, "");
	    
    for(i=0;i<scps->len;i++)
    {
	if(query_cup_supercup_begins(&scp(i)))
	{
	    if(season == 1)
		scp(i).last_week = cup_get_last_week_from_first(&scp(i), week + 1);

	    fixture_write_cup_fixtures(&scp(i));
	    g_ptr_array_add(acps, &scp(i));
	}
    }
}

/** Start a new week round. */
void
start_week_round(void)
{
    WeekFunc *start_func = start_week_round_funcs;

    while(*start_func != NULL)
    {
	(*start_func)();
	start_func++;
    }

    if(/*d*/FALSE && !query_user_games_this_week_round() &&
       ((week_round == 1 && 
	 !query_user_games_in_week_round(week - 1, fixture_get_last_week_round(week - 1))) ||
	(week_round > 1 && 
	 !query_user_games_in_week_round(week, week_round - 1))))
	end_week_round();
    else
    {
        cur_user = 0;
        game_gui_show_main();

	/*d ??*/
/* 	if(week_round == 1) */
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

    /*d*/
    printf("succ %d \n ", usr(0).counters[COUNT_USER_SUCCESS]);
}

/** Age increase etc. of players.
    CPU teams get updated at the end of their matches
    (to avoid cup teams getting updated too often). */
void
start_week_update_user_teams(void)
{
    gint i;
    
    for(i=0;i<users->len;i++)
	team_update_user_team_weekly(usr(i).tm);
}

/** Do some things at the beginning of each new round for
    the user teams. */
void
start_week_round_update_user_teams(void)
{
    gint i;
    
    for(i=0;i<users->len;i++)
	team_update_user_team_week_roundly(usr(i).tm);
}

/** Deduce wages etc. */
void
start_week_update_user_finances(void)
{
    gint i;

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
	    if(g_array_index(lig(i).fixtures, Fixture, j).week_number > week)
		return FALSE;

    for(i=0;i<acps->len;i++)
	for(j=0;j<acp(i)->fixtures->len;j++)
	    if(g_array_index(acp(i)->fixtures, Fixture, j).week_number > week)
		return FALSE;

    return TRUE;
}

/** Manage promotions and relegations at the beginning of a new season. */
void
start_new_season_team_movements(void)
{
    gint i, j, k;
    GArray *team_movements = g_array_new(FALSE, FALSE, sizeof(TeamMove));

    for(i=0;i<ligs->len;i++)
	league_get_team_movements(&lig(i), team_movements);

    for(i=0;i<team_movements->len;i++)
	g_array_append_val(lig(g_array_index(team_movements, TeamMove, i).league_idx).teams,
			   g_array_index(team_movements, TeamMove, i).tm);

    g_array_free(team_movements, TRUE);
    
    for(i=0;i<ligs->len;i++)
    {
	for(j=0;j<lig(i).teams->len;j++)
	{
	    g_array_index(lig(i).teams, Team, j).clid = lig(i).id;
	    for(k=0;k<g_array_index(lig(i).teams, Team, j).players->len;k++)
		g_array_index(g_array_index(lig(i).teams, Team, j).players, Player, k).team =
		    &g_array_index(lig(i).teams, Team, j);
	}
    }
}
