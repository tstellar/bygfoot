#include "file.h"
#include "free.h"
#include "game_gui.h"
#include "interface.h"
#include "main.h"
#include "misc_interface.h"
#include "misc2_interface.h"
#include "option.h"
#include "support.h"
#include "user.h"
#include "variables.h"
#include "window.h"

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
    
    file_get_definitions_dir(country_dir);

    dir_contents = file_dir_get_contents((const gchar*)country_dir, "country_");

    for(i=0;i<dir_contents->len;i++)
	combo_strings = g_list_append(combo_strings,
				      ((GString*)g_ptr_array_index(dir_contents, i))->str);

    gtk_combo_set_popdown_strings(GTK_COMBO(combo_country), combo_strings);

    free_g_string_array(&dir_contents);
}

/** Create and show a window. Which one depends on the argument.
    @param window_type An integer telling us which window to
    create.
    @param count_popups Whether this window adds to the popup
    counter that determines when the main window gets (in)sensitive.
    @return The pointer to the new window.
    @see #Windows */
GtkWidget*
window_create(gint window_type)
{
    gint old_popups_active = popups_active;
    gchar buf[SMALL];
    GtkWidget *wind = NULL;
    
    sprintf(buf, "Bygfoot Football Manager %s", VERS);

    switch(window_type)
    {
	default:
	    g_warning("window_create: unknown window type %d\n", window_type);
	    break;
	case WINDOW_MAIN:
	    if(window.main == NULL)
	    {
		window.main = create_main_window();
		wind = window.main;
		game_gui_print_message("Welcome to Bygfoot "VERS);
	    }
	    else
		wind = window.main;
	    break;
	case WINDOW_STARTUP:
	    if(window.startup != NULL)
		g_warning("window_create: called on already existing window\n");
	    else
	    {
		popups_active++;
		window.startup = create_window_startup();
	    }
	    wind = window.startup;
	    break;
	case WINDOW_LIVE:
	    if(window.live != NULL)
		g_warning("window_create: called on already existing window\n");
	    else
	    {
		popups_active++;
		window.live = create_window_live();
	    }
	    strcpy(buf, "Bygfoot Live game");
	    wind = window.live;
	    gtk_spin_button_set_value(
		GTK_SPIN_BUTTON(lookup_widget(wind, "spinbutton_speed")),
		(gfloat)option_int("int_opt_user_live_game_speed", usr(stat2).options));
	    break;
	case WINDOW_STARTUP_USERS:
	    if(window.startup_users != NULL)
		g_warning("window_create: called on already existing window\n");
	    else
	    {
		popups_active++;
		window.startup_users = create_window_startup_users();
	    }
	    strcpy(buf, "Users");
	    wind = window.startup_users;
	    break;
	case WINDOW_WARNING:
	    if(window.warning != NULL)
		g_warning("window_create: called on already existing window\n");
	    else
		window.warning = create_window_warning();
	    wind = window.warning;
	    strcpy(buf, "Erm...");
	    break;
	case WINDOW_PROGRESS:
	    if(window.progress != NULL)
		g_warning("window_create: called on already existing window\n");
	    else
		window.progress = create_window_progress();
	    wind = window.progress;
	    strcpy(buf, "");
	    break;
    }

    gtk_window_set_title(GTK_WINDOW(wind), buf);
    gtk_widget_show(wind);

    if(popups_active < old_popups_active && window.main != NULL)
	gtk_widget_set_sensitive(window.main, FALSE);

    return wind;
}

/** Destroy a window widget and set the popups and
    main window sensitivity correctly. 
    @param window The window we destroy.
    @param count_popups Whether this window adds to the popup
    counter that determines when the main window gets (in)sensitive. */
void
window_destroy(GtkWidget **wind, gboolean count_popups)
{
    if(*wind == NULL)
	return;

    if(*wind != window.main && count_popups)
    {
	popups_active--;

	if(popups_active == 0 && window.main != NULL)
	    gtk_widget_set_sensitive(window.main, TRUE);
    }

    gtk_widget_destroy(*wind);

    *wind = NULL;
}
