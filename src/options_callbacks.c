#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "options_callbacks.h"
#include "options_interface.h"
#include "support.h"


gboolean
on_opt_window_delete_event             (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

  return FALSE;
}


void
on_check_show_live_toggled             (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_spin_live_duration_activate         (GtkEntry        *entry,
                                        gpointer         user_data)
{

}


void
on_button_opt_change_font_clicked      (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_checkbutton_autosave_toggled        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_checkbutton_sort_transfers_toggled  (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_checkbutton_notify_toggled          (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_button_opt_ok_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button_opt_cancel_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{

}

