#include <string.h>

#include "callback_func.h"
#include "free.h"
#include "game_gui.h"
#include "live_game.h"
#include "misc_callback_func.h"
#include "misc_callbacks.h"
#include "option.h"
#include "treeview.h"
#include "user.h"
#include "variables.h"
#include "window.h"

void
on_button_add_player_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
    misc_callback_add_player();
}


void
on_entry_player_name_activate          (GtkEntry        *entry,
                                        gpointer         user_data)
{
    on_button_add_player_clicked(NULL, NULL);
}


void
on_team_selection_ok_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
    misc_callback_start_game();
}

void
on_team_selection_cancel_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
    free_memory();
    gtk_main_quit();
}


void
on_team_selection_tv_row_activated     (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data)
{
    if(GTK_WIDGET_IS_SENSITIVE(lookup_widget(window.startup, "team_selection_ok")))
	misc_callback_start_game();
}


void
on_radiobutton_country_toggled         (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_button_select_country_file_clicked  (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_checkbutton_load_randomly_toggled   (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_checkbutton_only_names_toggled      (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_button_start_editor_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_team_selection_load_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{

}


gboolean
on_fsel_window_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

  return FALSE;
}


void
on_button_fsel_ok_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button_fsel_cancel_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{

}


gboolean
on_button_font_sel_cancel_clicked      (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

  return FALSE;
}


void
on_button_font_sel_ok_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button_font_sel_apply_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{

}


gboolean
on_live_window_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    if(GTK_WIDGET_IS_SENSITIVE(lookup_widget(widget, "button_live_close")))
    {
	on_button_live_close_clicked(NULL, NULL);
	return FALSE;
    }

    return TRUE;
}


void
on_button_live_close_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
    stat2 = -1;
    callback_show_next_live_game();
}


void
on_button_help_close_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_combo_country_entry_changed         (GtkEditable     *editable,
                                        gpointer         user_data)
{
    GtkWidget *combo_country_entry =
	lookup_widget(GTK_WIDGET(editable), "combo_country_entry");
    const gchar *entry_text = gtk_entry_get_text(GTK_ENTRY(combo_country_entry));

    if(strlen(entry_text) == 0)
	return;

    misc_callback_show_team_list(GTK_WIDGET(editable), entry_text);
}

void
on_button_pause_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
    misc_callback_pause_live_game();
}


void
on_button_resume_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
    if(game_check_live_game_resume_state())
    {
	gtk_widget_hide(GTK_WIDGET(button));
	if(g_array_index(usr(stat2).live_game.units, LiveGameUnit, 
			 usr(stat2).live_game.units->len - 1).event.type != 
	   LIVE_GAME_EVENT_PENALTIES)
	    gtk_widget_show(lookup_widget(window.live, "button_pause"));
	game_gui_set_main_window_sensitivity(FALSE);
	live_game_resume();
    }
    else
	game_gui_show_warning("There were too many substitutions. Only 3 per game are allowed.");
}

void
on_spinbutton_speed_value_changed      (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{
    option_set_int("int_opt_user_live_game_speed",
		   usr(stat2).options, gtk_spin_button_get_value_as_int(spinbutton));
}



gboolean
on_treeview_users_button_press_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    misc_callback_remove_user(event);


    return FALSE;
}
