#include "callbacks.h"
#include "callback_func.h"
#include "game_gui.h"
#include "load_save.h"
#include "main.h"
#include "option.h"
#include "player.h"
#include "team.h"
#include "transfer.h"
#include "treeview.h"
#include "user.h"
#include "window.h"

gboolean
on_button_quit_clicked                 (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    on_menu_quit_activate(NULL, NULL);

    return FALSE;
}


void
on_menu_new_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

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
    main_exit_program(EXIT_OK, NULL);
}


void
on_menu_about_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

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
    stat0 = STATUS_MAIN;
    game_gui_show_main();
}


void
on_button_transfers_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
    stat0 = STATUS_SHOW_TRANSFER_LIST;
    game_gui_print_message(_("Left click to make an offer. Right click to remove offer."));
    treeview_show_transfer_list(GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right")));
}


void
on_button_preview_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
    stat0 = STATUS_SHOW_PREVIEW;
    treeview_show_preview();
}


void
on_button_new_week_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
    callback_show_next_live_game();
}


void
on_button_help_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{

}


gboolean
on_player_list1_button_press_event     (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    gint idx = -1;

    if(treeview_select_row(GTK_TREE_VIEW(widget), event))
	idx = treeview_get_index(GTK_TREE_VIEW(widget), 0);

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
	case STATUS_BROWSE_TEAMS:
	    callback_show_team(SHOW_NEXT);
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
	case STATUS_BROWSE_TEAMS:
	    callback_show_team(SHOW_PREVIOUS);
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
}

void
on_menu_tables_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    stat0 = STATUS_SHOW_TABLES;
    callback_show_tables(SHOW_CURRENT);
}


void
on_players_activate                   (GtkMenuItem     *menuitem,
				       gpointer         user_data)
{

}


void
on_menu_season_history_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_show_info_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_put_on_transfer_list_activate  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    if(selected_row[0] == -1)
	game_gui_print_message(_("You haven't selected a player."));
    else if(query_transfer_player_is_on_list(player_of(current_user.tm, selected_row[0])))
	game_gui_print_message(_("The player is already on the list."));
    else
    {
	transfer_add_remove_user_player(player_of(current_user.tm, selected_row[0]));
	selected_row[0] = -1;
    }
}


void
on_menu_remove_from_transfer_list_activate (GtkMenuItem     *menuitem,
					    gpointer         user_data)
{
    if(selected_row[0] == -1)
	game_gui_print_message(_("You haven't selected a player."));
    else if(!query_transfer_player_is_on_list(player_of(current_user.tm, selected_row[0])))
	game_gui_print_message(_("The player is not on the list."));
    else
    {
	transfer_add_remove_user_player(player_of(current_user.tm, selected_row[0]));
	selected_row[0] = -1;
    }
}


void
on_menu_fire_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    if(selected_row[0] == -1)
	game_gui_print_message(_("You haven't selected a player."));
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
    else if(player_of(current_user.tm, selected_row[0])->id ==
	    opt_user_int("int_opt_user_penalty_shooter"))
    {
	opt_user_set_int("int_opt_user_penalty_shooter", -1);
	game_gui_print_message(_("Penalty/free kick shooter deselected."));
	treeview_show_user_player_list();
    }
    else
    {
	sprintf(buf, "%s will shoot penalties and free kicks when he plays.",
		player_of(current_user.tm, selected_row[0])->name->str);
	opt_user_set_int("int_opt_user_penalty_shooter",
			 player_of(current_user.tm, selected_row[0])->id);
	game_gui_print_message(buf);
	treeview_show_user_player_list();
    }

    selected_row[0] = -1;
}

void
on_menu_my_league_results_activate     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    stat0 = STATUS_SHOW_LEAGUE_RESULTS;
    treeview_show_league_results(GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right")));
}


void
on_menu_browse_teams_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    GtkWidget *treeview_right =
	lookup_widget(window.main, "treeview_right");

    treeview_show_team_list(GTK_TREE_VIEW(treeview_right), TRUE, TRUE);

    stat0 = STATUS_SHOW_TEAM_LIST;
}


gboolean
on_treeview_right_button_press_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    gint idx;

    if(gtk_tree_selection_get_mode(
	   gtk_tree_view_get_selection(GTK_TREE_VIEW(widget))) ==
       GTK_SELECTION_NONE)
	return TRUE;

    if(treeview_select_row(GTK_TREE_VIEW(widget), event))
	idx = treeview_get_index(GTK_TREE_VIEW(widget), 0);
    else
	return TRUE;

    switch(stat0)
    {
	case STATUS_SHOW_FINANCES:
	    if(event->button == 1)		
		callback_get_loan();
	    else if(event->button == 3)
		callback_pay_loan();
	    break;
	case STATUS_SHOW_TRANSFER_LIST:
	    if(event->button == 1 || trans(idx - 1).tm == current_user.tm)
		callback_transfer_list_clicked(idx - 1);
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
    }

    return TRUE;
}

void
on_menu_next_user_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    cur_user = (cur_user + 1) % users->len;

    user_event_show_next();

    game_gui_show_main();
}


void
on_menu_previous_user_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    cur_user = (cur_user == 0) ? users->len - 1 : cur_user - 1;

    user_event_show_next();

    game_gui_show_main();
}


void
on_menu_show_user_list_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

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

    return FALSE;
}

void
on_menu_manage_users_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    stat0 = STATUS_USER_MANAGEMENT;
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
	game_gui_show_warning("No match to show.");
	return;
    }

    stat0 = STATUS_SHOW_LAST_MATCH;
    callback_show_last_match();
}


void
on_menu_user_show_last_stats_activate  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    if(current_user.live_game.units->len == 0)
    {
	game_gui_show_warning("No match to show.");
	return;
    }

    stat0 = STATUS_SHOW_LAST_MATCH_STATS;
    treeview_show_game_stats(GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right")),
			     &current_user.live_game);
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

    team_change_attribute_with_message(current_user.tm, TEAM_ATTRIBUTE_BOOST, new_boost);

    game_gui_write_meters();
    game_gui_write_radio_items();

    if(stat0 == STATUS_MAIN)
	treeview_show_next_opponent();

    return FALSE;
}

void
on_menu_show_finances_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    stat0 = STATUS_SHOW_FINANCES;
    game_gui_print_message("Left-click on the list to get a loan. Right-click to pay back.");
    treeview_show_finances(GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right")),
			   &current_user);
}


void
on_menu_show_stadium_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    window_show_stadium();
}

gboolean
on_menu_check_button_press_event       (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    game_gui_read_check_items(widget);

    return FALSE;
}

void
on_menu_offer_new_contract_activate    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    if(selected_row[0] == -1)
    {
	game_gui_print_message(_("You haven't selected a player."));
	return;
    }

    callback_offer_new_contract(selected_row[0]);
    selected_row[0] = -1;
}


void
on_player_menu_show_info_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

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
    callback_show_player_list(SHOW_CURRENT);
    stat0 = STATUS_SHOW_PLAYER_LIST;
}
