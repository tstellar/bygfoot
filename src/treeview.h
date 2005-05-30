#ifndef TREEVIEW_H
#define TREEVIEW_H

#include "bygfoot.h"
#include "league_struct.h"
#include "live_game_struct.h"
#include "player_struct.h"
#include "user_struct.h"
#include "table_struct.h"

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
treeview_set_up_player_list (GtkTreeView *treeview, gint *attributes, gint max, gboolean show_separator);

void
treeview_show_player_list(GtkTreeView *treeview, GPtrArray *players, PlayerListAttribute attribute,
			  gboolean show_separator);

void
treeview_show_user_player_list(void);

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
treeview_show_users(GtkTreeView *treeview);

GtkTreeModel*
treeview_create_users(void);

void
treeview_set_up_users(GtkTreeView *treeview);

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
treeview_create_fixtures_header(const Fixture *fix, GtkListStore *liststore, gboolean blank_line);

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
treeview_table_write_header(GtkListStore *ls, const Table *table, gint number);

void
treeview_show_finances(GtkTreeView *treeview, const User* user);

void
treeview_set_up_finances(GtkTreeView *treeview);

GtkTreeModel*
treeview_create_finances(const User* user);

void
treeview_show_player_list_team(GtkTreeView *treeview, const Team *tm, gint scout);

void
treeview_show_transfer_list(GtkTreeView *treeview);

void
treeview_show_next_opponent(void);

void
treeview_set_up_next_opponent(GtkTreeView *treeview);

GtkTreeModel*
treeview_create_next_opponent(void);

void
treeview_show_league_results(GtkTreeView *treeview);

void
treeview_set_up_league_results(GtkTreeView *treeview);

GtkTreeModel*
treeview_create_league_results(void);

void
treeview_create_next_opponent_values(GtkListStore *liststore, const Fixture *fix);

void
treeview_show_all_players(gint clid);

GtkTreeModel*
treeview_create_preview(void);

void
treeview_show_preview(void);

void
treeview_show_player_info(const Player *pl);

void
treeview_set_up_player_info(GtkTreeView *treeview);

GtkTreeModel*
treeview_create_player_info(const Player *pl);

void
treeview_create_stadium_summary(GtkListStore *liststore);

void
treeview_show_fixtures_week(gint week_number, gint week_round_number);

GtkTreeModel*
treeview_create_fixtures_week(gint week_number, gint week_round_number);

void
treeview_show_user_history(void);

void
treeview_set_up_user_history(GtkTreeView *treeview);

GtkTreeModel*
treeview_create_user_history(void);

void
treeview_show_league_stats(gint clid);

void
treeview_set_up_league_stats(GtkTreeView *treeview);

void
treeview_create_league_stats(GtkListStore *ls, const LeagueStat *league_stat);

void
treeview_show_season_history(gint page, gint season_number);

void
treeview_create_season_history_champions(GtkListStore *ls, const GArray* league_champs,
					 const GArray *cup_champs);

GtkTreeModel*
treeview_create_country_list(const GPtrArray *country_list);

void
treeview_show_contributors(const OptionList *help_list);

#endif
