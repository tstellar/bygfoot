#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "options_callbacks.h"
#include "options_interface.h"
#include "support.h"
#include "variables.h"
#include "window.h"

void
on_button_options_ok_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button_options_cancel_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
    window_destroy(&window.options, TRUE);
}

