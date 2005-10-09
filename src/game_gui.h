#ifndef GAME_GUI_H
#define GAME_GUI_H

#include <stdarg.h>
#include <glib/gprintf.h>

#include "bygfoot.h"
#include "live_game_struct.h"
#include "option_struct.h"

#define setsav1 gtk_widget_set_sensitive(lookup_widget(window.main, "button_save"), FALSE);gtk_widget_set_sensitive(lookup_widget(window.main, "menu_save"), FALSE)
#define setsav0 gtk_widget_set_sensitive(lookup_widget(window.main, "button_save"), TRUE);gtk_widget_set_sensitive(lookup_widget(window.main, "menu_save"), TRUE)
#define getsav GTK_WIDGET_SENSITIVE(lookup_widget(window.main, "button_save"))

void
game_gui_live_game_show_unit(const LiveGameUnit *unit);

void
game_gui_live_game_set_hscale(const LiveGameUnit *unit, GtkHScale *hscale);

void
game_gui_set_main_window_header(void);

void
game_gui_show_main(void);

void
game_gui_show_warning(const gchar *format, ...)
__attribute__ ((format (printf, 1, 2)));

void
game_gui_print_message(gchar *format, ...)
__attribute__ ((format (printf, 1, 2)));

gboolean
game_gui_print_message_source(gpointer data);

void
game_gui_print_message_with_delay(const gchar *format, ...)
__attribute__ ((format (printf, 1, 2)));

void
game_gui_get_radio_items(GtkWidget **style, GtkWidget **scout,
			 GtkWidget **physio, GtkWidget **boost,
			 GtkWidget **yc, GtkWidget **ya_pos_pref);

void
game_gui_read_radio_items(GtkWidget *widget);

void
game_gui_write_radio_items(void);

gboolean
game_gui_clear_entry_message(gpointer data);

void
game_gui_set_main_window_sensitivity(gboolean value);

void
game_gui_write_av_skills(void);

void
game_gui_write_meters(const Team *tm);

void
game_gui_show_job_offer(Team *team, gint type);

void
game_gui_write_check_items(void);

void
game_gui_read_check_items(GtkWidget *widget);

void
game_gui_set_help_labels(void);

#endif
