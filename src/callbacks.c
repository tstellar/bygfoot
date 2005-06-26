#include "callbacks.h"
#include "callback_func.h"
#include "debug.h"
#include "free.h"
#include "game.h"
#include "game_gui.h"
#include "gui.h"
#include "load_save.h"
#include "main.h"
#include "option.h"
#include "player.h"
#include "team.h"
#include "transfer.h"
#include "treeview.h"
#include "treeview_helper.h"
#include "user.h"
#include "window.h"

gboolean
on_button_quit_clicked                 (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    on_menu_quit_activate(NULL, NULL);

    return TRUE;
}


void
on_menu_new_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    window_destroy(&window.main, FALSE);
    free_memory();

    main_init(-1, NULL);
    window_show_startup();
    stat0 = STATUS_TEAM_SELECTION;
}


void
on_menu_open_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    stat1 = STATUS_LOAD_GAME;
    window_show_file_sel();
}


void
on_menu_save_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    if(!opt_int("int_opt_save_will_overwrite") ||
       strlen(save_file->str) == 0)
	on_menu_save_as_activate(NULL, NULL);
    else
	load_save_save_game(save_file->str);
}


void
on_menu_save_as_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    stat1 = STATUS_SAVE_GAME;
    window_show_file_sel();
}

void
on_menu_quit_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    if(!getsav || !opt_int("int_opt_confirm_quit"))
	main_exit_program(EXIT_OK, NULL);
    else
    {
	stat1 = STATUS_QUERY_QUIT;
	window_show_yesno(_("The current game state is unsaved and will be lost. Continue?"));
    }
}




void
on_menu_help_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    window_show_help(2);
}


void
on_menu_contributors_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    window_show_help(1);
}


void
on_menu_about_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    window_show_help(0);
}


void
on_button_load_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
    on_menu_open_activate(NULL, NULL);
}


void
on_button_save_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
    on_menu_save_activate(NULL, NULL);
}


void
on_button_back_to_main_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
    if(stat0 != STATUS_LIVE_GAME_PAUSE)
	stat0 = STATUS_MAIN;
    gtk_notebook_set_current_page(GTK_NOTEBOOK(lookup_widget(window.main, "notebook_player")), 0);
    game_gui_show_main();

    gui_set_arrows();
}


void
on_button_transfers_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
    if(sett_int("int_opt_disable_transfers"))
	game_gui_print_message(_("Transfers are disabled in this country definition."));
    else if(week < transfer_get_deadline())
    {
	stat0 = STATUS_SHOW_TRANSFER_LIST;
	game_gui_print_message(_("Left click to make an offer. Right click to remove offer."));
	treeview_show_transfer_list(GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right")));
	gtk_notebook_set_current_page(GTK_NOTEBOOK(lookup_widget(window.main, "notebook_player")), 1);
	
	gui_set_arrows();
    }
    else
	game_gui_print_message(_("The transfer deadline is over."));
}


void
on_button_new_week_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
    if(transfer_offers_pending())
	game_gui_show_warning(_("You still have some transfer business to manage."));
    else if(query_user_no_turn())
    {
	stat1 = STATUS_QUERY_USER_NO_TURN;
	window_show_yesno(_("At least one user didn't take his turn this week. Continue?"));
    }
    else if(opt_int("int_opt_confirm_unfit") &&
	    query_user_teams_have_unfit())
    {
	stat1 = STATUS_QUERY_UNFIT;
	window_show_yesno(_("There are injured or banned players in one of the user teams. Continue?"));
    }
    else
    {
	load_save_autosave();
	callback_show_next_live_game();
    }
}


gboolean
on_player_list1_button_press_event     (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    gint idx = -1;

    if(event->button == 2)
    {
	on_menu_rearrange_team_activate(NULL, NULL);
	return TRUE;
    }
    else if(stat0 == STATUS_LIVE_GAME_PAUSE && event->button == 3)
    {
	on_menu_reset_players_activate(NULL, NULL);
	return TRUE;
    }

    if(treeview_helper_select_row(GTK_TREE_VIEW(widget), event))
	idx = treeview_helper_get_index(GTK_TREE_VIEW(widget), 0);

    if(idx < 0 || idx - 1 == selected_row[0])
    {
	selected_row[0] = -1;
	return FALSE;
    }

    callback_player_clicked(idx - 1, event);

    return FALSE;
}

void
on_button_browse_forward_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
    switch(stat0)
    {
	case STATUS_SHOW_FIXTURES:
	    callback_show_fixtures(SHOW_NEXT);
	    break;
	case STATUS_SHOW_FIXTURES_WEEK:
	    callback_show_fixtures_week(SHOW_NEXT);
	    break;
	case STATUS_BROWSE_TEAMS:
	    callback_show_team(SHOW_NEXT);
	    break;
	case STATUS_SHOW_PLAYER_INFO:
	    selected_row[0] = (selected_row[0] + 1) % current_user.tm->players->len;
	    on_menu_show_info_activate(NULL, NULL);
	    break;
	case STATUS_SHOW_SEASON_HISTORY:
	    callback_show_season_history(SHOW_NEXT);
	    break;
    }
}


void
on_button_browse_back_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
    switch(stat0)
    {
	case STATUS_SHOW_FIXTURES:
	    callback_show_fixtures(SHOW_PREVIOUS);
	    break;
	case STATUS_SHOW_FIXTURES_WEEK:
	    callback_show_fixtures_week(SHOW_PREVIOUS);
	    break;
	case STATUS_BROWSE_TEAMS:
	    callback_show_team(SHOW_PREVIOUS);
	    break;
	case STATUS_SHOW_PLAYER_INFO:
	    selected_row[0] = (selected_row[0] == 0) ? current_user.tm->players->len - 1 : selected_row[0] - 1;
	    on_menu_show_info_activate(NULL, NULL);
	    break;
	case STATUS_SHOW_SEASON_HISTORY:
	    callback_show_season_history(SHOW_PREVIOUS);
	    break;
    }
}

void
on_button_cl_back_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
    switch(stat0)
    {
	case STATUS_SHOW_FIXTURES:
	    callback_show_fixtures(SHOW_PREVIOUS_LEAGUE);
	    break;
	case STATUS_SHOW_TABLES:
	    callback_show_tables(SHOW_PREVIOUS_LEAGUE);
	    break;
	case STATUS_BROWSE_TEAMS:
	    callback_show_team(SHOW_PREVIOUS_LEAGUE);
	    break;
	case STATUS_SHOW_PLAYER_LIST:
	    callback_show_player_list(SHOW_PREVIOUS_LEAGUE);
	    break;
	case STATUS_SHOW_LEAGUE_STATS:
	    callback_show_league_stats(SHOW_PREVIOUS_LEAGUE);
	    break;
	case STATUS_SHOW_SEASON_HISTORY:
	    callback_show_season_history(SHOW_PREVIOUS_LEAGUE);
	    break;
    }
}


void
on_button_cl_forward_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
    switch(stat0)
    {
	case STATUS_SHOW_FIXTURES:
	    callback_show_fixtures(SHOW_NEXT_LEAGUE);
	    break;
	case STATUS_SHOW_TABLES:
	    callback_show_tables(SHOW_NEXT_LEAGUE);
	    break;
	case STATUS_BROWSE_TEAMS:
	    callback_show_team(SHOW_NEXT_LEAGUE);
	    break;
	case STATUS_SHOW_PLAYER_LIST:
	    callback_show_player_list(SHOW_NEXT_LEAGUE);
	    break;
	case STATUS_SHOW_LEAGUE_STATS:
	    callback_show_league_stats(SHOW_NEXT_LEAGUE);
	    break;
	case STATUS_SHOW_SEASON_HISTORY:
	    callback_show_season_history(SHOW_NEXT_LEAGUE);
	    break;
    }
}


void
on_menu_preferences_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    window_show_options();
}

void
on_menu_fixtures_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    stat0 = STATUS_SHOW_FIXTURES;
    callback_show_fixtures(SHOW_TEAM);

    gui_set_arrows();
}

void
on_menu_fixtures_week_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    stat0 = STATUS_SHOW_FIXTURES_WEEK;
    callback_show_fixtures_week(SHOW_CURRENT);

    gui_set_arrows();
}


void
on_menu_tables_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    stat0 = STATUS_SHOW_TABLES;
    callback_show_tables(SHOW_CURRENT);

    gui_set_arrows();
}


void
on_menu_league_stats_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    stat0 = STATUS_SHOW_LEAGUE_STATS;
    callback_show_league_stats(SHOW_CURRENT);

    gui_set_arrows();
}


void
on_menu_season_history_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    if(season == 1)
    {
	game_gui_print_message(_("This is your first season."));
	return;
    }

    stat0 = STATUS_SHOW_SEASON_HISTORY;
    callback_show_season_history(SHOW_CURRENT);

    gui_set_arrows();
}


void
on_menu_put_on_transfer_list_activate  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    if(sett_int("int_opt_disable_transfers"))
	game_gui_print_message(_("Transfers are disabled in this country definition."));
    else if(selected_row[0] == -1)
	game_gui_print_message(_("You haven't selected a player."));
    else if(query_transfer_player_is_on_list(player_of_idx_team(current_user.tm, selected_row[0])))
	game_gui_print_message(_("The player is already on the list."));
    else
    {
	setsav0;
	transfer_add_remove_user_player(player_of_idx_team(current_user.tm, selected_row[0]));
    }
}


void
on_menu_remove_from_transfer_list_activate (GtkMenuItem     *menuitem,
					    gpointer         user_data)
{
    if(selected_row[0] == -1)
	game_gui_print_message(_("You haven't selected a player."));
    else if(!query_transfer_player_is_on_list(player_of_idx_team(current_user.tm, selected_row[0])))
	game_gui_print_message(_("The player is not on the list."));
    else
    {
	setsav0;
	transfer_add_remove_user_player(player_of_idx_team(current_user.tm, selected_row[0]));
    }
}


void
on_menu_fire_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    if(selected_row[0] == -1)
	game_gui_print_message(_("You haven't selected a player."));
    else if(current_user.tm->players->len == 11)
	game_gui_show_warning(_("Your team can't have less than 11 players."));
    else
    {
	callback_fire_player(selected_row[0]);
	selected_row[0] = -1;
    }
}


void
on_menu_shoots_penalties_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    gchar buf[SMALL];

    if(selected_row[0] == -1)
	game_gui_print_message(_("You haven't selected a player."));
    else if(player_of_idx_team(current_user.tm, selected_row[0])->id ==
	    opt_user_int("int_opt_user_penalty_shooter"))
    {
	opt_user_set_int("int_opt_user_penalty_shooter", -1);
	game_gui_print_message(_("Penalty/free kick shooter deselected."));
	treeview_show_user_player_list();
	setsav0;
    }
    else
    {
	sprintf(buf, _("%s will shoot penalties and free kicks when he plays."),
		player_of_idx_team(current_user.tm, selected_row[0])->name->str);
	opt_user_set_int("int_opt_user_penalty_shooter",
			 player_of_idx_team(current_user.tm, selected_row[0])->id);
	game_gui_print_message(buf);
	treeview_show_user_player_list();
	setsav0;
    }
}

void
on_menu_my_league_results_activate     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    stat0 = STATUS_SHOW_LEAGUE_RESULTS;
    treeview_show_league_results(GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right")));

    gui_set_arrows();
}


void
on_menu_browse_teams_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    GtkWidget *treeview_right =
	lookup_widget(window.main, "treeview_right");

    treeview_show_team_list(GTK_TREE_VIEW(treeview_right), TRUE, TRUE);

    stat0 = STATUS_SHOW_TEAM_LIST;

    gui_set_arrows();
}


gboolean
on_treeview_right_button_press_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    gint idx;

    if(stat0 == STATUS_SHOW_FINANCES)
    {
	if(event->button == 1)
	    callback_get_loan();
	else if(event->button == 3)
	    callback_pay_loan();
	else if(event->button == 2)
	    on_menu_show_stadium_activate(NULL, NULL);
    }

    if(gtk_tree_selection_get_mode(
	   gtk_tree_view_get_selection(GTK_TREE_VIEW(widget))) ==
       GTK_SELECTION_NONE)
	return TRUE;

    if(treeview_helper_select_row(GTK_TREE_VIEW(widget), event))
	idx = treeview_helper_get_index(GTK_TREE_VIEW(widget), 0);
    else
	return TRUE;

    switch(stat0)
    {
	case STATUS_SHOW_TRANSFER_LIST:
	    if(trans(idx - 1).tm == current_user.tm ||
	       (trans(idx - 1).offers->len > 0 && transoff(idx - 1, 0).status == TRANSFER_OFFER_ACCEPTED) ||
	       event->button == 1)
		callback_transfer_list_clicked(event->button, idx - 1);
	    else if(event->button == 3)
	    {
		if(transfer_remove_offer(idx - 1, current_user.tm))
		    game_gui_print_message(_("Your offer has been removed."));
		else
		    game_gui_print_message(_("You haven't made an offer for the player."));
		return TRUE;
	    }
	    break;
	case STATUS_SHOW_TEAM_LIST:
	    callback_show_team(SHOW_CURRENT);
	    break;
	case STATUS_MAIN:
	    callback_show_next_opponent();
	    break;
	case STATUS_SHOW_PLAYER_LIST:
	    callback_show_player_team();
	    break;
    }

    gui_set_arrows();

    return TRUE;
}

void
on_menu_next_user_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    if(transfer_offers_pending())
    {
	game_gui_show_warning(_("You still have some transfer business to manage."));
	return;
    }

    cur_user = (cur_user + 1) % users->len;
    user_event_show_next();

    on_button_back_to_main_clicked(NULL, NULL);
}


void
on_menu_previous_user_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    if(transfer_offers_pending())
    {
	game_gui_show_warning(_("You still have some transfer business to manage."));
	return;
    }

    cur_user = (cur_user == 0) ? users->len - 1 : cur_user - 1;
    user_event_show_next();

    on_button_back_to_main_clicked(NULL, NULL);
}


void
on_menu_custom_structure_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    stat1 = STATUS_CUSTOM_STRUCTURE;
    window_show_digits(_("Enter a structure. The digits must sum up to 10."),
		       NULL, -1, _("Structure"), current_user.tm->structure);
}


gboolean
on_menu_team_button_press_event        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    game_gui_read_radio_items(widget);
    setsav0;

    return FALSE;
}

void
on_menu_manage_users_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    stat0 = stat1 = STATUS_USER_MANAGEMENT;
    window_create(WINDOW_USER_MANAGEMENT);
    treeview_show_users(GTK_TREE_VIEW(lookup_widget(window.user_management, "treeview_user_management_users")));
    treeview_show_team_list(GTK_TREE_VIEW(lookup_widget(window.user_management, "treeview_user_management_teams")),
			    FALSE, FALSE);
}


void
on_menu_user_show_last_match_activate  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    if(current_user.live_game.units->len == 0)
    {
	game_gui_show_warning(_("No match stored."));
	return;
    }

    stat1 = STATUS_SHOW_LAST_MATCH;
    stat3 = 0;
    callback_show_last_match(TRUE);
}


void
on_menu_user_show_last_stats_activate  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    if(current_user.live_game.units->len == 0)
    {
	game_gui_show_warning(_("No match stored."));
	return;
    }

    stat0 = STATUS_SHOW_LAST_MATCH_STATS;
    callback_show_last_match_stats();

    gui_set_arrows();
}


void
on_menu_user_show_coming_matches_activate
                                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    stat0 = STATUS_SHOW_PREVIEW;
    treeview_show_preview();

    gui_set_arrows();
}


gboolean
on_eventbox_style_button_press_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    gint new_style = -1;

    if(event->type != GDK_BUTTON_PRESS)
	return FALSE;

    if(event->button == 3)
	new_style = (current_user.tm->style != 2) ?
	    current_user.tm->style + 1 : -2;
    else if(event->button == 1)
	new_style = (current_user.tm->style != -2) ?
	    current_user.tm->style - 1 : 2;
    else
	return FALSE;

    team_change_attribute_with_message(current_user.tm, TEAM_ATTRIBUTE_STYLE, new_style);

    game_gui_write_meters();
    game_gui_write_radio_items();
    
    if(stat0 == STATUS_MAIN)
	treeview_show_next_opponent();

    setsav0;

    return FALSE;
}


gboolean
on_eventbox_boost_button_press_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    gint new_boost = -1; 

    if(event->type != GDK_BUTTON_PRESS)
	return FALSE;

    if(event->button == 3)
	new_boost = (current_user.tm->boost != 1) ?
	    current_user.tm->boost + 1 : -1;
    else if(event->button == 1)
	new_boost = (current_user.tm->boost != -1) ?
	    current_user.tm->boost - 1 : 1;
    else
	return FALSE;

    if(new_boost == 1 && sett_int("int_opt_disable_boost_on"))
    {
	new_boost = 0;
	game_gui_print_message(_("Boost ON is disabled in this country definition."));
    }
    else
	team_change_attribute_with_message(current_user.tm, TEAM_ATTRIBUTE_BOOST, new_boost);

    game_gui_write_meters();
    game_gui_write_radio_items();

    if(stat0 == STATUS_MAIN)
	treeview_show_next_opponent();

    setsav0;

    return FALSE;
}

void
on_menu_show_finances_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    if(sett_int("int_opt_disable_finances"))
    {
	game_gui_print_message(_("Finances are disabled in this country definition."));
	return;
    }

    stat0 = STATUS_SHOW_FINANCES;
    game_gui_print_message(_("Left-click: get loan; Right-click: pay back; Middle click: stadium window."));
    treeview_show_finances(GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right")),
			   &current_user);

    gui_set_arrows();
}


void
on_menu_show_stadium_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    if(sett_int("int_opt_disable_stadium"))
    {
	game_gui_print_message(_("Stadium management is disabled in this country definition."));
	return;
    }

    window_show_stadium();
}

gboolean
on_menu_check_button_press_event       (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    game_gui_read_check_items(widget);

    setsav0;
	
    return FALSE;
}

void
on_menu_offer_new_contract_activate    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    if(sett_int("int_opt_disable_contracts"))
    {
	game_gui_print_message(_("Contracts are disabled in this country definition."));
	return;
    }

    if(selected_row[0] == -1)
    {
	game_gui_print_message(_("You haven't selected a player."));
	return;
    }

    callback_offer_new_contract(selected_row[0]);

    setsav0;
}


void
on_menu_show_info_activate      (GtkMenuItem     *menuitem,
				 gpointer         user_data)
{
    if(selected_row[0] == -1)
    {
	game_gui_print_message(_("You haven't selected a player."));
	return;
    }    

    if(stat0 != STATUS_LIVE_GAME_PAUSE)
	stat0 = STATUS_SHOW_PLAYER_INFO;
    treeview_show_player_info(player_of_idx_team(current_user.tm, selected_row[0]));

    gui_set_arrows();
}


void
on_player_menu_put_on_transfer_list_activate
                                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    on_menu_put_on_transfer_list_activate(NULL, NULL);
}


void
on_player_menu_remove_from_transfer_list_activate
                                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    on_menu_remove_from_transfer_list_activate(NULL, NULL);
}


void
on_player_menu_offer_new_contract_activate
                                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    on_menu_offer_new_contract_activate(NULL, NULL);
}


void
on_player_menu_fire_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    on_menu_fire_activate(NULL, NULL);
}


void
on_player_menu_shoots_penalties_activate
                                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    on_menu_shoots_penalties_activate(NULL, NULL);
}

void
on_menu_browse_players_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    stat0 = STATUS_SHOW_PLAYER_LIST;
    callback_show_player_list(SHOW_CURRENT);

    gui_set_arrows();
}

void
on_player_menu_show_info_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    on_menu_show_info_activate(NULL, NULL);
}


void
on_menu_rearrange_team_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    team_rearrange(current_user.tm);
    treeview_show_user_player_list();
}


void
on_menu_load_last_save_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    if(load_save_load_game("last_save"))
    {
	cur_user = 0;
	on_button_back_to_main_clicked(NULL, NULL);
	setsav1;
    }
}

void
on_menu_user_show_history_activate     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    stat0 = STATUS_SHOW_USER_HISTORY;
    treeview_show_user_history();

    gui_set_arrows();
}

void
on_button_reset_players_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
    on_menu_reset_players_activate(NULL, NULL);
}


void
on_menu_reset_players_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    gint idx = -1;

    if(team_is_user(usr(stat2).live_game.fix->teams[0]) == -1 ||
       team_is_user(usr(stat2).live_game.fix->teams[1]) == -1)
	idx  = (team_is_user(usr(stat2).live_game.fix->teams[0]) == -1);
    else
    {
	if(team_is_user(usr(stat2).live_game.fix->teams[0]) == cur_user)
	    idx = 0;
	else
	    idx = 1;
    }
    
    game_reset_players(idx);
}

gboolean
on_button_quit_button_press_event      (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    if(event->button == 3 && counters[COUNT_SHOW_DEBUG] == 0)
    {
	counters[COUNT_SHOW_DEBUG] = 1;
	g_timeout_add(3000, (GSourceFunc)debug_reset_counter, NULL);
    }
    else if(event->button == 2 && counters[COUNT_SHOW_DEBUG] == 1)
    {
	window_create(WINDOW_DEBUG);
	counters[COUNT_SHOW_DEBUG] = 0;
    }

    return FALSE;
}

