#ifndef LEAGUE_H
#define LEAGUE_H

#include "bygfoot.h"
#include "league_struct.h"
#include "fixture_struct.h"

#define league_cup_get_teams(clid) (clid < ID_CUP_START) ? league_from_clid(clid)->teams : cup_from_clid(clid)->teams
#define league_cup_get_fixtures(clid) (clid < ID_CUP_START) ? league_from_clid(clid)->fixtures : cup_from_clid(clid)->fixtures
#define league_cup_get_name_string(clid) (clid < ID_CUP_START) ? league_from_clid(clid)->name->str : cup_from_clid(clid)->name->str
#define league_cup_get_yellow_red(clid) (clid < ID_CUP_START) ? league_from_clid(clid)->yellow_red : cup_from_clid(clid)->yellow_red

League
league_new(void);

PromRelElement
prom_rel_element_new(void);

gint
league_new_id(void);

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

#endif
