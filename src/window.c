#include "window.h"

/**
   Show the country selection window. All files with prefix
   'country_' from $HOME/.bygfoot/definitions are appended to a combo box.
*/
void
window_show_startup(void)
{
    GtkWidget *window_startup =
	create_window_startup();
    GtkWidget *combo_country =
	lookup_widget(window_startup, "combo_country");
    gchar country_dir[SMALL];
    GPtrArray *dir_contents = NULL;
    GList *combo_strings = NULL;
    gint i;
    
    set_version(window_startup);

    sprintf(country_dir, "%s/.bygfoot/definitions/", g_get_home_dir());

    dir_contents = file_dir_get_contents((const gchar*)country_dir, "country_");

    if(dir_contents == NULL)
    {
	exit(EXIT_DIR_OPEN);
    }

    for(i=0;i<dir_contents->len;i++)
	combo_strings = g_list_append(combo_strings,
				      ((GString*)g_ptr_array_index(dir_contents, i))->str);

    gtk_combo_set_popdown_strings(GTK_COMBO(combo_country), combo_strings);

    file_dir_free_contents(dir_contents);

    gtk_widget_show(window_startup);
}

/** Set 'Bygfoot x.y.z' into the title of a window.
    @param window The window widget pointer.
    @see #VERS */
GtkWidget*
set_version(GtkWidget *window)
{
    gchar buf[SMALL];

    sprintf(buf, "Bygfoot Football Manager %s", VERS);
    gtk_window_set_title(GTK_WINDOW(window), buf);

    return window;
}

