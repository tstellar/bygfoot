#include <gtk/gtk.h>


gboolean
on_button_exit_clicked                 (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_button_choose_def_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
on_entry_country_file_changed          (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_button_browse_team_files_clicked    (GtkButton       *button,
                                        gpointer         user_data);

void
on_entry_team_changed                  (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_button_change_structures_clicked    (GtkEntry        *entry,
                                        gpointer         user_data);

void
on_spinbutton_average_skill_value_changed
                                        (GtkSpinButton   *spinbutton,
                                        gpointer         user_data);

gboolean
on_treeview_players_button_press_event (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_button_update_clicked               (GtkEntry        *entry,
                                        gpointer         user_data);

void
on_button_prev_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_next_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_swap_clicked                 (GtkEntry        *entry,
                                        gpointer         user_data);

void
on_button_up_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_down_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_editor_help_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_save_defs_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_import_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_export_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_play_ws_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_save_play_clicked            (GtkButton       *button,
                                        gpointer         user_data);
