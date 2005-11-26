/*
   misc_callbacks.h

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

#ifndef MISC_CALLBACS_H
#define MISC_CALLBACS_H

#include "bygfoot.h"
#include "misc_interface.h"
#include "support.h"

void
on_team_selection_cancel_clicked       (GtkButton       *button,
                                        gpointer         user_data);

void
on_team_selection_tv_row_activated     (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data);
void
on_team_selection_ok_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
on_team_selection_load_clicked         (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_button_font_sel_cancel_clicked      (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_button_font_sel_ok_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_font_sel_apply_clicked       (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_live_window_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_button_live_close_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_pause_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_resume_clicked               (GtkButton       *button,
                                        gpointer         user_data);


void
on_spinbutton_speed_value_changed      (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_button_add_player_clicked           (GtkButton       *button,
                                        gpointer         user_data);


gboolean
on_treeview_users_button_press_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_entry_player_name_activate          (GtkEntry        *entry,
                                        gpointer         user_data);

void
on_spinbutton_capacity_value_changed   (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_spinbutton_safety_value_changed     (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

void
on_button_stadium_ok_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_stadium_cancel_clicked       (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_spinbutton_capacity_button_press_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_button_startup_resume_clicked       (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_window_stadium_delete_event         (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_combo_country_changed               (GtkComboBox     *combobox,
                                        gpointer         user_data);

void
on_spinbutton_verbosity_value_changed  (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

gboolean
on_spinbutton_speed_button_press_event (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_button_sponsors_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
on_treeview_sponsors_row_activated     (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data);

gboolean
on_window_sponsors_delete_event        (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_window_sponsors_delete_event        (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_treeview_sponsors_row_activated     (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data);

void
on_button_sponsors_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_sponsors_wait_clicked        (GtkButton       *button,
                                        gpointer         user_data);
#endif

gboolean
on_eventbox_lg_style_button_press_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_eventbox_lg_boost_button_press_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);
