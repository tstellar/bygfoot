#include "window.h"
#include "interface.h"
#include "main.h"
#include "misc_interface.h"
#include "file.h"
#include "free.h"
#include "support.h"

/**  These are used to keep track of open windows.
     @see window_create() */
enum Windows
{
    WINDOW_MAIN = 0,
    WINDOW_STARTUP,
    WINDOW_END
};

/**
   Show the country selection window. All files with prefix
   'country_' from $HOME/.bygfoot/definitions are appended to a combo box.
*/
void
window_show_startup(void)
{
    GtkWidget *window_startup =
	window_create(WINDOW_STARTUP);
    GtkWidget *combo_country =
	lookup_widget(window_startup, "combo_country");
    gchar country_dir[SMALL];
    GPtrArray *dir_contents = NULL;
    GList *combo_strings = NULL;
    gint i;
    
    sprintf(country_dir, "%s/.bygfoot/definitions/", g_get_home_dir());

    dir_contents = file_dir_get_contents((const gchar*)country_dir, "country_");

    if(dir_contents == NULL)
	main_exit_program(EXIT_DIR_OPEN_FAILED);

    for(i=0;i<dir_contents->len;i++)
	combo_strings = g_list_append(combo_strings,
				      ((GString*)g_ptr_array_index(dir_contents, i))->str);

    gtk_combo_set_popdown_strings(GTK_COMBO(combo_country), combo_strings);

    free_g_string_array(&dir_contents);
}

/** Set 'Bygfoot x.y.z' into the title of a window.
    @param window The window widget pointer.
    @see #VERS */
GtkWidget*
window_set_version(GtkWidget *window)
{
    gchar buf[SMALL];

    sprintf(buf, "Bygfoot Football Manager %s", VERS);
    gtk_window_set_title(GTK_WINDOW(window), buf);

    return window;
}

/** Create and show a window. Which one depends on the argument.
    @param window_type An integer telling us which window to
    create.
    @return The pointer to the new window.
    @see #Windows */
GtkWidget*
window_create(gint window_type)
{
    GtkWidget *window = NULL;

    switch(window_type)
    {
	default:
	    window = create_main_window();
	    window_set_version(window);
	    break;
	case WINDOW_STARTUP:
	    window = create_window_startup();
	    window_set_version(window);
	    break;
    }    

    gtk_widget_show(window);

    return window;
}
