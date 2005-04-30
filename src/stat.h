#ifndef STAT_H
#define STAT_H

#include "bygfoot.h"
#include "league_struct.h"
#include "stat_struct.h"

LeagueStat
stat_league_new(gint clid);

GArray*
stat_update_league_teams(const GArray *teams, gint compare_type);

void
stat_update_leagues(void);

void
stat_update_league_players(League *league);

#endif
