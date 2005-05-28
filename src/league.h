#ifndef LEAGUE_H
#define LEAGUE_H

#include "bygfoot.h"
#include "league_struct.h"
#include "fixture_struct.h"

#define league_cup_get_teams(clid) (clid < ID_CUP_START) ? ((gpointer)(league_from_clid(clid)->teams)) : ((gpointer)(cup_from_clid(clid)->teams))
#define league_cup_get_fixtures(clid) (clid < ID_CUP_START) ? (league_from_clid(clid)->fixtures) : (cup_from_clid(clid)->fixtures)
#define league_cup_get_name_string(clid) (clid < ID_CUP_START) ? league_from_clid(clid)->name->str : cup_from_clid(clid)->name->str
#define league_cup_get_yellow_red(clid) (clid < ID_CUP_START) ? (league_from_clid(clid)->yellow_red) : (cup_from_clid(clid)->yellow_red)

#define query_league_has_prom_games(league) (strlen(league->prom_rel.prom_games_dest_sid->str) > 0)

League
league_new(gboolean new_id);

PromRelElement
prom_rel_element_new(void);

TableElement
league_table_element_new(Team *team);

gint
league_cup_get_index_from_clid(gint clid);

League*
league_from_clid(gint clid);

gint
league_cup_get_next_clid(gint clid);

gint
league_cup_get_previous_clid(gint clid);

Fixture*
league_cup_get_next_fixture(gint clid, gint week_number, gint week_round_number);

Fixture*
league_cup_get_previous_fixture(gint clid, gint week_number, gint week_round_number);

gint
league_get_index(gint clid);

gint
league_cup_average_capacity(gint clid);

void
league_get_team_movements(League *league, GArray *team_movements);

gint
league_index_from_sid(const gchar *sid);

void
league_remove_team_with_id(League *league, gint id);

void
league_season_start(League *league);

gboolean
query_league_rank_in_prom_games(const League *league, gint rank);

gboolean
query_league_prom_games_begin(const League *league);

gboolean
query_league_matches_in_week(const League *league, gint week_number);

#endif
