/*
   options_callbacks.c

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

#include "file.h"
#include "option.h"
#include "options_callbacks.h"
#include "options_interface.h"
#include "option_gui.h"
#include "support.h"
#include "training.h"
#include "treeview2.h"
#include "user.h"
#include "variables.h"
#include "window.h"

G_MODULE_EXPORT void
on_button_options_ok_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef DEBUG
    printf("on_button_options_ok_clicked\n");
#endif

    gboolean save_global =
	gtk_toggle_button_get_active(
	    GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_save_global"))),
	save_user = 
	gtk_toggle_button_get_active(
	    GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_save_user")));
    const gchar *conf_dir = file_get_first_support_dir();
    gchar buf[SMALL];

    option_gui_write_options();

    if(save_global)
    {
	sprintf(buf, "%s%sbygfoot.conf", conf_dir, G_DIR_SEPARATOR_S);
	file_save_opt_file(buf, &options);
    }

    if(save_user)
    {
	if(strcmp(current_user.name, "NONAME") == 0)
	    sprintf(buf, "%s%sbygfoot_user.conf", conf_dir, G_DIR_SEPARATOR_S);
	else
	    sprintf(buf, "%s%sbygfoot_%s.conf", conf_dir, G_DIR_SEPARATOR_S, current_user.name);

	file_save_opt_file(buf, &current_user.options);
    }

    window_destroy(&window.options);
}


G_MODULE_EXPORT void
on_button_options_cancel_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef DEBUG
    printf("on_button_options_cancel_clicked\n");
#endif

    window_destroy(&window.options);
}


G_MODULE_EXPORT void
on_button_font_name_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef DEBUG
    printf("on_button_font_name_clicked\n");
#endif

    window_create(WINDOW_FONT_SEL);
}


G_MODULE_EXPORT void
on_button_reload_constants_clicked     (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef DEBUG
    printf("on_button_reload_constants_clicked\n");
#endif

    const gchar *constants_file =
	gtk_entry_get_text(GTK_ENTRY(lookup_widget(window.options, "entry_constants_file")));

    file_load_opt_file(constants_file, &constants, TRUE);
}


G_MODULE_EXPORT gboolean
on_checkbutton_save_global_button_press_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
#ifdef DEBUG
    printf("on_checkbutton_save_global_button_press_event\n");
#endif

    if(event->button == 3)
    {
	gchar *conf_file = file_find_support_file("bygfoot.conf", TRUE);
	
	file_load_opt_file(conf_file, &options, FALSE);
	g_free(conf_file);

	option_gui_set_up_window();
	return TRUE;
    }

    return FALSE;
}


G_MODULE_EXPORT gboolean
on_checkbutton_save_user_button_press_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
#ifdef DEBUG
    printf("on_checkbutton_save_user_button_press_event\n");
#endif


    if(event->button == 3)
    {
	file_load_user_conf_file(&current_user);
	option_gui_set_up_window();
	return TRUE;
    }

    return FALSE;
}


G_MODULE_EXPORT gboolean
on_window_options_delete_event         (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
#ifdef DEBUG
    printf("on_window_options_delete_event\n");
#endif

    on_button_options_cancel_clicked(NULL, NULL);

    return TRUE;
}


G_MODULE_EXPORT void
on_spinbutton_recreation_value_changed (GtkSpinButton   *spinbutton,
                                        gpointer         user_data)
{
#ifdef DEBUG
    printf("on_spinbutton_recreation_value_changed\n");
#endif

    gchar buf[SMALL];

    sprintf(buf, "%d", CAMP_SCALE_MAX - gtk_spin_button_get_value_as_int(spinbutton));
    
    gtk_label_set_text(GTK_LABEL(lookup_widget(window.options, "label_training")), buf);
}

G_MODULE_EXPORT void
on_button_edit_constants_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
    window_show_constants();    
}


G_MODULE_EXPORT gboolean
on_window_constants_destroy_event      (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    on_button_constants_close_clicked(NULL, NULL);
    return FALSE;
}


G_MODULE_EXPORT gboolean
on_window_constants_delete_event       (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    on_button_constants_close_clicked(NULL, NULL);
    return FALSE;
}


G_MODULE_EXPORT void
on_button_constants_reload_clicked     (GtkButton       *button,
                                        gpointer         user_data)
{
    if(gtk_notebook_get_current_page(
           GTK_NOTEBOOK(lookup_widget(GTK_WIDGET(button), "notebook_constants"))) == 3)
        file_load_opt_file(opt_str("string_opt_appearance_file"),
                           &constants_app, TRUE);
    else
        file_load_opt_file(opt_str("string_opt_constants_file"),
                           &constants, TRUE);

    treeview2_show_constants();
}


G_MODULE_EXPORT void
on_button_constants_close_clicked      (GtkButton       *button,
                                        gpointer         user_data)
{
    window_destroy(&window.constants);
}


G_MODULE_EXPORT void
on_button_constants_save_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
    gchar buf[SMALL];

    if(gtk_notebook_get_current_page(
           GTK_NOTEBOOK(lookup_widget(GTK_WIDGET(button), "notebook_constants"))) == 3)
    {
        sprintf(buf, "%s%s%s", file_get_first_support_dir(),
                G_DIR_SEPARATOR_S, opt_str("string_opt_appearance_file"));
        file_save_opt_file(buf, &constants_app);
    }
    else
    {
        sprintf(buf, "%s%s%s", file_get_first_support_dir(),
                G_DIR_SEPARATOR_S, opt_str("string_opt_constants_file"));
        file_save_opt_file(buf, &constants);
    }
}
