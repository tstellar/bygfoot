#ifndef LEAGUE_H
#define LEAGUE_H

#include "bygfoot.h"
#include "league_struct.h"

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
