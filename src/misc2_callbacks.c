#include "bygfoot.h"
#include "finance.h"
#include "game_gui.h"
#include "main.h"
#include "misc2_callbacks.h"
#include "misc2_interface.h"
#include "support.h"
#include "user.h"
#include "variables.h"
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
on_graph_window_delete_event           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

  return FALSE;
}


void
on_optionmenu_player_changed           (GtkOptionMenu   *optionmenu,
                                        gpointer         user_data)
{

}


void
on_optionmenu_team_changed             (GtkOptionMenu   *optionmenu,
                                        gpointer         user_data)
{

}


void
on_button_close_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{

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

    switch(stat0)
    {
	case STATUS_GET_LOAN:
	    finance_get_loan(values[0]);
	    break;
	case STATUS_PAY_LOAN:
	    finance_pay_loan(values[0]);
	    break;
    }

    game_gui_set_main_window_header();
}


void
on_button_digits_cancel_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
    window_destroy(&window.digits, TRUE);
}
