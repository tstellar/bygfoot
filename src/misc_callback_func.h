/*
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

void
misc_callback_startup_load(const gchar *filename);

void
misc_callback_new_sponsor(void);

#endif
