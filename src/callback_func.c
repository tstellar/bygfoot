#include "callback_func.h"
#include "cup.h"
#include "fixture.h"
#include "game_gui.h"
#include "league.h"
#include "live_game.h"
#include "option.h"
#include "player.h"
#include "start_end.h"
#include "support.h"
#include "team.h"
#include "treeview.h"
#include "user.h"
#include "variables.h"
#include "window.h"

/** Show the users' live games. */
void
callback_show_next_live_game(void)
{
    gint i, j;

    stat0 = STATUS_SHOW_LIVE_GAME;

    if(week_round == 1)
    {
	for(i=0;i<ligs->len;i++)
	    for(j=0;j<lig(i).fixtures->len;j++)
		if(g_array_index(lig(i).fixtures, Fixture, j).week_number == week &&
		   g_array_index(lig(i).fixtures, Fixture, j).week_round_number == week_round &&
		   fixture_user_team_involved(&g_array_index(lig(i).fixtures, Fixture, j)) != -1 &&
		   g_array_index(lig(i).fixtures, Fixture, j).attendance == -1)
		{
		    live_game_calculate_fixture(&g_array_index(lig(i).fixtures, Fixture, j));
		    return;
		}
    }
    else
    {
	for(i=0;i<cps->len;i++)
	    for(j=0;j<cp(i).fixtures->len;j++)
		if(g_array_index(cp(i).fixtures, Fixture, j).week_number == week &&
		   g_array_index(cp(i).fixtures, Fixture, j).week_round_number == week_round &&
		   fixture_user_team_involved(&g_array_index(cp(i).fixtures, Fixture, j)) != -1 &&
		   g_array_index(cp(i).fixtures, Fixture, j).attendance == -1)
		{
		    live_game_calculate_fixture(&g_array_index(cp(i).fixtures, Fixture, j));
		    return;
		}
    }

    window_destroy(&window.live, TRUE);

    /* no more user games to show: end round. */
    end_week_round();
    stat0 = STATUS_MAIN;
}

/** Handle a click on the player list.
    @param idx The player number.
    @param event The type of button click. */
void
callback_player_clicked(gint idx, GdkEventButton *event)
{
    /** Only accept single-clicks right now. */
    if(event->type != GDK_BUTTON_PRESS)
	return;

    if(event->button == 1)
    {
	if(selected_row[0] == -1)
	{
	    selected_row[0] = idx;
	    return;
	}

	player_swap(usr(current_user).tm, selected_row[0],
		    usr(current_user).tm, idx);
	if(opt_user_int("int_opt_user_swap_adapts") == 1)
	{
	    team_change_structure(usr(current_user).tm, 
				  team_find_appropriate_structure(usr(current_user).tm));
	    team_rearrange(usr(current_user).tm);
	}
	game_gui_write_av_skills();

	selected_row[0] = -1;

	treeview_show_user_player_list(&usr(current_user), 1);
    }
}

/** Show the last match of the current user. */
void
callback_show_last_match(void)
{
    gint i;

    if(usr(current_user).live_game.units == NULL)
    {
	game_gui_show_warning("No match to show.");
	return;
    }

    stat2 = current_user;

    window_create(WINDOW_LIVE);

    treeview_show_game_stats(GTK_TREE_VIEW(lookup_widget(window.live, "treeview_stats")),
			     &usr(current_user).live_game);
    live_game_set_match(&usr(current_user).live_game);

    for(i=0;i<usr(current_user).live_game.units->len;i++)
	game_gui_live_game_show_unit(&g_array_index(usr(current_user).live_game.units, LiveGameUnit, i));
}
