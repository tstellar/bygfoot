#include <unistd.h>

#include "file.h"
#include "free.h"
#include "gui.h"
#include "league.h"
#include "live_game.h"
#include "maths.h"
#include "misc.h"
#include "support.h"
#include "team.h"
#include "treeview.h"
#include "treeview_cell.h"
#include "variables.h"
#include "window.h"

/** Return the filename of the icon going with the LiveGameEvent
    with type event_type.
    @param event_type The type of the event.
    @return A filename specifying a pixmap. */
gchar*
treeview_live_game_icon(gint event_type)
{
    switch(event_type)
    {
	default:
	    return "";
	    break;
	case LIVE_GAME_EVENT_FOUL_YELLOW:
	    return "yellow.png";
	    break;
	case LIVE_GAME_EVENT_FOUL_RED:
	    return "red.png";
	    break;
	case LIVE_GAME_EVENT_SEND_OFF:
	    return "red.png";
	    break;
	case LIVE_GAME_EVENT_SAVE:
	    return "save.png";
	    break;
	case LIVE_GAME_EVENT_GOAL:
	    return "goal.png";
	    break;
    }

    return "";
}

/** Return a new pixbuf created from the specified filename.
    @param filename Name of a pixmap file located in one of the support directories. 
    @return A new pixbuf or NULL on error. */
GdkPixbuf*
treeview_pixbuf_from_filename(gchar *filename)
{
    GdkPixbuf *symbol = NULL;
    GError *error = NULL;
    gchar *symbol_file = NULL;

    if(filename != NULL && strlen(filename) != 0)
    {
	symbol_file = file_find_support_file(filename);
	if(symbol_file != NULL)
	{
	    symbol = gdk_pixbuf_new_from_file(symbol_file, &error);
	    misc_print_error(&error, FALSE);
	    g_free(symbol_file);
	}
    }
    else
	symbol = NULL;

    return symbol;
}

/** Return the number in the 'column'th column of the currently
    selected row of the treeview.
    @param treeview The treeview argument.
    @param column The column we'd like to get the contents of.
    @return The number in the given column of the selected row.
*/
gint
treeview_get_index(GtkTreeView *treeview, gint column)
{
    gint value;
    GtkTreeSelection *selection	= 
	gtk_tree_view_get_selection(treeview);
    GtkTreeModel     *model;
    GtkTreeIter       iter;
    
    gtk_tree_selection_get_selected(selection, &model, &iter);

    gtk_tree_model_get(model, &iter, column,
		       &value, -1);

    return value;
}

/** Return the pointer in the 'column'th column of the currently
    selected row of the treeview.
    @param treeview The treeview argument.
    @param column The column we'd like to get the content of.
    @return The pointer in the given column of the selected row.
*/
/*d*/
gpointer
treeview_get_pointer(GtkTreeView *treeview, gint column)
{
    gpointer ptr;
    GtkTreeSelection *selection	=
	gtk_tree_view_get_selection(treeview);
    GtkTreeModel     *model;
    GtkTreeIter       iter;
    
    gtk_tree_selection_get_selected(selection, &model, &iter);

    gtk_tree_model_get(model, &iter, column,
		       &ptr, -1);

    return ptr;
}

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

/** Return number of given column or -1 if not found or on error.
    @param col The column pointer.
    @return The index of the column within the treeview. */
gint
treeview_get_col_number_column (GtkTreeViewColumn *col)
{
    GList *cols;
    gint num;

    g_return_val_if_fail ( col != NULL, -1 );
    g_return_val_if_fail ( col->tree_view != NULL, -1 );

    cols = gtk_tree_view_get_columns(GTK_TREE_VIEW(col->tree_view));
    num = g_list_index(cols, (gpointer) col);
    g_list_free(cols);

    return num;
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
    gint i, j, cnt = 1;
    GtkListStore  *liststore;
    GtkTreeIter iter;
    GdkPixbuf *symbol = NULL;

    liststore = gtk_list_store_new(4,
				   G_TYPE_INT,
				   GDK_TYPE_PIXBUF,
				   G_TYPE_POINTER,
				   G_TYPE_STRING);

    for(i=0;i<ligs->len;i++)
    {
	symbol = treeview_pixbuf_from_filename(lig(i).symbol->str);

	for(j=0;j<lig(i).teams->len;j++)
	{
	    gtk_list_store_append(liststore, &iter);
	    gtk_list_store_set(liststore, &iter,
			       0, cnt++,
			       1, symbol,
			       2, (gpointer)&g_array_index(lig(i).teams, Team, j),
			       3, lig(i).name->str,
			       -1);
	}	    

	if(symbol != NULL)
	    g_object_unref(symbol);
    }

    if(!show_cup_teams)    
	return GTK_TREE_MODEL(liststore);

    for(i=0;i<cps->len;i++)
	if(cp(i).type == CUP_TYPE_INTERNATIONAL)
	    for(j=0;j<cp(i).teams->len;j++)
	    {
		symbol = 
		    treeview_pixbuf_from_filename(g_array_index(cp(i).teams, Team, j).symbol->str);

		gtk_list_store_append(liststore, &iter);
		gtk_list_store_set(liststore, &iter,
				   0, cnt++,
				   1, symbol,
				   2, (gpointer)&g_array_index(cp(i).teams, Team, j),
				   3, cp(i).name->str,
				   -1);
		if(symbol != NULL)
		    g_object_unref(symbol);
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

    /* Numbering the teams */
    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "text", 0);
    if(strcmp(font_name->str, "0") != 0)
	g_object_set(renderer, "font", font_name->str, NULL);

    /* Flags */
    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = gtk_cell_renderer_pixbuf_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "pixbuf", 1);

    /* Team column */
    col = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(col, _("Team"));
    gtk_tree_view_append_column(treeview, col);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_set_cell_data_func(col, renderer,
					    treeview_cell_team_selection,
					    GINT_TO_POINTER(2), NULL);
    /* League column */
    col = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(col, _("League"));
    gtk_tree_view_append_column(treeview, col);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "text", 3);
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

/** Create the list store for a player list. 
    @param players The array containing the players.
    @param attributes An array containing the attributes we show.
    @param max The size of the attribute array.
    @param separator Whether we draw a blank line after the 11th player. */
GtkTreeModel*
treeview_create_player_list(GPtrArray *players, gint *attributes, gint max, gboolean show_separator)
{
    gint i, j;
    GtkListStore  *liststore;
    GtkTreeIter iter;
    GType types[max + 1];

    types[0] = G_TYPE_INT;
    for(i=0;i<max;i++)
	types[i + 1] = G_TYPE_POINTER;

    liststore = gtk_list_store_newv(max + 1, types);

    for(i=0;i<players->len;i++)
    {
	gtk_list_store_append(liststore, &iter);
	if(show_separator && i == 11)
	{
	    gtk_list_store_set(liststore, &iter, 0, CONSTANT_TREEVIEW_CELL_INT_EMPTY, -1);
	    for(j=0;j<max;j++)
		gtk_list_store_set(liststore, &iter, j + 1, NULL, -1);
	    
	    gtk_list_store_append(liststore, &iter);
	    gtk_list_store_set(liststore, &iter, 0, i + 1, -1);
	}
	else
	    gtk_list_store_set(liststore, &iter, 0, i + 1, -1);

	for(j=0;j<max;j++)
	    gtk_list_store_set(liststore, &iter, j + 1, g_ptr_array_index(players, i), -1);
    }

    return (GtkTreeModel*)liststore;
}

/** Set up the tree view for a player list */
void
treeview_set_up_player_list (GtkTreeView *treeview, gint *attributes, gint max)
{
    gint i;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    gchar *titles[PLAYER_LIST_ATTRIBUTE_END] =
	{_("Name"),
	 _("CPos"),
	 _("Pos"),
	 _("CSk"),
	 _("Sk"),
	 _("Fit"),
	 _("Ga"),
	 _("Go"),
	 _("Status"),
	 _("YC"),
	 _("Age"),
	 _("Etal"),
	 _("Value"),
	 _("Wage"),
	 _("Contract"),
	 _("Team"),
	 _("League")};

    gtk_tree_selection_set_mode(
	gtk_tree_view_get_selection(treeview),
	GTK_SELECTION_SINGLE);
    
    /* number the players */
    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_set_cell_data_func(col, renderer,
					    treeview_cell_int_to_cell,
					    NULL, NULL);
    for(i=0;i<max;i++)
    {
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, titles[attributes[i]]);
	gtk_tree_view_append_column(treeview, col);
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_set_cell_data_func(col, renderer,
						treeview_cell_player_to_cell,
						GINT_TO_POINTER(attributes[i]),
						NULL);

	if(attributes[i] != PLAYER_LIST_ATTRIBUTE_NAME)
	{
	    gtk_tree_view_column_set_alignment(col, 0.5);
	    g_object_set(renderer, "xalign", 0.5,
			 NULL);
	}
    }
}


/** Fill a treeview with the players from the pointer array. Show
    player attributes according to 'attrib'.
    @param treeview The treeview we fill.
    @param players The pointer array with the players. We free it afterwards.
    @param attrib The #PlayerListAttrib that determines which attributes to show.
    @param show_separator Whether we draw a blank line after the 11th player. */
void
treeview_show_player_list(GtkTreeView *treeview, GPtrArray *players, PlayerListAttribute attribute,
			  gboolean show_separator)
{
    gint i, cnt = 0;
    gint columns = math_sum_int_array(attribute.on_off, PLAYER_LIST_ATTRIBUTE_END);
    gint attributes[columns];
    GtkTreeModel *model = NULL;
    
    treeview_clear(treeview);
    gtk_tree_view_set_headers_visible(treeview, TRUE);

    for(i=0;i<PLAYER_LIST_ATTRIBUTE_END;i++)
	if(attribute.on_off[i])
	    attributes[cnt++] = i;

    treeview_set_up_player_list(treeview, attributes, columns);

    model = treeview_create_player_list(players, attributes, columns, show_separator);

    gtk_tree_view_set_model(treeview, model);
    g_object_unref(model);
}

/** Show the list of the user's players in the left view.
    @param player_list The tab we use. */
void
treeview_show_user_player_list(gint player_list)
{
    GtkWidget *treeview = (player_list == 1) ?
	lookup_widget(main_window, "player_list1") :
	lookup_widget(main_window, "player_list2");

    treeview_show_player_list(GTK_TREE_VIEW(treeview), team_get_player_pointers(my_team),
			      player_list_attributes[(player_list != 1)], TRUE);
}

/** Show the commentary and the minute belonging to the unit. 
    @param unit The #LiveGameUnit we show. */
void
treeview_live_game_show_commentary(const LiveGameUnit *unit)
{
    GdkPixbuf *symbol = NULL;
    GtkAdjustment *adjustment =
	gtk_scrolled_window_get_vadjustment(
	    GTK_SCROLLED_WINDOW(lookup_widget(live_game.window,
					      "scrolledwindow9")));
    GtkListStore *liststore =
	GTK_LIST_STORE(
	    gtk_tree_view_get_model(GTK_TREE_VIEW(lookup_widget(live_game.window, "treeview_commentary"))));
    GtkTreeIter iter;
    gchar buf[SMALL];

    symbol = 
	treeview_pixbuf_from_filename(treeview_live_game_icon(unit->event.type));
    sprintf(buf, "%d.", live_game_unit_get_minute(unit));
    gtk_list_store_prepend(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, buf, 1, symbol,
		       2, unit->event.commentary->str, -1);

    if(symbol != NULL)
	g_object_unref(symbol);

    adjustment->value = adjustment->lower - adjustment->page_size;
    gtk_adjustment_value_changed(adjustment);
}

/** Create the list store for the live game 
    commentary treeview.
    @param unit The unit we show.
    @return The liststore. */
GtkTreeModel*
treeview_live_game_create_init_commentary(const LiveGameUnit *unit)
{
    gint i, j;
    GtkListStore  *liststore;
    GtkTreeIter iter;
    GdkPixbuf *symbol = NULL;
    gchar buf[SMALL];

    liststore = gtk_list_store_new(3,
				   G_TYPE_STRING,
				   GDK_TYPE_PIXBUF,
				   G_TYPE_STRING);

    sprintf(buf, "%d.", unit->minute);

    symbol = 
	treeview_pixbuf_from_filename(treeview_live_game_icon(unit->event.type));

    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, buf, 1, symbol, 2,
		       unit->event.commentary->str, -1);

    if(symbol != NULL)
	g_object_unref(symbol);

    return GTK_TREE_MODEL(liststore);
}

/** Set up the commentary treeview for the live game. */
void
treeview_live_game_set_up_commentary(void)
{
    GtkTreeView *treeview =
	GTK_TREE_VIEW(lookup_widget(live_game.window, "treeview_commentary"));
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;

    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(treeview),
				GTK_SELECTION_NONE);
    
    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "text", 0);

    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = gtk_cell_renderer_pixbuf_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "pixbuf", 1);

    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "text", 2);
}

/** Show the first commentary of the live game    
    @param unit The #LiveGameUnit we show. */
void
treeview_live_game_show_initial_commentary(const LiveGameUnit *unit)
{
    GtkTreeView *treeview =
	GTK_TREE_VIEW(lookup_widget(live_game.window, "treeview_commentary"));
    GtkTreeModel *model = NULL;
    
    treeview_clear(treeview);
    gtk_tree_view_set_headers_visible(treeview, FALSE);

    treeview_live_game_set_up_commentary();

    model = treeview_live_game_create_init_commentary(unit);

    gtk_tree_view_set_model(treeview, model);
    g_object_unref(model);
}

/** Fill the liststore for the live game result treeview.
    @param unit The current live game unit.
    @return The liststore we created. */
GtkTreeModel*
treeview_live_game_create_result(const LiveGameUnit *unit)
{
    GtkListStore  *liststore;
    GtkTreeIter iter;
    GdkPixbuf *symbol = NULL;    

    symbol = 
	treeview_pixbuf_from_filename("possession_ball.png");

    liststore = gtk_list_store_new(5,
				   GDK_TYPE_PIXBUF,
				   G_TYPE_POINTER,
				   G_TYPE_POINTER,
				   G_TYPE_POINTER,
				   GDK_TYPE_PIXBUF);

    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, NULL, 1, (gpointer)live_game.fix,
		       2, (gpointer)live_game.fix, 3, (gpointer)live_game.fix, 4, NULL, -1);

    gtk_list_store_set(liststore, &iter, 0 + (unit->possession == 1) * 4, symbol, -1);

    if(symbol != NULL)
	g_object_unref(symbol);

    return GTK_TREE_MODEL(liststore);
}

/** Set up the treeview columns for the result. */
void
treeview_live_game_set_up_result(void)
{
    GtkTreeView *treeview =
	GTK_TREE_VIEW(lookup_widget(live_game.window, "treeview_result"));
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;

    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(treeview),
				GTK_SELECTION_NONE);
    
    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = gtk_cell_renderer_pixbuf_new();
    gtk_tree_view_column_pack_start(col, renderer, FALSE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "pixbuf", 0);

    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_set_cell_data_func(col, renderer,
					    treeview_cell_live_game_result,
					    NULL, NULL);

    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_set_cell_data_func(col, renderer,
					    treeview_cell_live_game_result,
					    NULL, NULL);

    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_set_cell_data_func(col, renderer,
					    treeview_cell_live_game_result,
					    NULL, NULL);

    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = gtk_cell_renderer_pixbuf_new();
    gtk_tree_view_column_pack_start(col, renderer, FALSE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "pixbuf", 4);
}

/** Write the current result of the live game into
    the corresponding treeview.
    @param unit The current live game unit. */
void
treeview_live_game_show_result(const LiveGameUnit *unit)
{
    GtkTreeView *treeview =
	GTK_TREE_VIEW(lookup_widget(live_game.window, "treeview_result"));
    GtkTreeModel *model = NULL;
    
    treeview_clear(treeview);
    gtk_tree_view_set_headers_visible(treeview, FALSE);

    treeview_live_game_set_up_result();

    model = treeview_live_game_create_result(unit);

    gtk_tree_view_set_model(treeview, model);
    g_object_unref(model);    
}
