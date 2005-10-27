/*
   Bygfoot Football Manager -- a small and simple GTK2-based
   football management game.

   http://bygfoot.sourceforge.net

   Copyright (C) 2005  Gyözö Both (gyboth@bygfoot.com)

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "bet.h"
#include "cup.h"
#include "file.h"
#include "fixture.h"
#include "free.h"
#include "league.h"
#include "misc.h"
#include "option.h"
#include "player.h"
#include "support.h"
#include "team.h"
#include "transfer.h"
#include "treeview.h"
#include "treeview2.h"
#include "treeview_helper.h"
#include "user.h"
#include "variables.h"

/** Select the row that's been clicked on. */
gboolean
treeview_helper_select_row(GtkTreeView *treeview, GdkEventButton *event)
{
    GtkTreeSelection *selection =
	gtk_tree_view_get_selection(treeview);
    GtkTreePath *path;
    
    if(!gtk_tree_view_get_path_at_pos(treeview,
				      event->x, event->y,
				      &path, NULL, NULL, NULL))
	return FALSE;

    gtk_tree_selection_select_path(selection, path);
    gtk_tree_path_free(path);

    return TRUE;
}

/** Return the number in the 'column'th column of the currently
    selected row of the treeview.
    @param treeview The treeview argument.
    @param column The column we'd like to get the contents of.
    @return The number in the given column of the selected row.
*/
gint
treeview_helper_get_index(GtkTreeView *treeview, gint column)
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
gpointer
treeview_helper_get_pointer(GtkTreeView *treeview, gint column)
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
treeview_helper_clear(GtkTreeView *treeview)
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
treeview_helper_get_col_number_column (GtkTreeViewColumn *col)
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

/** Return the row index of the iter in the model. 
    Since we only work with flat models (no children),
    the path is a single number. */
gint
treeview_helper_iter_get_row(GtkTreeModel *model, GtkTreeIter *iter)
{
    gchar *path = gtk_tree_model_get_string_from_iter(model, iter);
    gint row_idx = (gint)g_ascii_strtod(path, NULL);

    g_free(path);

    return row_idx;
}

/** Return a cell renderer with font name set
    according to the font option. */
GtkCellRenderer*
treeview_helper_cell_renderer_text_new(void)
{
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();

    if(strlen(opt_str("string_opt_font_name")) > 0)
	g_object_set(renderer, "font", opt_str("string_opt_font_name"), NULL);
    
    return renderer;
}

/** Return the filename of the icon going with the LiveGameEvent
    with type event_type.
    @param event_type The type of the event.
    @return A filename specifying a pixmap. */
gchar*
treeview_helper_live_game_icon(gint event_type)
{
    if(event_type == LIVE_GAME_EVENT_START_MATCH ||
       event_type == LIVE_GAME_EVENT_END_MATCH ||
       event_type == LIVE_GAME_EVENT_HALF_TIME ||
       event_type == LIVE_GAME_EVENT_EXTRA_TIME ||
       event_type == LIVE_GAME_EVENT_PENALTIES)
	return const_app("string_live_game_event_start_match_icon");
    else if(event_type == LIVE_GAME_EVENT_LOST_POSSESSION)
	return const_app("string_live_game_event_lost_possession_icon");
    else if(event_type == LIVE_GAME_EVENT_PENALTY)
	return const_app("string_live_game_event_penalty_icon");
    else if(event_type == LIVE_GAME_EVENT_SCORING_CHANCE ||
	    event_type == LIVE_GAME_EVENT_FREE_KICK)
	return const_app("string_live_game_event_scoring_chance_icon");
    else if(event_type == LIVE_GAME_EVENT_HEADER)
	return const_app("string_live_game_event_header_icon");
    else if(event_type == LIVE_GAME_EVENT_GOAL)
	return const_app("string_live_game_event_goal_icon");
    else if(event_type == LIVE_GAME_EVENT_OWN_GOAL)
	return const_app("string_live_game_event_own_goal_icon");
    else if(event_type == LIVE_GAME_EVENT_POST)
	return const_app("string_live_game_event_post_icon");
    else if(event_type == LIVE_GAME_EVENT_CROSS_BAR)
	return const_app("string_live_game_event_cross_bar_icon");
    else if(event_type == LIVE_GAME_EVENT_SAVE)
	return const_app("string_live_game_event_save_icon");
    else if(event_type == LIVE_GAME_EVENT_MISS)
	return const_app("string_live_game_event_miss_icon");
    else if(event_type == LIVE_GAME_EVENT_FOUL)
	return const_app("string_live_game_event_foul_icon");
    else if(event_type == LIVE_GAME_EVENT_FOUL_YELLOW)
	return const_app("string_live_game_event_foul_yellow_icon");
    else if(event_type == LIVE_GAME_EVENT_SEND_OFF ||
	    event_type == LIVE_GAME_EVENT_FOUL_RED ||
	    event_type == LIVE_GAME_EVENT_FOUL_RED_INJURY)
	return const_app("string_live_game_event_send_off_icon");
    else if(event_type == LIVE_GAME_EVENT_INJURY)
	return const_app("string_live_game_event_injury_icon");
    else if(event_type == LIVE_GAME_EVENT_TEMP_INJURY)
	return const_app("string_live_game_event_temp_injury_icon");
    else if(event_type == LIVE_GAME_EVENT_STADIUM_BREAKDOWN)
	return const_app("string_live_game_event_stadium_breakdown_icon");
    else if(event_type == LIVE_GAME_EVENT_STADIUM_FIRE)
	return const_app("string_live_game_event_stadium_fire_icon");
    else if(event_type == LIVE_GAME_EVENT_STADIUM_RIOTS)
	return const_app("string_live_game_event_stadium_riots_icon");
    else if(event_type == LIVE_GAME_EVENT_SUBSTITUTION)
	return const_app("string_live_game_event_substitution_icon");
    else if(event_type == LIVE_GAME_EVENT_STRUCTURE_CHANGE)
	return const_app("string_live_game_event_structure_change_icon");
    else if(event_type == LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_DEFEND)
	return const_app("string_game_gui_style_all_out_defend_icon");
    else if(event_type == LIVE_GAME_EVENT_STYLE_CHANGE_DEFEND)
	return const_app("string_game_gui_style_defend_icon");
    else if(event_type == LIVE_GAME_EVENT_STYLE_CHANGE_BALANCED)
	return const_app("string_game_gui_style_balanced_icon");
    else if(event_type == LIVE_GAME_EVENT_STYLE_CHANGE_ATTACK)
	return const_app("string_game_gui_style_attack_icon");
    else if(event_type == LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_ATTACK)
	return const_app("string_game_gui_style_all_out_attack_icon");
    else if(event_type == LIVE_GAME_EVENT_BOOST_CHANGE_ANTI)
	return const_app("string_game_gui_boost_anti_icon");
    else if(event_type == LIVE_GAME_EVENT_BOOST_CHANGE_OFF)
	return const_app("string_game_gui_boost_off_icon");
    else if(event_type == LIVE_GAME_EVENT_BOOST_CHANGE_ON)
	return const_app("string_game_gui_boost_on_icon");
    else
	return "";
}

/** Return the appropriate symbol name from the constants
    for the user history type. */
gchar*
treeview_helper_get_user_history_icon(gint history_type)
{
    switch(history_type)
    {
	default:
	    g_warning("treeview_helper_get_user_history_icon: unknown type %d.\n", history_type);
	    return NULL;
	case USER_HISTORY_START_GAME:
	    return const_app("string_treeview_helper_user_history_symbol_start_game_icon");
    	case USER_HISTORY_FIRE_FINANCES:
	    return const_app("string_treeview_helper_user_history_symbol_fire_finances_icon");
    	case  USER_HISTORY_FIRE_FAILURE:
	    return const_app("string_treeview_helper_user_history_symbol_fire_failure_icon");
    	case  USER_HISTORY_JOB_OFFER_ACCEPTED:
	    return const_app("string_treeview_helper_user_history_symbol_job_offer_accepted_icon");
    	case  USER_HISTORY_END_SEASON:
	    return const_app("string_treeview_helper_user_history_symbol_end_season_icon");
    	case  USER_HISTORY_WIN_FINAL:
	    return const_app("string_treeview_helper_user_history_symbol_win_final_icon");
    	case  USER_HISTORY_LOSE_FINAL:
	    return const_app("string_treeview_helper_user_history_symbol_lose_final_icon");
    	case  USER_HISTORY_PROMOTED:
	    return const_app("string_treeview_helper_user_history_symbol_promoted_icon");
    	case  USER_HISTORY_RELEGATED:
	    return const_app("string_treeview_helper_user_history_symbol_relegated_icon");
    	case USER_HISTORY_REACH_CUP_ROUND:
	    return const_app("string_treeview_helper_user_history_symbol_reach_cup_round_icon");
    	case USER_HISTORY_CHAMPION:
	    return const_app("string_treeview_helper_user_history_symbol_champion_icon");
    }

    return NULL;
}

/** Return a new pixbuf created from the specified filename.
    @param filename Name of a pixmap file located in one of the support directories. 
    @return A new pixbuf or NULL on error. */
GdkPixbuf*
treeview_helper_pixbuf_from_filename(gchar *filename)
{
    GdkPixbuf *symbol = NULL;
    GError *error = NULL;
    gchar *symbol_file = NULL;

    if(filename != NULL && strlen(filename) != 0)
    {
	symbol_file = file_find_support_file(filename, FALSE);
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

/** Unref an object if non-null (mostly it's a pixbuf added to a treeview).*/
void
treeview_helper_unref(GObject *object)
{
    if(object == NULL)
	return;

    g_object_unref(object);
}


/** Insert an icon into the liststore at the given iter and
    column.
    @param icon_name The filename of the icon. */
void
treeview_helper_insert_icon(GtkListStore *ls, GtkTreeIter *iter, gint column_nr,
			    gchar *icon_name)
{
    GdkPixbuf *symbol = treeview_helper_pixbuf_from_filename(icon_name);

    gtk_list_store_set(ls, iter, column_nr, symbol, -1);
    
    treeview_helper_unref(G_OBJECT(symbol));
}

/** Function comparing two teams in a team list treeview. */
gint
treeview_helper_team_compare(GtkTreeModel *model,
		      GtkTreeIter *a,
		      GtkTreeIter *b,
		      gpointer user_data)
{
    gint type = GPOINTER_TO_INT(user_data);
    Team *tm1, *tm2;
    gint return_value = 0;

    gtk_tree_model_get(model, a, 4, &tm1, -1);
    gtk_tree_model_get(model, b, 4, &tm2, -1);

    switch(type)
    {
	default:
	    g_warning("treeview_team_compare: unknown type %d.\n", type);
	    break;
	case TEAM_COMPARE_AV_SKILL:
	    return_value = misc_float_compare(team_get_average_skill(tm1, FALSE),
					      team_get_average_skill(tm2, FALSE));
	    break;
    }

    return return_value;
}

/** Compare two players in a treeview. */
gint
treeview_helper_player_compare(GtkTreeModel *model,
			       GtkTreeIter *a,
			       GtkTreeIter *b,
			       gpointer user_data)
{
    gint type = GPOINTER_TO_INT(user_data);
    Player *pl1, *pl2;
    gint return_value = 0;

    gtk_tree_model_get(model, a, 4, &pl1, -1);
    gtk_tree_model_get(model, b, 4, &pl2, -1);
    
    if(pl1 == NULL && pl2 == NULL)
	return_value = 0;
    else if(pl1 == NULL)
	return_value = 1;
    else if(pl2 == NULL)
	return_value = -1;
    else
    {
	switch(type)
	{
	    default:
		g_warning("treeview_player_compare: unknown type %d.\n", type);
		break;
	    case PLAYER_LIST_ATTRIBUTE_POS:
		return_value = misc_int_compare(pl1->pos, pl2->pos);
		break;
	    case PLAYER_LIST_ATTRIBUTE_GOALS:
		if(pl1->pos == 0 && pl2->pos == 0)
		    return_value = misc_int_compare(player_games_goals_get(pl2, pl2->team->clid, PLAYER_VALUE_GOALS),
						    player_games_goals_get(pl1, pl1->team->clid, PLAYER_VALUE_GOALS));
		else if(pl1->pos == 0 || pl2->pos == 0)
		    return_value = (pl1->pos == 0) ? 1 : -1;
		else
		    return_value = misc_int_compare(player_games_goals_get(pl1, pl1->team->clid, PLAYER_VALUE_GOALS),
						    player_games_goals_get(pl2, pl2->team->clid, PLAYER_VALUE_GOALS));
		break;
	    case PLAYER_LIST_ATTRIBUTE_SHOTS:
		return_value = misc_int_compare(player_games_goals_get(pl1, pl1->team->clid, PLAYER_VALUE_SHOTS),
						player_games_goals_get(pl2, pl2->team->clid, PLAYER_VALUE_SHOTS));
		break;
	    case PLAYER_LIST_ATTRIBUTE_GAMES:
		return_value = misc_int_compare(player_games_goals_get(pl1, pl1->team->clid, PLAYER_VALUE_GAMES),
						player_games_goals_get(pl2, pl2->team->clid, PLAYER_VALUE_GAMES));
		break;
	    case PLAYER_LIST_ATTRIBUTE_SKILL:
		return_value = misc_float_compare(pl1->skill, pl2->skill);
		break;
	    case PLAYER_LIST_ATTRIBUTE_AGE:
		return_value = misc_float_compare(pl1->age, pl2->age);
		break;
	    case PLAYER_LIST_ATTRIBUTE_ETAL:
		return_value = misc_float_compare(pl1->etal[current_user.scout % 10], 
						  pl2->etal[current_user.scout % 10]);
		break;
	    case PLAYER_LIST_ATTRIBUTE_VALUE:
		return_value = misc_int_compare(pl1->value, pl2->value);
		break;
	    case PLAYER_LIST_ATTRIBUTE_WAGE:
		return_value = misc_int_compare(pl1->wage, pl2->wage);
		break;
	}
    }

    return return_value;

}

/** Find out whether the team given by the index in the league
    table would participate in an international cup and set the
    colours accordingly. */
gboolean
treeview_helper_get_table_element_colour_cups(const League *league, 
					      gint idx, gchar **colour_bg)
{
    gint i, j, k;
    const CupRound *cup_round = NULL;
    gint league_idx = league_cup_get_index_from_clid(league->id) + 1;
    gchar buf[SMALL];
    gchar *cup_highlight_colour = NULL;

    sprintf(buf, "LEAGUE%d", league_idx);

    for(i=0;i<cps->len;i++)
    {
	cup_highlight_colour = cup_get_highlight_colour(&cp(i));

	if(cup_highlight_colour != NULL)
	    for(k=0;k<cp(i).rounds->len;k++)
	    {
		cup_round = &g_array_index(cp(i).rounds, CupRound, k);
		for(j=0;j<cup_round->choose_teams->len;j++)
		    if(strcmp(g_array_index(cup_round->choose_teams, 
					    CupChooseTeam, j).sid, buf) == 0 ||
		       strcmp(g_array_index(cup_round->choose_teams, 
					    CupChooseTeam, j).sid, league->sid) == 0)
		    {
			if((idx + 1 >= g_array_index(cup_round->choose_teams, 
						     CupChooseTeam, j).start_idx &&
			    idx + 1 <= g_array_index(cup_round->choose_teams, 
						     CupChooseTeam, j).end_idx && 
			    g_array_index(cup_round->choose_teams, 
					  CupChooseTeam, j).randomly) ||
			   (idx + 1 >= g_array_index(cup_round->choose_teams, 
						     CupChooseTeam, j).start_idx &&
			    idx + 1 < g_array_index(cup_round->choose_teams, 
						    CupChooseTeam, j).start_idx + 
			    g_array_index(cup_round->choose_teams, 
					  CupChooseTeam, j).number_of_teams &&
			    !g_array_index(cup_round->choose_teams, 
					   CupChooseTeam, j).randomly))
			{			    			    
			    *colour_bg = cup_highlight_colour;
			    return TRUE;
			}
		    }
	    }
    }
    
    return FALSE;
}

/** Find out whether the team given 
    would participate in an international cup and set the
    colour accordingly. */
gboolean
treeview_helper_get_table_element_colour_cups_cup(const Cup *cup, 
						  const Team *tm, gchar **colour_bg)
{
    gint i, j, k, idx = -1;
    const CupRound *cup_round = NULL;
    gchar *cup_highlight_colour = NULL;
    GPtrArray *teams = cup_get_teams_sorted(cup);

    for(i=0;i<teams->len;i++)
	if((Team*)g_ptr_array_index(teams, i) == tm)
	{
	    idx = i;
	    break;
	}

    for(i=0;i<cps->len;i++)
    {
	cup_highlight_colour = cup_get_highlight_colour(&cp(i));

	if(cup_highlight_colour != NULL)
	    for(k=0;k<cp(i).rounds->len;k++)
	    {
		cup_round = &g_array_index(cp(i).rounds, CupRound, k);
		for(j=0;j<cup_round->choose_teams->len;j++)
		    if(strcmp(g_array_index(
				  cup_round->choose_teams, CupChooseTeam, j).sid,
			      cup->sid) == 0)
		    {
			if((idx + 1 >= g_array_index(cup_round->choose_teams, 
						     CupChooseTeam, j).start_idx &&
			    idx + 1 <= g_array_index(cup_round->choose_teams, 
						     CupChooseTeam, j).end_idx && 
			    g_array_index(cup_round->choose_teams, 
					  CupChooseTeam, j).randomly) ||
			   (idx + 1 >= g_array_index(cup_round->choose_teams, 
						     CupChooseTeam, j).start_idx &&
			    idx + 1 < g_array_index(cup_round->choose_teams, 
						    CupChooseTeam, j).start_idx + 
			    g_array_index(cup_round->choose_teams, 
					  CupChooseTeam, j).number_of_teams &&
			    !g_array_index(cup_round->choose_teams, 
					   CupChooseTeam, j).randomly))
			{			    			    
			    *colour_bg = cup_highlight_colour;
			    return TRUE;
			}
		    }
	    }
    }
    
    return FALSE;
}

/** Get the colours for a team in the tables.
    @param table The table pointer.
    @param idx The index of the element we're looking at.
    @param user Whether to take into account user colours. */
void
treeview_helper_get_table_element_colours(const Table *table, gint idx, 
					  gchar **colour_fg, gchar **colour_bg,
					  gboolean user)
{
    gint i;
    const TableElement *elem = &g_array_index(table->elements, TableElement, idx);
    const PromRelElement *pelem = NULL;
    const League *league = NULL;
    GPtrArray *cup_advance = NULL;
    
    *colour_fg = const_app("string_treeview_helper_color_default_foreground");
    *colour_bg = const_app("string_treeview_helper_color_default_background");
    
    if(user && elem->team == current_user.tm)
    {
	*colour_fg = const_app("string_treeview_current_user_fg");
	*colour_bg = const_app("string_treeview_current_user_bg");
    }
    else if(user && team_is_user(elem->team) != -1)
    {
	*colour_fg = const_app("string_treeview_user_fg");
	*colour_bg = const_app("string_treeview_user_bg");
    }
    else if(table->clid < ID_CUP_START)
    {
	league = league_from_clid(table->clid);

	if(idx + 1 == 1)
	    *colour_bg = const_app("string_treeview_table_first");
	else
	{	    
	    if(!treeview_helper_get_table_element_colour_cups(league, idx, colour_bg))
	    {
		for(i=0;i<league->prom_rel.elements->len;i++)
		{
		    pelem = &g_array_index(league_from_clid(table->clid)->prom_rel.elements, PromRelElement, i);
		    if(pelem->ranks[0] <= idx + 1 && idx + 1 <= pelem->ranks[1])
		    {
			if(pelem->type == PROM_REL_PROMOTION)
			    *colour_bg = const_app("string_treeview_table_promotion");
			else if(pelem->type == PROM_REL_RELEGATION)
			    *colour_bg = const_app("string_treeview_table_relegation");
		    }
		}
	    }
	}
    }
    else
    {
	cup_advance = 
	    fixture_get_round_robin_advance(cup_from_clid(table->clid), table->round);
	for(i=0;i<cup_advance->len;i++)
	    if((Team*)g_ptr_array_index(cup_advance, i) == elem->team)
	    {
		*colour_bg = const_app("string_treeview_table_promotion");	
		free_g_ptr_array(&cup_advance);
		return;
	    }

	free_g_ptr_array(&cup_advance);

	treeview_helper_get_table_element_colour_cups_cup(
	    cup_from_clid(table->clid), elem->team, colour_bg);
    }
}

/** Set the char pointers to the constant determining the background and foreground
    colours of user entries in treeviews if the team is a user team. */
void
treeview_helper_set_user_colours(const Team *tm, gchar **colour_bg, gchar **colour_fg)
{
    if(tm == current_user.tm)
    {
	*colour_fg = const_app("string_treeview_current_user_fg");
	*colour_bg = const_app("string_treeview_current_user_bg");
    }
    else if(team_is_user(tm) != -1)
    {
	*colour_fg = const_app("string_treeview_user_fg");
	*colour_bg = const_app("string_treeview_user_bg");
    }
    else
    {
	*colour_fg = const_app("string_treeview_helper_color_default_foreground");
	*colour_bg = const_app("string_treeview_helper_color_default_background");
    }
}


/** Determine the attributes shown in the player list depending on the scout
    quality. */
PlayerListAttribute
treeview_helper_get_attributes_from_scout(gint scout)
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
	    attribute.on_off[PLAYER_LIST_ATTRIBUTE_GOALS] = 
	    attribute.on_off[PLAYER_LIST_ATTRIBUTE_STATUS] = 
	    /*d*/
	    attribute.on_off[PLAYER_LIST_ATTRIBUTE_FITNESS] = 
	    attribute.on_off[PLAYER_LIST_ATTRIBUTE_CSKILL] = 
	    attribute.on_off[PLAYER_LIST_ATTRIBUTE_CPOS] = TRUE;

    return attribute;
}

/** Render a cell in the team selection treeview.
    @see The GTK reference. */
void
treeview_helper_team_selection(GtkTreeViewColumn *col,
			       GtkCellRenderer   *renderer,
			       GtkTreeModel      *model,
			       GtkTreeIter       *iter,
			       gpointer           user_data)
{
    gchar buf[SMALL];
    gint column = treeview_helper_get_col_number_column(col);
    gpointer team_pointer;

    gtk_tree_model_get(model, iter, column, &team_pointer, -1);

    if(column == 2)
	g_object_set(renderer, "text", ((Team*)team_pointer)->name, NULL);
    else if(column == 4)
    {
	sprintf(buf, "%.1f", team_get_average_skill((Team*)team_pointer, FALSE));
	g_object_set(renderer, "text", buf, NULL);
    }
    else
	g_warning("treeview_helper_team_selection: unknown column: %d\n", column);
}

/** Render an integer. This is only so that we know when to draw nothing. */
void
treeview_helper_int_to_cell(GtkTreeViewColumn *col,
			  GtkCellRenderer   *renderer,
			  GtkTreeModel      *model,
			  GtkTreeIter       *iter,
			  gpointer           user_data)
{
    gint column = treeview_helper_get_col_number_column(col);
    gint value;
    gchar  buf[SMALL];

    gtk_tree_model_get(model, iter, column, &value, -1);
    
    if(value == const_int("int_treeview_helper_int_empty"))
	strcpy(buf, "");
    else
	sprintf(buf, "%d", value);

    g_object_set(renderer, "text", buf, NULL);
}

/** Render a cell in the player info view. */
void
treeview_helper_player_ext_info_to_cell(GtkTreeViewColumn *col,
				      GtkCellRenderer   *renderer,
				      GtkTreeModel      *model,
				      GtkTreeIter       *iter,
				      gpointer           user_data)
{
    gint column = treeview_helper_get_col_number_column(col);
    gint row_idx = treeview_helper_iter_get_row(model, iter);
    const Player *pl;

    g_object_set(renderer, "text", "", "foreground",
		 const_app("string_treeview_helper_color_default_foreground"),
		 "background",
		 const_app("string_treeview_helper_color_default_background"), NULL);

    gtk_tree_model_get(model, iter, column, &pl, -1);

    if(pl == NULL)
	return;

    switch(row_idx)
    {
	default:
	    g_warning("treeview_helper_player_ext_info_to_cell: unknown row index %d\n",
		      row_idx);
	    break;
	case PLAYER_INFO_ATTRIBUTE_NAME:
	    treeview_helper_player_to_cell(col, renderer, model,
					 iter, GINT_TO_POINTER(PLAYER_LIST_ATTRIBUTE_NAME));
	    break;
	case PLAYER_INFO_ATTRIBUTE_POS:
	    treeview_helper_player_to_cell(col, renderer, model,
					 iter, GINT_TO_POINTER(PLAYER_LIST_ATTRIBUTE_POS));
	    break;
	case PLAYER_INFO_ATTRIBUTE_CPOS:
	    treeview_helper_player_to_cell(col, renderer, model,
					 iter, GINT_TO_POINTER(PLAYER_LIST_ATTRIBUTE_CPOS));
	    break;
	case PLAYER_INFO_ATTRIBUTE_SKILL:
	    treeview_helper_player_to_cell(col, renderer, model,
					 iter, GINT_TO_POINTER(PLAYER_LIST_ATTRIBUTE_SKILL));
	    break;
	case PLAYER_INFO_ATTRIBUTE_CSKILL:
	    treeview_helper_player_to_cell(col, renderer, model,
					 iter, GINT_TO_POINTER(PLAYER_LIST_ATTRIBUTE_CSKILL));
	    break;
	case PLAYER_INFO_ATTRIBUTE_FITNESS:
	    treeview_helper_player_to_cell(col, renderer, model,
					 iter, GINT_TO_POINTER(PLAYER_LIST_ATTRIBUTE_FITNESS));
	    break;
	case PLAYER_INFO_ATTRIBUTE_ETAL:
	    treeview_helper_player_to_cell(col, renderer, model,
					 iter, GINT_TO_POINTER(PLAYER_LIST_ATTRIBUTE_ETAL));
	    break;
	case PLAYER_INFO_ATTRIBUTE_AGE:
	    treeview_helper_player_to_cell(col, renderer, model,
					 iter, GINT_TO_POINTER(PLAYER_LIST_ATTRIBUTE_AGE));
	    break;
	case PLAYER_INFO_ATTRIBUTE_VALUE:
	    treeview_helper_player_to_cell(col, renderer, model,
					 iter, GINT_TO_POINTER(PLAYER_LIST_ATTRIBUTE_VALUE));
	    break;
	case PLAYER_INFO_ATTRIBUTE_WAGE:
	    treeview_helper_player_to_cell(col, renderer, model,
					 iter, GINT_TO_POINTER(PLAYER_LIST_ATTRIBUTE_WAGE));
	    break;
	case PLAYER_INFO_ATTRIBUTE_CONTRACT:
	    treeview_helper_player_to_cell(col, renderer, model,
					 iter, GINT_TO_POINTER(PLAYER_LIST_ATTRIBUTE_CONTRACT));
	    break;
	case PLAYER_INFO_ATTRIBUTE_HEALTH:
	    treeview_helper_player_info_health_to_cell(renderer, pl);
	    break;
	case PLAYER_INFO_ATTRIBUTE_GAMES_GOALS:
	    treeview_helper_player_info_games_goals_to_cell(renderer, pl->games_goals);
	    break;
	case PLAYER_INFO_ATTRIBUTE_YELLOW_CARDS:
	    treeview_helper_player_info_yellow_to_cell(renderer, pl->cards);
	    break;
	case PLAYER_INFO_ATTRIBUTE_BANNED:
	    treeview_helper_player_info_banned_to_cell(renderer, pl->cards);
	    break;
	case PLAYER_INFO_ATTRIBUTE_STREAK:
	    treeview_helper_player_info_streak_to_cell(renderer, pl->streak);
	    break;
	case PLAYER_INFO_ATTRIBUTE_CAREER:
	    treeview_helper_player_info_career_to_cell(renderer, pl);
	    break;
	case PLAYER_INFO_ATTRIBUTE_OFFERS:
	    if(pl->offers > 0)
		g_object_set(renderer, "text", _("Player doesn't negotiate anymore"), NULL);
	    else
		g_object_set(renderer, "text", _("Player accepts new offers"), NULL);
	    break;
    }
}

void
treeview_helper_player_info_career_to_cell(GtkCellRenderer *renderer, const Player *pl)
{
    gint i;
    gfloat goals_game = 0,
	shot_perc = 0;
    gchar *titles[PLAYER_VALUE_END] =
	{_("Games"),
	 _("Goals"),
	 _("Shots"),
	 _("Yellows"),
	 _("Reds")};
    gchar buf[SMALL], buf2[SMALL], format[SMALL];

    strcpy(buf, "");
    for(i=0;i<PLAYER_VALUE_END;i++)
    {
	sprintf(buf2, "%s %d ", titles[i], pl->career[i]);
	strcat(buf, buf2);
    }

    if(pl->career[PLAYER_VALUE_GAMES] > 0)
	goals_game = (gfloat)pl->career[PLAYER_VALUE_GOALS] /
	    (gfloat)pl->career[PLAYER_VALUE_GAMES];

    if(pl->pos == PLAYER_POS_GOALIE)
    {
	strcpy(format, _("\nGoals/Game %.1f Save %% %.1f"));
	if(pl->career[PLAYER_VALUE_SHOTS] > 0)
	    shot_perc = 100 - ((gfloat)pl->career[PLAYER_VALUE_GOALS] * 100 /
			       (gfloat)pl->career[PLAYER_VALUE_SHOTS]);
    }
    else
    {
	strcpy(format, _("\nGoals/Game %.1f Shot %% %.1f"));
	if(pl->career[PLAYER_VALUE_SHOTS] > 0)
	    shot_perc = (gfloat)pl->career[PLAYER_VALUE_GOALS] * 100 /
		(gfloat)pl->career[PLAYER_VALUE_SHOTS];
    }

    sprintf(buf2, format, goals_game, shot_perc);    
    strcat(buf, buf2);

    g_object_set(renderer, "text", buf, NULL);
}

void
treeview_helper_player_info_banned_to_cell(GtkCellRenderer *renderer, const GArray *cards)
{
    gint i;
    gchar buf[SMALL], buf2[SMALL];

    strcpy(buf, "");

    for(i=0;i<cards->len;i++)
	if(g_array_index(cards, PlayerCard, i).red > 0)
	{
	    /* Ban info of a player in the format:
	       'Cup/league name: Number of weeks banned' */
	    sprintf(buf2, _("%s: %d weeks\n"),
		    league_cup_get_name_string(g_array_index(cards, PlayerCard, i).clid),
		    g_array_index(cards, PlayerCard, i).red);
	    strcat(buf, buf2);
	}	

    g_object_set(renderer, "text", buf, NULL);

    if(strlen(buf) > 0)
	g_object_set(renderer, "background", 
		     const_app("string_treeview_helper_color_player_banned"), NULL);
}

void
treeview_helper_player_info_yellow_to_cell(GtkCellRenderer *renderer, const GArray *cards)
{
    gint i;
    gint yellow_red = -1;
    gchar buf[SMALL], buf2[SMALL];

    strcpy(buf, "");

    for(i=0;i<cards->len;i++)
    {
	yellow_red = league_cup_get_yellow_red(g_array_index(cards, PlayerCard, i).clid);

	if(g_array_index(cards, PlayerCard, i).yellow > 0)
	{
	    if(yellow_red < 1000)
	    {
		sprintf(buf2, "%s: %d (%d)\n",
			league_cup_get_name_string(g_array_index(cards, PlayerCard, i).clid),
			g_array_index(cards, PlayerCard, i).yellow, yellow_red);
	    }
	    else
		/* Yellow cards of a player in a cup/league. No limit means there isn't a limit
		   after which the player gets banned for a match automatically. */
		sprintf(buf2, _("%s: %d (no limit)\n"),
			league_cup_get_name_string(g_array_index(cards, PlayerCard, i).clid),
			g_array_index(cards, PlayerCard, i).yellow);
	    
	    strcat(buf, buf2);
	}
    }

    g_object_set(renderer, "text", buf, NULL);
}

void
treeview_helper_player_info_games_goals_to_cell(GtkCellRenderer *renderer, const GArray *games_goals)
{
    gint i;
    gchar buf[SMALL], buf2[SMALL];

    strcpy(buf, "");

    for(i=0;i<games_goals->len;i++)
    {
	sprintf(buf2, "%s: %d/%d\n", 
		league_cup_get_name_string(g_array_index(games_goals, PlayerGamesGoals, i).clid),
		g_array_index(games_goals, PlayerGamesGoals, i).games,
		g_array_index(games_goals, PlayerGamesGoals, i).goals);		
	strcat(buf, buf2);
    }

    g_object_set(renderer, "text", buf, NULL);
}

void
treeview_helper_player_info_streak_to_cell(GtkCellRenderer *renderer, gint streak)
{
    if(streak == PLAYER_STREAK_HOT)
	g_object_set(renderer, "text", _("The player is on a hot streak"), NULL);
    else if(streak == PLAYER_STREAK_COLD)
	g_object_set(renderer, "text", _("The player is on a cold streak"), NULL);
}

void
treeview_helper_player_info_health_to_cell(GtkCellRenderer *renderer, const Player *pl)
{
    gchar buf[SMALL];

    if(pl->health != 0)
    {
	sprintf(buf, _("%s (expected recovery in %d weeks)"),
		player_injury_to_char(pl->health), pl->recovery);
	g_object_set(renderer, "background",
		     const_app("string_treeview_helper_color_player_injury"), NULL);
    }
    else
	/* Player health: ok. */
	strcpy(buf, _("OK"));

    g_object_set(renderer, "text", buf, NULL);
}

/** Render a player list cell. */
void
treeview_helper_player_to_cell(GtkTreeViewColumn *col,
			       GtkCellRenderer   *renderer,
			       GtkTreeModel      *model,
			       GtkTreeIter       *iter,
			       gpointer           user_data)
{
    gint column = treeview_helper_get_col_number_column(col);
    gint attribute = GPOINTER_TO_INT(user_data);
    gchar  buf[SMALL];
    const Player *pl;

    g_object_set(renderer, "text", "", "foreground",
		 const_app("string_treeview_helper_color_default_foreground"),
		 "background",
		 const_app("string_treeview_helper_color_default_background"), NULL);
    strcpy(buf, "");

    gtk_tree_model_get(model, iter, column, &pl, -1);

    if(pl == NULL)
	return;

    switch(attribute)
    {
	default:
	    g_warning("treeview_helper_player_to_cell: unknown attribute %d.\n", attribute);
	    break;
	case PLAYER_LIST_ATTRIBUTE_NAME:
	    treeview_helper_player_name_to_cell(renderer, buf, pl);
	    break;
	case PLAYER_LIST_ATTRIBUTE_CPOS:
	    treeview_helper_player_pos_to_cell(renderer, buf, pl, 
					       PLAYER_LIST_ATTRIBUTE_CPOS);
	    break;
	case PLAYER_LIST_ATTRIBUTE_POS:
	    treeview_helper_player_pos_to_cell(renderer, buf, pl,
					       PLAYER_LIST_ATTRIBUTE_POS);
	    break;
	case PLAYER_LIST_ATTRIBUTE_CSKILL:
	    treeview_helper_player_cskill_to_cell(renderer, buf, pl);
	    break;
	case PLAYER_LIST_ATTRIBUTE_SKILL:
	    sprintf(buf, "%.*f", opt_int("int_opt_player_precision"), pl->skill);
	    break;
	case PLAYER_LIST_ATTRIBUTE_FITNESS:
	    treeview_helper_player_fitness_to_cell(renderer, buf, pl->fitness);
	    break;
	case PLAYER_LIST_ATTRIBUTE_GAMES:
	    treeview_helper_player_games_goals_to_cell(buf, pl, PLAYER_VALUE_GAMES);
	    break;
	case PLAYER_LIST_ATTRIBUTE_GOALS:
	    treeview_helper_player_games_goals_to_cell(buf, pl, PLAYER_VALUE_GOALS);
	    break;
	case PLAYER_LIST_ATTRIBUTE_SHOTS:
	    treeview_helper_player_games_goals_to_cell(buf, pl, PLAYER_VALUE_SHOTS);
	    break;
	case PLAYER_LIST_ATTRIBUTE_STATUS:
	    treeview_helper_player_status_to_cell(NULL, renderer, 
						  NULL, NULL, (gpointer)pl);
	    break;
	case PLAYER_LIST_ATTRIBUTE_CARDS:
	    treeview_helper_player_cards_to_cell(buf, pl);
	    break;
	case PLAYER_LIST_ATTRIBUTE_AGE:
	    sprintf(buf, "%.*f", opt_int("int_opt_player_precision"), pl->age);
	    break;
	case PLAYER_LIST_ATTRIBUTE_ETAL:
	    sprintf(buf, "%.*f", opt_int("int_opt_player_precision"),
		    pl->etal[current_user.scout % 10]);
	    break;
	case PLAYER_LIST_ATTRIBUTE_VALUE:
	    misc_print_grouped_int(pl->value, buf);
	    break;
	case PLAYER_LIST_ATTRIBUTE_WAGE:
	    misc_print_grouped_int(pl->wage, buf);
	    break;
	case PLAYER_LIST_ATTRIBUTE_CONTRACT:
	    treeview_helper_player_contract_to_cell(renderer, buf, pl->contract);
	    break;
	case PLAYER_LIST_ATTRIBUTE_TEAM:
	    if(debug < 50)
		sprintf(buf, "%s", pl->team->name);
	    else
		sprintf(buf, "%s (%s)", pl->team->name, pl->team->strategy_sid);
	    break;
	case PLAYER_LIST_ATTRIBUTE_LEAGUE_CUP:
	    strcpy(buf, league_cup_get_name_string(pl->team->clid));
	    break;
    }

    if(attribute != PLAYER_LIST_ATTRIBUTE_STATUS)
	g_object_set(renderer, "text", buf, NULL);
}

/** Render a cell of a player name. */
void
treeview_helper_player_name_to_cell(GtkCellRenderer *renderer, gchar *buf, const Player *pl)
{
    const TransferOffer *off = transfer_player_has_offer(pl);
    const gchar *colour_fg = const_app("string_treeview_helper_color_default_foreground"),
	*colour_bg = const_app("string_treeview_helper_color_default_background");

    strcpy(buf, pl->name);
    if(pl->team == current_user.tm &&
       opt_user_int("int_opt_user_penalty_shooter") == pl->id)
	/* Penalty shooter. */
	strcat(buf, _(" (P)"));
    
    if(off != NULL)
    {
	if(pl->team == current_user.tm)
	{
	    colour_bg = const_app("string_treeview_helper_color_transfer_offer_user_bg");
	    colour_fg = const_app("string_treeview_helper_color_transfer_offer_user_fg");
	}
	else if(off->tm == current_user.tm)
	{
	    colour_bg = const_app("string_treeview_helper_color_transfer_offer_accepted_current_bg");
	    colour_fg = const_app("string_treeview_helper_color_transfer_offer_accepted_current_fg");
	}
	else
	{
	    colour_bg = const_app("string_treeview_helper_color_transfer_offer_accepted_other_bg");
	    colour_fg = const_app("string_treeview_helper_color_transfer_offer_accepted_other_fg");
	}
    }
    else if(query_transfer_current_team_offer(pl))
    {
	    colour_bg = const_app("string_treeview_helper_color_transfer_offer_bg");
	    colour_fg = const_app("string_treeview_helper_color_transfer_offer_fg");
    }
       
    g_object_set(renderer, "background", colour_bg, NULL);
    g_object_set(renderer, "foreground", colour_fg, NULL);
}

/** Render a cell of a player contract.
    @param renderer The cell renderer.
    @param buf The string the cell will contain.
    @param contract_time The contract time in weeks. */
void
treeview_helper_player_contract_to_cell(GtkCellRenderer *renderer, gchar *buf, gfloat contract_time)
{
    sprintf(buf, "%.*f", 1 + opt_int("int_opt_player_precision"),
	    contract_time);

    if(contract_time < const_float("float_treeview_helper_limit_player_contract_below3"))
	g_object_set(renderer, "foreground", 
		     const_app("string_treeview_helper_color_player_contract_below3"), NULL);
    else if(contract_time < const_float("float_treeview_helper_limit_player_contract_below2"))
	g_object_set(renderer, "foreground", 
		     const_app("string_treeview_helper_color_player_contract_below2"), NULL);
    else if(contract_time < const_float("float_treeview_helper_limit_player_contract_below1"))
	g_object_set(renderer, "foreground", 
		     const_app("string_treeview_helper_color_player_contract_below1"), NULL);
    else
	g_object_set(renderer, "foreground", 
		     const_app("string_treeview_helper_color_player_contract_normal"), NULL);
}

/** Render a cell of player yellow cards.
    @param buf The string the cell will contain.
    @param pl The pointer to the player. */
void
treeview_helper_player_cards_to_cell(gchar *buf, const Player *pl)
{
    gint yellow;
    Fixture *fix =
	team_get_fixture(pl->team, FALSE);

    if(fix == NULL)
    {
	if(pl->cards->len == 0)
	    strcpy(buf, "0");
	else
	{
	    if(opt_user_int("int_opt_user_show_overall"))
		sprintf(buf, "%d(%d)", g_array_index(pl->cards, PlayerCard, 0).yellow,
			player_all_cards(pl));
	    else
		sprintf(buf, "%d", g_array_index(pl->cards, PlayerCard, 0).yellow);
	}

	return;
    }

    yellow = player_card_get(pl, fix->clid, PLAYER_VALUE_CARD_YELLOW);

    if(yellow != -1)
    {
	if(opt_user_int("int_opt_user_show_overall"))
	    sprintf(buf, "%d(%d)", yellow,
		    player_all_cards(pl));
	else
	    sprintf(buf, "%d", yellow);
    }
    else
	strcpy(buf, "0");
}

/** Render a cell of player status.
    @param render The cell renderer.
    @param buf The string the cell will contain.
    @param pl The pointer to the player. */
void
treeview_helper_player_status_to_cell(GtkTreeViewColumn *col,
				      GtkCellRenderer   *renderer,
				      GtkTreeModel      *model,
				      GtkTreeIter       *iter,
				      gpointer           user_data)
{
    const Player *pl = NULL;
    gchar buf[SMALL];
    gint ban = 0, column = -1;
    GdkPixbuf *symbol = NULL;
    gboolean render_icon = (user_data == NULL);

    if(render_icon)
    {
	column = treeview_helper_get_col_number_column(col);
	gtk_tree_model_get(model, iter, column, &pl, -1);
    }
    else
	pl = (const Player*)user_data;	

    if(pl == NULL)
    {
	if(render_icon)
	    g_object_set(renderer, "pixbuf", NULL, NULL);
	return;
    }

    ban = player_is_banned(pl);
    
    if(pl->health != PLAYER_INJURY_NONE)
    {
	/* Injury info. */
	if(!render_icon)
	{
	    sprintf(buf, _("INJ(%d)"), pl->recovery);
	    g_object_set(renderer, "background",
			 const_app("string_treeview_helper_color_player_injury"), NULL);
	}
	else
	    symbol =
		treeview_helper_pixbuf_from_filename(
		    const_app("string_treeview_helper_player_status_injury"));
    }
    else if(ban > 0)
    {
	/* Red card info (how long the player is banned). */
	if(!render_icon)
	{
	    sprintf(buf, _("BAN(%d)"), ban);
	    g_object_set(renderer, "background",
			 const_app("string_treeview_helper_color_player_banned"), NULL);
	}
	else
	    symbol =
		treeview_helper_pixbuf_from_filename(
		    const_app("string_treeview_helper_player_status_ban"));
    }
    else
    {
	if(!render_icon)
	    /* Player status: ok. */
	    strcpy(buf, _("OK"));
	else
	{
	    if(ban == -1)
		symbol =
		    treeview_helper_pixbuf_from_filename(
			const_app("string_treeview_helper_player_status_yellow_danger"));
	    else if(pl->streak == PLAYER_STREAK_HOT)
		symbol =
		    treeview_helper_pixbuf_from_filename(
			const_app("string_treeview_helper_player_status_hot_streak"));
	    else if(pl->streak == PLAYER_STREAK_COLD)
		symbol =
		    treeview_helper_pixbuf_from_filename(
			const_app("string_treeview_helper_player_status_cold_streak"));
	    else
		symbol =
		    treeview_helper_pixbuf_from_filename(
			const_app("string_treeview_helper_player_status_ok"));
	}
    }    

    if(render_icon)
    {
	g_object_set(renderer, "pixbuf", symbol, NULL);
	treeview_helper_unref(G_OBJECT(symbol));
    }
    else
	g_object_set(renderer, "text", buf, NULL);
}

/** Render a cell of player games or goals.
    @param buf The string the cell will contain.
    @param pl The pointer to the player.
    @param goals Whether we render games or goals. */
void
treeview_helper_player_games_goals_to_cell(gchar *buf, const Player *pl, gint type)
{
    Fixture *fix = team_get_fixture(pl->team, FALSE);
    gint clid = pl->team->clid;

    if(pl->games_goals->len == 0)
    {
	strcpy(buf, "0");
	return;
    }

    if(fix != NULL)
	clid = fix->clid;

    if(opt_user_int("int_opt_user_show_overall"))
	sprintf(buf, "%d(%d)", player_games_goals_get(pl, clid, type),
		player_all_games_goals(pl, type));
    else
	sprintf(buf, "%d", player_games_goals_get(pl, clid, type));
}

/** Render a cell of player fitness.
    @param render The cell renderer.
    @param buf The string the cell will contain.
    @param fitness The fitness of the player. */
void
treeview_helper_player_fitness_to_cell(GtkCellRenderer *renderer, gchar *buf, gfloat fitness)
{
    sprintf(buf, "%.*f%%", opt_int("int_opt_player_precision"),
	    fitness * 100);

    if(fitness < const_float("float_treeview_helper_limit_player_fitness_below3"))
	g_object_set(renderer, "foreground", 
		     const_app("string_treeview_helper_color_player_fitness_below3"), NULL);
    else if(fitness < const_float("float_treeview_helper_limit_player_fitness_below2"))
	g_object_set(renderer, "foreground", 
		     const_app("string_treeview_helper_color_player_fitness_below2"), NULL);
    else if(fitness < const_float("float_treeview_helper_limit_player_fitness_below2"))
	g_object_set(renderer, "foreground", 
		     const_app("string_treeview_helper_color_player_fitness_below2"), NULL);
    else
	g_object_set(renderer, "foreground", 
		     const_app("string_treeview_helper_color_player_fitness_normal"), NULL);
}

/** Render a cell of player position or cposition.
    @param render The cell renderer.
    @param buf The string the cell will contain.
    @param pos The position of the player. */
void
treeview_helper_player_pos_to_cell(GtkCellRenderer *renderer, gchar *buf, 
				   const Player *pl, gint type)
{
    gint pos = (type == PLAYER_LIST_ATTRIBUTE_POS) ?
	pl->pos : pl->cpos;

    switch(pos)
    {
	default:
	    /* Goalie */
	    strcpy(buf, _("G"));
	    g_object_set(renderer, "background", 
			 const_app("string_treeview_helper_color_player_pos_goalie_bg"),
			 "foreground", 
			 const_app("string_treeview_helper_color_player_pos_goalie_fg"), NULL);
	    break;
	case PLAYER_POS_DEFENDER:
	    /* Defender */
	    strcpy(buf, _("D"));
	    g_object_set(renderer, "background", 
			 const_app("string_treeview_helper_color_player_pos_defender_bg"),
			 "foreground", 
			 const_app("string_treeview_helper_color_player_pos_defender_fg"), NULL);
	    break;
	case PLAYER_POS_MIDFIELDER:
	    /* Midfielder */
	    strcpy(buf, _("M"));
	    g_object_set(renderer, "background", 
			 const_app("string_treeview_helper_color_player_pos_midfielder_bg"),
			 "foreground", 
			 const_app("string_treeview_helper_color_player_pos_midfielder_fg"), NULL);
	    break;
	case PLAYER_POS_FORWARD:
	    /* Forward */
	    strcpy(buf, _("F"));
	    g_object_set(renderer, "background", 
			 const_app("string_treeview_helper_color_player_pos_forward_bg"),
			 "foreground", 
			 const_app("string_treeview_helper_color_player_pos_forward_fg"), NULL);
	    break;
    }

    if(pl->cskill == 0)
	g_object_set(renderer, "background", 
		     const_app("string_treeview_helper_color_player_pos_disabled_bg"),
		     "foreground", 
		     const_app("string_treeview_helper_color_player_pos_disabled_fg"), NULL);
}

/** Render a cell of player cskill.
    @param render The cell renderer.
    @param buf The string the cell will contain.
    @param pl The pointer to the player. */
void
treeview_helper_player_cskill_to_cell(GtkCellRenderer *renderer, gchar *buf, const Player *pl)
{
    sprintf(buf, "%.*f", opt_int("int_opt_player_precision"),
	    player_get_game_skill(pl, FALSE) /
	    (1 + (gfloat)pl->streak * const_float("float_player_streak_influence_skill")));
	    
    if(pl->cskill < pl->skill)
	g_object_set(renderer, "background", 
		     const_app("string_treeview_helper_color_player_bad_cskill_bg"),
		     "foreground", 
		     const_app("string_treeview_helper_color_player_bad_cskill_fg"), NULL);
    else
	g_object_set(renderer, "background", 
		     const_app("string_treeview_helper_color_default_background"),
		     "foreground", 
		     const_app("string_treeview_helper_color_default_foreground"), NULL);
	
}

/** Render the result (team names and goals) in the live game view. */
void
treeview_helper_live_game_result(GtkTreeViewColumn *col,
				 GtkCellRenderer   *renderer,
				 GtkTreeModel      *model,
				 GtkTreeIter       *iter,
				 gpointer           user_data)
{    
    gint column = treeview_helper_get_col_number_column(col);
    gchar  buf[SMALL];
    gchar *data = NULL;

    strcpy(buf, "");

    gtk_tree_model_get(model, iter, column, &data, -1);
    sprintf(buf, "<span %s>%s</span>", 
	    const_app("string_treeview_helper_live_game_result_attributes"),
	    data);

    g_object_set(renderer, "markup", buf, NULL);
}

gint
treeview_helper_int_compare(GtkTreeModel *model,
			    GtkTreeIter *a,
			    GtkTreeIter *b,
			    gpointer user_data)
{
    gint column = GPOINTER_TO_INT(user_data);
    gint value1, value2;

    gtk_tree_model_get(model, a, column, &value1, -1);
    gtk_tree_model_get(model, b, column, &value2, -1);

    return misc_int_compare(value1, value2);
}

void
treeview_helper_mm_teams(GtkTreeViewColumn *col,
			 GtkCellRenderer   *renderer,
			 GtkTreeModel      *model,
			 GtkTreeIter       *iter,
			 gpointer           user_data)
{
    gint column = treeview_helper_get_col_number_column(col);
    const MemMatch *mm = NULL;
    const gchar *text = NULL;

    g_object_set(renderer, "background",
		 const_app("string_treeview_helper_color_default_background"), NULL);
    g_object_set(renderer, "foreground",
		 const_app("string_treeview_helper_color_default_foreground"), NULL);
    g_object_set(renderer, "text", "", NULL);

    if(column == 1 || column == 2)
    {
	gtk_tree_model_get(model, iter, column, &mm, -1);
	if(mm != NULL)
	{
	    if(column == 1)
		g_object_set(renderer, "text", mm->lg.team_names[mm->user_team], NULL);
	    else
		g_object_set(renderer, "text", mm->lg.team_names[!mm->user_team], NULL);

	    if(!mm->neutral &&
	       ((column == 1 && mm->user_team == 1) ||
		(column == 2 && mm->user_team == 0)))
		g_object_set(renderer, "background",
			     const_app("string_treeview_live_game_commentary_away_bg"), NULL);
	}
	else if(column == 1)
	{
	    g_object_set(renderer, "text", _("ADD LAST MATCH"), NULL);
	    g_object_set(renderer, "background", 
			 const_app("string_treeview_helper_mmatches_add_bg"), NULL);
	    g_object_set(renderer, "foreground", 
			 const_app("string_treeview_helper_mmatches_add_fg"), NULL);
	}
    }
    else if(column >= TREEVIEW_MMATCH_COL_REPLAY)
    {
	gtk_tree_model_get(model, iter, column, &text, -1);
	g_object_set(renderer, "text", text, NULL);
    }
    
    gtk_tree_model_get(model, iter, 1, &mm, -1);

    if(mm != NULL)
    {
	if(column == TREEVIEW_MMATCH_COL_REPLAY)
	{
	    g_object_set(renderer, "background",
			 const_app("string_treeview_helper_mmatches_replay_bg"), NULL);
	    g_object_set(renderer, "foreground",
			 const_app("string_treeview_helper_mmatches_replay_fg"), NULL);
	}
	else if(column == TREEVIEW_MMATCH_COL_REMOVE)
	{
	    g_object_set(renderer, "background",
			 const_app("string_treeview_helper_mmatches_remove_bg"), NULL);
	    g_object_set(renderer, "foreground",
			 const_app("string_treeview_helper_mmatches_remove_fg"), NULL);
	}
	else if(column == TREEVIEW_MMATCH_COL_EXPORT)
	{
	    g_object_set(renderer, "background",
			 const_app("string_treeview_helper_mmatches_export_bg"), NULL);
	    g_object_set(renderer, "foreground",
			 const_app("string_treeview_helper_mmatches_export_fg"), NULL);
	}
    }
}

void
treeview_helper_season_results(GtkTreeViewColumn *col,
			       GtkCellRenderer   *renderer,
			       GtkTreeModel      *model,
			       GtkTreeIter       *iter,
			       gpointer           user_data)
{
    gint column = treeview_helper_get_col_number_column(col);
    gchar buf[SMALL];
    const Fixture *fix = NULL;
    gint user_idx = -1;

    gtk_tree_model_get(model, iter, column, &fix, -1);

    if(fix == NULL)
	return;

    user_idx = (fix->team_ids[1] == current_user.tm->id);

    if(column == 3)
    {
	strcpy(buf, fix->teams[!user_idx]->name);
	if(user_idx == 1)
	    g_object_set(renderer, "background",
			 const_app("string_treeview_live_game_commentary_away_bg"), NULL);
	else
	    g_object_set(renderer, "background",
			 const_app("string_treeview_helper_color_default_background"), NULL);
    }
    else if(column == 4)
    {
	if(fix->attendance == -1)
	{
	    strcpy(buf, "");
	    g_object_set(renderer, "background",
			 const_app("string_treeview_helper_color_default_background"), NULL);
	}
	else if((fix->result[0][2] + fix->result[1][2] > 0 && 
		 fix->result[user_idx][2] > fix->result[!user_idx][2]) ||
		(fix->result[0][2] + fix->result[1][2] == 0 && 
		 math_sum_int_array(fix->result[user_idx], 2) >
		 math_sum_int_array(fix->result[!user_idx], 2)))
	{
	    /* Won. */
	    strcpy(buf, _("W"));
	    g_object_set(renderer, "background",
			 const_app("string_treeview_helper_season_results_win_bg"), NULL);
	}
	else if((fix->result[0][2] + fix->result[1][2] > 0 && 
		 fix->result[user_idx][2] < fix->result[!user_idx][2]) ||
		(fix->result[0][2] + fix->result[1][2] == 0 && 
		 math_sum_int_array(fix->result[user_idx], 2) <
		 math_sum_int_array(fix->result[!user_idx], 2)))
	{
	    /* Lost. */
	    strcpy(buf, _("L"));
	    g_object_set(renderer, "background",
			 const_app("string_treeview_helper_season_results_loss_bg"), NULL);
	}
	else
	{
	    /* Draw. */
	    strcpy(buf, _("Dw"));
	    g_object_set(renderer, "background",
			 const_app("string_treeview_helper_season_results_draw_bg"), NULL);
	}
    }
    else if(column == 5)
	fixture_result_to_buf(fix, buf, (user_idx == 1));

    g_object_set(renderer, "text", buf, NULL);
}

void
treeview_helper_bet_odds(GtkTreeViewColumn *col,
			 GtkCellRenderer   *renderer,
			 GtkTreeModel      *model,
			 GtkTreeIter       *iter,
			 gpointer           user_data)
{
    gint column = treeview_helper_get_col_number_column(col);
    gchar buf[SMALL];
    const BetMatch *bet = NULL;
    const BetUser *bet_user = NULL;

    gtk_tree_model_get(model, iter, column, &bet, -1);

    g_object_set(renderer, "background",
		 const_app("string_treeview_helper_color_default_background"), 
		 "foreground",
		 const_app("string_treeview_helper_color_default_foreground"), 
		 "text", "", NULL);

    if(bet == NULL)
	return;

    strcpy(buf, "");
    bet_user = bet_is_user(bet);

    if(bet->fix->attendance == -1)
	sprintf(buf, "%.2f", bet->odds[column - 1]);
    else if(column == 2)
	sprintf(buf, "%d - %d", bet->fix->result[0][0],
		bet->fix->result[1][0]);
    
    if(bet->fix->attendance == -1 &&
       bet_user != NULL &&
       bet_user->outcome == column - 1)
	g_object_set(renderer, "background",
		     const_app("string_treeview_helper_color_user_bet_bg"), 
		     "foreground",
		     const_app("string_treeview_helper_color_user_bet_fg"), NULL);

    g_object_set(renderer, "text", buf, NULL);
}
