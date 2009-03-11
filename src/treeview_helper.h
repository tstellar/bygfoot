/*
   treeview_helper.h

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

#ifndef TREEVIEW_HELPER_H
#define TREEVIEW_HELPER_H

#include "bygfoot.h"

#include "league_struct.h"
#include "player_struct.h"
#include "table_struct.h"
#include "team_struct.h"

gint
treeview_helper_iter_get_row(GtkTreeModel *model, GtkTreeIter *iter);

gboolean
treeview_helper_select_row(GtkTreeView *treeview, GdkEventButton *event);

GdkPixbuf*
treeview_helper_pixbuf_from_filename(gchar *filename);

void
treeview_helper_unref(GObject *object);

void
treeview_helper_insert_icon(GtkTreeModel *ls, GtkTreeIter *iter, gint column_nr,
			    gchar *icon_name);

GtkCellRenderer*
treeview_helper_cell_renderer_text_new(void);

gchar*
treeview_helper_live_game_icon(gint event_type);

gint
treeview_helper_get_index(GtkTreeView *treeview, gint column);

gpointer
treeview_helper_get_pointer(GtkTreeView *treeview, gint column);

void
treeview_helper_clear(GtkTreeView *treeview);

gint
treeview_helper_get_col_number_column (GtkTreeViewColumn *col);

void
treeview_helper_get_table_element_colours(const Table *table, gint table_index, gint idx,
					  gchar **colour_fg, 
					  gchar **colour_bg, gboolean user);

gboolean
treeview_helper_get_table_element_colour_cups(const League *league, gint table_index,
					      gint idx, gchar **colour_bg);

gboolean
treeview_helper_get_table_element_colour_cups_cup(const Cup *cup, 
						  const Team *tm, gchar **colour_bg);

void
treeview_helper_set_user_colours(const gchar *team_name,
				 gchar **colour_bg, gchar **colour_fg);

PlayerListAttribute
treeview_helper_get_attributes_from_scout(gint scout);

gint
treeview_helper_team_compare(GtkTreeModel *model,
		      GtkTreeIter *a,
		      GtkTreeIter *b,
		      gpointer user_data);

gint
treeview_helper_player_compare(GtkTreeModel *model,
			GtkTreeIter *a,
			GtkTreeIter *b,
			gpointer user_data);


void
treeview_helper_team_selection(GtkTreeViewColumn *col,
			     GtkCellRenderer   *renderer,
			     GtkTreeModel      *model,
			     GtkTreeIter       *iter,
			     gpointer           user_data);


void
treeview_helper_int_to_cell(GtkTreeViewColumn *col,
			  GtkCellRenderer   *renderer,
			  GtkTreeModel      *model,
			  GtkTreeIter       *iter,
			  gpointer           user_data);

void
treeview_helper_player_to_cell(GtkTreeViewColumn *col,
			     GtkCellRenderer   *renderer,
			     GtkTreeModel      *model,
			     GtkTreeIter       *iter,
			     gpointer           user_data);

void
treeview_helper_player_ext_info_to_cell(GtkTreeViewColumn *col,
				      GtkCellRenderer   *renderer,
				      GtkTreeModel      *model,
				      GtkTreeIter       *iter,
				      gpointer           user_data);

void
treeview_helper_player_name_to_cell(GtkCellRenderer *renderer, gchar *buf, const Player *pl);

void
treeview_helper_player_contract_to_cell(GtkCellRenderer *renderer, gchar *buf, gfloat contract_time);

void
treeview_helper_player_cards_to_cell(gchar *buf, const Player *pl);

void
treeview_helper_player_status_to_cell(GtkTreeViewColumn *col,
				      GtkCellRenderer   *renderer,
				      GtkTreeModel      *model,
				      GtkTreeIter       *iter,
				      gpointer           user_data);

void
treeview_helper_player_games_goals_to_cell(gchar *buf, const Player *pl, gint type);

void
treeview_helper_player_fitness_to_cell(GtkCellRenderer *renderer, gchar *buf, gfloat fitness);

void
treeview_helper_player_pos_to_cell(GtkCellRenderer *renderer, gchar *buf, 
				   const Player *pl, gint type);

void
treeview_helper_live_game_result(GtkTreeViewColumn *col,
			       GtkCellRenderer   *renderer,
			       GtkTreeModel      *model,
			       GtkTreeIter       *iter,
			       gpointer           user_data);

void
treeview_helper_player_cskill_to_cell(GtkCellRenderer *renderer, gchar *buf, const Player *pl);

void
treeview_helper_player_info_health_to_cell(GtkCellRenderer *renderer, const Player *pl);

void
treeview_helper_player_info_games_goals_to_cell(GtkCellRenderer *renderer, const GArray *games_goals);

void
treeview_helper_player_info_yellow_to_cell(GtkCellRenderer *renderer, const GArray *cards);

void
treeview_helper_player_info_banned_to_cell(GtkCellRenderer *renderer, const GArray *cards);

void
treeview_helper_player_info_career_to_cell(GtkCellRenderer *renderer, const Player *pl);

void
treeview_helper_player_info_streak_to_cell(GtkCellRenderer *renderer, gint streak);


gchar*
treeview_helper_get_user_history_icon(gint history_type);

gint
treeview_helper_int_compare(GtkTreeModel *model,
			    GtkTreeIter *a,
			    GtkTreeIter *b,
			    gpointer user_data);

void
treeview_helper_mm_teams(GtkTreeViewColumn *col,
			 GtkCellRenderer   *renderer,
			 GtkTreeModel      *model,
			 GtkTreeIter       *iter,
			 gpointer           user_data);

void
treeview_helper_season_results(GtkTreeViewColumn *col,
			       GtkCellRenderer   *renderer,
			       GtkTreeModel      *model,
			       GtkTreeIter       *iter,
			       gpointer           user_data);

void
treeview_helper_bet_odds(GtkTreeViewColumn *col,
			 GtkCellRenderer   *renderer,
			 GtkTreeModel      *model,
			 GtkTreeIter       *iter,
			 gpointer           user_data);

gboolean
treeview_helper_search_equal_teams(GtkTreeModel *model,
                                   gint column,
                                   const gchar *key,
                                   GtkTreeIter *iter,
                                   gpointer search_data);

gboolean
treeview_helper_search_equal_strings(GtkTreeModel *model,
                                     gint column,
                                     const gchar *key,
                                     GtkTreeIter *iter,
                                     gpointer search_data);

void
treeview_helper_job_exchange(GtkTreeViewColumn *col,
			     GtkCellRenderer   *renderer,
			     GtkTreeModel      *model,
			     GtkTreeIter       *iter,
			     gpointer           user_data);


void
treeview_helper_news(GtkTreeViewColumn *col,
                     GtkCellRenderer   *renderer,
                     GtkTreeModel      *model,
                     GtkTreeIter       *iter,
                     gpointer           user_data);

void
treeview_helper_news_additional(GtkTreeViewColumn *col,
                                GtkCellRenderer   *renderer,
                                GtkTreeModel      *model,
                                GtkTreeIter       *iter,
                                gpointer           user_data);

void
treeview_helper_player_name_editing_done(GtkCellRendererText *renderer,
                                         gchar               *path,
                                         gchar               *new_text,
                                         gpointer             user_data);
void
treeview_helper_player_name_editing_started(GtkCellRenderer *renderer,
                                            GtkCellEditable *editable,
                                            gchar           *path,
                                            gpointer         user_data);

void
treeview_helper_player_name_editing_canceled(GtkCellRendererText *renderer,
                                             gpointer             user_data);

void
treeview_helper_constants_editing_done(GtkCellRendererText *renderer,
                                       gchar               *path,
                                       gchar               *new_text,
                                       gpointer             user_data);

void
treeview_helper_constants_app_editing_done(GtkCellRendererText *renderer,
                                           gchar               *path,
                                           gchar               *new_text,
                                           gpointer             user_data);

#endif
