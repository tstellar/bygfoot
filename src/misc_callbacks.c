#include "callbacks.h"
#include "callback_func.h"
#include "game.h"
#include "game_gui.h"
#include "gui.h"
#include "live_game.h"
#include "load_save.h"
#include "main.h"
#include "misc_callback_func.h"
#include "misc_callbacks.h"
#include "option.h"
#include "user.h"
#include "variables.h"
#include "window.h"
#include "xml_country.h"

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
    main_exit_program(EXIT_OK, NULL);
}


void
on_team_selection_tv_row_activated     (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data)
{
    on_button_add_player_clicked(NULL, NULL);
}

void
on_team_selection_load_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
    stat1 = STATUS_LOAD_GAME_TEAM_SELECTION;
    window_show_file_sel();
}


gboolean
on_fsel_window_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    on_button_fsel_cancel_clicked(NULL, NULL);

    return FALSE;
}


void
on_button_fsel_ok_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
    const gchar *filename = 
	gtk_file_selection_get_filename(GTK_FILE_SELECTION(window.file_sel));

    if(stat1 == STATUS_SAVE_GAME)
	load_save_save_game(filename);
    else if(stat1 == STATUS_LOAD_GAME)
	load_save_load_game(filename);
    else if(stat1 == STATUS_LOAD_GAME_TEAM_SELECTION)
	misc_callback_startup_load(filename);

    window_destroy(&window.file_sel, FALSE);
}


void
on_button_fsel_cancel_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
    window_destroy(&window.file_sel, FALSE);
}


gboolean
on_button_font_sel_cancel_clicked      (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    window_destroy(&window.font_sel, FALSE);

    return FALSE;
}


void
on_button_font_sel_ok_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
    gchar *font = 
	gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(window.font_sel));

    if(font != NULL)
    {
	gtk_entry_set_text(GTK_ENTRY(lookup_widget(window.options, "entry_font_name")), font);
	g_free(font);
    }

    window_destroy(&window.font_sel, FALSE);
}


void
on_button_font_sel_apply_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
    gchar *font = 
	gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(window.font_sel));

    if(font != NULL)
    {
	opt_set_str("string_opt_font_name", font);
	gtk_entry_set_text(GTK_ENTRY(lookup_widget(window.options, "entry_font_name")), font);
	g_free(font);

	stat0 = STATUS_MAIN;
	game_gui_show_main();

	setsav0;
    }
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
    if(stat0 != STATUS_SHOW_LAST_MATCH)
	callback_show_next_live_game();
    else
	window_destroy(&window.live, TRUE);
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
	gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
	if(g_array_index(usr(stat2).live_game.units, LiveGameUnit, 
			 usr(stat2).live_game.units->len - 1).event.type != 
	   LIVE_GAME_EVENT_PENALTIES)
	    gtk_widget_set_sensitive(lookup_widget(window.live, "button_pause"), TRUE);
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
		   &usr(stat2).options, gtk_spin_button_get_value_as_int(spinbutton));
}



gboolean
on_treeview_users_button_press_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    misc_callback_remove_user(event);

    return FALSE;
}

void
on_spinbutton_capacity_value_changed   (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{
    misc_callback_update_stadium_window(TRUE);
}


void
on_spinbutton_safety_value_changed     (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{
    misc_callback_update_stadium_window(FALSE);
}


void
on_button_stadium_ok_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
    misc_callback_improve_stadium();

    if(stat0 == STATUS_SHOW_FINANCES)
	on_menu_show_finances_activate(NULL, NULL);
}


void
on_button_stadium_cancel_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
    window_destroy(&window.stadium, TRUE);
}


gboolean
on_spinbutton_capacity_button_press_event  (GtkWidget       *widget,
					    GdkEventButton  *event,
					    gpointer         user_data)
{
    if(event->button == 3)
    {
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget), 0);
	return TRUE;
    }
    
    return FALSE;
}

void
on_button_startup_resume_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
    misc_callback_startup_load("last_save");
}

gboolean
on_window_stadium_delete_event         (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

    return TRUE;
}

