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
on_checkbutton_yesno_toggled           (GtkToggleButton *togglebutton,
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

#endif
