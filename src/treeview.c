#include "cup.h"
#include "finance.h"
#include "fixture.h"
#include "free.h"
#include "game.h"
#include "language.h"
#include "league.h"
#include "live_game.h"
#include "maths.h"
#include "misc.h"
#include "player.h"
#include "option.h"
#include "support.h"
#include "team.h"
#include "transfer.h"
#include "treeview.h"
#include "treeview_helper.h"
#include "user.h"

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
    GtkListStore  *ls;
    GtkTreeIter iter;

    ls = gtk_list_store_new(5,
			    G_TYPE_INT,
			    GDK_TYPE_PIXBUF,
			    G_TYPE_POINTER,
			    G_TYPE_STRING,
			    G_TYPE_POINTER);

    for(i=0;i<ligs->len;i++)
    {
	for(j=0;j<lig(i).teams->len;j++)
	{
	    if(team_is_user(&g_array_index(lig(i).teams, Team, j)) == -1)
	    {
		gtk_list_store_append(ls, &iter);
		treeview_helper_insert_icon(ls, &iter, 1, g_array_index(lig(i).teams, Team, j).symbol->str);
		gtk_list_store_set(ls, &iter,
				   0, cnt++,
				   2, (gpointer)&g_array_index(lig(i).teams, Team, j),
				   3, lig(i).name->str,
				   4, (gpointer)&g_array_index(lig(i).teams, Team, j),
				   -1);
	    }
	}	    
    }

    if(!show_cup_teams)    
	return GTK_TREE_MODEL(ls);

    for(i=0;i<cps->len;i++)
	if(cp(i).teams->len > 0)
	    for(j=0;j<cp(i).teams->len;j++)
	    {
		gtk_list_store_append(ls, &iter);
		treeview_helper_insert_icon(ls, &iter, 1, 
					    ((Team*)g_ptr_array_index(cp(i).teams, j))->symbol->str);
		gtk_list_store_set(ls, &iter,
				   0, cnt++,
				   2, g_ptr_array_index(cp(i).teams, j),
				   3, cp(i).name->str,
				   4, g_ptr_array_index(cp(i).teams, j),
				   -1);
	    }

    gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(ls), 4,
				    treeview_helper_team_compare, GINT_TO_POINTER(TEAM_COMPARE_AV_SKILL), NULL);

    return GTK_TREE_MODEL(ls);
}

/**
   Sets up the treeview for the team selection window.
   Columns and cell renderers are added etc.
   @param treeview The treeview that gets configured.
*/
void
treeview_set_up_team_selection_treeview(GtkTreeView *treeview)
{
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;

    gtk_tree_selection_set_mode(
	gtk_tree_view_get_selection(treeview),
	GTK_SELECTION_BROWSE);
    gtk_tree_view_set_headers_visible(treeview, TRUE);    
    gtk_tree_view_set_rules_hint(treeview, TRUE);

    /* Numbering the teams */
    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = treeview_helper_cell_renderer_text_new();
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
    renderer = treeview_helper_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_set_cell_data_func(col, renderer,
					    treeview_helper_team_selection,
					    NULL, NULL);
    /* League column */
    col = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(col, _("League"));
    gtk_tree_view_append_column(treeview, col);
    renderer = treeview_helper_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "text", 3);

    col = gtk_tree_view_column_new();
    /* Average skill */
    gtk_tree_view_column_set_title(col, _("Av.Sk."));
    gtk_tree_view_column_set_sort_column_id(col, 4);
    gtk_tree_view_append_column(treeview, col);
    renderer = treeview_helper_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_set_cell_data_func(col, renderer,
					    treeview_helper_team_selection,
					    NULL, NULL);

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
    GtkTreePath *path = gtk_tree_path_new_from_string("0");
     
    treeview_helper_clear(treeview);

    treeview_set_up_team_selection_treeview(treeview);
    
    gtk_tree_view_set_model(treeview, team_list);
    
    selection = gtk_tree_view_get_selection(treeview);
    gtk_tree_selection_select_path(selection, path);
    g_object_unref(team_list);
    gtk_tree_path_free(path);
}

/** Create the list store for a player list. 
    @param players The array containing the players.
    @param attributes An array containing the attributes we show.
    @param max The size of the attribute array.
    @param separator Whether we draw a blank line after the 11th player. */
GtkTreeModel*
treeview_create_player_list(GPtrArray *players, gint *attributes, gint max, 
			    gboolean show_separator, gboolean sortable)
{
    gint i, j;
    GtkListStore  *ls;
    GtkTreeIter iter;
    GType types[max + 1];

    types[0] = G_TYPE_INT;
    for(i=0;i<max;i++)
	types[i + 1] = G_TYPE_POINTER;

    ls = gtk_list_store_newv(max + 1, types);

    for(i=0;i<players->len;i++)
    {
	gtk_list_store_append(ls, &iter);
	if(show_separator && i == 11)
	{
	    gtk_list_store_set(ls, &iter, 0, const_int("int_treeview_helper_int_empty"), -1);
	    for(j=0;j<max;j++)
		gtk_list_store_set(ls, &iter, j + 1, NULL, -1);
	    
	    gtk_list_store_append(ls, &iter);
	    gtk_list_store_set(ls, &iter, 0, i + 1, -1);
	}
	else
	    gtk_list_store_set(ls, &iter, 0, i + 1, -1);

	for(j=0;j<max;j++)
	    gtk_list_store_set(ls, &iter, j + 1, g_ptr_array_index(players, i), -1);
    }

    if(sortable)
    {
	for(i=0;i<max;i++)
	    if(attributes[i] == PLAYER_LIST_ATTRIBUTE_POS ||
	       attributes[i] == PLAYER_LIST_ATTRIBUTE_SKILL ||
	       attributes[i] == PLAYER_LIST_ATTRIBUTE_GOALS ||
	       attributes[i] == PLAYER_LIST_ATTRIBUTE_SHOTS ||
	       attributes[i] == PLAYER_LIST_ATTRIBUTE_GAMES ||
	       attributes[i] == PLAYER_LIST_ATTRIBUTE_AGE ||
	       attributes[i] == PLAYER_LIST_ATTRIBUTE_ETAL ||
	       attributes[i] == PLAYER_LIST_ATTRIBUTE_VALUE ||
	       attributes[i] == PLAYER_LIST_ATTRIBUTE_WAGE)
		gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(ls), i + 1,
						treeview_helper_player_compare, GINT_TO_POINTER(attributes[i]), NULL);
    }
    
    return (GtkTreeModel*)ls;
}

/** Set up the tree view for a player list */
void
treeview_set_up_player_list(GtkTreeView *treeview, gint *attributes, gint max,
			    gboolean show_separator, gboolean sortable)
{
    gint i;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    gchar *titles[PLAYER_LIST_ATTRIBUTE_END] =
	{_("Name"),
	 /* Current position of a player. */
	 _("CPos"),
	 /* Position of a player. */
	 _("Pos"),
	 /* Current skill of a player. */
	 _("CSk"),
	 /* Skill of a player. */
	 _("Sk"),
	 /* Fitness of a player. */
	 _("Fit"),
	 /* Games of a player. */
	 _("Ga"),
	 /* Shots of a player. */
	 _("Sh"),
	 /* Goals of a player. */
	 _("Go"),
	 /* Status of a player. */
	 _("Status"),
	 /* Yellow cards of a player. */
	 _("YC"),
	 _("Age"),
	 /* Estimated talent of a player. */
	 _("Etal"),
	 _("Value"),
	 _("Wage"),
	 _("Contract"),
	 _("Team"),
	 _("League")};

    gtk_tree_selection_set_mode(
	gtk_tree_view_get_selection(treeview),
	GTK_SELECTION_SINGLE);
    gtk_tree_view_set_rules_hint(treeview, FALSE);
    gtk_tree_view_set_headers_visible(treeview, TRUE);
    
    /* number the players */
    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = treeview_helper_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_set_cell_data_func(col, renderer,
					    treeview_helper_int_to_cell,
					    NULL, NULL);
    for(i=0;i<max;i++)
    {
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, titles[attributes[i]]);
	gtk_tree_view_append_column(treeview, col);
	renderer = treeview_helper_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_set_cell_data_func(col, renderer,
						treeview_helper_player_to_cell,
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
	
	if(sortable && 
	   (attributes[i] == PLAYER_LIST_ATTRIBUTE_POS ||
	    attributes[i] == PLAYER_LIST_ATTRIBUTE_SKILL ||
	    attributes[i] == PLAYER_LIST_ATTRIBUTE_GOALS ||
	    attributes[i] == PLAYER_LIST_ATTRIBUTE_SHOTS ||
	    attributes[i] == PLAYER_LIST_ATTRIBUTE_GAMES ||
	    attributes[i] == PLAYER_LIST_ATTRIBUTE_AGE ||
	    attributes[i] == PLAYER_LIST_ATTRIBUTE_ETAL ||
	    attributes[i] == PLAYER_LIST_ATTRIBUTE_VALUE ||
	    attributes[i] == PLAYER_LIST_ATTRIBUTE_WAGE))
	    gtk_tree_view_column_set_sort_column_id(col, i + 1);	    
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
    gboolean sortable = (treeview != GTK_TREE_VIEW(lookup_widget(window.main, "player_list1")));
    
    treeview_helper_clear(treeview);

    for(i=0;i<PLAYER_LIST_ATTRIBUTE_END;i++)
	if(attribute.on_off[i])
	    attributes[cnt++] = i;

    treeview_set_up_player_list(treeview, attributes, columns, show_separator, sortable);

    model = treeview_create_player_list(players, attributes, columns, show_separator, sortable);

    gtk_tree_view_set_model(treeview, model);
    g_object_unref(model);

    g_ptr_array_free(players, TRUE);
}

/** Show the list of the current user's players in the left view. */
void
treeview_show_user_player_list(void)
{
    gint i;
    GPtrArray *players = NULL;
    PlayerListAttribute attribute;
    GtkWidget *treeview[2] =
	{lookup_widget(window.main, "player_list1"),
	 lookup_widget(window.main, "player_list2")};

    for(i=0;i<2;i++)
    {
	players = player_get_pointers_from_array(current_user.tm->players);
	user_set_player_list_attributes(&current_user, &attribute, i + 1);
	treeview_show_player_list(GTK_TREE_VIEW(treeview[i]), players, attribute, TRUE);
    }
}

/** Show the player list of a foreign team depending on the
    scout quality. */
void
treeview_show_player_list_team(GtkTreeView *treeview, const Team *tm, gint scout)
{
    GPtrArray *players = player_get_pointers_from_array(tm->players);

    treeview_show_player_list(treeview, players, 
			      treeview_helper_get_attributes_from_scout(scout), TRUE);
}

/** Show the commentary and the minute belonging to the unit. 
    @param unit The #LiveGameUnit we show. */
void
treeview_live_game_show_commentary(const LiveGameUnit *unit)
{
    GtkAdjustment *adjustment =
	gtk_scrolled_window_get_vadjustment(
	    GTK_SCROLLED_WINDOW(lookup_widget(window.live,
					      "scrolledwindow9")));
    GtkListStore *ls =
	GTK_LIST_STORE(
	    gtk_tree_view_get_model(GTK_TREE_VIEW(lookup_widget(window.live, "treeview_commentary"))));
    GtkTreeIter iter;
    gchar buf[SMALL], buf2[SMALL];

    sprintf(buf, "%3d.", live_game_unit_get_minute(unit));
    if(unit->possession == 1)
	sprintf(buf2, "<span background='%s' foreground='%s'>%s</span>",
		const_app("string_treeview_live_game_commentary_away_bg"),
		const_app("string_treeview_live_game_commentary_away_fg"),
		unit->event.commentary->str);
    else
	sprintf(buf2, "<span background='%s' foreground='%s'>%s</span>",
		const_app("string_treeview_helper_color_default_background"),
		const_app("string_treeview_helper_color_default_foreground"),
		unit->event.commentary->str);
    
    gtk_list_store_prepend(ls, &iter);
    treeview_helper_insert_icon(ls, &iter, 1, treeview_helper_live_game_icon(unit->event.type));
    gtk_list_store_set(ls, &iter, 0, buf, 2, buf2, -1);

    adjustment->value = adjustment->lower - adjustment->page_size;
    gtk_adjustment_value_changed(adjustment);
}

/** Create the list store for the live game 
    commentary treeview.
    @param unit The unit we show.
    @return The ls. */
GtkTreeModel*
treeview_live_game_create_init_commentary(const LiveGameUnit *unit)
{
    GtkListStore  *ls;
    GtkTreeIter iter;
    gchar buf[SMALL];

    ls = gtk_list_store_new(3,
			    G_TYPE_STRING,
			    GDK_TYPE_PIXBUF,
			    G_TYPE_STRING);

    sprintf(buf, "%3d.", unit->minute);

    gtk_list_store_append(ls, &iter);
    treeview_helper_insert_icon(ls, &iter, 1, treeview_helper_live_game_icon(unit->event.type));
    gtk_list_store_set(ls, &iter, 0, buf, 2, unit->event.commentary->str, -1);

    return GTK_TREE_MODEL(ls);
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
    gtk_tree_view_set_headers_visible(treeview, FALSE);
    
    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = treeview_helper_cell_renderer_text_new();
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
    renderer = treeview_helper_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "markup", 2);
}

/** Show the first commentary of the live game    
    @param unit The #LiveGameUnit we show. */
void
treeview_live_game_show_initial_commentary(const LiveGameUnit *unit)
{
    GtkTreeView *treeview =
	GTK_TREE_VIEW(lookup_widget(window.live, "treeview_commentary"));
    GtkTreeModel *model = NULL;
    
    treeview_helper_clear(treeview);

    treeview_live_game_set_up_commentary();

    model = treeview_live_game_create_init_commentary(unit);

    gtk_tree_view_set_model(treeview, model);
    g_object_unref(model);
}

/** Fill the ls for the live game result treeview.
    @param unit The current live game unit.
    @return The ls we created. */
GtkTreeModel*
treeview_live_game_create_result(const LiveGameUnit *unit)
{
    GtkListStore  *ls;
    GtkTreeIter iter;
    gchar buf[SMALL];

    ls = gtk_list_store_new(3,
			    G_TYPE_STRING,
			    G_TYPE_STRING,
			    G_TYPE_STRING);

    live_game_unit_result_to_buf(unit, buf, FALSE);
    gtk_list_store_append(ls, &iter);
    gtk_list_store_set(ls, &iter, 
		       0, ((LiveGame*)statp)->team_names[0]->str,
		       1, buf, 
		       2, ((LiveGame*)statp)->team_names[1]->str, -1);

    return GTK_TREE_MODEL(ls);
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
    gtk_tree_view_set_headers_visible(treeview, FALSE);
    
    for(i=0;i<3;i++)
    {
	col = gtk_tree_view_column_new();
	gtk_tree_view_append_column(treeview, col);
	renderer = treeview_helper_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_set_cell_data_func(col, renderer,
						treeview_helper_live_game_result,
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
    
    treeview_helper_clear(treeview);

    treeview_live_game_set_up_result();

    model = treeview_live_game_create_result(unit);

    gtk_tree_view_set_model(treeview, model);
    g_object_unref(model);    
}

/** Fill a tree model with the users. */
GtkTreeModel*
treeview_create_users(void)
{
    gint i;
    GtkListStore  *ls;
    GtkTreeIter iter;

    ls = gtk_list_store_new(4,
			    G_TYPE_INT,
			    G_TYPE_STRING,
			    G_TYPE_STRING,
			    G_TYPE_STRING);
    for(i=0;i<users->len;i++)
    {
	gtk_list_store_append(ls, &iter);
	gtk_list_store_set(ls, &iter, 0, i + 1,
			   1, usr(i).name->str,
			   2, usr(i).tm->name->str,
			   -1);

	if(stat0 == STATUS_TEAM_SELECTION)
	{
	    if(usr(i).scout == -1)
		gtk_list_store_set(ls, &iter, 3,
				   league_from_clid(usr(i).tm->clid)->name->str, -1);
	    else
		gtk_list_store_set(ls, &iter, 3,
				   lig(usr(i).scout).name->str, -1);
	}
	else
	    gtk_list_store_set(ls, &iter, 3,
			       league_from_clid(usr(i).tm->clid)->name->str, -1);
    }

    return GTK_TREE_MODEL(ls);
}

/** Set up the users treeview.
    @param treeview The treeview we use. */
void
treeview_set_up_users(GtkTreeView *treeview)
{
    gint i;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    gchar *titles[4] =
	{"",
	 _("Name"),
	 _("Team"),
	 _("League")};

    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(treeview),
				GTK_SELECTION_SINGLE);
    gtk_tree_view_set_rules_hint(treeview, FALSE);
    gtk_tree_view_set_headers_visible(treeview, TRUE);

    for(i=0;i<4;i++)
    {
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, titles[i]);
	gtk_tree_view_append_column(treeview, col);
	renderer = treeview_helper_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, (i != 3));
	gtk_tree_view_column_add_attribute(col, renderer,
					   "text", i);
    }    
}

/** Show the list of users at startup.
    @param treeview The treeview we use. */
void
treeview_show_users(GtkTreeView *treeview)
{
    GtkTreeModel *model = NULL;

    treeview_helper_clear(treeview);
    
    treeview_set_up_users(treeview);
    model = treeview_create_users();
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
    GtkListStore  *ls;
    GtkTreeIter iter;
    gchar buf[2][SMALL];
    gchar buf3[SMALL];
    gchar *categories[LIVE_GAME_STAT_VALUE_END] =
	/* 'Normal' goals, ie. no penalties or free kicks. */
	{_("Goals (regular)"),
	 _("Shots"),   
	 _("Shot %"),   
	 _("Possession"),   
	 _("Penalties"),   
	 _("Fouls"),   
	 _("Yellows"),   
	 _("Reds"),   
	 _("Injuries")};

    ls = gtk_list_store_new(3,
			    G_TYPE_STRING,
			    G_TYPE_STRING,
			    G_TYPE_STRING);

    fixture_result_to_buf(live_game->fix, buf[0]);
    gtk_list_store_append(ls, &iter);
    gtk_list_store_set(ls, &iter, 0, live_game->fix->teams[0]->name->str,
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
			const_app("string_treeview_helper_color_player_injury"));
	    else if(k == LIVE_GAME_STAT_ARRAY_REDS)
		sprintf(buf[0], "<span background='%s'>   ", 
			const_app("string_treeview_helper_color_player_banned"));
	    else if(k == LIVE_GAME_STAT_ARRAY_YELLOWS)
		sprintf(buf[0], "<span background='%s'>   ", 
			const_app("string_treeview_helper_color_player_yellow_danger"));
	}

	for(i=0;i<MAX(stats->players[0][k]->len,
		      stats->players[1][k]->len);i++)
	{
	    gtk_list_store_append(ls, &iter);
	    gtk_list_store_set(ls, &iter, 1, "", -1);
	    for(j=0;j<2;j++)
	    {
		if(i < stats->players[j][k]->len)
		{
		    sprintf(buf3, "%s%s%s", buf[0],
			    ((GString*)g_ptr_array_index(stats->players[j][k], i))->str,
			    buf[1]);
		    gtk_list_store_set(ls, &iter, j * 2, buf3, -1);
		}
	    }
	}
    }

    misc_print_grouped_int(live_game->fix->attendance, buf[0]);
    sprintf(buf[1], _("Attendance\n%s"), buf[0]);
    gtk_list_store_append(ls, &iter);
    gtk_list_store_set(ls, &iter, 0, buf[1], 1, "", 2, "", -1);

    if(live_game->stadium_event != -1)
    {
	gtk_list_store_append(ls, &iter);
	if(live_game->stadium_event == LIVE_GAME_EVENT_STADIUM_BREAKDOWN)
	    sprintf(buf[0], _("<span background='%s'>There were technical problems\nin the stadium.</span>"),
		    const_app("string_treeview_stadium_event_bg"));
	else if(live_game->stadium_event == LIVE_GAME_EVENT_STADIUM_RIOTS)
	    sprintf(buf[0], _("<span background='%s'>There were riots\nin the stadium.</span>"),
		    const_app("string_treeview_stadium_event_bg"));
	else
	    sprintf(buf[0], _("<span background='%s'>There was a fire\nin the stadium.</span>"),
		    const_app("string_treeview_stadium_event_bg"));

	gtk_list_store_set(ls, &iter, 0, buf[0], 1, "", 2, "", -1);
    }

    gtk_list_store_append(ls, &iter);
    gtk_list_store_set(ls, &iter, 0, "", 1, "", 2, "", -1);

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

	gtk_list_store_append(ls, &iter);
	gtk_list_store_set(ls, &iter, 0, buf[0],
			   1, categories[i],
			   2, buf[1], -1);
    }

    return GTK_TREE_MODEL(ls);
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
    gtk_tree_view_set_rules_hint(treeview, FALSE);
    gtk_tree_view_set_headers_visible(treeview, FALSE);

    for(i=0;i<3;i++)
    {
	col = gtk_tree_view_column_new();
	gtk_tree_view_append_column(treeview, col);
	renderer = treeview_helper_cell_renderer_text_new();
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

    treeview_helper_clear(treeview);
    
    treeview_set_up_game_stats(treeview);
    model = treeview_create_game_stats(live_game);
    gtk_tree_view_set_model(treeview, model);
    g_object_unref(model);
}

/** Write some general information like cup/league name,
    week etc. into a ls.
    @param fix A 'sample' fixture.
    @param ls The ls we edit.
    @param blank_line Whether to draw a blank line after the header. */
void
treeview_create_fixtures_header(const Fixture *fix, GtkListStore *ls, gboolean blank_line)
{
    GtkTreeIter iter;
    gchar buf[SMALL], buf2[SMALL], buf3[SMALL],
	round_name[SMALL];
    gchar *name = NULL;
    gchar *symbol = NULL;

    sprintf(buf3, _("Week %d Round %d"), fix->week_number, fix->week_round_number);

    if(fix->clid < ID_CUP_START)
    {
	name = league_from_clid(fix->clid)->name->str;
	strcpy(round_name, "");
	symbol = league_from_clid(fix->clid)->symbol->str;
    }
    else
    {
	name = cup_from_clid(fix->clid)->name->str;
	cup_round_name(fix, buf);
	sprintf(round_name, "\n%s", buf);
	strcat(buf3, "\n");
	symbol = cup_from_clid(fix->clid)->symbol->str;
    }
    
    sprintf(buf, "<span background='%s' foreground='%s'>%s%s</span>", 
	    const_app("string_treeview_fixture_header_bg"),
	    const_app("string_treeview_fixture_header_fg"),
	    name, round_name);
    sprintf(buf2, "<span background='%s' foreground='%s'>%s</span>",
	    const_app("string_treeview_fixture_header_bg"),
	    const_app("string_treeview_fixture_header_fg"), buf3);

    gtk_list_store_append(ls, &iter);
    treeview_helper_insert_icon(ls, &iter, 0, symbol);
    treeview_helper_insert_icon(ls, &iter, 4, symbol);
    gtk_list_store_set(ls, &iter, 1, buf, 2, "", 3, buf2, -1);

    if(blank_line)
    {
	gtk_list_store_append(ls, &iter);
	gtk_list_store_set(ls, &iter, 0, NULL, 1, "", 2, "", 3, "", 4, NULL, -1);
    }
}

/** Display a fixture in a ls. */
void
treeview_create_fixture(const Fixture *fix, GtkListStore *ls)
{
    gint i, rank;
    GtkTreeIter iter;
    gchar *symbol[2] = {NULL, NULL};
    gchar buf_result[SMALL], buf[3][SMALL];
    gchar *colour_fg = NULL, *colour_bg = NULL;

    if(fix->clid >= ID_CUP_START &&
       query_cup_is_international(fix->clid))
	for(i=0;i<2;i++)
	    symbol[i] = fix->teams[i]->symbol->str;
    
    if(fixture_user_team_involved(fix) != -1)
	treeview_helper_set_user_colours(usr(fixture_user_team_involved(fix)).tm,
					 &colour_bg, &colour_fg);
    else
    {
	colour_fg = const_app("string_treeview_helper_color_default_foreground");
	colour_bg = const_app("string_treeview_helper_color_default_background");
    }

    fixture_result_to_buf(fix, buf_result);

    for(i=0;i<2;i++)
	if(query_fixture_has_tables(fix))
	{
	    if(fix->clid < ID_CUP_START)
		rank = team_get_league_rank(fix->teams[i]);
	    else
		rank = team_get_cup_rank(fix->teams[i], cup_get_last_tables_round(fix->clid));

	    sprintf(buf[i], "<span background='%s' foreground='%s'>%s [%d]</span>",
		    colour_bg, colour_fg, fix->teams[i]->name->str, rank);
	}
	else if(fix->clid >= ID_CUP_START &&
		query_cup_is_national(fix->clid))
	    sprintf(buf[i], "<span background='%s' foreground='%s'>%s (%d)</span>",
		    colour_bg, colour_fg, fix->teams[i]->name->str,
		    league_from_clid(fix->teams[i]->clid)->layer);
	else
	    sprintf(buf[i], "<span background='%s' foreground='%s'>%s</span>",
		    colour_bg, colour_fg, fix->teams[i]->name->str);

    sprintf(buf[2], "<span background='%s' foreground='%s'>%s</span>",
	    colour_bg, colour_fg, buf_result);

    gtk_list_store_append(ls, &iter);
    treeview_helper_insert_icon(ls, &iter, 0, symbol[0]);
    treeview_helper_insert_icon(ls, &iter, 4, symbol[1]);
    gtk_list_store_set(ls, &iter,
		       1, buf[0], 2, buf[2], 3, buf[1], -1);
}

GtkTreeModel*
treeview_create_fixtures(gint clid, gint week_number, gint week_round_number)
{
    gint i;
    GtkListStore  *ls;
    GPtrArray *fixtures = fixture_get_week_list_clid(clid, week_number, week_round_number);

    if(fixtures->len == 0)
    {
	free_g_ptr_array(&fixtures);
	return NULL;
    }

    ls = gtk_list_store_new(5,
			    GDK_TYPE_PIXBUF,
			    G_TYPE_STRING,
			    G_TYPE_STRING,
			    G_TYPE_STRING,
			    GDK_TYPE_PIXBUF);

    treeview_create_fixtures_header((Fixture*)g_ptr_array_index(fixtures, 0),
				    ls, TRUE);

    for(i=0;i<fixtures->len;i++)
	treeview_create_fixture((Fixture*)g_ptr_array_index(fixtures, i), ls);

    g_ptr_array_free(fixtures, TRUE);

    return GTK_TREE_MODEL(ls);
}

void
treeview_set_up_fixtures(GtkTreeView *treeview)
{
    gint i;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;

    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(treeview),
				GTK_SELECTION_NONE);
    gtk_tree_view_set_rules_hint(treeview, FALSE);
    gtk_tree_view_set_headers_visible(treeview, FALSE);

    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = gtk_cell_renderer_pixbuf_new();
    g_object_set(renderer, "cell-background", 
		 const_app("string_treeview_symbol_bg"), NULL);
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "pixbuf", 0);

    for(i=0;i<3;i++)
    {
	col = gtk_tree_view_column_new();
	gtk_tree_view_append_column(treeview, col);
	renderer = treeview_helper_cell_renderer_text_new();
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
		 const_app("string_treeview_symbol_bg"), NULL);
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

    treeview_helper_clear(treeview);
    
    treeview_set_up_fixtures(treeview);
    model = treeview_create_fixtures(clid, week_number, week_round_number);
    gtk_tree_view_set_model(treeview, model);
    g_object_unref(model);
}

/** Write the header for a table into the ls
    @param clid The cup or league id.
    @param number The number of the table if we display more than one. */
void
treeview_table_write_header(GtkListStore *ls, const Table *table, gint number)
{
    gint i;
    gchar buf[SMALL];
    gchar *symbol = NULL;
    GtkTreeIter iter;

    if(table->clid < ID_CUP_START)
    {
	symbol = league_from_clid(table->clid)->symbol->str;
	strcpy(buf, league_from_clid(table->clid)->name->str);
    }
    else
    {
	symbol = cup_from_clid(table->clid)->symbol->str;
	if(g_array_index(cup_from_clid(table->clid)->rounds, CupRound,
			 table->round).tables->len > 1)			 
	    /*  A group of a round robin stage of a cup. */
	    sprintf(buf, _("%s Group %d"), cup_from_clid(table->clid)->name->str, number);
	else
	    sprintf(buf, "%s", cup_from_clid(table->clid)->name->str);
    }

    gtk_list_store_append(ls, &iter);
    treeview_helper_insert_icon(ls, &iter, 0, symbol);
    gtk_list_store_set(ls, &iter, 1, "", 2, NULL, 3, buf, -1);

    for(i=4;i<12;i++)
	gtk_list_store_set(ls, &iter, i, "", -1);
}

/** Display a table in the ls. 
    @param number The number of the table if we display more than one
    (cups, round robin); or -1 for leagues. */
void
treeview_create_single_table(GtkListStore *ls, const Table *table, gint number)
{
    gint i, j;
    GtkTreeIter iter;
    TableElement *elem = NULL;
    gchar buf[10][SMALL];
    gchar *colour_bg = NULL, *colour_fg = NULL;

    treeview_table_write_header(ls, table, number);

    for(i=0;i<table->elements->len;i++)
    {
	gtk_list_store_append(ls, &iter);

	elem = &g_array_index(table->elements, TableElement, i);

	if(table->clid >= ID_CUP_START)
	    treeview_helper_insert_icon(ls, &iter, 0, elem->team->symbol->str);
	
	if(elem->old_rank > i)
	    treeview_helper_insert_icon(ls, &iter, 2, 
					const_app("string_treeview_table_up_icon"));
	else if(elem->old_rank < i)
	    treeview_helper_insert_icon(ls, &iter, 2, 
					const_app("string_treeview_table_down_icon"));
	else
	    treeview_helper_insert_icon(ls, &iter, 2, 
					const_app("string_treeview_table_stay_icon"));

	treeview_helper_get_table_element_colours(table, i, &colour_fg, &colour_bg, FALSE);
	sprintf(buf[0], "<span background='%s' foreground='%s'>%d</span>",
		colour_bg, colour_fg, i + 1);

	treeview_helper_get_table_element_colours(table, i, &colour_fg, &colour_bg, TRUE);
	sprintf(buf[1], "<span background='%s' foreground='%s'>%s</span>", 
		colour_bg, colour_fg, elem->team->name->str);

	gtk_list_store_set(ls, &iter, 1, buf[0], 3, buf[1], -1);

	for(j=2;j<10;j++)
	    if(j - 2 != TABLE_GD)
		sprintf(buf[j], "%d", elem->values[j - 2]);
	    else
		sprintf(buf[j], "%+d", elem->values[j - 2]);

	for(j=2;j<10;j++)
	    gtk_list_store_set(ls, &iter, j + 2, buf[j], -1);
    }

    gtk_list_store_append(ls, &iter);
    gtk_list_store_set(ls, &iter, 0, NULL, 1, "", 2, NULL, -1);
    for(j=3;j<11;j++)
	gtk_list_store_set(ls, &iter, j, "", -1);
}

/** Create a league table or one or more cup tables. */
GtkTreeModel*
treeview_create_table(gint clid)
{
    gint i;
    GtkListStore *ls = 
	gtk_list_store_new(12,
			   GDK_TYPE_PIXBUF,
			   G_TYPE_STRING,
			   GDK_TYPE_PIXBUF,
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
	treeview_create_single_table(ls, 
				     &league_from_clid(clid)->table, -1);
    else
    {
	for(i=0;i<cup_get_last_tables(clid)->len;i++)
	    treeview_create_single_table(ls, 
					 &g_array_index(cup_get_last_tables(clid), Table, i), i + 1);
    }

    return GTK_TREE_MODEL(ls);
}

void
treeview_set_up_table(GtkTreeView *treeview)
{
    gint i;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    gchar *titles[9] =
	{_("Team"),
	 /* Games played (a number). */
	 _("PL"),
	 /* Games won. */
	 _("W"),
	 /* Games drawn. */
	 _("Dw"),
	 /* Games lost. */
	 _("L"),
	 /* Goals for. */
	 _("GF"),
	 /* Goals against. */
	 _("GA"),
	 /* Goal difference. */
	 _("GD"),
	 /* Points. */
	 _("PTS")};

    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(treeview),
				GTK_SELECTION_NONE);
    gtk_tree_view_set_rules_hint(treeview, FALSE);
    gtk_tree_view_set_headers_visible(treeview, TRUE);

    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = gtk_cell_renderer_pixbuf_new();
    g_object_set(renderer, "cell-background", 
		 const_app("string_treeview_symbol_bg"), NULL);
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "pixbuf", 0);

    col = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(col, "");
    gtk_tree_view_append_column(treeview, col);
    renderer = treeview_helper_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, FALSE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "markup", 1);
    g_object_set(renderer, "xalign", 0.5, NULL);

    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = gtk_cell_renderer_pixbuf_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "pixbuf", 2);

    for(i=0;i<9;i++)
    {
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, titles[i]);
	gtk_tree_view_append_column(treeview, col);
	renderer = treeview_helper_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, FALSE);
	gtk_tree_view_column_add_attribute(col, renderer,
					   "markup", i + 3);
	g_object_set(renderer, "xalign", 0.5 - (0.5 * (i == 0)), NULL);
    }
}

/** Show the table going with a league or cup. */
void
treeview_show_table(GtkTreeView *treeview, gint clid)
{
    GtkTreeModel *model = NULL;

    treeview_helper_clear(treeview);
    
    treeview_set_up_table(treeview);
    model = treeview_create_table(clid);
    gtk_tree_view_set_model(treeview, model);
    g_object_unref(model);
}

/** Print some quick info about the stadium in the finances view. */
void
treeview_create_stadium_summary(GtkListStore *ls)
{
    gchar buf[SMALL];
    GtkTreeIter iter;

    if(current_user.tm->stadium.name != NULL)
    {
	gtk_list_store_append(ls, &iter);
	gtk_list_store_set(ls, &iter, 0, _("Stadium"), 
			   1, current_user.tm->stadium.name->str, 2, "", -1);
    }

    gtk_list_store_append(ls, &iter);
    misc_print_grouped_int(current_user.tm->stadium.capacity, buf);
    gtk_list_store_set(ls, &iter, 0, _("Stadium capacity"), 1, buf, 2, "", -1);

    gtk_list_store_append(ls, &iter);
    sprintf(buf, "%.0f%%", current_user.tm->stadium.safety * 100);
    gtk_list_store_set(ls, &iter, 0, _("Stadium safety"), 1, buf, 2, "", -1);

    if(current_user.counters[COUNT_USER_STADIUM_CAPACITY] + 
       current_user.counters[COUNT_USER_STADIUM_SAFETY] != 0)
    {
	gtk_list_store_append(ls, &iter);
	sprintf(buf, _("Improvement in progress.\n%d seats and %d%% safety still to be done.\nExpected finish: %d weeks."),
		current_user.counters[COUNT_USER_STADIUM_CAPACITY],
		current_user.counters[COUNT_USER_STADIUM_SAFETY],
		MAX(finance_get_stadium_improvement_duration(
			(gfloat)current_user.counters[COUNT_USER_STADIUM_CAPACITY], TRUE),
		    finance_get_stadium_improvement_duration(
			(gfloat)current_user.counters[COUNT_USER_STADIUM_SAFETY] / 100, FALSE)));

	gtk_list_store_set(ls, &iter, 0, _("Stadium status"), 1, buf, 2, "", -1);
    }
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
	 _("Ticket income"),
	 _("Sponsorship")};
    gchar *out_titles[MON_OUT_TRANSFERS] =
	{_("Wages"),
	 _("Physio"),
	 _("Scout"),
	 _("Youth coach"),
	 _("Youth academy"),
	 _("Journey costs"),
	 /* Money paid to players a user fired. */
	 _("Compensations")};

    GtkTreeIter iter;
    GtkListStore *ls = 
	gtk_list_store_new(3,
			   G_TYPE_STRING,
			   G_TYPE_STRING,
			   G_TYPE_STRING);

    gtk_list_store_append(ls, &iter);
    /* Finances balance. */
    gtk_list_store_set(ls, &iter, 0, _("Bi-weekly balance"), 1, "", 2, "", -1);

    for(i=0;i<MON_IN_TRANSFERS;i++)
	if(in[i] != 0)
	{
	    misc_print_grouped_int(in[i], buf);
	    gtk_list_store_append(ls, &iter);
	    gtk_list_store_set(ls, &iter, 0, in_titles[i], 1, buf, 2, "", -1);
	    balance += in[i];
	}

    for(i=0;i<MON_OUT_TRANSFERS;i++)
	if(out[i] != 0)
	{
	    misc_print_grouped_int(out[i], buf3);
	    sprintf(buf, "<span foreground='%s'>%s</span>",
		    const_app("string_treeview_finances_expenses_fg"), buf3);
	    gtk_list_store_append(ls, &iter);
	    gtk_list_store_set(ls, &iter, 0, out_titles[i], 1, "", 2, buf, -1);
	    balance += out[i];
	}    

    misc_print_grouped_int(balance, buf);
    gtk_list_store_append(ls, &iter);
    /* Finances balance. */
    gtk_list_store_set(ls, &iter, 0, _("Balance"), 1, "", 2, "", -1);
    if(balance >= 0)
	strcpy(buf2, buf);
    else
	sprintf(buf2, "<span foreground='%s'>%s</span>",
		const_app("string_treeview_finances_expenses_fg"), buf);
    gtk_list_store_set(ls, &iter, 1 + (balance < 0), buf2, -1);

    gtk_list_store_append(ls, &iter);
    gtk_list_store_set(ls, &iter, 0, "", 1, "", 2, "", -1);

    if(in[MON_IN_TRANSFERS] != 0 || out[MON_OUT_TRANSFERS] != 0)
    {
	misc_print_grouped_int(in[MON_IN_TRANSFERS], buf);
	misc_print_grouped_int(out[MON_OUT_TRANSFERS], buf3);
	sprintf(buf2, "<span foreground='%s'>%s</span>",
		const_app("string_treeview_finances_expenses_fg"), buf3);
	gtk_list_store_append(ls, &iter);
	gtk_list_store_set(ls, &iter, 0, _("Transfers"), 1, buf, 2, buf2, -1);
    }

    if(out[MON_OUT_STADIUM_IMPROVEMENT] + out[MON_OUT_STADIUM_BILLS] != 0)
    {
	misc_print_grouped_int(out[MON_OUT_STADIUM_IMPROVEMENT] + out[MON_OUT_STADIUM_BILLS], 
			       buf);
	sprintf(buf2, "<span foreground='%s'>%s</span>",
		const_app("string_treeview_finances_expenses_fg"), buf);
	gtk_list_store_append(ls, &iter);
	gtk_list_store_set(ls, &iter, 0, _("Stadium expenses"), 1, "", 2, buf2, -1);
    }

    gtk_list_store_append(ls, &iter);
    gtk_list_store_set(ls, &iter, 0, "", 1, "", 2, "", -1);

    misc_print_grouped_int(user->money, buf);
    gtk_list_store_append(ls, &iter);
    if(user->money >= 0)
	strcpy(buf2, buf);
    else
	sprintf(buf2, "<span foreground='%s'>%s</span>",
		const_app("string_treeview_finances_expenses_fg"), buf);
    gtk_list_store_set(ls, &iter, 0, _("Money"), 1, buf2, 2, "", -1);

    misc_print_grouped_int(finance_team_drawing_credit_loan(user->tm, FALSE), buf);
    gtk_list_store_append(ls, &iter);
    gtk_list_store_set(ls, &iter, 0, _("Drawing credit"), 1, buf, 2, "", -1);
    
    if(user->debt != 0)
    {
	misc_print_grouped_int(user->debt, buf);
	sprintf(buf2, "<span foreground='%s'>%s</span>",
		const_app("string_treeview_finances_expenses_fg"), buf);
	sprintf(buf, _("Debt (repay in %d weeks)"), user->counters[COUNT_USER_LOAN]);
	gtk_list_store_append(ls, &iter);
	gtk_list_store_set(ls, &iter, 0, buf, 1, "", 2, buf2, -1);
    }

    gtk_list_store_append(ls, &iter);
    gtk_list_store_set(ls, &iter, 0, "", 1, "", 2, "", -1);

    gtk_list_store_append(ls, &iter);
    gtk_list_store_set(ls, &iter, 0, _("Sponsor"), 1, user->sponsor.name->str,
		       2, "", -1);

    misc_print_grouped_int(user->sponsor.benefit, buf);
    /* Contract time and money a sponsor pays. */
    sprintf(buf2, _("%.1f months / %s"), ((gfloat)user->sponsor.contract) / 4, buf);
    gtk_list_store_append(ls, &iter);
    gtk_list_store_set(ls, &iter, 0, _("Contract / Money"), 1, buf2, 2, "", -1);

    gtk_list_store_append(ls, &iter);
    gtk_list_store_set(ls, &iter, 0, "", 1, "", 2, "", -1);

    sprintf(buf, "%d%%", user->youth_academy.percentage);
    gtk_list_store_append(ls, &iter);
    gtk_list_store_set(ls, &iter, 0, _("Youth acad. invest."), 1, buf, 2, "", -1);

    gtk_list_store_append(ls, &iter);
    gtk_list_store_set(ls, &iter, 0, "", 1, "", 2, "", -1);

    treeview_create_stadium_summary(ls);

    return GTK_TREE_MODEL(ls);
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
    gtk_tree_view_set_rules_hint(treeview, FALSE);
    gtk_tree_view_set_headers_visible(treeview, TRUE);

    for(i=0;i<3;i++)
    {
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, titles[i]);
	gtk_tree_view_append_column(treeview, col);
	renderer = treeview_helper_cell_renderer_text_new();
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

    treeview_helper_clear(treeview);
    
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
	g_ptr_array_add(players, player_of_id_team(trans(i).tm, trans(i).id));

    treeview_show_player_list(treeview, players, 
			      treeview_helper_get_attributes_from_scout(current_user.scout), FALSE);
}

/** Create attack, midfield and defend bars. */
void
treeview_create_next_opponent_values(GtkListStore *ls, const Fixture *fix)
{
    gint i, j;
    gchar buf[SMALL], buf2[SMALL];
    gfloat max_values[3],
	team_values[2][GAME_TEAM_VALUE_END];
    gchar *titles[3] =
	{_("Defend"),
	 _("Midfield"),
	 _("Attack")};
    GtkTreeIter iter;
    
    game_get_values(fix, team_values, 0);
    game_get_max_values(max_values);

    for(i=0;i<3;i++)
    {
	strcpy(buf2, "");
	for(j=0;j<(gint)rint((gfloat)const_int("int_treeview_max_pipes") *
			     (team_values[fix->teams[0] == current_user.tm][i] / max_values[i]));j++)
	    strcat(buf2, "|");

	sprintf(buf, "%s\n<span foreground='%s'>", buf2,
		const_app("string_treeview_opponent_value_colour_fg"));
	for(j=0;j<(gint)rint((gfloat)const_int("int_treeview_max_pipes") *
			     (team_values[fix->teams[0] != current_user.tm][i] / max_values[i]));j++)
	    strcat(buf, "|");
	strcat(buf, "</span>");

	gtk_list_store_append(ls, &iter);
	gtk_list_store_set(ls, &iter, 0, titles[i], 1, buf, -1);
    }
}

GtkTreeModel*
treeview_create_next_opponent(void)
{
    gchar buf[SMALL], buf2[SMALL];
    const Fixture *fix = team_get_fixture(current_user.tm, FALSE);
    const Team *opp = (fix == NULL) ? NULL :
	fix->teams[fix->teams[0] == current_user.tm];
    GtkListStore *ls = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
    GtkTreeIter iter;

    if(opp == NULL)
	return NULL;
    
    if(fix->clid < ID_CUP_START)
	strcpy(buf, league_cup_get_name_string(fix->clid));
    else
    {
	cup_round_name(fix, buf2);
	sprintf(buf, "%s (%s)", league_cup_get_name_string(fix->clid), buf2);
    }		

    gtk_list_store_append(ls, &iter);
    gtk_list_store_set(ls, &iter, 0, _("Your next opponent"), 1, buf, -1);

    gtk_list_store_append(ls, &iter);

    if(fix->week_number == week && 
       fix->week_round_number == week_round)
	sprintf(buf, _("<span foreground='%s' background='%s'>Week %d Round %d</span>"),
		const_app("string_treeview_helper_color_default_foreground"),
		const_app("string_treeview_helper_color_default_background"),
		fix->week_number, fix->week_round_number);
    else
	sprintf(buf, _("<span foreground='%s' background='%s'>Week %d Round %d</span>"),
		const_app("string_treeview_opponent_match_later_fg"),
		const_app("string_treeview_opponent_match_later_bg"),
		fix->week_number, fix->week_round_number);	

    if(!fix->home_advantage)
	strcpy(buf2, _("Neutral ground"));
    else if(fix->teams[0] == current_user.tm)
	strcpy(buf2, _("Home"));
    else
	strcpy(buf2, _("Away"));
    gtk_list_store_set(ls, &iter, 0, buf, 1, buf2, -1);

    gtk_list_store_append(ls, &iter);
    gtk_list_store_set(ls, &iter, 0, "", 1, "", -1);

    gtk_list_store_append(ls, &iter);
    gtk_list_store_set(ls, &iter, 0, _("Team"), 1, opp->name->str, -1);
    
    if(opp->clid < ID_CUP_START)
    {
	sprintf(buf, "%d (%s)", team_get_league_rank(opp), league_cup_get_name_string(opp->clid));
	gtk_list_store_append(ls, &iter);
	gtk_list_store_set(ls, &iter, 0, _("Rank"), 1, buf, -1);
    }

    sprintf(buf, "%.1f", team_get_average_skill(opp, TRUE));
    if(team_get_average_skill(opp, TRUE) >
       team_get_average_skill(current_user.tm, TRUE))
	sprintf(buf2, " (<span foreground='%s'>%+.1f</span>)",
		const_app("string_treeview_opponent_skill_positive_fg"),
		team_get_average_skill(opp, TRUE) -
		team_get_average_skill(current_user.tm, TRUE));
    else
	sprintf(buf2, " (<span foreground='%s'>%+.1f</span>)",
		const_app("string_treeview_opponent_skill_negative_fg"),
		team_get_average_skill(opp, TRUE) -
		team_get_average_skill(current_user.tm, TRUE));

    strcat(buf, buf2);
    gtk_list_store_append(ls, &iter);
    gtk_list_store_set(ls, &iter, 0, _("Average skill"), 1, buf, -1);

    gtk_list_store_append(ls, &iter);
    gtk_list_store_set(ls, &iter, 0, _("Playing style"), 1, 
		       team_attribute_to_char(TEAM_ATTRIBUTE_STYLE, opp->style), -1);

    sprintf(buf, "%d", opp->structure);
    gtk_list_store_append(ls, &iter);
    gtk_list_store_set(ls, &iter, 0, _("Team structure"), 1, buf, -1);

    treeview_create_next_opponent_values(ls, fix);

    team_write_results(opp, buf, buf2);
    gtk_list_store_append(ls, &iter);
    gtk_list_store_set(ls, &iter, 0, _("Latest results"), 1, buf, -1);
    gtk_list_store_append(ls, &iter);
    gtk_list_store_set(ls, &iter, 0, _("Goals"), 1, buf2, -1);

    team_write_own_results(opp, buf, FALSE);
    gtk_list_store_append(ls, &iter);    
    /* The user's results against a specific team. */
    gtk_list_store_set(ls, &iter, 0, _("Your results"), 1, buf, -1);
    
    return GTK_TREE_MODEL(ls);
}

void
treeview_set_up_next_opponent(GtkTreeView *treeview)
{
    gint i;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;

    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(treeview),
				GTK_SELECTION_SINGLE);
    gtk_tree_view_set_rules_hint(treeview, FALSE);
    gtk_tree_view_set_headers_visible(treeview, FALSE);

    for(i=0;i<2;i++)
    {
	col = gtk_tree_view_column_new();
	gtk_tree_view_append_column(treeview, col);
	renderer = treeview_helper_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, FALSE);
	gtk_tree_view_column_add_attribute(col, renderer,
					   "markup", i);
    }
}

/** Show some information about the next opponent. */
void
treeview_show_next_opponent(void)
{
    GtkTreeView *treeview = 
	GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right"));
    GtkTreeModel *model = NULL;

    treeview_helper_clear(treeview);
    
    treeview_set_up_next_opponent(treeview);
    model = treeview_create_next_opponent();
    gtk_tree_view_set_model(treeview, model);

    treeview_helper_unref(G_OBJECT(model));
}

GtkTreeModel*
treeview_create_league_results(void)
{
    gint i;
    gchar name[SMALL], results[SMALL];
    GArray *table_elements = 
	league_from_clid(current_user.tm->clid)->table.elements;
    GtkListStore *ls = 
	gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING,
			   G_TYPE_STRING);
    GtkTreeIter iter;

    for(i=0;i<table_elements->len;i++)
	if(g_array_index(table_elements, TableElement, i).team != current_user.tm)
	{
	    if(team_is_user(g_array_index(table_elements, TableElement, i).team) != -1)
		sprintf(name, "<span background='%s'>%s</span>",
			const_app("string_treeview_user_bg"),
			g_array_index(table_elements, TableElement, i).team->name->str);
	    else
		strcpy(name, g_array_index(table_elements, TableElement, i).team->name->str);

	    team_write_own_results(g_array_index(table_elements, TableElement, i).team,
				   results, TRUE);

	    gtk_list_store_append(ls, &iter);
	    gtk_list_store_set(ls, &iter, 0, name,
			       1, results, -1);
	}

    return GTK_TREE_MODEL(ls);
}

void
treeview_set_up_league_results(GtkTreeView *treeview)
{
    gint i;
    gchar *titles[2] =
	{_("Team"),
	 _("Results")};
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;

    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(treeview),
				GTK_SELECTION_NONE);
    gtk_tree_view_set_rules_hint(treeview, TRUE);
    gtk_tree_view_set_headers_visible(treeview, TRUE);

    for(i=0;i<2;i++)
    {
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, titles[i]);
	gtk_tree_view_append_column(treeview, col);
	renderer = treeview_helper_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, FALSE);
	gtk_tree_view_column_add_attribute(col, renderer,
					   "markup", i);
    }
}

/** Show the results of the current user against fellow
    league teams. */
void
treeview_show_league_results(GtkTreeView *treeview)
{
    GtkTreeModel *model = NULL;

    treeview_helper_clear(treeview);
    
    treeview_set_up_league_results(treeview);
    model = treeview_create_league_results();
    gtk_tree_view_set_model(treeview, model);
    g_object_unref(model);
}

/** Show a list of all players in the teams of the cup or league. */
void
treeview_show_all_players(gint clid)
{
    gint i, j;
    GPtrArray *players = g_ptr_array_new();
    const GArray *teams = NULL;
    const GPtrArray *teamsp = NULL;
    
    if(clid < ID_CUP_START)
    {
	teams = (GArray*)league_cup_get_teams(clid);
	for(i=0;i<teams->len;i++)
	    if(&g_array_index(teams, Team, i) != current_user.tm)
		for(j=0;j<g_array_index(teams, Team, i).players->len;j++)
		    g_ptr_array_add(players, &g_array_index(g_array_index(teams, Team, i).players,
							    Player, j));
    }
    else
    {
	teamsp = (GPtrArray*)league_cup_get_teams(clid);
	for(i=0;i<teamsp->len;i++)
	    if((Team*)g_ptr_array_index(teamsp, i) != current_user.tm)
		for(j=0;j<((Team*)g_ptr_array_index(teamsp, i))->players->len;j++)
		    g_ptr_array_add(players, &g_array_index(((Team*)g_ptr_array_index(teamsp, i))->players,
							    Player, j));
    }
	
    treeview_show_player_list(GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right")),
			      players, 
			      treeview_helper_get_attributes_from_scout(current_user.scout), FALSE);    
}

GtkTreeModel*
treeview_create_preview(void)
{
    gint i;
    GtkListStore  *ls;
    GPtrArray *fixtures = fixture_get_coming(current_user.tm);
    gint max = MIN(const_int("int_treeview_coming_matches"), fixtures->len);
    GtkTreeIter iter;

    if(fixtures->len == 0)
    {
	free_g_ptr_array(&fixtures);
	return NULL;
    }

    ls = gtk_list_store_new(5,
			    GDK_TYPE_PIXBUF,
			    G_TYPE_STRING,
			    G_TYPE_STRING,
			    G_TYPE_STRING,
			    GDK_TYPE_PIXBUF);

    for(i=0;i<max;i++)
    {
	treeview_create_fixtures_header((Fixture*)g_ptr_array_index(fixtures, i),
					ls, FALSE);
	treeview_create_fixture((Fixture*)g_ptr_array_index(fixtures, i), ls);

	gtk_list_store_append(ls, &iter);
	gtk_list_store_set(ls, &iter, 0, NULL, 1, "", 2, "", 3, "", 4, NULL, -1);
    }
	
    g_ptr_array_free(fixtures, TRUE);

    return GTK_TREE_MODEL(ls);
}

/** Show a preview of the current user's next games. */
void
treeview_show_preview(void)
{
    GtkTreeView *treeview = GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right"));
    GtkTreeModel *model = NULL;

    treeview_helper_clear(treeview);
    
    treeview_set_up_fixtures(treeview);
    model = treeview_create_preview();
    gtk_tree_view_set_model(treeview, model);
    g_object_unref(model);
}

GtkTreeModel*
treeview_create_player_info(const Player *pl)
{
    gint i;
    GtkListStore  *ls = gtk_list_store_new(2,
					   G_TYPE_STRING,
					   G_TYPE_POINTER);
    GtkTreeIter iter;
    gchar *titles[PLAYER_INFO_ATTRIBUTE_END] =
	{_("Name"),
	 _("Position"),
	 _("Current position"),
	 _("Skill"),
	 _("Current skill"),
	 _("Fitness"),
	 _("Estimated talent"),
	 _("Age"),
	 _("Health"),
	 _("Value"),
	 _("Wage"),
	 _("Contract"),
	 _("Games/Goals\n"),
	 /* 'Limit' is the number of yellow cards until a player gets
	  banned automatically for a match. */
	 _("Yellow cards (limit)\n"),
	 _("Banned\n"),
	 _("Career values"),
	 _("New contract\noffers")};

    for(i=0;i<PLAYER_INFO_ATTRIBUTE_END;i++)
    {
	gtk_list_store_append(ls, &iter);
	gtk_list_store_set(ls, &iter, 0, titles[i], 1, pl, -1);
    }

    return GTK_TREE_MODEL(ls);
}

void
treeview_set_up_player_info(GtkTreeView *treeview)
{
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;

    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(treeview),
				GTK_SELECTION_NONE);
    gtk_tree_view_set_rules_hint(treeview, FALSE);
    gtk_tree_view_set_headers_visible(treeview, FALSE);

    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = treeview_helper_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, FALSE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "markup", 0);

    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = treeview_helper_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, FALSE);
    gtk_tree_view_column_set_cell_data_func(col, renderer,
					    treeview_helper_player_ext_info_to_cell,
					    NULL, NULL);
}

/** Show extended information about the player in the right treeview. */
void
treeview_show_player_info(const Player *pl)
{
    GtkTreeView *treeview = GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right"));
    GtkTreeModel *model = NULL;

    treeview_helper_clear(treeview);
    
    treeview_set_up_player_info(treeview);
    model = treeview_create_player_info(pl);
    gtk_tree_view_set_model(treeview, model);
    g_object_unref(model);
}


GtkTreeModel*
treeview_create_fixtures_week(gint week_number, gint week_round_number)
{
    gint i;
    GPtrArray *fixtures = fixture_get_week_list(week_number, week_round_number);
    GtkListStore *ls = gtk_list_store_new(5,
					  GDK_TYPE_PIXBUF,
					  G_TYPE_STRING,
					  G_TYPE_STRING,
					  G_TYPE_STRING,
					  GDK_TYPE_PIXBUF);
    GtkTreeIter iter;

    for(i=0;i<fixtures->len;i++)
    {
	if(((Fixture*)g_ptr_array_index(fixtures, i))->clid >= ID_CUP_START ||
	   ((Fixture*)g_ptr_array_index(fixtures, i))->clid == current_user.tm->clid ||	   
	   opt_user_int("int_opt_user_show_all_leagues"))
	{
	    if(i == 0 ||
	       ((Fixture*)g_ptr_array_index(fixtures, i))->clid != 
	       ((Fixture*)g_ptr_array_index(fixtures, i - 1))->clid)
	    {
		gtk_list_store_append(ls, &iter);
		gtk_list_store_set(ls, &iter, 0, NULL, -1);
		treeview_create_fixtures_header((Fixture*)g_ptr_array_index(fixtures, i),
						ls, TRUE);
	    }

	    treeview_create_fixture((Fixture*)g_ptr_array_index(fixtures, i), ls);
	}
    }

    g_ptr_array_free(fixtures, TRUE);

    return GTK_TREE_MODEL(ls);
}

/** Show a fixture list of all matches in the given week and round. */
void
treeview_show_fixtures_week(gint week_number, gint week_round_number)
{
    GtkTreeView *treeview = GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right"));
    GtkTreeModel *model = NULL;

    treeview_helper_clear(treeview);
    
    treeview_set_up_fixtures(treeview);
    model = treeview_create_fixtures_week(week_number, week_round_number);
    gtk_tree_view_set_model(treeview, model);
    g_object_unref(model);
}

GtkTreeModel*
treeview_create_user_history(void)
{
    gint i;
    gchar buf[SMALL];
    GtkListStore *ls = 
	gtk_list_store_new(5, G_TYPE_INT, G_TYPE_INT, G_TYPE_STRING, GDK_TYPE_PIXBUF, G_TYPE_STRING);
    GtkTreeIter iter;
    
    for(i=0;i<current_user.history->len;i++)
    {
	user_history_to_string(&g_array_index(current_user.history, UserHistory, i), buf);

	gtk_list_store_append(ls, &iter);
	treeview_helper_insert_icon(ls, &iter, 3,
				    treeview_helper_get_user_history_icon(
					g_array_index(current_user.history, UserHistory, i).type));
	gtk_list_store_set(ls, &iter,
			   0, g_array_index(current_user.history, UserHistory, i).season,
			   1, g_array_index(current_user.history, UserHistory, i).week,
			   2, team_of_id(g_array_index(current_user.history, UserHistory, i).team_id)->name->str,
			   4, buf, -1);

    }

    return GTK_TREE_MODEL(ls);
}

void
treeview_set_up_user_history(GtkTreeView *treeview)
{
    gint i;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    /* Season. */
    gchar *titles[3] ={_("Sea"), 
		       _("Week"), _("Team")};

    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(treeview),
				GTK_SELECTION_NONE);
    gtk_tree_view_set_rules_hint(treeview, TRUE);
    gtk_tree_view_set_headers_visible(treeview, TRUE);

    for(i=0;i<3;i++)
    {
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, titles[i]);
	gtk_tree_view_append_column(treeview, col);
	renderer = treeview_helper_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer,
					   "text", i);
	g_object_set(renderer, "xalign", 0.5, NULL);
    }

    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = gtk_cell_renderer_pixbuf_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "pixbuf", 3);
    g_object_set(renderer, "xalign", 0.5, NULL);

    col = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(col, _("Event"));
    gtk_tree_view_append_column(treeview, col);
    renderer = treeview_helper_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, FALSE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "text", 4);
}

/** Show the history array of the current user in the
    right treeview. */
void
treeview_show_user_history(void)
{
    GtkTreeView *treeview = GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right"));
    GtkTreeModel *model = NULL;

    treeview_helper_clear(treeview);
    
    treeview_set_up_user_history(treeview);
    model = treeview_create_user_history();
    gtk_tree_view_set_model(treeview, model);
    g_object_unref(model);    
}

void
treeview_create_league_stats(GtkListStore *ls, const LeagueStat *league_stat)
{
    gint i, j;
    gchar buf[SMALL], buf2[SMALL], buf3[SMALL], buf4[SMALL];
    GtkTreeIter iter;
    GArray *teams[2] = {league_stat->teams_off, league_stat->teams_def};
    GArray *players[2] = {league_stat->player_scorers, league_stat->player_goalies};
    gchar *team_titles[2] = {_("Best offensive teams"),
			    _("Best defensive teams")};
    gchar *team_icons[2] = {const_app("string_treeview_league_stats_off_teams_icon"),
			    const_app("string_treeview_league_stats_def_teams_icon")};
    gchar *player_titles[2][2] = {{_("Best goal scorers"), _("Shot %")},
				  {_("Best goalkeepers"),
				   /* % of saves for goalies (#goals / #shots) */
				   _("Save %")}};
    gchar *player_icons[2] = {const_app("string_treeview_league_stats_scorers_icon"),
			      const_app("string_treeview_league_stats_goalies_icon")};
    gchar *colour_fg = NULL, *colour_bg = NULL;
    
    gtk_list_store_append(ls, &iter);
    treeview_helper_insert_icon(ls, &iter, 0, league_from_clid(league_stat->clid)->symbol->str);
    gtk_list_store_set(ls, &iter, 1, const_int("int_treeview_helper_int_empty"),
		       2, league_from_clid(league_stat->clid)->name->str, 3, "", 4, "", 5, "", -1);

    for(i=0;i<2;i++)
    {
	gtk_list_store_append(ls, &iter);
	gtk_list_store_set(ls, &iter, 0, NULL,
			   1, const_int("int_treeview_helper_int_empty"),
			   2, "", 3, "", 4, "", 5, "", -1);
	
	gtk_list_store_append(ls, &iter);
	treeview_helper_insert_icon(ls, &iter, 0, team_icons[i]);
	gtk_list_store_set(ls, &iter, 1, const_int("int_treeview_helper_int_empty"),
			   2, team_titles[i], 4, _("Goals"), -1);
	
	for(j=0;j<teams[i]->len;j++)
	{
	    sprintf(buf, "%d : %d",
		    g_array_index(teams[i], Stat, j).value1,
		    g_array_index(teams[i], Stat, j).value2);
	    
	    treeview_helper_set_user_colours(team_of_id(g_array_index(teams[i], Stat, j).team_id),
					     &colour_bg, &colour_fg);

	    sprintf(buf2, "<span background='%s' foreground='%s'>%s</span>",
		    colour_bg, colour_fg, team_of_id(g_array_index(teams[i], Stat, j).team_id)->name->str);
	    
	    gtk_list_store_append(ls, &iter);
	    gtk_list_store_set(ls, &iter, 0, NULL, 1, j + 1,
			       2, buf2,
			       4, buf, 5, "", -1);
	}
    }

    for(i=0;i<2;i++)
    {
	gtk_list_store_append(ls, &iter);
	gtk_list_store_set(ls, &iter, 0, NULL,
			   1, const_int("int_treeview_helper_int_empty"),
			   2, "", 3, "", 4, "", 5, "", -1);
	
	gtk_list_store_append(ls, &iter);
	treeview_helper_insert_icon(ls, &iter, 0, player_icons[i]);
	gtk_list_store_set(ls, &iter, 1, const_int("int_treeview_helper_int_empty"),
			   /* Goals. */
			   2, player_titles[i][0], 3, _("Go"),
			   /* Goals per game. */
			   4, _("Go/Ga"), 5, player_titles[i][1], -1);

	for(j=0;j<players[i]->len;j++)
	{
	    sprintf(buf, "%.1f",
		    (gfloat)g_array_index(players[i], Stat, j).value1 /
		    (gfloat)g_array_index(players[i], Stat, j).value2);
	    if(i == 0)
		sprintf(buf2, "%.1f %%", (g_array_index(players[i], Stat, j).value3 > 0) ?
			100 * ((gfloat)g_array_index(players[i], Stat, j).value1 /
			       (gfloat)g_array_index(players[i], Stat, j).value3) : 0);
	    else
		sprintf(buf2, "%.1f %%", (g_array_index(players[i], Stat, j).value3 > 0) ?
			100 * (1 - (gfloat)g_array_index(players[i], Stat, j).value1 /
			       (gfloat)g_array_index(players[i], Stat, j).value3) : 0);

	    treeview_helper_set_user_colours(team_of_id(g_array_index(players[i], Stat, j).team_id),
					     &colour_bg, &colour_fg);
	    sprintf(buf3, "<span background='%s' foreground='%s'>%s (%s)</span>",
		    colour_bg, colour_fg,
		    g_array_index(players[i], Stat, j).value_string->str,
		    team_of_id(g_array_index(players[i], Stat, j).team_id)->name->str);
	    sprintf(buf4, "%d", g_array_index(players[i], Stat, j).value1);

	    gtk_list_store_append(ls, &iter);
	    gtk_list_store_set(ls, &iter, 0, NULL, 1, j + 1,
			       2, buf3, 3, buf4, 4, buf, 5, buf2, -1);
	}
    }
}

void
treeview_set_up_league_stats(GtkTreeView *treeview)
{
    gint i;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;

    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(treeview),
				GTK_SELECTION_NONE);
    gtk_tree_view_set_rules_hint(treeview, FALSE);
    gtk_tree_view_set_headers_visible(treeview, FALSE);

    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = gtk_cell_renderer_pixbuf_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "pixbuf", 0);
    g_object_set(renderer, "xalign", 0.5, NULL);

    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = treeview_helper_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_set_cell_data_func(col, renderer,
					    treeview_helper_int_to_cell,
					    NULL, NULL);
    for(i=2;i<6;i++)
    {
	col = gtk_tree_view_column_new();
	gtk_tree_view_append_column(treeview, col);
	renderer = treeview_helper_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer,
					   "markup", i);
	if(i > 3)
	    g_object_set(renderer, "xalign", 0.5, NULL);
    }
}

/** Show the stats of the league with specified id. */
void
treeview_show_league_stats(gint clid)
{
    GtkTreeView *treeview = GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right"));
    GtkListStore *model = 
	gtk_list_store_new(6, GDK_TYPE_PIXBUF, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING,
			   G_TYPE_STRING, G_TYPE_STRING);
    
    treeview_helper_clear(treeview);
    
    treeview_set_up_league_stats(treeview);
    treeview_create_league_stats(model, &league_from_clid(clid)->stats);
    gtk_tree_view_set_model(treeview, GTK_TREE_MODEL(model));
    g_object_unref(model);
}

void
treeview_create_season_history_champions(GtkListStore *ls, const GArray* league_champs,
					 const GArray *cup_champs)
{
    gint i, j;
    GtkTreeIter iter;
    const GArray *champs[2] = {league_champs, cup_champs};
    gchar *titles[2] = {_("League champions"),
			_("Cup champions")};
    gchar *icons[2] = {const_app("string_treeview_season_hist_league_champions_icon"),
		       const_app("string_treeview_season_hist_cup_champions_icon")};
    
    for(i=0;i<2;i++)
    {
	gtk_list_store_append(ls, &iter);
	treeview_helper_insert_icon(ls, &iter, 0, icons[i]);
	gtk_list_store_set(ls, &iter, 1, const_int("int_treeview_helper_int_empty"),
			   2, titles[i], 3, "", 4, "", 5, "", -1);

	for(j=0;j<champs[i]->len;j++)
	{
	    gtk_list_store_append(ls, &iter);
	    gtk_list_store_set(ls, &iter, 0, NULL, 1, const_int("int_treeview_helper_int_empty"),
			       2, g_array_index(champs[i], ChampStat, j).cl_name->str,
			       3, g_array_index(champs[i], ChampStat, j).team_name->str,
			       4, "", 5, "", -1);
	}

	gtk_list_store_append(ls, &iter);
	gtk_list_store_set(ls, &iter, 0, NULL, 
			   1, const_int("int_treeview_helper_int_empty"),
			   2, "", 3, "", 4, "", 5, "", -1);
    }
}

/** Show the season history of the specified season
    and the specified page. Page is either a page of league stats
    (if != -1) or -1 which means we show the champions. */
void
treeview_show_season_history(gint page, gint season_number)
{
    gchar buf[SMALL];
    GtkTreeView *treeview = GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right"));
    GtkListStore *model = 
	gtk_list_store_new(6, GDK_TYPE_PIXBUF, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING,
			   G_TYPE_STRING, G_TYPE_STRING);
    GtkTreeIter iter;
    SeasonStat *stat = &g_array_index(season_stats, SeasonStat, season_number);

    treeview_helper_clear(treeview);

    treeview_set_up_league_stats(treeview);

    sprintf(buf, _("Season %d"), season_number + 1);
    gtk_list_store_append(model, &iter);
    gtk_list_store_set(model, &iter, 0, NULL,
		       1, const_int("int_treeview_helper_int_empty"),
		       2, buf, -1);

    if(page != -1)
	treeview_create_league_stats(model, 
				     &g_array_index(stat->league_stats, LeagueStat, page));
    else
	treeview_create_season_history_champions(model, stat->league_champs,
						 stat->cup_champs);

    gtk_tree_view_set_model(treeview, GTK_TREE_MODEL(model));
    g_object_unref(model);
}

/** Create the model for the startup country files combo. 
    @param country_list The list of country files found. */
GtkTreeModel*
treeview_create_country_list(const GPtrArray *country_list)
{
    gint i;
    GtkListStore *ls = gtk_list_store_new(2, GDK_TYPE_PIXBUF, G_TYPE_STRING);
    GtkTreeIter iter;
    gchar buf[SMALL], buf2[SMALL], trash[SMALL];
    
    for(i=0;i<country_list->len;i++)
    {
	sscanf(((GString*)g_ptr_array_index(country_list, i))->str, "country_%[^.]%[.xml]",
	       buf2, trash);
	sprintf(buf, "flag_%s.png", buf2);

	gtk_list_store_append(ls, &iter);
	treeview_helper_insert_icon(ls, &iter, 0, buf);
	gtk_list_store_set(ls, &iter, 1, 
			   ((GString*)g_ptr_array_index(country_list, i))->str, -1);
    }

    return GTK_TREE_MODEL(ls);
}

/** Show the list of contributors in the help window.*/
void
treeview_show_contributors(const OptionList *help_list)
{
    GtkTreeView *treeview = GTK_TREE_VIEW(lookup_widget(window.help, "treeview_contributors"));
    GtkListStore *ls = gtk_list_store_new(1, G_TYPE_STRING);
    GtkTreeIter iter;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    gint i;
    gchar buf[SMALL];

    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(treeview),
				GTK_SELECTION_NONE);
    gtk_tree_view_set_rules_hint(treeview, FALSE);
    gtk_tree_view_set_headers_visible(treeview, FALSE);

    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = treeview_helper_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "markup", 0);

    for(i=0;i<help_list->list->len;i++)
    {
	gtk_list_store_append(ls, &iter);
	
	if(g_str_has_prefix(g_array_index(help_list->list, Option, i).name->str,
			    "string_contrib_title"))
	{
	    sprintf(buf, "\n<span %s>%s</span>", 
		    const_app("string_help_window_title_attribute"),
		    g_array_index(help_list->list, Option, i).string_value->str);
	    gtk_list_store_set(ls, &iter, 0, buf, -1);
	}
	else if(g_str_has_prefix(g_array_index(help_list->list, Option, i).name->str,
				 "string_contrib_"))
	{
	    strcpy(buf, g_array_index(help_list->list, Option, i).string_value->str);
	    gtk_list_store_set(ls, &iter, 0, buf, -1);
	}

    }

    gtk_tree_view_set_model(treeview, GTK_TREE_MODEL(ls));
    g_object_unref(ls);
}

GtkTreeModel*
treeview_create_league_list(void)
{
    gint i;
    GtkListStore *ls = gtk_list_store_new(1, G_TYPE_STRING);
    GtkTreeIter iter;

    gtk_list_store_append(ls, &iter);
    gtk_list_store_set(ls, &iter, 0, _("Current league"), -1);

    for(i=0;i<ligs->len;i++)
    {
	gtk_list_store_append(ls, &iter);
	gtk_list_store_set(ls, &iter, 0, lig(i).name->str, -1);
    }

    return GTK_TREE_MODEL(ls);
}

/** Show the league list in the combo box
    in the startup window. */
void
treeview_show_leagues_combo(void)
{
    GtkTreeModel *model = treeview_create_league_list();
    GtkComboBox *combo_leagues =
	GTK_COMBO_BOX(lookup_widget(window.startup, "comboboxentry_start_league"));
    GtkCellRenderer *renderer = NULL;
    
    gtk_combo_box_set_model(combo_leagues, model);
    g_object_unref(model);

    if(gtk_combo_box_entry_get_text_column(GTK_COMBO_BOX_ENTRY(combo_leagues)) == -1)
	gtk_combo_box_entry_set_text_column(GTK_COMBO_BOX_ENTRY(combo_leagues), 0);

    gtk_cell_layout_clear(GTK_CELL_LAYOUT(combo_leagues));

    renderer = treeview_helper_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo_leagues), renderer, FALSE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo_leagues), renderer, "text", 0, NULL);

    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_leagues), 0);
}

GtkTreeModel*
treeview_create_language_list(void)
{
    gint i;
    GtkListStore *ls = gtk_list_store_new(2, GDK_TYPE_PIXBUF, G_TYPE_STRING);
    GtkTreeIter iter;
    GPtrArray *names = 
	misc_separate_strings(const_str("string_language_names"));
    GPtrArray *symbols = 
	misc_separate_strings(const_str("string_language_symbols"));

    gtk_list_store_append(ls, &iter);
    /* Language is system-set (not user chosen). */
    gtk_list_store_set(ls, &iter, 0, NULL, 1, _("System"), -1);

    for(i=0;i<names->len;i++)
    {
	gtk_list_store_append(ls, &iter);
	treeview_helper_insert_icon(ls, &iter, 0, ((GString*)g_ptr_array_index(symbols, i))->str);
	gtk_list_store_set(ls, &iter, 1, 
			   ((GString*)g_ptr_array_index(names, i))->str, -1);
    }

    free_g_string_array(&names);
    free_g_string_array(&symbols);

    return GTK_TREE_MODEL(ls);
}

/** Show the list of available languages. */
void
treeview_show_language_combo(void)
{
    GtkTreeModel *model = treeview_create_language_list();
    GtkComboBox *combo_languages =
	GTK_COMBO_BOX(lookup_widget(window.options, "comboboxentry_languages"));
    GtkCellRenderer *renderer = NULL;
    gint idx = language_get_current_index();

    gtk_combo_box_set_model(combo_languages, model);
    g_object_unref(model);

    if(gtk_combo_box_entry_get_text_column(GTK_COMBO_BOX_ENTRY(combo_languages)) == -1)
	gtk_combo_box_entry_set_text_column(GTK_COMBO_BOX_ENTRY(combo_languages), 1); 
    gtk_cell_layout_clear(GTK_CELL_LAYOUT(combo_languages));

    renderer = gtk_cell_renderer_pixbuf_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo_languages), renderer, FALSE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo_languages), renderer, "pixbuf", 0, NULL);

    renderer = treeview_helper_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo_languages), renderer, FALSE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo_languages), renderer, "text", 1, NULL);

    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_languages), idx + 1);
}

GtkTreeModel*
treeview_create_sponsors(const GArray *sponsors)
{
    gint i;
    GtkListStore *ls = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_INT, 
					  G_TYPE_INT);
    GtkTreeIter iter;

    for(i=0;i<sponsors->len;i++)
    {
	gtk_list_store_append(ls, &iter);
	gtk_list_store_set(ls, &iter, 
			   0, g_array_index(sponsors, UserSponsor, i).name->str,
			   1, g_array_index(sponsors, UserSponsor, i).contract / 4,
			   2, g_array_index(sponsors, UserSponsor, i).benefit,
			   -1);
    }

    for(i=1;i<3;i++)
	gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(ls), i,
					treeview_helper_int_compare, GINT_TO_POINTER(i), NULL);

    return GTK_TREE_MODEL(ls);
}

void
treeview_set_up_sponsors(GtkTreeView *treeview)
{
    gint i;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    gchar *titles[3] = 
	{_("Company"),
	 _("Contract length\n(Months)"),
	 _("Money / week")};
    
    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(treeview),
				GTK_SELECTION_BROWSE);
    gtk_tree_view_set_rules_hint(treeview, TRUE);
    gtk_tree_view_set_headers_visible(treeview, TRUE);

    for(i=0;i<3;i++)
    {
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, titles[i]);
	gtk_tree_view_append_column(treeview, col);	
	renderer = treeview_helper_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer,
					   "markup", i);

	if(i > 0)
	{
	    g_object_set(renderer, "xalign", 0.5, NULL);
	    gtk_tree_view_column_set_sort_column_id(col, i);
	}
    }
}

/** Show a list of sponsor offers. */
void
treeview_show_sponsors(const GArray *sponsors)
{
    GtkTreeView *treeview = GTK_TREE_VIEW(lookup_widget(window.sponsors,
							"treeview_sponsors"));
    GtkTreeModel *model = treeview_create_sponsors(sponsors);
    GtkTreePath *path = gtk_tree_path_new_from_string("0");

    treeview_helper_clear(treeview);
    
    treeview_set_up_sponsors(treeview);
    gtk_tree_view_set_model(treeview, GTK_TREE_MODEL(model));
    gtk_tree_selection_select_path(gtk_tree_view_get_selection(treeview), path);
    g_object_unref(model);
    gtk_tree_path_free(path);
}
