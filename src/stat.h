#ifndef STAT_H
#define STAT_H

#include "bygfoot.h"
#include "stat_struct.h"

GArray*
stat_update_league_teams(const GArray *teams, gint compare_type);

void
stat_update_leagues(void);

void
stat_update_league_players(League *league);

#endif
