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

#endif
