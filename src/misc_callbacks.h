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
on_fsel_window_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_button_fsel_ok_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_fsel_cancel_clicked          (GtkButton       *button,
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
on_button_help_close_clicked           (GtkButton       *button,
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
#endif
