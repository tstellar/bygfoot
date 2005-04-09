#include "cup.h"
#include "file.h"
#include "finance.h"
#include "fixture.h"
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
{end_week_round_autosave, end_week_round_results, end_week_round_sort_tables, 
 end_week_round_update_fixtures, NULL};

/** Array of functions called when a week round
    is started. */
WeekFunc start_week_round_funcs[] =
{start_week_round_update_user_teams, NULL};

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
    start_load_cup_teams();

    for(i=0;i<ligs->len;i++)
	fixture_write_league_fixtures(&lig(i));

    for(i=0;i<cps->len;i++)
	fixture_write_cup_fixtures(&cp(i));
}

/** Fill some global variables with default values at the
    beginning of a new game. */
void
start_write_variables(void)
{
    gint i;

    season = week = week_round = 1;
    cur_user = 0;
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

    for(i=0;i<cps->len;i++)
	if(query_fixture_in_week_round(cp(i).id, week, week_round))
	{
	    new_week = FALSE;
	    break;
	}

    if(new_week)
    {
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
    
    if(week_round == 1)
    {
	for(i=0;i<ligs->len;i++)
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
    else
    {
	for(i=0;i<cps->len;i++)
	    for(j=0;j<cp(i).fixtures->len;j++)
		if(g_array_index(cp(i).fixtures, Fixture, j).week_number == week &&
		   g_array_index(cp(i).fixtures, Fixture, j).week_round_number == week_round &&
		   g_array_index(cp(i).fixtures, Fixture, j).attendance == -1)
		{
		    live_game_calculate_fixture(&g_array_index(cp(i).fixtures, Fixture, j));

		    done++;
		    fixture_result_to_buf(&g_array_index(cp(i).fixtures, Fixture, j), buf);
		    sprintf(buf2, "%s %s %s",
			    g_array_index(cp(i).fixtures, Fixture, j).teams[0]->name->str,
			    buf,
			    g_array_index(cp(i).fixtures, Fixture, j).teams[1]->name->str);
		    gui_show_progress((gfloat)done / num_matches, buf2);
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

    for(i=0;i<cps->len;i++)
	if(cp(i).tables !=  NULL && cp(i).tables->len != 0 && 
	   query_fixture_in_week_round(cp(i).id, week, week_round) &&
	   g_array_index(cp(i).fixtures, Fixture, cp(i).fixtures->len - 1).round ==
	   g_array_index(cp(i).tables, Table, 0).round)
	    for(j=0;j<cp(i).tables->len;j++)
		g_array_sort_with_data(g_array_index(cp(i).tables, Table, j).elements,
				       (GCompareDataFunc)table_element_compare_func,
				       GINT_TO_POINTER(cp(i).id));
}

/** Update cup fixtures. */
void
end_week_round_update_fixtures(void)
{
    gint i;

    for(i=0;i<cps->len;i++)
	if(cp(i).next_fixture_update_week == week &&
	   cp(i).next_fixture_update_week_round == week_round)
	    fixture_update(&cp(i));
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

    if(!query_user_games_this_week_round())
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

/** Save the game if autosave is on. */
void
end_week_round_autosave(void)
{
    if(!opt_int("int_opt_autosave") ||
       !query_user_games_this_week_round())
	return;

    counters[COUNT_AUTOSAVE] = (counters[COUNT_AUTOSAVE] + 1) % opt_int("int_opt_autosave_interval");

    if(counters[COUNT_AUTOSAVE] == 0)
	load_save_autosave();
}
