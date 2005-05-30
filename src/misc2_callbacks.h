#ifndef MISC2_CALLBACKS_H
#define MISC2_CALLBACKS_H

#include <gtk/gtk.h>


gboolean
on_button_cancel_clicked               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_button_ok_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

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
#endif
