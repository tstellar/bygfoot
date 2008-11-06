/*
   misc3_callbacks.h

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

#ifndef MISC2_CALLBACKS_H
#define MISC2_CALLBACKS_H

#include <gtk/gtk.h>


gboolean
on_window_bets_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_button_bet_close_clicked            (GtkButton       *button,
                                        gpointer         user_data);
gboolean
on_checkbutton_bet_all_leagues_button_press_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_checkbutton_bet_cups_button_press_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_treeview_bets_button_press_event    (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_checkbutton_bet_user_recent_button_press_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);
#endif

gboolean
on_window_splash_delete_event          (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_button_splash_new_game_clicked      (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_splash_load_game_clicked     (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_splash_resume_game_clicked   (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_splash_quit_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_splash_hint_back_clicked     (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_splash_hint_next_clicked     (GtkButton       *button,
                                        gpointer         user_data);

void
on_spinbutton_start_week_changed       (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_button_calculate_start_week_clicked (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_calculate_installment_clicked
                                        (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_alr_confirm_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_alr_cancel_clicked           (GtkButton       *button,
                                        gpointer         user_data);
