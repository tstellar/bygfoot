#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "bygfoot-update.h"
#include "bu_functions.h"
#include "bu_callbacks.h"
#include "bu_interface.h"
#include "bu_support.h"


void
on_button_packdir_fs_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *fs = create_fileselection();
    gtk_widget_show(fs);
    status = STATUS_FS_PACK;
}


void
on_button_tempdir_fs_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *fs = create_fileselection();
    gtk_widget_show(fs);
    status = STATUS_FS_TMP;
}


void
on_button_countrydir_fs_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *fs = create_fileselection();
    gtk_widget_show(fs);
    status = STATUS_FS_COUNTRY;
}


void
on_button_ok_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
    callback_update();
}

void
on_button_fs_ok_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *fs = lookup_widget(GTK_WIDGET(button), "fileselection");
    GtkWidget *entry = NULL;

    if(status == STATUS_FS_PACK)
	entry = lookup_widget(update_window, "entry_packdir");
    else if(status == STATUS_FS_TMP)
	entry = lookup_widget(update_window, "entry_tmpdir");
    else if(status == STATUS_FS_COUNTRY)
	entry = lookup_widget(update_window, "entry_countrydir");

    gtk_entry_set_text(GTK_ENTRY(entry),
		       gtk_file_selection_get_filename(
			   GTK_FILE_SELECTION(fs)));

    gtk_widget_destroy(fs);
    status = STATUS_NORMAL;
}


void
on_button_fs_cancel_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
    gtk_widget_destroy(lookup_widget(GTK_WIDGET(button), "fileselection"));
    status = STATUS_NORMAL;
}

void
on_button_quit_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
    gtk_widget_destroy(update_window);
    gtk_main_quit();
}



void
on_button_version_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
    update_download(TRUE);
}

