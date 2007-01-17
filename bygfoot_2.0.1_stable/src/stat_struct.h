/*
   stat_struct.h

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

#ifndef STAT_STRUCT_H
#define STAT_STRUCT_H

/** A statistics element holding some
    string and integer values. */
typedef struct
{
    gchar *team_name;
    gint value1, value2, value3;
    gchar *value_string;
} Stat;

/** A structure holding some stat arrays about a league. */
typedef struct
{    
    gchar *league_symbol;
    gchar *league_name;

    /** Best offensive and defensive teams. */
    GArray *teams_off, *teams_def;
    /** Best goal getters and goalies. */
    GArray *player_scorers, *player_goalies;
} LeagueStat;

/** A team name and a competition name. */
typedef struct
{
    gchar *team_name, *cl_name;
} ChampStat;

/** A season statistics structure. */
typedef struct
{
    /** Which season */
    gint season_number;

    /** League and cup winners. */
    GArray *league_champs;
    GArray *cup_champs;
    
    /** The league stats at the end of the season. */
    GArray *league_stats;
} SeasonStat;

#endif
