#include "callbacks.h"
#include "callback_func.h"
#include "game_gui.h"
#include "main.h"
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

}


void
on_menu_save_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_save_as_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_start_editor_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_start_update_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

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
on_menu_team_editor_help_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_button_load_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button_save_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{

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

    treeview_select_row(GTK_TREE_VIEW(widget), event);
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
on_players1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_season_history_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_player_activate                (GtkMenuItem     *menuitem,
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

}


void
on_menu_remove_from_transfer_list_activate (GtkMenuItem     *menuitem,
					    gpointer         user_data)
{

}


void
on_menu_fire_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_shoots_penalties_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_all_out_defend_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_defend_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_balanced_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_attack_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_all_out_attack_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_scout_best_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_scout_good_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_scout_average_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_scout_bad_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_physio_best_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_physio_good_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_physio_average_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_physio_bad_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_my_league_results_activate     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_browse_teams_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    GtkWidget *treeview_right =
	lookup_widget(window.main, "treeview_right");

    treeview_show_team_list(GTK_TREE_VIEW(treeview_right), TRUE, TRUE);

    stat0 = STATUS_BROWSE_TEAMS;
}


gboolean
on_treeview_right_button_press_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    gint idx;

    treeview_select_row(GTK_TREE_VIEW(widget), event);
    idx = treeview_get_index(GTK_TREE_VIEW(widget), 0);

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
    }

    return FALSE;
}



void
on_menu_get_loan_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_pay_loan_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

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

}


void
on_menu_user_show_last_match_activate  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    stat0 = STATUS_SHOW_LAST_MATCH;
    callback_show_last_match();
}


void
on_menu_user_show_last_stats_activate  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
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

