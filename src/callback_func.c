/*
   callback_func.c

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

#include "callback_func.h"
#include "callbacks.h"
#include "cup.h"
#include "finance.h"
#include "fixture.h"
#include "game_gui.h"
#include "gui.h"
#include "league.h"
#include "live_game.h"
#include "maths.h"
#include "misc.h"
#include "option.h"
#include "player.h"
#include "start_end.h"
#include "team.h"
#include "transfer.h"
#include "treeview.h"
#include "treeview_helper.h"
#include "user.h"
#include "window.h"

#include "bet.h"

/** Show the users' live games. */
void
callback_show_next_live_game(void)
{
    gint i, j;

    for(i=0;i<users->len;i++)
	usr(i).counters[COUNT_USER_TOOK_TURN] = 0;

    for(i=0;i<ligs->len;i++)
	for(j=0;j<lig(i).fixtures->len;j++)
	    if(g_array_index(lig(i).fixtures, Fixture, j).week_number == week &&
	       g_array_index(lig(i).fixtures, Fixture, j).week_round_number == week_round &&
	       fixture_user_team_involved(&g_array_index(lig(i).fixtures, Fixture, j)) != -1 &&
	       g_array_index(lig(i).fixtures, Fixture, j).attendance == -1 &&
	       option_int("int_opt_user_show_live_game",
			  &usr(fixture_user_team_involved(&g_array_index(lig(i).fixtures, Fixture, j))).
			  options))
	    {
		live_game_calculate_fixture(&g_array_index(lig(i).fixtures, Fixture, j));
		return;
	    }

    for(i=0;i<acps->len;i++)
	for(j=0;j<acp(i)->fixtures->len;j++)
	    if(g_array_index(acp(i)->fixtures, Fixture, j).week_number == week &&
	       g_array_index(acp(i)->fixtures, Fixture, j).week_round_number == week_round &&
	       fixture_user_team_involved(&g_array_index(acp(i)->fixtures, Fixture, j)) != -1 &&
	       g_array_index(acp(i)->fixtures, Fixture, j).attendance == -1 &&
	       option_int("int_opt_user_show_live_game",
			  &usr(fixture_user_team_involved(&g_array_index(acp(i)->fixtures, Fixture, j))).
			  options))
	    {
		live_game_calculate_fixture(&g_array_index(acp(i)->fixtures, Fixture, j));
		return;
	    }

    window_destroy(&window.live, TRUE);

    /* no more user games to show: end round. */
    end_week_round();

    setsav0;
}

/** Handle a click on the player list.
    @param idx The player number.
    @param event The type of button click. */
void
callback_player_clicked(gint idx, GdkEventButton *event)
{
    /* Only accept single-clicks right now. */
    if(event->type != GDK_BUTTON_PRESS)
	return;

    /*d*/
    gint i;
    for(i=0;i<lig(1).teams->len;i++)
	printf("%d %s %d\n", i, g_array_index(lig(1).teams,
					      Team, i).name,
	       g_array_index(lig(1).teams,
			     Team, i).id);

    if(event->button == 1)
    {
	if(selected_row == -1)
	{
	    selected_row = idx;
	    return;
	}

	player_swap(current_user.tm, selected_row,
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

	selected_row = -1;

	treeview_show_user_player_list();
	if(stat0 == STATUS_MAIN)
	    treeview_show_next_opponent();
    }
    else if(event->button == 3)
    {
	if(stat0 == STATUS_SHOW_TRANSFER_LIST)
	{
	    selected_row = -1;
	    transfer_add_remove_user_player(player_of_idx_team(current_user.tm, idx));
	}
	else if(stat0 == STATUS_SHOW_YA)
	{
	    selected_row = idx;
	    on_menu_move_to_youth_academy_activate(NULL, NULL);
	}
	else
	{
	    selected_row = idx;
	    window_show_menu_player((GdkEvent*)event);
	}
    }

    setsav0;
}

/** Show the last match of the current user. 
    @param start Whether we start the replay from the beginning or continue it. */
void
callback_show_last_match(gboolean start, LiveGame *lg)
{
    gint i;

    stat4 = -1;

    if(start)
    {
	stat2 = cur_user;
	statp = lg;

	window_create(WINDOW_LIVE);

	gui_set_sensitive_lg_meters(FALSE);

	treeview_show_game_stats(GTK_TREE_VIEW(lookup_widget(window.live, "treeview_stats")), lg);
    }
    else
    {
	gtk_widget_set_sensitive(lookup_widget(window.live, "button_pause"), TRUE);
	gtk_widget_set_sensitive(lookup_widget(window.live, "button_resume"), FALSE);
    }

    for(i=stat3;i<lg->units->len;i++)
    {
	game_gui_live_game_show_unit(&g_array_index(lg->units, LiveGameUnit, i));

	if(stat4 == STATUS_SHOW_LAST_MATCH_PAUSE ||
	   stat4 == STATUS_SHOW_LAST_MATCH_ABORT)
	{
	    stat3 = i + 1;
	    break;
	}
    }

    if(stat4 == STATUS_SHOW_LAST_MATCH_PAUSE)
    {
	gtk_widget_set_sensitive(lookup_widget(window.live, "button_pause"), FALSE);
	gtk_widget_set_sensitive(lookup_widget(window.live, "button_resume"), TRUE);
    }
    else if(stat4 == STATUS_SHOW_LAST_MATCH_ABORT)
    {
	window_destroy(&window.live, TRUE);
	stat1 = stat2 = stat3 = stat4 = -1;
    }
    else
	stat3 = -1;
}

/** Show the last match stats of the current user. */
void
callback_show_last_match_stats(void)
{
    current_user.live_game.fix = 
	fixture_from_id(current_user.live_game.fix_id);
    
    treeview_show_game_stats(GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right")),
			     &current_user.live_game);
}

/** Show fixtures by week and round (as opposed to
    competition-wise).
    @param type Whether to show current, next or previous weeks. */
void
callback_show_fixtures_week(gint type)
{
    switch(type)
    {
	default:
	    g_warning("callback_show_fixtures_week: unknown type %d \n", type);
	    return;
	    break;
	case SHOW_CURRENT:
	    if(week == 1 && week_round == 1)
	    {
		stat1 = week;
		stat2 = week_round;
	    }
	    else if(week_round == 1)
	    {
		stat1 = week - 1;
		stat2 = fixture_get_last_week_round(week - 1);
	    }
	    else
	    {
		stat1 = week;
		stat2 = week_round - 1;
	    }
	    break;
	case SHOW_NEXT:
	    fixture_get_next_week(&stat1, &stat2);
	    break;
	case SHOW_PREVIOUS:
	    fixture_get_previous_week(&stat1, &stat2);
	    break;
    }

    treeview_show_fixtures_week(stat1, stat2);
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
	clid = team_get_table_clid(current_user.tm);
    else if(type == SHOW_NEXT_LEAGUE)
	clid = league_cup_get_next_clid(stat1, FALSE);
    else if(type == SHOW_PREVIOUS_LEAGUE)
	clid = league_cup_get_previous_clid(stat1, FALSE);
    else
    {
	g_warning("callback_show_tables: unknown type %d \n", type);
	return;
    }

    while((clid < ID_CUP_START && !league_from_clid(clid)->active) ||
	  (clid >= ID_CUP_START && cup_has_tables(clid) == -1))
    {
	if(type == SHOW_PREVIOUS_LEAGUE)
	    clid = league_cup_get_previous_clid(clid, FALSE);
	else
	    clid = league_cup_get_next_clid(clid, FALSE);
    }

    stat1 = clid;

    treeview_show_table(GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right")), 
			clid);
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

    stat1 = STATUS_GET_LOAN;
    misc_print_grouped_int(max_loan, buf2);
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

    stat1 = STATUS_PAY_LOAN;
    misc_print_grouped_int(max_payback, buf2);
    sprintf(buf, _("You can pay back at most %s"), buf2);

    window_show_digits(buf, _("Payback"), max_payback, NULL, 0);
}

/** Manage a click on a player of the current team on the
    transfer list.
    @param button The mouse button number.
    @param idx The index of the selected player in the transfer list. */
void
callback_transfer_list_user(gint button, gint idx)
{
    gchar buf[SMALL],
	buf2[SMALL], buf3[SMALL];

    if(button == 3)
    {
	transfer_remove_player(idx);
	on_button_transfers_clicked(NULL, NULL);
	setsav0;
    }
    else if(button == 1)
    {
	if(trans(idx).offers->len == 0)
	    game_gui_print_message(_("There are no offers for the player."));
	else if(trans(idx).offers->len > 0 &&
		transoff(idx, 0).status != TRANSFER_OFFER_ACCEPTED)
	    game_gui_print_message(_("There are some offers for the player which you rejected or will see next week."));
	else
	{
	    misc_print_grouped_int(transoff(idx, 0).fee, buf2);
	    misc_print_grouped_int(ABS(transoff(idx, 0).fee -
				       player_of_id_team(current_user.tm,
							 trans(idx).id)->value), buf3);
	    if(transoff(idx, 0).fee -
	       player_of_id_team(current_user.tm, trans(idx).id)->value > 0)
		strcat(buf3, _(" more"));
	    else
		strcat(buf3, _(" less"));
		
	    sprintf(buf, _("%s would like to buy %s. They offer %s for him, which is %s than the player's value. Do you accept?"), transoff(idx, 0).tm->name,
		    player_of_id_team(current_user.tm, trans(idx).id)->name,
		    buf2, buf3);
	    stat1 = STATUS_TRANSFER_OFFER_USER;
	    stat2 = idx;
	    window_show_transfer_dialog(buf);
	}
    }
}

/** Handle a click on a cpu player for which the offer 
    got accepted. */
void
callback_transfer_list_cpu(gint button, gint idx)
{
    gchar buf[SMALL], buf2[SMALL], buf3[SMALL];

    if(button == 2)
	return;

    if(button == 3)
    {
	g_array_remove_index(trans(idx).offers, 0);
	if(trans(idx).offers->len > 0)
	    transfer_offers_notify(&trans(idx), FALSE);

	game_gui_print_message(_("Your offer has been removed."));
	on_button_transfers_clicked(NULL, NULL);
	return;
    }
    
    if(current_user.tm->players->len == const_int("int_team_max_players"))
    {
	game_gui_show_warning(_("Your roster is already full. You can't buy more players."));
	return;
    }

    misc_print_grouped_int(transoff(idx, 0).fee, buf2);
    misc_print_grouped_int(transoff(idx, 0).wage, buf3);

    sprintf(buf, _("You offered a transfer fee of %s and a wage of %s for %s. The owners and the player are satisfied with your offer. Do you still want to buy the player?"), 
	    buf2, buf3, player_of_id_team(trans(idx).tm, trans(idx).id)->name);
    stat1 = STATUS_TRANSFER_OFFER_CPU;
    stat2 = idx;
    window_show_transfer_dialog(buf);
}

/** Handle a click on the transfer list. 
    @param button The mouse button number.
    @param idx The index of the selected player in the transfer list. */
void
callback_transfer_list_clicked(gint button, gint idx)
{
    gchar buf[SMALL];
    Transfer *tr = &trans(idx);
    gint old_fee, old_wage = -1;

    if(tr->tm == current_user.tm)
    {
	callback_transfer_list_user(button, idx);
	return;
    }
    else if(tr->offers->len > 0 &&
	    transoff(idx, 0).status == TRANSFER_OFFER_ACCEPTED)
    {
	if(transoff(idx, 0).tm == current_user.tm)
	{
	    if(team_is_user(tr->tm) != -1)
	    {
		game_gui_print_message(_("User %s didn't consider your offer yet."),
			user_from_team(tr->tm)->name);
	    }
	    else
		callback_transfer_list_cpu(button, idx);
	}
	else
	    game_gui_print_message(_("The player is locked (the team owners are considering an offer currently)."));

	return;
    }
    
    if(week >= transfer_get_deadline())
    {
	game_gui_print_message(_("The transfer deadline is over."));
	return;
    }
    
    stat1 = STATUS_SHOW_TRANSFER_LIST;
    stat2 = idx;
    
    if(tr->offers->len > 0)
	transfer_get_previous_offer(tr, current_user.tm, &old_fee, &old_wage);

    if(old_wage == -1)
    {
	sprintf(buf, _("You are making an offer for %s. Your scout's recommendations for fee and wage are preset."),
		player_of_id_team(tr->tm, tr->id)->name);
	
	window_show_digits(buf, _("Fee"), tr->fee[current_user.scout % 10],
			   _("Wage"), tr->wage[current_user.scout % 10]);
    }
    else
    {
	sprintf(buf, _("You are making an offer for %s again. Your previous values for fee and wage are preset."),
		player_of_id_team(tr->tm, tr->id)->name);
	
	window_show_digits(buf, _("Fee"), old_fee, _("Wage"), old_wage);
    }
}

/** Show the contract window for the player with the specified index. */
void
callback_offer_new_contract(gint idx)
{
    gint i;
    gchar buf[SMALL];
    Player *pl = player_of_idx_team(current_user.tm, idx);
    gfloat scout_dev = math_rnd(-const_float("float_transfer_scout_deviance_wage"),
				const_float("float_transfer_scout_deviance_wage")) *
	(current_user.scout % 10 + 1);
    GtkSpinButton *spinbuttons[4];

    if(pl->contract >= 2)
    {
	game_gui_show_warning(_("You can't offer a new contract if the old one is still above 2 years."));
	return;
    }
    else if(pl->offers == const_int("int_contract_max_offers"))
    {
	game_gui_show_warning(_("The player won't negotiate with you anymore."));
	return;
    }

    stat1 = player_assign_wage(pl);
    statp = (gpointer)pl;

    if(pl->age < pl->peak_age)
	stat1 = MAX(stat1, pl->wage);
    else
	stat1 = MIN(stat1, pl->wage);

    window_create(WINDOW_CONTRACT);

    sprintf(buf, _("You are negotiating with %s about a new contract. Pay attention to what you're doing; if you don't come to terms with him within %d offers, he's going to leave your team after his current contract expires (unless you sell him). You may only abort BEFORE making the first offer.\nYour scout's recommendations are preset:"), 
	    pl->name,
	    const_int("int_contract_max_offers"));
    gtk_label_set_text(GTK_LABEL(lookup_widget(window.contract, "label_contract")), buf);

    for(i=0;i<4;i++)
    {
	sprintf(buf, "spinbutton_contract%d", i + 1);
	spinbuttons[i] = GTK_SPIN_BUTTON(lookup_widget(window.contract, buf));

	gtk_spin_button_set_value(spinbuttons[i],
				  rint((gfloat)stat1 * 
				       (1 + (i * const_float("float_contract_scale_factor") *
					     powf(-1, (pl->age > pl->peak_age)))) *
				       (1 + scout_dev)));
    }

    setsav0;
}

/** Show the player list of a team in the browse-teams mode. */
void
callback_show_team(gint type)
{
    GtkTreeView *treeview_right = GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right"));
    const Team *tm;
    const GArray *teams = NULL;
    const GPtrArray *teamsp = NULL;
    gint len = -1;

    if(type == SHOW_CURRENT)
    {
	tm = (const Team*)treeview_helper_get_pointer(treeview_right, 2);
	stat1 = team_get_index(tm);
	stat2 = tm->clid;
    }
    else
    {
	if(type == SHOW_NEXT_LEAGUE)
	{
	    stat2 = league_cup_get_next_clid(stat2, TRUE);
	    while(stat2 >= ID_CUP_START && cup_from_clid(stat2)->teams->len == 0)
		stat2 = league_cup_get_next_clid(stat2, TRUE);
	}
	else if(type == SHOW_PREVIOUS_LEAGUE)
	{
	    stat2 = league_cup_get_previous_clid(stat2, TRUE);
	    while(stat2 >= ID_CUP_START && cup_from_clid(stat2)->teams->len == 0)
		stat2 = league_cup_get_previous_clid(stat2, TRUE);
	}

	if(stat2 < ID_CUP_START)
	{
	    teams = (GArray*)league_cup_get_teams(stat2);
	    len = teams->len;
	}
	else
	{
	    teamsp = (GPtrArray*)league_cup_get_teams(stat2);
	    len = teamsp->len;
	}

	if(type == SHOW_NEXT)
	    stat1 = (stat1 == len - 1) ? 0 : stat1 + 1;
	else if(type == SHOW_PREVIOUS)
	    stat1 = (stat1 == 0) ? len - 1 : stat1 - 1;
	else
	    stat1 = 0;

	if(stat2 < ID_CUP_START)
	    tm = &g_array_index(teams, Team, stat1);
	else
	    tm = (Team*)g_ptr_array_index(teamsp, stat1);	
    }

    stat0 = STATUS_BROWSE_TEAMS;

    if(tm != current_user.tm)
	treeview_show_player_list_team(treeview_right, tm, current_user.scout % 10);
    else
	callback_show_team((type == SHOW_PREVIOUS) ? SHOW_PREVIOUS : SHOW_NEXT);
}

/** Show a sortable list of all players in a league or cup. */
void
callback_show_player_list(gint type)
{
    stat0 = STATUS_SHOW_PLAYER_LIST;

    switch(type)
    {
	default:
	    g_warning("callback_show_player_list: unknown type %d \n", type);
	    return;
	    break;
	case SHOW_CURRENT:
	    stat1 = current_user.tm->clid;
	    break;
	case SHOW_NEXT_LEAGUE:
	    stat1 = league_cup_get_next_clid(stat1, TRUE);
	    while(stat1 >= ID_CUP_START && cup_from_clid(stat1)->teams->len == 0)
		stat1 = league_cup_get_next_clid(stat1, TRUE);
	    break;
	case SHOW_PREVIOUS_LEAGUE:
	    stat1 = league_cup_get_previous_clid(stat1, TRUE);
	    while(stat1 >= ID_CUP_START && cup_from_clid(stat1)->teams->len == 0)
		stat1 = league_cup_get_previous_clid(stat1, TRUE);
	    break;
    }

    treeview_show_all_players(stat1);
}

/** Fire a player. */
void
callback_fire_player(gint idx)
{
    gchar buf[SMALL], buf2[SMALL];
    Player *pl = player_of_idx_team(current_user.tm, idx);

    stat1 = STATUS_FIRE_PLAYER;
    stat2 = idx;
    stat3 = (gint)rint(pl->wage * const_float("float_player_fire_wage_factor") * pl->contract);

    misc_print_grouped_int(stat3, buf2);

    sprintf(buf, _("You want to fire %s. Since his contract expires in %.1f years, he demands a compensation of %s. Do you accept?"), pl->name, pl->contract, buf2);

    window_show_yesno(buf);
}

/** Show a page with the information in the league stats
    structure. */
void
callback_show_league_stats(gint type)
{
    switch(type)
    {
	default:
	    g_warning("callback_show_league_stats: unknown type %d \n", type);
	    return;
	    break;
	case SHOW_CURRENT:
	    stat1 = current_user.tm->clid;
	    while(stat1 >= ID_CUP_START ||
		  !league_from_clid(stat1)->active)
		stat1 = league_cup_get_next_clid(stat1, FALSE);
	    break;
	case SHOW_NEXT_LEAGUE:
	    stat1 = league_cup_get_next_clid(stat1, FALSE);
	    while(stat1 >= ID_CUP_START)
		stat1 = league_cup_get_next_clid(stat1, FALSE);
	    break;
	case SHOW_PREVIOUS_LEAGUE:
	    stat1 = league_cup_get_previous_clid(stat1, FALSE);
	    while(stat1 >= ID_CUP_START)
		stat1 = league_cup_get_previous_clid(stat1, FALSE);
	    break;
    }

    treeview_show_league_stats(stat1);
}

/** Show the appropriate season history page in the right treeview. */
void
callback_show_season_history(gint type)
{
    switch(type)
    {
	default:
	    g_warning("callback_show_season_history: unknown type %d \n", type);
	    return;
	    break;
	case SHOW_CURRENT:
	    stat1 = -1;
	    stat2 = season - 2;
	    break;
	case SHOW_NEXT_LEAGUE:
	    if(stat1 == -1)
		stat1 = 0;
	    else if(stat1 == ligs->len - 1)
		stat1 = -1;
	    else
		stat1++;
	    break;
	case SHOW_PREVIOUS_LEAGUE:
	    if(stat1 == -1)
		stat1 = ligs->len - 1;
	    else if(stat1 == 0)
		stat1 = -1;
	    else
		stat1--;
	    break;
	case SHOW_NEXT:
	    stat2 = (stat2 + 1) % (season - 1);
	    break;
	case SHOW_PREVIOUS:
	    stat2 = (stat2 == 0) ? season - 2 : stat2 - 1;
	    break;
    }
    
    treeview_show_season_history(stat1, stat2);
}

/** Show the player list of the next opponent. */
void
callback_show_next_opponent(void)
{
    const Fixture *fix = team_get_fixture(current_user.tm, FALSE);
    const Team *opp = (fix == NULL) ? NULL :
	fix->teams[fix->teams[0] == current_user.tm];
    GtkTreeView *treeview_right = 
	GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right"));

    if(opp == NULL)
	return;

    stat0 = STATUS_BROWSE_TEAMS;
    stat1 = team_get_index(opp);
    stat2 = opp->clid;

    treeview_show_player_list_team(treeview_right, opp, current_user.scout % 10);
}

/** Show the player list after the user clicked on a player in
    the browse players mode. */
void
callback_show_player_team(void)
{
    GtkTreeView *treeview_right = 
	GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right"));
    const Player *pl = 
	(const Player*)treeview_helper_get_pointer(treeview_right, 2);
    
    stat0 = STATUS_BROWSE_TEAMS;
    stat1 = team_get_index(pl->team);
    stat2 = pl->team->clid;

    treeview_show_player_list_team(treeview_right, pl->team, current_user.scout % 10);    
}

/** Show the youth players of the current user. */
void
callback_show_youth_academy(void)
{
    gint i;
    PlayerListAttribute attributes;

    for(i=0;i<PLAYER_LIST_ATTRIBUTE_END;i++)
	attributes.on_off[i] = 0;

    attributes.on_off[PLAYER_LIST_ATTRIBUTE_NAME] =
	attributes.on_off[PLAYER_LIST_ATTRIBUTE_POS] =
	attributes.on_off[PLAYER_LIST_ATTRIBUTE_SKILL] =
	attributes.on_off[PLAYER_LIST_ATTRIBUTE_FITNESS] =
	attributes.on_off[PLAYER_LIST_ATTRIBUTE_STATUS] =
	attributes.on_off[PLAYER_LIST_ATTRIBUTE_AGE] =
	attributes.on_off[PLAYER_LIST_ATTRIBUTE_ETAL] = 1;

    if(stat0 != STATUS_SHOW_YA)
	game_gui_print_message(_("Right click to move players to and from the youth academy; left click for context menu."));

    treeview_show_player_list(GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right")),
			      player_get_pointers_from_array(current_user.youth_academy.players),
			      attributes, FALSE);
}
