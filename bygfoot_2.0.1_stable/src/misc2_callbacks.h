/*
   misc2_callbacks.h

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


void
on_button_offer_ok_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_offer_cancel_clicked         (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_button_warning_clicked              (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_button_digits_ok_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_digits_cancel_clicked        (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_window_yesno_delete_event           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_button_yesno_yes_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_yesno_no_clicked             (GtkButton       *button,
                                        gpointer         user_data);


void
on_button_contract_offer_clicked       (GtkButton       *button,
                                        gpointer         user_data);


gboolean
on_window_contract_delete_event        (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_entry_user_management_activate      (GtkEntry        *entry,
                                        gpointer         user_data);

void
on_button_user_management_add_clicked  (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_user_management_close_clicked
                                        (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_treeview_user_management_users_button_press_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_treeview_user_management_teams_row_activated
                                        (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data);

gboolean
on_window_debug_delete_event           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_button_debug_apply_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_debug_close_activate         (GtkButton       *button,
                                        gpointer         user_data);

void
on_entry_debug_activate                (GtkEntry        *entry,
                                        gpointer         user_data);

gboolean
on_window_job_offer_delete_event       (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_window_digits_delete_event          (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_window_user_management_delete_event (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_window_help_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_button_help_close_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
on_spinbutton1_activate                (GtkEntry        *entry,
                                        gpointer         user_data);

void
on_spinbutton2_activate                (GtkEntry        *entry,
                                        gpointer         user_data);

gboolean
on_window_transfer_dialog_delete_event (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_button_transfer_yes_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_transfer_no_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_transfer_later_clicked       (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_contract_cancel_clicked      (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_window_mmatches_delete_event        (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_treeview_mmatches_button_press_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_button_mm_save_close_clicked        (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_mm_add_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_mm_file_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_mm_reload_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_mm_reload_close_clicked      (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_mm_import_clicked            (GtkButton       *button,
                                        gpointer         user_data);
#endif
