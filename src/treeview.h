#ifndef TREEVIEW_H
#define TREEVIEW_H

#include "bygfoot.h"
#include "live_game_struct.h"
#include "player_struct.h"

GdkPixbuf*
treeview_pixbuf_from_filename(gchar *filename);

gchar*
treeview_live_game_icon(gint event_type);

gint
treeview_get_index(GtkTreeView *treeview, gint column);

gpointer
treeview_get_pointer(GtkTreeView *treeview, gint column);

void
treeview_clear(GtkTreeView *treeview);

gint
treeview_get_col_number_column (GtkTreeViewColumn *col);

GtkTreeModel*
treeview_create_team_selection_list(gboolean show_cup_teams);

void
treeview_set_up_team_selection_treeview (GtkTreeView *treeview);

void
treeview_show_team_list(GtkTreeView *treeview, gboolean show_cup_teams);

GtkTreeModel*
treeview_create_player_list(GPtrArray *players, gint *attributes, gint max, gboolean show_separator);

void
treeview_set_up_player_list (GtkTreeView *treeview, gint *attributes, gint max);

void
treeview_show_player_list(GtkTreeView *treeview, GPtrArray *players, PlayerListAttribute attribute,
			  gboolean show_separator);

void
treeview_show_user_player_list(gint player_list);

void
treeview_live_game_show_game_unit(const LiveGameUnit *unit);

void
treeview_live_game_show_commentary(const LiveGameUnit *unit);

void
treeview_live_game_show_initial_commentary(const LiveGameUnit *unit);

void
treeview_live_game_show_teams(void);

void
treeview_live_game_set_up_commentary(void);

GtkTreeModel*
treeview_live_game_create_init_commentary(const LiveGameUnit *unit);

GtkTreeModel*
treeview_live_game_create_result(const LiveGameUnit *unit);

void
treeview_live_game_set_up_result(void);

void
treeview_live_game_show_result(const LiveGameUnit *unit);

#endif
