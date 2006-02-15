/*
   window.h

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

#ifndef WINDOW_H
#define WINDOW_H

#include "bygfoot.h"

/**  These are used to keep track of open windows.
     @see window_create() */
enum Windows
{
    WINDOW_MAIN = 0,
    WINDOW_STARTUP,
    WINDOW_LIVE,
    WINDOW_WARNING,
    WINDOW_PROGRESS,
    WINDOW_DIGITS,
    WINDOW_STADIUM,
    WINDOW_JOB_OFFER,
    WINDOW_YESNO,
    WINDOW_OPTIONS,
    WINDOW_FONT_SEL,
    WINDOW_FILE_CHOOSER,
    WINDOW_CONTRACT,
    WINDOW_USER_MANAGEMENT,
    WINDOW_DEBUG,
    WINDOW_HELP,
    WINDOW_TRANSFER_DIALOG,
    WINDOW_SPONSORS,
    WINDOW_MMATCHES,
    WINDOW_BETS,
    WINDOW_SPLASH,
    WINDOW_END
};

void
window_show_help(gint page);

void
window_show_startup(void);

GtkWidget*
window_create(gint window_type);

void
window_destroy(GtkWidget **wind, gboolean count_popups);

void
window_show_digits(const gchar *text_main, const gchar* text1,
		   gint value1, const gchar* text2, gint value2);

void
window_show_file_sel(void);

void
window_show_stadium(void);

void
window_show_yesno(const gchar *text);

void
window_show_options(void);

void
window_show_menu_player(GdkEvent *event);

void
window_show_menu_youth(GdkEvent *event);

void
window_live_set_up(void);

void
window_show_transfer_dialog(const gchar *text);

void
window_show_news(void);

void
window_show_mmatches(void);

void
window_main_save_geometry(void);

void
window_main_load_geometry(void);

void
window_show_bets(void);

void
window_show_progress(gint pictype);

void
window_show_splash(void);

void
window_load_hint_number(void);

void
window_save_hint_number(void);

void
window_splash_show_hint(void);

#endif
