#include "misc_callback_func.h"

/* show the teams from the leagues in the country in
   the startup window */
void
misc_callback_show_team_list(GtkWidget *widget, const gchar *country_file)
{
    GtkWidget *treeview_startup =
	lookup_widget(widget, "treeview_startup");

    xml_country_read(country_file);

    treeview_show_team_list(GTK_TREE_VIEW(treeview_startup), FALSE);
}
