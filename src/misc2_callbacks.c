#include "finance.h"
#include "game_gui.h"
#include "main.h"
#include "misc2_callbacks.h"
#include "misc2_callback_func.h"
#include "misc2_interface.h"
#include "player.h"
#include "support.h"
#include "transfer.h"
#include "treeview.h"
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
    user_change_team(&current_user, (Team*)statp);
    stat0 = STATUS_MAIN;
    game_gui_show_main();

    window_destroy(&window.job_offer, TRUE);
}


void
on_button_offer_cancel_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
    if(stat0 != STATUS_JOB_OFFER_SUCCESS)
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

    if(stat0 == STATUS_SHOW_EVENT)
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
	    break;
    }

    if(destroy_window)
	window_destroy(&window.digits, TRUE);

    game_gui_set_main_window_header();
}


void
on_button_digits_cancel_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
    window_destroy(&window.digits, TRUE);
}

gboolean
on_window_yesno_delete_event           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    on_button_yesno_no_clicked(NULL, NULL);

    return FALSE;
}


void
on_checkbutton_yesno_toggled           (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_button_yesno_yes_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
    switch(stat0)
    {
	default:
	    g_warning("on_button_yesno_yes_clicked: unknown status %d\n", stat0);
	    break;
	case STATUS_TRANSFER_OFFER:
	    misc2_callback_transfer_user_player();
	    break;
	case STATUS_FIRE_PLAYER:
	    player_remove_from_team(current_user.tm, stat1);
	    current_user.money -= stat2;
	    current_user.money_out[1][MON_OUT_COMPENSATIONS] -= stat2;
	    treeview_show_user_player_list();
	    break;
	case STATUS_USER_MANAGEMENT:
	    user_remove(stat1, TRUE);
	    treeview_show_users(GTK_TREE_VIEW(lookup_widget(window.user_management,
							    "treeview_user_management_users")));
	    treeview_show_team_list(GTK_TREE_VIEW(lookup_widget(window.user_management, 
								"treeview_user_management_teams")),
				    FALSE, FALSE);
	    break;
    }
    
    window_destroy(&window.yesno, TRUE);
}


void
on_button_yesno_no_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
    window_destroy(&window.yesno, TRUE);
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
}


gboolean
on_treeview_user_management_users_button_press_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    gchar buf[SMALL];
    gint idx = -1;

    if(!treeview_select_row(GTK_TREE_VIEW(widget), event))
	return TRUE;
    
    idx = treeview_get_index(GTK_TREE_VIEW(widget), 0) - 1;

    if(users->len == 1)
    {
	game_gui_show_warning("You can't play Bygfoot with 0 users!");
	return TRUE;
    }

    stat1 = idx;
    sprintf(buf, "Remove user %s from the game?", usr(idx).name->str);
    window_show_yesno(buf, FALSE);
    
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
