/*
   options_callbacks.h

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

#include <gtk/gtk.h>

#ifndef OPTIONS_CALLBACKS_H
#define OPTIONS_CALLBACKS_H

void
on_button_options_ok_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_options_cancel_clicked       (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_font_name_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_reload_constants_clicked     (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_checkbutton_save_global_button_press_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_checkbutton_save_user_button_press_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_window_options_delete_event         (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);
#endif

void
on_spinbutton_recreation_value_changed (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);
