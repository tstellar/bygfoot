#include "treeview.h"

/**
 * Removes all columns from a GTK treeview. I didn't find a better way
 * to completely clear a treeview :-/.
 * @param treeview The pointer to the treeview widget we'd like to clear.
 */
void
treeview_clear(GtkTreeView *treeview)
{
    gint i;
    gint number_of_columns;
    GtkTreeView *list = (treeview == NULL) ?
	GTK_TREE_VIEW(lookup_widget(main_window, "player_info")) :
	treeview;

    gtk_tree_view_set_model(GTK_TREE_VIEW(list),
			    NULL);

    number_of_columns = 0;
    while(gtk_tree_view_get_column(GTK_TREE_VIEW(list),
				   number_of_columns) != NULL)
	number_of_columns++;

    for(i = number_of_columns - 1; i >= 0; i--)
	gtk_tree_view_remove_column(
	    GTK_TREE_VIEW(list),
	    gtk_tree_view_get_column(GTK_TREE_VIEW(list),
				     i));
}

/**
   Creates the model for the treeview in the team selection window.
   The model contains a list of all the teams from the leagues in
   the country::leagues array; if show_cup_teams is TRUE, the
   teams from international cups are shown, too.
   @param show_cup_teams Whether or not teams from international
   cups are shown.
   @return The model containing the team names.
*/
GtkTreeModel*
treeview_create_team_selection_list(gboolean show_cup_teams)
{
    gint i, j, counter = 1;
    GtkListStore  *liststore;
    GtkTreeIter iter;

    liststore = gtk_list_store_new(3,
				   G_TYPE_INT,
				   G_TYPE_STRING,
				   G_TYPE_STRING);   

    for(i=0;i<ligs->len;i++)
    {
	for(j=0;j<lig(i).teams->len;j++)
	{
	    gtk_list_store_append(liststore, &iter);
	    gtk_list_store_set(liststore, &iter,
			       0, counter++,
			       1, g_array_index(lig(i).teams, Team, j).name->str,
			       2, lig(i).name->str,
			       -1);
	}	    
    }
    
    return GTK_TREE_MODEL(liststore);
}

/**
   Sets up the treeview for the team selection window.
   Columns and cell renderers are added etc.
   @param treeview The treeview that gets configured.
*/
void
treeview_set_up_team_selection_treeview (GtkTreeView *treeview)
{
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;

    gtk_tree_selection_set_mode(
	gtk_tree_view_get_selection(treeview),
	GTK_SELECTION_BROWSE);
    
    gtk_tree_view_set_rules_hint(treeview, TRUE);

    /* number the teams */
    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "text", 0);
    if(strcmp(font_name->str, "0") != 0)
	g_object_set(renderer, "font", font_name->str, NULL);

    /* set up team name column */
    col = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(col, _("Team"));
    gtk_tree_view_append_column(treeview, col);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "text", 1);
    if(strcmp(font_name->str, "0") != 0)
	g_object_set(renderer, "font", font_name->str, NULL);

    /* league column */
    col = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(col, _("League"));
    gtk_tree_view_append_column(treeview, col);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "text", 2);
}

/** Shows the list of teams in the game.
    If show_cup_teams is TRUE, the teams from
    international cups are shown, too.
    @param treeview The treeview we show the list in.
    @param show_cup_teams Whether or not teams from international
    cups are shown.
*/
void
treeview_show_team_list(GtkTreeView *treeview, gboolean show_cup_teams)
{
    GtkTreeModel *team_list = 
	treeview_create_team_selection_list(show_cup_teams);
    GtkTreeSelection *selection;
     
    treeview_clear(treeview);

    treeview_set_up_team_selection_treeview(treeview);
    
    gtk_tree_view_set_model(treeview, team_list);
    
    selection = gtk_tree_view_get_selection(treeview);
    gtk_tree_selection_select_path(selection,
				   gtk_tree_path_new_from_string("0"));
    g_object_unref(team_list);
}
