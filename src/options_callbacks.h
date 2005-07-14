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
