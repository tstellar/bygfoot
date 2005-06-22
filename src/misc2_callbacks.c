#include "callbacks.h"
#include "callback_func.h"
#include "debug.h"
#include "finance.h"
#include "game_gui.h"
#include "load_save.h"
#include "main.h"
#include "misc2_callbacks.h"
#include "misc2_callback_func.h"
#include "misc2_interface.h"
#include "player.h"
#include "support.h"
#include "transfer.h"
#include "treeview.h"
#include "treeview_helper.h"
#include "user.h"
#include "window.h"

gboolean
on_button_cancel_clicked               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

  return FALSE;
}


void
on_button_ok_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button_offer_ok_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
    if(stat2 == STATUS_JOB_OFFER_SUCCESS)
	user_history_add(&current_user, USER_HISTORY_JOB_OFFER_ACCEPTED, 
			 current_user.tm->id, ((Team*)statp)->id, ((Team*)statp)->clid, "");

    user_change_team(&current_user, (Team*)statp);

    stat0 = STATUS_MAIN;
    game_gui_show_main();

    window_destroy(&window.job_offer, TRUE);

    setsav0;
}


void
on_button_offer_cancel_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
    if(stat2 != STATUS_JOB_OFFER_SUCCESS)
    {
	if(users->len == 1)
	    main_exit_program(EXIT_USER_FIRED, NULL);
	else
	{
	    user_remove(cur_user, TRUE);
	    cur_user = 0;	    
	}
    }

    window_destroy(&window.job_offer, TRUE);
    stat0 = STATUS_MAIN;
    game_gui_show_main();
}


gboolean
on_button_warning_clicked              (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    window_destroy(&window.warning, FALSE);

    if(stat4 == STATUS_SHOW_EVENT)
	user_event_show_next();

    return FALSE;
}


void
on_button_digits_ok_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkSpinButton *spinbutton1 = GTK_SPIN_BUTTON(lookup_widget(window.digits, "spinbutton1")),
	*spinbutton2 = GTK_SPIN_BUTTON(lookup_widget(window.digits, "spinbutton2"));
    gint values[2] =
	{gtk_spin_button_get_value_as_int(spinbutton1),
	 gtk_spin_button_get_value_as_int(spinbutton2)};
    gboolean destroy_window = TRUE;

    switch(stat1)
    {
	case STATUS_GET_LOAN:
	    finance_get_loan(values[0]);
	    break;
	case STATUS_PAY_LOAN:
	    finance_pay_loan(values[0]);
	    break;
	case STATUS_SHOW_TRANSFER_LIST:
	    if(transfer_add_offer(stat2, current_user.tm, values[0], values[1]))
		game_gui_print_message(_("Your offer has been updated."));
	    else
		game_gui_print_message(_("Your offer will be considered next week."));
	    break;
	case STATUS_CUSTOM_STRUCTURE:
	    destroy_window = misc2_callback_change_structure(values[1]);
	    if(destroy_window && stat0 == STATUS_LIVE_GAME_PAUSE)
		gtk_widget_set_sensitive(window.main, TRUE);
	    break;
    }

    if(destroy_window)
	window_destroy(&window.digits, TRUE);

    game_gui_set_main_window_header();

    setsav0;
}


void
on_button_digits_cancel_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
    window_destroy(&window.digits, TRUE);
    if(stat0 == STATUS_LIVE_GAME_PAUSE)
	gtk_widget_set_sensitive(window.main, TRUE);
}

gboolean
on_window_yesno_delete_event           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    on_button_yesno_no_clicked(NULL, NULL);

    if(stat4 == STATUS_SHOW_EVENT)
	user_event_show_next();

    return FALSE;
}

void
on_button_yesno_yes_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{    
    window_destroy(&window.yesno, TRUE);
    setsav0;

    switch(stat1)
    {
	default:
	    g_warning("on_button_yesno_yes_clicked: unknown status %d\n", stat1);
	    break;
	case STATUS_FIRE_PLAYER:
	    player_remove_from_team(current_user.tm, stat2);
	    current_user.money -= stat3;
	    current_user.money_out[1][MON_OUT_COMPENSATIONS] -= stat3;
	    treeview_show_user_player_list();
	    game_gui_set_main_window_header();
	    break;
	case STATUS_USER_MANAGEMENT:
	    user_remove(stat2, TRUE);
	    treeview_show_users(GTK_TREE_VIEW(lookup_widget(window.user_management,
							    "treeview_user_management_users")));
	    treeview_show_team_list(GTK_TREE_VIEW(lookup_widget(window.user_management, 
								"treeview_user_management_teams")),
				    FALSE, FALSE);
	    break;
	case STATUS_QUERY_UNFIT:
	    load_save_autosave();
	    callback_show_next_live_game();
	    break;
	case STATUS_QUERY_QUIT:
	    main_exit_program(EXIT_OK, NULL);
	    break;
	case STATUS_QUERY_USER_NO_TURN:
	    load_save_autosave();
	    callback_show_next_live_game();
	    break;
    }

    if(stat4 == STATUS_SHOW_EVENT)
	user_event_show_next();
}


void
on_button_yesno_no_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
    window_destroy(&window.yesno, TRUE);

    if(stat4 == STATUS_SHOW_EVENT)
	user_event_show_next();
}


void
on_button_contract_offer_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
    misc2_callback_contract_offer();
}

gboolean
on_window_contract_delete_event        (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

    return TRUE;
}


void
on_entry_user_management_activate      (GtkEntry        *entry,
                                        gpointer         user_data)
{
    on_button_user_management_add_clicked(NULL, NULL);
}


void
on_button_user_management_add_clicked  (GtkButton       *button,
                                        gpointer         user_data)
{
    misc2_callback_add_user();
}


void
on_button_user_management_close_clicked
                                        (GtkButton       *button,
                                        gpointer         user_data)
{
    window_destroy(&window.user_management, FALSE);
    on_button_back_to_main_clicked(NULL, NULL);
}


gboolean
on_treeview_user_management_users_button_press_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    gchar buf[SMALL];
    gint idx = -1;

    if(!treeview_helper_select_row(GTK_TREE_VIEW(widget), event))
	return TRUE;
    
    idx = treeview_helper_get_index(GTK_TREE_VIEW(widget), 0) - 1;

    if(users->len == 1)
    {
	game_gui_show_warning(_("You can't play Bygfoot without users!"));
	return TRUE;
    }

    stat2 = idx;
    sprintf(buf, _("Remove user %s from the game?"), usr(idx).name->str);
    window_show_yesno(buf);
    
    return FALSE;
}

void
on_treeview_user_management_teams_row_activated
                                        (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data)
{
    on_button_user_management_add_clicked(NULL, NULL);
}

gboolean
on_window_debug_delete_event           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    on_button_debug_close_activate(NULL, NULL);

    return FALSE;
}


void
on_button_debug_apply_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkEntry *entry_debug =
	GTK_ENTRY(lookup_widget(window.wdebug, "entry_debug"));
    const gchar *entry_text = gtk_entry_get_text(entry_debug);
    gchar buf[SMALL];
    gint value = -1;

    sscanf(entry_text, "%[^-0-9]%d", buf, &value);
    debug_action(buf, value);

    gtk_entry_set_text(entry_debug, "");
}


void
on_button_debug_close_activate         (GtkButton       *button,
                                        gpointer         user_data)
{
    window_destroy(&window.wdebug, FALSE);
}


void
on_entry_debug_activate                (GtkEntry        *entry,
                                        gpointer         user_data)
{
    on_button_debug_apply_clicked(NULL, NULL);
}


gboolean
on_window_job_offer_delete_event       (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    on_button_offer_cancel_clicked(NULL, NULL);
    return FALSE;
}


gboolean
on_window_digits_delete_event          (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    on_button_digits_cancel_clicked(NULL, NULL);

    return FALSE;
}


gboolean
on_window_user_management_delete_event (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    on_button_user_management_close_clicked(NULL, NULL);
    return FALSE;
}


gboolean
on_window_help_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    on_button_help_close_clicked(NULL, NULL);

    return FALSE;
}


void
on_button_help_close_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
    window_destroy(&window.help, FALSE);
}

void
on_spinbutton1_activate                (GtkEntry        *entry,
                                        gpointer         user_data)
{
    gtk_spin_button_update(GTK_SPIN_BUTTON(lookup_widget(window.digits, "spinbutton1")));
    on_button_digits_ok_clicked(NULL, NULL);
}


void
on_spinbutton2_activate                (GtkEntry        *entry,
                                        gpointer         user_data)
{
    gtk_spin_button_update(GTK_SPIN_BUTTON(lookup_widget(window.digits, "spinbutton2")));
    on_button_digits_ok_clicked(NULL, NULL);
}


gboolean
on_window_transfer_dialog_delete_event (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    on_button_transfer_later_clicked(NULL, NULL);
    return FALSE;
}


void
on_button_transfer_yes_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
    if(stat1 == STATUS_TRANSFER_OFFER_USER)
	misc2_callback_transfer_user_player();
    else if(STATUS_TRANSFER_OFFER_CPU)
	misc2_callback_transfer_cpu_player();

    window_destroy(&window.transfer_dialog, FALSE);
}


void
on_button_transfer_no_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
    gint len = 1;

    transoff(stat2, 0).status = TRANSFER_OFFER_REJECTED;
    if(transoff(stat2, 0).tm == current_user.tm)
    {
	g_array_remove_index(trans(stat2).offers, 0);
	len = 0;
    }

    if(trans(stat2).offers->len > len)
	transfer_offers_notify(&trans(stat2), FALSE);

    treeview_show_user_player_list();
    game_gui_set_main_window_header();
    on_button_transfers_clicked(NULL, NULL);

    window_destroy(&window.transfer_dialog, FALSE);
}


void
on_button_transfer_later_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
    window_destroy(&window.transfer_dialog, FALSE);
}

