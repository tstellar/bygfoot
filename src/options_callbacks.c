#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "file.h"
#include "options_callbacks.h"
#include "options_interface.h"
#include "option_gui.h"
#include "support.h"
#include "variables.h"
#include "window.h"

void
on_button_options_ok_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
    option_gui_write_options();

    window_destroy(&window.options, TRUE);
}


void
on_button_options_cancel_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
    window_destroy(&window.options, TRUE);
}


void
on_button_font_name_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
    window_create(WINDOW_FONT_SEL);
}


void
on_button_reload_constants_clicked     (GtkButton       *button,
                                        gpointer         user_data)
{
    const gchar *constants_file =
	gtk_entry_get_text(GTK_ENTRY(lookup_widget(window.options, "entry_constants_file")));

    file_load_constants_file(constants_file);
}

