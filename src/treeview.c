#include <unistd.h>

#include "cup.h"
#include "file.h"
#include "finance.h"
#include "fixture.h"
#include "free.h"
#include "gui.h"
#include "league.h"
#include "live_game.h"
#include "maths.h"
#include "misc.h"
#include "option.h"
#include "support.h"
#include "team.h"
#include "transfer.h"
#include "treeview.h"
#include "treeview_cell.h"
#include "user.h"
#include "variables.h"
#include "window.h"

/** Select the row that's been clicked on. */
void
treeview_select_row(GtkTreeView *treeview, GdkEventButton *event)
{
    GtkTreeSelection *selection =
	gtk_tree_view_get_selection(treeview);
    GtkTreePath *path;
    
    if(!gtk_tree_view_get_path_at_pos(treeview,
				      event->x, event->y,
				      &path, NULL, NULL, NULL))
	return;

    gtk_tree_selection_select_path(selection, path);
    gtk_tree_path_free(path);
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
	GTK_TREE_VIEW(lookup_widget(window.main, "player_info")) :
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

/** Return the filename of the icon going with the LiveGameEvent
    with type event_type.
    @param event_type The type of the event.
    @return A filename specifying a pixmap. */
gchar*
treeview_live_game_icon(gint event_type)
{
    if(event_type == LIVE_GAME_EVENT_START_MATCH ||
       event_type == LIVE_GAME_EVENT_END_MATCH ||
       event_type == LIVE_GAME_EVENT_HALF_TIME ||
       event_type == LIVE_GAME_EVENT_EXTRA_TIME ||
       event_type == LIVE_GAME_EVENT_PENALTIES)
	return const_str("string_live_game_event_start_match_icon");
    else if(event_type == LIVE_GAME_EVENT_LOST_POSSESSION)
	return const_str("string_live_game_event_lost_possession_icon");
    else if(event_type == LIVE_GAME_EVENT_PENALTY)
	return const_str("string_live_game_event_penalty_icon");
    else if(event_type == LIVE_GAME_EVENT_SCORING_CHANCE ||
	    event_type == LIVE_GAME_EVENT_FREE_KICK)
	return const_str("string_live_game_event_scoring_chance_icon");
    else if(event_type == LIVE_GAME_EVENT_GOAL)
	return const_str("string_live_game_event_goal_icon");
    else if(event_type == LIVE_GAME_EVENT_OWN_GOAL)
	return const_str("string_live_game_event_own_goal_icon");
    else if(event_type == LIVE_GAME_EVENT_POST ||
	    event_type == LIVE_GAME_EVENT_CROSS_BAR)
	return const_str("string_live_game_event_post_icon");
    else if(event_type == LIVE_GAME_EVENT_SAVE)
	return const_str("string_live_game_event_save_icon");
    else if(event_type == LIVE_GAME_EVENT_MISSED)
	return const_str("string_live_game_event_missed_icon");
    else if(event_type == LIVE_GAME_EVENT_FOUL)
	return const_str("string_live_game_event_foul_icon");
    else if(event_type == LIVE_GAME_EVENT_FOUL_YELLOW)
	return const_str("string_live_game_event_foul_yellow_icon");
    else if(event_type == LIVE_GAME_EVENT_SEND_OFF)
	return const_str("string_live_game_event_send_off_icon");
    else if(event_type == LIVE_GAME_EVENT_INJURY)
	return const_str("string_live_game_event_injury_icon");
    else if(event_type == LIVE_GAME_EVENT_TEMP_INJURY)
	return const_str("string_live_game_event_temp_injury_icon");
    else if(event_type == LIVE_GAME_EVENT_STADIUM_BREAKDOWN)
	return const_str("string_live_game_event_stadium_breakdown_icon");
    else if(event_type == LIVE_GAME_EVENT_STADIUM_FIRE)
	return const_str("string_live_game_event_stadium_fire_icon");
    else if(event_type == LIVE_GAME_EVENT_STADIUM_RIOTS)
	return const_str("string_live_game_event_stadium_riots_icon");
    else if(event_type == LIVE_GAME_EVENT_SUBSTITUTION)
	return const_str("string_live_game_event_substitution_icon");
    else if(event_type == LIVE_GAME_EVENT_STRUCTURE_CHANGE)
	return const_str("string_live_game_event_structure_change_icon");
    else if(event_type == LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_DEFEND)
	return const_str("string_game_gui_style_all_out_defend_icon");
    else if(event_type == LIVE_GAME_EVENT_STYLE_CHANGE_DEFEND)
	return const_str("string_game_gui_style_defend_icon");
    else if(event_type == LIVE_GAME_EVENT_STYLE_CHANGE_BALANCED)
	return const_str("string_game_gui_style_balanced_icon");
    else if(event_type == LIVE_GAME_EVENT_STYLE_CHANGE_ATTACK)
	return const_str("string_game_gui_style_attack_icon");
    else if(event_type == LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_ATTACK)
	return const_str("string_game_gui_style_all_out_attack_icon");
    else if(event_type == LIVE_GAME_EVENT_BOOST_CHANGE_ANTI)
	return const_str("string_game_gui_boost_anti_icon");
    else if(event_type == LIVE_GAME_EVENT_BOOST_CHANGE_OFF)
	return const_str("string_game_gui_boost_off_icon");
    else if(event_type == LIVE_GAME_EVENT_BOOST_CHANGE_ON)
	return const_str("string_game_gui_boost_on_icon");
    else
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

/**
   Creates the model for the treeview in the team selection window.
   The model contains a list of all the teams from the leagues in
   the country::leagues array; if show_cup_teams is TRUE, the
   teams from international cups are shown, too.
   @param show_cup_teams Whether or not teams from international
   cups are shown.
   @param show_user_teams Whether or not user teams are shown.
   @return The model containing the team names.
*/
GtkTreeModel*
treeview_create_team_selection_list(gboolean show_cup_teams, gboolean show_user_teams)
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
	    if(team_is_user(&g_array_index(lig(i).teams, Team, j)) == -1)
	    {
		gtk_list_store_append(liststore, &iter);
		gtk_list_store_set(liststore, &iter,
				   0, cnt++,
				   1, symbol,
				   2, (gpointer)&g_array_index(lig(i).teams, Team, j),
				   3, lig(i).name->str,
				   -1);
	    }
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
    @param show_user_teams Whether or not user teams are shown.
*/
void
treeview_show_team_list(GtkTreeView *treeview, gboolean show_cup_teams,
			gboolean show_user_teams)
{
    GtkTreeModel *team_list = 
	treeview_create_team_selection_list(show_cup_teams, show_user_teams);
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
	    gtk_list_store_set(liststore, &iter, 0, const_int("int_treeview_cell_int_empty"), -1);
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
	 _("Sh"),
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

	if(attributes[i] != PLAYER_LIST_ATTRIBUTE_NAME &&
	   attributes[i] != PLAYER_LIST_ATTRIBUTE_TEAM &&
	   attributes[i] != PLAYER_LIST_ATTRIBUTE_LEAGUE_CUP)
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
    @param attrib The #PlayerListAttribute that determines which attributes to show.
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

    g_ptr_array_free(players, TRUE);
}

/** Show the list of the user's players in the left view.
    @param user The user we show the players of. */
void
treeview_show_user_player_list(const User *user)
{
    gint i;
    GPtrArray *players = NULL;
    PlayerListAttribute attribute;
    GtkWidget *treeview[2] = 
	{lookup_widget(window.main, "player_list1"),
	 lookup_widget(window.main, "player_list2")};

    for(i=0;i<2;i++)
    {
	players = team_get_player_pointers(user->tm);
	user_set_player_list_attributes(user, &attribute, i + 1);
	treeview_show_player_list(GTK_TREE_VIEW(treeview[i]), players, attribute, TRUE);
    }
}

/** Determine the attributes shown in the player list depending on the scout
    quality. */
PlayerListAttribute
treeview_get_attributes_from_scout(gint scout)
{
    gint i;
    PlayerListAttribute attribute;

    for(i=0;i<PLAYER_LIST_ATTRIBUTE_END;i++)
	attribute.on_off[i] = FALSE;

    attribute.on_off[PLAYER_LIST_ATTRIBUTE_NAME] =
	attribute.on_off[PLAYER_LIST_ATTRIBUTE_POS] =
	attribute.on_off[PLAYER_LIST_ATTRIBUTE_SKILL] =
	attribute.on_off[PLAYER_LIST_ATTRIBUTE_TEAM] =
	attribute.on_off[PLAYER_LIST_ATTRIBUTE_LEAGUE_CUP] = TRUE;

    if(scout < 3)
	attribute.on_off[PLAYER_LIST_ATTRIBUTE_ETAL] = TRUE;
    if(scout < 2)
	attribute.on_off[PLAYER_LIST_ATTRIBUTE_AGE] = TRUE;
    if(scout < 1)
	attribute.on_off[PLAYER_LIST_ATTRIBUTE_GAMES] =
	    attribute.on_off[PLAYER_LIST_ATTRIBUTE_GOALS] = TRUE;

    return attribute;
}

/** Show the player list of a foreign team depending on the
    scout quality. */
void
treeview_show_player_list_team(GtkTreeView *treeview, const Team *tm, gint scout)
{
    GPtrArray *players = team_get_player_pointers(tm);

    treeview_show_player_list(treeview, players, 
			      treeview_get_attributes_from_scout(scout), TRUE);
}

/** Show the commentary and the minute belonging to the unit. 
    @param unit The #LiveGameUnit we show. */
void
treeview_live_game_show_commentary(const LiveGameUnit *unit)
{
    GdkPixbuf *symbol = NULL;
    GtkAdjustment *adjustment =
	gtk_scrolled_window_get_vadjustment(
	    GTK_SCROLLED_WINDOW(lookup_widget(window.live,
					      "scrolledwindow9")));
    GtkListStore *liststore =
	GTK_LIST_STORE(
	    gtk_tree_view_get_model(GTK_TREE_VIEW(lookup_widget(window.live, "treeview_commentary"))));
    GtkTreeIter iter;
    gchar buf[SMALL];

    symbol = 
	treeview_pixbuf_from_filename(treeview_live_game_icon(unit->event.type));
    sprintf(buf, "%3d.", live_game_unit_get_minute(unit));
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
    GtkListStore  *liststore;
    GtkTreeIter iter;
    GdkPixbuf *symbol = NULL;
    gchar buf[SMALL];

    liststore = gtk_list_store_new(3,
				   G_TYPE_STRING,
				   GDK_TYPE_PIXBUF,
				   G_TYPE_STRING);

    sprintf(buf, "%3d.", unit->minute);

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
	GTK_TREE_VIEW(lookup_widget(window.live, "treeview_commentary"));
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
	GTK_TREE_VIEW(lookup_widget(window.live, "treeview_commentary"));
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

    liststore = gtk_list_store_new(3,
				   G_TYPE_POINTER,
				   G_TYPE_POINTER,
				   G_TYPE_POINTER);

    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, (gpointer)usr(stat2).live_game.fix,
		       1, (gpointer)unit, 2, (gpointer)usr(stat2).live_game.fix, -1);

    return GTK_TREE_MODEL(liststore);
}

/** Set up the treeview columns for the result. */
void
treeview_live_game_set_up_result(void)
{
    gint i;
    GtkTreeView *treeview =
	GTK_TREE_VIEW(lookup_widget(window.live, "treeview_result"));
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;

    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(treeview),
				GTK_SELECTION_NONE);
    
    for(i=0;i<3;i++)
    {
	col = gtk_tree_view_column_new();
	gtk_tree_view_append_column(treeview, col);
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_set_cell_data_func(col, renderer,
						treeview_cell_live_game_result,
						NULL, NULL);
    }
}

/** Write the current result of the live game into
    the corresponding treeview.
    @param unit The current live game unit. */
void
treeview_live_game_show_result(const LiveGameUnit *unit)
{
    GtkTreeView *treeview =
	GTK_TREE_VIEW(lookup_widget(window.live, "treeview_result"));
    GtkTreeModel *model = NULL;
    
    treeview_clear(treeview);
    gtk_tree_view_set_headers_visible(treeview, FALSE);

    treeview_live_game_set_up_result();

    model = treeview_live_game_create_result(unit);

    gtk_tree_view_set_model(treeview, model);
    g_object_unref(model);    
}

/** Fill a tree model with the users. */
GtkTreeModel*
treeview_create_users_startup(void)
{
    gint i;
    GtkListStore  *liststore;
    GtkTreeIter iter;

    liststore = gtk_list_store_new(4,
				   G_TYPE_INT,
				   G_TYPE_STRING,
				   G_TYPE_STRING,
				   G_TYPE_STRING);
    for(i=0;i<users->len;i++)
    {
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, i + 1,
			   1, usr(i).name->str,
			   2, usr(i).tm->name->str,
			   -1);

	if(stat0 == STATUS_TEAM_SELECTION && usr(i).scout != 0)
	{
	    if(usr(i).scout == 1)
		gtk_list_store_set(liststore, &iter, 3,
				   lig(0).name->str, -1);
	    else
		gtk_list_store_set(liststore, &iter, 3,
				   lig(ligs->len - 1).name->str, -1);
	}
	else
	    gtk_list_store_set(liststore, &iter, 3,
			       league_from_clid(usr(i).tm->clid)->name->str, -1);
    }

    return GTK_TREE_MODEL(liststore);
}

/** Set up the users treeview.
    @param treeview The treeview we use. */
void
treeview_set_up_users_startup(GtkTreeView *treeview)
{
    gint i;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    gchar *titles[4] =
	{"",
	 _("Name"),
	 _("Team"),
	 _("Start in")};

    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(treeview),
				GTK_SELECTION_SINGLE);

    for(i=0;i<4;i++)
    {
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, titles[i]);
	gtk_tree_view_append_column(treeview, col);
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer,
					   "text", i);
    }    
}

/** Show the list of users at startup.
    @param treeview The treeview we use. */
void
treeview_show_users_startup(void)
{
    GtkTreeView *treeview =
	GTK_TREE_VIEW(lookup_widget(window.startup_users, "treeview_users"));
    GtkTreeModel *model = NULL;

    treeview_clear(treeview);
    gtk_tree_view_set_headers_visible(treeview, TRUE);
    
    treeview_set_up_users_startup(treeview);
    model = treeview_create_users_startup();
    gtk_tree_view_set_model(treeview, model);
    g_object_unref(model);
}

/** Fill a model with live game stats.
    @param live_game The live game.  */
GtkTreeModel*
treeview_create_game_stats(LiveGame *live_game)
{
    gint i, j, k;
    LiveGameStats *stats = &live_game->stats;
    GtkListStore  *liststore;
    GtkTreeIter iter;
    gchar buf[2][SMALL];
    gchar buf3[SMALL];
    gchar *categories[LIVE_GAME_STAT_VALUE_END] =
	{_("Goals (w/o pen.)"),
	 _("Shots"),   
	 _("Shot %"),   
	 _("Possession"),   
	 _("Penalties"),   
	 _("Fouls"),   
	 _("Yellows"),   
	 _("Reds"),   
	 _("Injuries")};

    liststore = gtk_list_store_new(3,
				   G_TYPE_STRING,
				   G_TYPE_STRING,
				   G_TYPE_STRING);

    sprintf(buf[0], "%d : %d",
	    math_sum_int_array(live_game->fix->result[0], 3),
	    math_sum_int_array(live_game->fix->result[1], 3));
    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, live_game->fix->teams[0]->name->str,
		       1, buf[0],
		       2, live_game->fix->teams[1]->name->str,
		       -1);

    for(k=0;k<LIVE_GAME_STAT_ARRAY_END;k++)
    {
	if(k == LIVE_GAME_STAT_ARRAY_SCORERS)
	{
	    strcpy(buf[0], "");
	    strcpy(buf[1], "");
	}
	else
	{
	    strcpy(buf[1], "   </span>");
	    if(k == LIVE_GAME_STAT_ARRAY_INJURED)
		sprintf(buf[0], "<span background='%s'>   ", 
			const_str("string_treeview_cell_color_player_injury"));
	    else if(k == LIVE_GAME_STAT_ARRAY_REDS)
		sprintf(buf[0], "<span background='%s'>   ", 
			const_str("string_treeview_cell_color_player_banned"));
	    else if(k == LIVE_GAME_STAT_ARRAY_YELLOWS)
		sprintf(buf[0], "<span background='%s'>   ", 
			const_str("string_treeview_cell_color_player_yellow_danger"));
	}

	for(i=0;i<MAX(stats->players[0][k]->len,
		      stats->players[1][k]->len);i++)
	{
	    gtk_list_store_append(liststore, &iter);
	    gtk_list_store_set(liststore, &iter, 1, "", -1);
	    for(j=0;j<2;j++)
	    {
		if(i < stats->players[j][k]->len)
		{
		    sprintf(buf3, "%s%s%s", buf[0],
			    ((GString*)g_ptr_array_index(stats->players[j][k], i))->str,
			    buf[1]);
		    gtk_list_store_set(liststore, &iter, j * 2, buf3, -1);
		}
	    }
	}
    }

    misc_print_grouped_int(live_game->fix->attendance, buf[0], FALSE);
    sprintf(buf[1], "Attendance\n%s", buf[0]);
    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, buf[1], 1, "", 2, "", -1);

    if(live_game->stadium_event != -1)
    {
	gtk_list_store_append(liststore, &iter);
	if(live_game->stadium_event == LIVE_GAME_EVENT_STADIUM_BREAKDOWN)
	    sprintf(buf[0], _("<span background='%s'>There were technical problems\nin the stadium.</span>"),
			      const_str("string_treeview_stadium_event_bg"));
	else if(live_game->stadium_event == LIVE_GAME_EVENT_STADIUM_RIOTS)
	    sprintf(buf[0], _("<span background='%s'>There were riots\nin the stadium.</span>"),
		    const_str("string_treeview_stadium_event_bg"));
	else
	    sprintf(buf[0], _("<span background='%s'>There was a fire\nin the stadium.</span>"),
		    const_str("string_treeview_stadium_event_bg"));

	gtk_list_store_set(liststore, &iter, 0, buf[0], 1, "", 2, "", -1);
    }

    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, "", 1, "", 2, "", -1);

    for(i=0;i<LIVE_GAME_STAT_VALUE_END;i++)
    {
	if(i != LIVE_GAME_STAT_VALUE_POSSESSION)
	    for(j=0;j<2;j++)
		sprintf(buf[j], "%d", stats->values[j][i]);
	else
	    for(j=0;j<2;j++)
		sprintf(buf[j], "%d", (gint)rint(100 * ((gfloat)stats->values[j][i] /
							((gfloat)stats->values[0][i] + 
							 (gfloat)stats->values[1][i]))));

	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, buf[0],
			   1, categories[i],
			   2, buf[1], -1);
    }

    return GTK_TREE_MODEL(liststore);
}

/** Configure a treeview to show game stats.
    @param treeview The treeview. */
void
treeview_set_up_game_stats(GtkTreeView *treeview)
{
    gint i;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;

    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(treeview),
				GTK_SELECTION_NONE);

    for(i=0;i<3;i++)
    {
	col = gtk_tree_view_column_new();
	gtk_tree_view_append_column(treeview, col);
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer,
					   "markup", i);

	g_object_set(renderer, "xalign", 0 + (2 - i) * 0.5,
		     NULL);
    }    
}

/** Show the stats of the live game in a treeview.
    @param live_game The live game. */
void
treeview_show_game_stats(GtkTreeView *treeview, LiveGame *live_game)
{
    GtkTreeModel *model = NULL;

    treeview_clear(treeview);
    gtk_tree_view_set_headers_visible(treeview, FALSE);
    
    treeview_set_up_game_stats(treeview);
    model = treeview_create_game_stats(live_game);
    gtk_tree_view_set_model(treeview, model);
    g_object_unref(model);
}

/** Write some general information like cup/league name,
    week etc. into a liststore.
    @param fix A 'sample' fixture.
    @param liststore The liststore we edit. */
void
treeview_create_fixtures_header(const Fixture *fix, GtkListStore *liststore)
{
    GtkTreeIter iter;
    GdkPixbuf *symbol = NULL;
    gchar buf[SMALL], buf2[SMALL], buf3[SMALL],
	round_name[SMALL];
    gchar *name = NULL;

    sprintf(buf3, _("Week %d Round %d"), fix->week_number, fix->week_round_number);

    if(fix->clid < ID_CUP_START)
    {
	name = league_from_clid(fix->clid)->name->str;
	strcpy(round_name, "");
	symbol = treeview_pixbuf_from_filename(league_from_clid(fix->clid)->symbol->str);
    }
    else
    {
	name = cup_from_clid(fix->clid)->name->str;
	cup_round_name(fix, buf);
	sprintf(round_name, "\n%s", buf);
	strcat(buf3, "\n");
	symbol = treeview_pixbuf_from_filename(cup_from_clid(fix->clid)->symbol->str);
    }
    
    sprintf(buf, "<span background='%s' foreground='%s'>%s%s</span>", 
	    const_str("string_treeview_fixture_header_bg"),
	    const_str("string_treeview_fixture_header_fg"),
	    name, round_name);
    sprintf(buf2, "<span background='%s' foreground='%s'>%s</span>",
	    const_str("string_treeview_fixture_header_bg"),
	    const_str("string_treeview_fixture_header_fg"), buf3);

    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, symbol, 1, buf, 2, "", 3, buf2, 4, symbol, -1);

    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, NULL, 1, "", 2, "", 3, "", 4, NULL, -1);

    if(symbol != NULL)
	g_object_unref(symbol);
}

/** Display a fixture in a liststore. */
void
treeview_create_fixture(const Fixture *fix, GtkListStore *liststore)
{
    gint i;
    GtkTreeIter iter;
    GdkPixbuf *symbol[2] = {NULL, NULL};
    gchar buf_result[SMALL], buf[3][SMALL];
    gchar *colour_fg = NULL, *colour_bg = NULL;

    if(fix->clid >= ID_CUP_START &&
       cup_from_clid(fix->clid)->type == CUP_TYPE_INTERNATIONAL)
	for(i=0;i<2;i++)
	    symbol[i] = treeview_pixbuf_from_filename(fix->teams[i]->symbol->str);
    
    if(fixture_user_team_involved(fix) != -1)
    {
	if(fixture_user_team_involved(fix) == cur_user)
	{
	    colour_fg = const_str("string_treeview_current_user_fg");
	    colour_bg = const_str("string_treeview_current_user_bg");
	}
	else
	{
	    colour_fg = const_str("string_treeview_user_fg");
	    colour_bg = const_str("string_treeview_user_bg");
	}
    }
    else
    {
	colour_fg = const_str("string_treeview_cell_color_default_foreground");
	colour_bg = const_str("string_treeview_cell_color_default_background");
    }   

    fixture_result_to_buf(fix, buf_result);

    for(i=0;i<2;i++)
	if(team_rank(fix->teams[i], fix->clid) != -1)
	    sprintf(buf[i], "<span background='%s' foreground='%s'>%s [%d]</span>",
		    colour_bg, colour_fg, fix->teams[i]->name->str,
		    team_rank(fix->teams[i], fix->clid));
	else if(fix->clid >= ID_CUP_START &&
		cup_from_clid(fix->clid)->type == CUP_TYPE_NATIONAL)
	    sprintf(buf[i], "<span background='%s' foreground='%s'>%s (%d)</span>",
		    colour_bg, colour_fg, fix->teams[i]->name->str,
		    league_get_index(fix->teams[i]->clid) + 1);
	else
	    sprintf(buf[i], "<span background='%s' foreground='%s'>%s</span>",
		    colour_bg, colour_fg, fix->teams[i]->name->str);

    sprintf(buf[2], "<span background='%s' foreground='%s'>%s</span>",
	    colour_bg, colour_fg, buf_result);

    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, symbol[0],
		       1, buf[0], 2, buf[2], 3, buf[1], 4, symbol[1], -1);

    for(i=0;i<2;i++)
	if(symbol[i] != NULL)
	    g_object_unref(symbol[i]);
}

GtkTreeModel*
treeview_create_fixtures(gint clid, gint week_number, gint week_round_number)
{
    gint i;
    GtkListStore  *liststore;
    GPtrArray *fixtures = fixture_get_week_round_list(clid, week_number, week_round_number);

    if(fixtures->len == 0)
    {
	free_g_ptr_array(&fixtures);
	return NULL;
    }

    liststore = gtk_list_store_new(5,
				   GDK_TYPE_PIXBUF,
				   G_TYPE_STRING,
				   G_TYPE_STRING,
				   G_TYPE_STRING,
				   GDK_TYPE_PIXBUF);

    treeview_create_fixtures_header((Fixture*)g_ptr_array_index(fixtures, 0),
				    liststore);

    for(i=0;i<fixtures->len;i++)
	treeview_create_fixture((Fixture*)g_ptr_array_index(fixtures, i), liststore);

    free_g_ptr_array(&fixtures);

    return GTK_TREE_MODEL(liststore);
}

void
treeview_set_up_fixtures(GtkTreeView *treeview)
{
    gint i;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;

    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(treeview),
				GTK_SELECTION_NONE);
    
    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = gtk_cell_renderer_pixbuf_new();
    g_object_set(renderer, "cell-background", 
		 const_str("string_treeview_symbol_bg"), NULL);
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "pixbuf", 0);

    for(i=0;i<3;i++)
    {
	col = gtk_tree_view_column_new();
	gtk_tree_view_append_column(treeview, col);
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer,
					   "markup", i + 1);

	g_object_set(renderer, "xalign", 0 + (2 - i) * 0.5,
		     NULL);
    }

    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = gtk_cell_renderer_pixbuf_new();
    g_object_set(renderer, "cell-background", 
		 const_str("string_treeview_symbol_bg"), NULL);
    gtk_tree_view_column_pack_start(col, renderer, FALSE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "pixbuf", 4);
}

/** Show some fixtures in a treeview.
    @param treeview The treeview we use.
    @param clid The cup / league id.
    @param week_number The week number of the fixtures.
    @param week_round_number The week round. */
void
treeview_show_fixtures(GtkTreeView *treeview, gint clid, 
		       gint week_number, gint week_round_number)
{
    GtkTreeModel *model = NULL;

    treeview_clear(treeview);
    gtk_tree_view_set_headers_visible(treeview, FALSE);
    
    treeview_set_up_fixtures(treeview);
    model = treeview_create_fixtures(clid, week_number, week_round_number);
    gtk_tree_view_set_model(treeview, model);
    g_object_unref(model);
}

/** Write the header for a table into the liststore
    @param clid The cup or league id.
    @param number The number of the table if we display more than one. */
void
treeview_table_write_header(GtkListStore *liststore, gint clid, gint number)
{
    gint i;
    gchar buf[SMALL];
    GdkPixbuf *symbol = NULL;
    GtkTreeIter iter;

    if(clid < ID_CUP_START)
    {
	symbol = treeview_pixbuf_from_filename(league_from_clid(clid)->symbol->str);
	strcpy(buf, league_from_clid(clid)->name->str);
    }
    else
    {
	symbol = treeview_pixbuf_from_filename(cup_from_clid(clid)->symbol->str);
	sprintf(buf, _("%s Group %d"), cup_from_clid(clid)->name->str, number);
    }

    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, symbol, 1, "", 2, buf, -1);

    for(i=3;i<11;i++)
	gtk_list_store_set(liststore, &iter, i, "", -1);

    if(symbol != NULL)
	g_object_unref(symbol);
}

/** Get the colours for a team in the tables.
    @param table The table pointer.
    @param idx The index of the element we're looking at.
    @param user Whether to take into account user colours. */
void
treeview_get_table_element_colours(const Table *table, gint idx, gchar *colour_fg, 
				   gchar *colour_bg, gboolean user)
{
    gint i;
    const TableElement *elem = &g_array_index(table->elements, TableElement, idx);
    const PromRelElement *pelem = NULL;
    const League *league = NULL;
    GPtrArray *cup_advance = NULL;
    

    strcpy(colour_fg, const_str("string_treeview_cell_color_default_foreground"));
    strcpy(colour_bg, const_str("string_treeview_cell_color_default_background"));
    
    if(user && elem->team == current_user.tm)
    {
	strcpy(colour_fg, const_str("string_treeview_current_user_fg"));
	strcpy(colour_bg, const_str("string_treeview_current_user_bg"));
    }
    else if(user && team_is_user(elem->team) != -1)
    {
	strcpy(colour_fg, const_str("string_treeview_user_fg"));
	strcpy(colour_bg, const_str("string_treeview_user_bg"));
    }
    else if(table->clid < ID_CUP_START)
    {
	league = league_from_clid(table->clid);

	if(idx + 1 == 1)
	    strcpy(colour_bg, const_str("string_treeview_table_first"));
	else
	{
	    for(i=0;i<league->prom_rel.elements->len;i++)
	    {
		pelem = &g_array_index(league_from_clid(table->clid)->prom_rel.elements, PromRelElement, i);
		if(pelem->ranks[0] <= idx + 1 && idx + 1 <= pelem->ranks[1])
		{
		    if(pelem->type == PROM_REL_PROMOTION)
			strcpy(colour_bg, const_str("string_treeview_table_promotion"));
		    else if(pelem->type == PROM_REL_RELEGATION)
		    strcpy(colour_bg, const_str("string_treeview_table_relegation"));
		}
	    }

	    if(strlen(league->prom_rel.prom_games_dest_sid->str) != 0 &&
	       g_array_index(league->prom_rel.prom_games_cup.choose_teams, 
			     CupChooseTeam, 0).start_idx <= idx + 1 &&
	       idx + 1 <= g_array_index(league->prom_rel.prom_games_cup.choose_teams,
					CupChooseTeam, 0).end_idx)
		strcpy(colour_bg, const_str("string_treeview_table_promgames"));
	}
    }
    else
    {
	cup_advance = 
	    fixture_get_round_robin_advance(cup_from_clid(table->clid), table->round);
	for(i=0;i<cup_advance->len;i++)
	    if((Team*)g_ptr_array_index(cup_advance, i) == elem->team)
		strcpy(colour_bg, const_str("string_treeview_table_promotion"));
	
	free_g_ptr_array(&cup_advance);
    }
}

/** Display a table in the liststore. 
    @param number The number of the table if we display more than one
    (cups, round robin); or -1 for leagues. */
void
treeview_create_single_table(GtkListStore *liststore, const Table *table, gint number)
{
    gint i, j;
    GtkTreeIter iter;
    GdkPixbuf *symbol = NULL;
    TableElement *elem = NULL;
    gchar buf[10][SMALL];
    gchar colour_bg[SMALL], colour_fg[SMALL];

    treeview_table_write_header(liststore, table->clid, number);    

    for(i=0;i<table->elements->len;i++)
    {
	gtk_list_store_append(liststore, &iter);

	elem = &g_array_index(table->elements, TableElement, i);
	if(table->clid >= ID_CUP_START)
	    symbol = treeview_pixbuf_from_filename(elem->team->symbol->str);
	
	gtk_list_store_set(liststore, &iter, 0, symbol, -1);

	treeview_get_table_element_colours(table, i, colour_fg, colour_bg, FALSE);
	sprintf(buf[0], "<span background='%s' foreground = '%s'>%d</span>",
		colour_bg, colour_fg, i + 1);

	treeview_get_table_element_colours(table, i, colour_fg, colour_bg, TRUE);
	sprintf(buf[1], "<span background='%s' foreground = '%s'>%s</span>", 
		colour_bg, colour_fg, elem->team->name->str);

	for(j=2;j<10;j++)
	    if(j - 2 != TABLE_GD)
		sprintf(buf[j], "%d", elem->values[j - 2]);
	    else
		sprintf(buf[j], "%+d", elem->values[j - 2]);

	for(j=0;j<10;j++)
	    gtk_list_store_set(liststore, &iter, j + 1, buf[j], -1);

	if(symbol != NULL)
	    g_object_unref(symbol);
    }

    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, NULL, -1);
    for(j=1;j<11;j++)
	gtk_list_store_set(liststore, &iter, j, "", -1);
}

/** Create a league table or one or more cup tables. */
GtkTreeModel*
treeview_create_table(gint clid)
{
    gint i;
    GArray *tables = NULL;
    GtkListStore *liststore = 
	gtk_list_store_new(11,
			   GDK_TYPE_PIXBUF,
			   G_TYPE_STRING,
			   G_TYPE_STRING,
			   G_TYPE_STRING,
			   G_TYPE_STRING,
			   G_TYPE_STRING,
			   G_TYPE_STRING,
			   G_TYPE_STRING,
			   G_TYPE_STRING,
			   G_TYPE_STRING,
			   G_TYPE_STRING);
    
    if(clid < ID_CUP_START)
	treeview_create_single_table(liststore, 
				     &league_from_clid(clid)->table, -1);
    else
    {
	tables = cup_from_clid(clid)->tables;
	for(i=0;i<tables->len;i++)
	    treeview_create_single_table(liststore, &g_array_index(tables, Table, i), i + 1);
    }

    return GTK_TREE_MODEL(liststore);
}

void
treeview_set_up_table(GtkTreeView *treeview)
{
    gint i;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    gchar *titles[11] =
	{"",
	 "",
	 _("Team"),
	 _("PL"),
	 _("W"),
	 _("D"),
	 _("L"),
	 _("GF"),
	 _("GA"),
	 _("GD"),
	 _("PTS")};

    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(treeview),
				GTK_SELECTION_NONE);
    
    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = gtk_cell_renderer_pixbuf_new();
    g_object_set(renderer, "cell-background", 
		 const_str("string_treeview_symbol_bg"), NULL);
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "pixbuf", 0);

    for(i=1;i<11;i++)
    {
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, titles[i]);
	gtk_tree_view_append_column(treeview, col);
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, FALSE);
	gtk_tree_view_column_add_attribute(col, renderer,
					   "markup", i);

	if(i == 1)
	    g_object_set(renderer, "xalign", 1.0, NULL);
    }
}

/** Show the table going with a league or cup. */
void
treeview_show_table(GtkTreeView *treeview, gint clid)
{
    GtkTreeModel *model = NULL;

    treeview_clear(treeview);
    gtk_tree_view_set_headers_visible(treeview, TRUE);
    
    treeview_set_up_table(treeview);
    model = treeview_create_table(clid);
    gtk_tree_view_set_model(treeview, model);
    g_object_unref(model);
}

GtkTreeModel*
treeview_create_finances(const User* user)
{
    gint i, balance = 0;
    gchar buf[SMALL], buf2[SMALL], buf3[SMALL];
    gint *in = user->money_in[0],
	*out = user->money_out[0];
    gchar *in_titles[MON_IN_TRANSFERS] =
	{_("Prize money"),
	 _("Ticket income")};
    gchar *out_titles[MON_OUT_TRANSFERS] =
	{_("Wages"),
	 _("Physio"),
	 _("Scout"),
	 _("Journey costs"),
	 _("Stadium improvements"),
	 _("Stadium bills")};
    GtkTreeIter iter;
    GtkListStore *liststore = 
	gtk_list_store_new(3,
			   G_TYPE_STRING,
			   G_TYPE_STRING,
			   G_TYPE_STRING);

    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, _("Bi-weekly balance"), 1, "", 2, "", -1);

    for(i=0;i<MON_IN_TRANSFERS;i++)
	if(in[i] != 0)
	{
	    misc_print_grouped_int(in[i], buf, FALSE);
	    gtk_list_store_append(liststore, &iter);
	    gtk_list_store_set(liststore, &iter, 0, in_titles[i], 1, buf, 2, "", -1);
	    balance += in[i];
	}

    if(in[MON_IN_TRANSFERS] != 0 || out[MON_OUT_TRANSFERS] != 0)
    {
	misc_print_grouped_int(in[MON_IN_TRANSFERS], buf, FALSE);
	misc_print_grouped_int(out[MON_IN_TRANSFERS], buf3, FALSE);
	sprintf(buf2, "<span foreground='%s'>%s</span>",
		const_str("string_treeview_finances_expenses_fg"), buf3);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, _("Transfers"), 1, buf, 2, buf2, -1);
	balance += (in[MON_IN_TRANSFERS] + out[MON_OUT_TRANSFERS]);
    }

    for(i=0;i<MON_OUT_TRANSFERS;i++)
	if(out[i] != 0)
	{
	    misc_print_grouped_int(out[i], buf3, FALSE);
	    sprintf(buf, "<span foreground='%s'>%s</span>",
		    const_str("string_treeview_finances_expenses_fg"), buf3);
	    gtk_list_store_append(liststore, &iter);
	    gtk_list_store_set(liststore, &iter, 0, out_titles[i], 1, "", 2, buf, -1);
	    balance += out[i];
	}    

    misc_print_grouped_int(balance, buf, FALSE);
    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, _("Balance"), 1, "", 2, "", -1);
    if(balance >= 0)
	strcpy(buf2, buf);
    else
	sprintf(buf2, "<span foreground='%s'>%s</span>",
		const_str("string_treeview_finances_expenses_fg"), buf);
    gtk_list_store_set(liststore, &iter, 1 + (balance < 0), buf2, -1);

    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, "", 1, "", 2, "", -1);

    misc_print_grouped_int(user->money, buf, FALSE);
    gtk_list_store_append(liststore, &iter);
    if(user->money >= 0)
	strcpy(buf2, buf);
    else
	sprintf(buf2, "<span foreground='%s'>%s</span>",
		const_str("string_treeview_finances_expenses_fg"), buf);
    gtk_list_store_set(liststore, &iter, 0, _("Money"), 1, buf2, 2, "", -1);

    misc_print_grouped_int(finance_team_drawing_credit_loan(user->tm, FALSE), buf, FALSE);
    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, _("Drawing credit"), 1, buf, 2, "", -1);
    
    if(user->debt != 0)
    {
	misc_print_grouped_int(user->debt, buf, FALSE);
	sprintf(buf2, "<span foreground='%s'>%s</span>",
		const_str("string_treeview_finances_expenses_fg"), buf);
	sprintf(buf, _("Debt (repay in %d weeks)"), user->counters[COUNT_USER_LOAN]);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, buf, 1, "", 2, buf2, -1);
    }

    return GTK_TREE_MODEL(liststore);
}

void
treeview_set_up_finances(GtkTreeView *treeview)
{
    gint i;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    gchar *titles[3] =
	{"",
	 _("Income"),
	 _("Expenses")};

    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(treeview),
				GTK_SELECTION_NONE);

    for(i=0;i<3;i++)
    {
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, titles[i]);
	gtk_tree_view_append_column(treeview, col);
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, FALSE);
	gtk_tree_view_column_add_attribute(col, renderer,
					   "markup", i);

	if(i == 0)
	    g_object_set(renderer, "xalign", 1.0, NULL);
    }
}

/** Show the finance overview of the user. */
void
treeview_show_finances(GtkTreeView *treeview, const User* user)
{
    GtkTreeModel *model = NULL;

    treeview_clear(treeview);
    gtk_tree_view_set_headers_visible(treeview, TRUE);
    
    treeview_set_up_finances(treeview);
    model = treeview_create_finances(user);
    gtk_tree_view_set_model(treeview, model);
    g_object_unref(model);
}

/** Show the transfer list. */
void
treeview_show_transfer_list(GtkTreeView *treeview)
{
    gint i;
    GPtrArray *players = g_ptr_array_new();

    for(i=0;i<transfer_list->len;i++)
	g_ptr_array_add(players, trans(i).pl);

    treeview_show_player_list(treeview, players, 
			      treeview_get_attributes_from_scout(current_user.scout), FALSE);
}
