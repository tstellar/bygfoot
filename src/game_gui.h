#ifndef GAME_GUI_H
#define GAME_GUI_H

#include "bygfoot.h"
#include "live_game_struct.h"

void
game_gui_live_game_show_unit(const LiveGameUnit *unit);

void
game_gui_live_game_set_hscale(const LiveGameUnit *unit, GtkHScale *hscale);

void
game_gui_set_main_window_header(void);

void
game_gui_show_main(void);

void
game_gui_get_radio_items(GtkWidget **style, GtkWidget **scout,
			 GtkWidget **physio, GtkWidget **boost);

void
game_gui_print_message(gchar *text);

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
game_gui_show_warning(gchar *text);

void
game_gui_write_meters(void);

void
game_gui_show_job_offer(Team *team, gint type);

#endif
