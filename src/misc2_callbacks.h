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
on_graph_window_delete_event           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_optionmenu_player_changed           (GtkOptionMenu   *optionmenu,
                                        gpointer         user_data);

void
on_optionmenu_team_changed             (GtkOptionMenu   *optionmenu,
                                        gpointer         user_data);

void
on_button_close_clicked                (GtkButton       *button,
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
