#include "callback_func.h"
#include "callbacks.h"
#include "cup.h"
#include "finance.h"
#include "fixture.h"
#include "game_gui.h"
#include "league.h"
#include "live_game.h"
#include "misc.h"
#include "option.h"
#include "player.h"
#include "start_end.h"
#include "team.h"
#include "transfer.h"
#include "treeview.h"
#include "user.h"
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
		   g_array_index(lig(i).fixtures, Fixture, j).attendance == -1 &&
		    option_int("int_opt_user_show_live_game",
			       usr(fixture_user_team_involved(&g_array_index(lig(i).fixtures, Fixture, j))).
			       options))
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
		   g_array_index(cp(i).fixtures, Fixture, j).attendance == -1 &&
		   option_int("int_opt_user_show_live_game",
			      usr(fixture_user_team_involved(&g_array_index(cp(i).fixtures, Fixture, j))).
			      options))
		{
		    live_game_calculate_fixture(&g_array_index(cp(i).fixtures, Fixture, j));
		    return;
		}
    }

    window_destroy(&window.live, TRUE);

    /* no more user games to show: end round. */
    end_week_round();
}

/** Handle a click on the player list.
    @param idx The player number.
    @param event The type of button click. */
void
callback_player_clicked(gint idx, GdkEventButton *event)
{
    gchar buf[SMALL];

    /* Only accept single-clicks right now. */
    if(event->type != GDK_BUTTON_PRESS)
	return;

    if(event->button == 1)
    {
	if(selected_row[0] == -1)
	{
	    selected_row[0] = idx;
	    return;
	}

	player_swap(current_user.tm, selected_row[0],
		    current_user.tm, idx);
	if(opt_user_int("int_opt_user_swap_adapts") == 1 &&
	   current_user.tm->structure !=
	   team_find_appropriate_structure(current_user.tm))
	{
	    team_change_structure(current_user.tm,
				  team_find_appropriate_structure(current_user.tm));
	    team_rearrange(current_user.tm);
	}
	game_gui_write_av_skills();

	selected_row[0] = -1;

	treeview_show_user_player_list(&current_user);
    }
    else if(event->button == 3)
    {
	if(stat0 == STATUS_SHOW_TRANSFER_LIST)
	{
	    selected_row[0] = -1;
	    
	    if(!query_transfer_player_is_on_list(player_of(current_user.tm, idx)))
	    {
		transfer_add_player(player_of(current_user.tm, idx),
				    (gint)rint(((gfloat)const_int("int_transfer_time_lower") +
						(gfloat)const_int("int_transfer_time_upper")) / 2));
		sprintf(buf, _("%s has been added to the transfer list for %d weeks."),
			player_of(current_user.tm, idx)->name->str, 
			(gint)rint(((gfloat)const_int("int_transfer_time_lower") +
				    (gfloat)const_int("int_transfer_time_upper")) / 2));
		game_gui_print_message(buf);
	    }
	    else
		transfer_remove_player_ptr(player_of(current_user.tm, idx));

	    treeview_show_transfer_list(GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right")));
	}
    }
}

/** Show the last match of the current user. */
void
callback_show_last_match(void)
{
    gint i;

    if(current_user.live_game.units == NULL)
    {
	game_gui_show_warning("No match to show.");
	return;
    }

    stat2 = cur_user;

    window_create(WINDOW_LIVE);

    treeview_show_game_stats(GTK_TREE_VIEW(lookup_widget(window.live, "treeview_stats")),
			     &current_user.live_game);
    live_game_set_match(&current_user.live_game);

    for(i=0;i<current_user.live_game.units->len;i++)
	game_gui_live_game_show_unit(&g_array_index(current_user.live_game.units, LiveGameUnit, i));
}

/** Show some fixtures.
    @param type An integer telling us which league/cup and which
    week and round to show. */
void
callback_show_fixtures(gint type)
{
    const Fixture *fix = fixture_get(type, stat1, stat2, stat3,
				     current_user.tm);

    treeview_show_fixtures(GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right")),
			   fix->clid, fix->week_number, fix->week_round_number);
    stat1 = fix->clid;
    stat2 = fix->week_number;
    stat3 = fix->week_round_number;
}

/** Show tables.
    @type Integer telling us whether to show the current user's
    tables or those of the previous/next league/cup. */
void
callback_show_tables(gint type)
{
    gint clid = -1;

    if(type == SHOW_CURRENT)
	clid = current_user.tm->clid;
    else if(type == SHOW_NEXT_LEAGUE)
    {
	clid = league_cup_get_next_clid(stat1);
	while(clid >= ID_CUP_START && cup_from_clid(clid)->tables->len == 0)
	    clid = league_cup_get_next_clid(clid);
    }
    else if(type == SHOW_PREVIOUS_LEAGUE)
    {
	clid = league_cup_get_previous_clid(stat1);
	while(clid >= ID_CUP_START && cup_from_clid(clid)->tables->len == 0)
	    clid = league_cup_get_previous_clid(clid);
    }

    stat1 = clid;

    treeview_show_table(GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right")), clid);
}

/** Open the digits window to get a loan. */
void
callback_get_loan(void)
{
    gchar buf[SMALL], buf2[SMALL];
    gint max_loan = 
	finance_team_drawing_credit_loan(current_user.tm, TRUE) + current_user.debt;

    if(max_loan <= 0)
    {
	game_gui_print_message(_("The bank doesn't grant you more money."));
	return;
    }

    stat0 = STATUS_GET_LOAN;
    misc_print_grouped_int(max_loan, buf2, FALSE);
    sprintf(buf, _("You can take out at most %s."), buf2);

    window_show_digits(buf, _("Loan"), max_loan, NULL, 0);
}

/** Open the digits window to pay back a loan. */
void
callback_pay_loan(void)
{
    gchar buf[SMALL], buf2[SMALL];
    gint max_payback = MIN(BUDGET(cur_user), -current_user.debt);

    if(current_user.debt == 0)
    {
	game_gui_print_message(_("You are not indebted."));
	return;
    }

    if(max_payback <= 0)
    {
	game_gui_print_message(_("You don't have enough money to pay back."));
	return;
    }

    stat0 = STATUS_PAY_LOAN;
    misc_print_grouped_int(max_payback, buf2, FALSE);
    sprintf(buf, _("You can pay back at most %s"), buf2);

    window_show_digits(buf, _("Payback"), max_payback, NULL, 0);
}

/** Handle a click on the transfer list. 
    @param idx The index of the selected player in the transfer list. */
void
callback_transfer_list_clicked(gint idx)
{
    gchar buf[SMALL];
    Transfer *tr = &trans(idx);

    if(tr->tm == current_user.tm)
    {
	transfer_remove_player(idx);
	on_button_transfers_clicked(NULL, NULL);
	return;
    }
    
    if(current_user.tm->players->len == const_int("int_team_max_players"))
    {
	game_gui_show_warning(_("Your roster is already full. You can't buy more players."));
	return;
    }

    sprintf(buf, _("You are making an offer for %s. Your scout's recommendations for value and wage are preset."),
	    player_of_id(tr->tm, tr->id)->name->str);
    stat1 = idx;

    window_show_digits(buf, _("Fee"), tr->fee[current_user.scout % 10],
		       _("Wage"), tr->wage[current_user.scout % 10]);
}
