/*
   option_gui.h

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

#ifndef OPTION_GUI_H
#define OPTION_GUI_H

#include "bygfoot.h"

void
option_gui_write_bool_widgets(gint **bool_options, GtkToggleButton **bool_widgets);

void
option_gui_write_spin_widgets(gint **spin_options, GtkSpinButton **spin_widgets);

void
option_gui_write_entry_widgets(gchar ***entry_options, GtkEntry **entry_widgets);

void
option_gui_set_up_window(void);

void
option_gui_write_options(void);

#endif

