#ifndef MISC_CALLBACK_FUNC_H
#define MISC_CALLBACK_FUNC_H

#include "bygfoot.h"

void
misc_callback_start_game(void);

void
misc_callback_show_team_list(GtkWidget *widget, const gchar *country_file);

void
misc_callback_add_player(void);

void
misc_callback_remove_user(GdkEventButton *event);

void
misc_callback_pause_live_game(void);

void
misc_callback_improve_stadium(void);

void
misc_callback_update_stadium_window(gboolean capacity);

#endif
