#ifndef TREEVIEW_CELL_H
#define TREEVIEW_CELL_H

#include "bygfoot.h"

void
treeview_cell_team_selection(GtkTreeViewColumn *col,
			     GtkCellRenderer   *renderer,
			     GtkTreeModel      *model,
			     GtkTreeIter       *iter,
			     gpointer           user_data);


void
treeview_cell_int_to_cell(GtkTreeViewColumn *col,
			  GtkCellRenderer   *renderer,
			  GtkTreeModel      *model,
			  GtkTreeIter       *iter,
			  gpointer           user_data);

void
treeview_cell_player_to_cell(GtkTreeViewColumn *col,
			     GtkCellRenderer   *renderer,
			     GtkTreeModel      *model,
			     GtkTreeIter       *iter,
			     gpointer           user_data);

void
treeview_cell_player_contract_to_cell(GtkCellRenderer *renderer, gchar *buf, gint contract_time);

void
treeview_cell_player_cards_to_cell(gchar *buf, const Player *pl);

void
treeview_cell_player_status_to_cell(GtkCellRenderer *renderer, gchar *buf, const Player *pl);

void
treeview_cell_player_games_goals_to_cell(gchar *buf, const Player *pl, gint type);

void
treeview_cell_player_fitness_to_cell(GtkCellRenderer *renderer, gchar *buf, gint fitness);

void
treeview_cell_player_pos_to_cell(GtkCellRenderer *renderer, gchar *buf, gint pos);

void
treeview_cell_live_game_result(GtkTreeViewColumn *col,
			       GtkCellRenderer   *renderer,
			       GtkTreeModel      *model,
			       GtkTreeIter       *iter,
			       gpointer           user_data);

void
treeview_cell_player_cskill_to_cell(GtkCellRenderer *renderer, gchar *buf, const Player *pl);

#endif
