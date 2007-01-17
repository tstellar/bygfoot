/*
   stat.h

   Bygfoot Football Manager -- a small and simple GTK2-based
   football management game.

   http://bygfoot.sourceforge.net

   Copyright (C) 2005  Gyözö Both (gyboth@bygfoot.com)

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef STAT_H
#define STAT_H

#include "bygfoot.h"
#include "league_struct.h"
#include "stat_struct.h"

SeasonStat
stat_season_stat_new(gint season_number);

LeagueStat
stat_league_new(const gchar *league_name, const gchar *league_symbol);

GArray*
stat_update_league_teams(const GArray *teams, gint compare_type);

void
stat_update_leagues(void);

void
stat_update_league_players(League *league);

void
stat_create_season_stat(void);

void
stat_show_av_league_goals(void);

#endif
