/*
   load_save.h

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

#ifndef LOAD_SAVE_H
#define LOAD_SAVE_H

#include "bygfoot.h"

void
load_save_save_game(const gchar* filename);

gboolean
load_save_load_game(const gchar* filename, gboolean create_main_window);

void
load_save_last_save_set(const gchar *filename);

gchar*
load_save_last_save_get(void);

void
load_save_autosave(void);

gboolean
load_game_from_command_line(const gchar *filename);

#endif
