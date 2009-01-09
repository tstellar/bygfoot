/*
   gui.h

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

#ifndef GUI_H
#define GUI_H

#include "bygfoot.h"

/** What kind of pictures we on top of the progress bar. */
enum PicType
{
    PIC_TYPE_NONE = 0,
    PIC_TYPE_SAVE,
    PIC_TYPE_LOAD,
    PIC_TYPE_MATCHPIC
};

void
gui_label_set_text_from_int(GtkLabel *label, gint number, gboolean append);

void
gui_show_progress(gfloat value, const gchar *text, gint pictype);

gdouble
gui_get_progress_bar_fraction(void);

void
gui_set_arrow_pair(gint pair, gboolean state);

void
gui_set_arrows(void);

void
gui_set_sensitive_lg_meters(gboolean state);

#endif
