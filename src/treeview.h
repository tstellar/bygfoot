#ifndef TREEVIEW_H
#define TREEVIEW_H

#include "bygfoot.h"
#include "live_game_struct.h"
#include "player_struct.h"
#include "user_struct.h"
#include "table_struct.h"

void
treeview_select_row(GtkTreeView *treeview, GdkEventButton *event);

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
treeview_create_team_selection_list(gboolean show_cup_teams,
				    gboolean show_user_teams);

void
treeview_set_up_team_selection_treeview (GtkTreeView *treeview);

void
treeview_show_team_list(GtkTreeView *treeview, gboolean show_cup_teams,
			gboolean show_user_teams);

GtkTreeModel*
treeview_create_player_list(GPtrArray *players, gint *attributes, gint max, gboolean show_separator);

void
treeview_set_up_player_list (GtkTreeView *treeview, gint *attributes, gint max);

void
treeview_show_player_list(GtkTreeView *treeview, GPtrArray *players, PlayerListAttribute attribute,
			  gboolean show_separator);

void
treeview_show_user_player_list(const User *user);

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

void
treeview_show_users_startup(void);

GtkTreeModel*
treeview_create_users_startup(void);

void
treeview_set_up_users_startup(GtkTreeView *treeview);

GtkTreeModel*
treeview_create_game_stats(LiveGame *live_game);

void
treeview_show_game_stats(GtkTreeView *treeview, LiveGame *live_game);

void
treeview_set_up_game_stats(GtkTreeView *treeview);

void
treeview_show_fixtures(GtkTreeView *treeview, gint clid, 
		       gint week_number, gint week_round_number);

void
treeview_set_up_fixtures(GtkTreeView *treeview);

GtkTreeModel*
treeview_create_fixtures(gint clid, gint week_number, gint week_round_number);

void
treeview_create_fixtures_header(const Fixture *fix, GtkListStore *liststore);

void
treeview_create_fixture(const Fixture *fix, GtkListStore *liststore);

void
treeview_show_table(GtkTreeView *treeview, gint clid);

void
treeview_set_up_table(GtkTreeView *treeview);

GtkTreeModel*
treeview_create_table(gint clid);

void
treeview_create_single_table(GtkListStore *liststore, const Table *table, gint number);

void
treeview_table_write_header(GtkListStore *liststore, gint clid, gint number);

void
treeview_get_table_element_colours(const Table *table, gint idx, gchar *colour_fg, 
				   gchar *colour_bg, gboolean user);

void
treeview_show_finances(GtkTreeView *treeview, const User* user);

void
treeview_set_up_finances(GtkTreeView *treeview);

GtkTreeModel*
treeview_create_finances(const User* user);

void
treeview_show_player_list_team(GtkTreeView *treeview, const Team *tm, gint scout);

PlayerListAttribute
treeview_get_attributes_from_scout(gint scout);

void
treeview_show_transfer_list(GtkTreeView *treeview);

#endif
