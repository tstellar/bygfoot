/*
   league_struct.h

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

#ifndef LEAGUE_STRUCT_H
#define LEAGUE_STRUCT_H

#include "bygfoot.h"
#include "stat_struct.h"
#include "table_struct.h"

enum PromRelType
{
    PROM_REL_PROMOTION = 0,
    PROM_REL_RELEGATION,
    PROM_REL_NONE
};

/**
   An element representing a promotion or relegation rule.
   This means, a PromRelElement specifies a range of teams
   that get promoted or relegated to a given league.
   @see PromRel
*/
typedef struct
{
    gint ranks[2]; /**< The range of teams; default 0 and 0 */
    gint from_table; /**< From which table to pick the teams in case there are several. Default: 0. */
    gchar *dest_sid; /**< The id of the destination league. Default "" */
    gint type; /**< Type. Promotion or relegation or none. */
} PromRelElement;

/**
   An struct representing promotion/relegation games.
*/
typedef struct
{
    /** The id of the league the promotion games winner gets promoted to. Default "" */
    gchar *dest_sid;
    /** The id of the league the promotion games losers get moved to. Default "" */
    gchar *loser_sid;
    /** Number of teams that advance from the promotion games. Default: 1. */
    gint number_of_advance;    
    /** The cup determining how the promotion games are handled. */
    gchar *cup_sid;

} PromGames;

/**
   This structure specifies how promotion and relegation is handled in a league.
   It contains promotion and relegation rules in an array and possibly also
   a rule about promotion games to be played.
   @see PromRelElement
*/
typedef struct
{
    /** Array with promotion/relegation rules.
	@see PromRelElement
    */
    GArray *elements;

    /** Array with promotion/relegation games.
	@see PromGames
    */
    GArray *prom_games;

} PromRel;

/**
   A structure describing a different league joined to the current one
   in the sense that there are matches played between teams from both leagues
   like in the US conference system.
*/
typedef struct
{
    /** Sid of the joined league. */
    gchar *sid;
    /** How many round robins to schedule. */
    gint rr;
} JoinedLeague;

/**
   A structure containing a week when a new table
   gets created with nullified values for the league;
   older tables get stored.
*/
typedef struct
{
    gint add_week;
    gchar *name;
} NewTable;

/**
   A structure describing a custom break in the fixtures
   schedule occuring at a particular week.
*/
typedef struct
{
    /** In which week the break occurs. */
    gint week_number;
    /** Length of break in weeks. */
    gint length;
} WeekBreak;

/**
   Representation of a league.
   @see PromRel
   @see Table
*/
typedef struct
{
    /** Default value "" */
    gchar *name, *short_name, *sid, *symbol;
    /** The sid of the player names file the 
	teams in the league take their names from.
	Default: 'general', meaning the 'player_names_general.xml' file. */
    gchar *names_file;
    /** @see PromRel */
    PromRel prom_rel;
    /** Numerical id, as opposed to the string id 'sid'. */
    gint id;
    /** Layer of the league; this specifies which leagues are
	parallel. */
    gint layer;
    /** The first week games are played. Default 1. */
    gint first_week;
    /** Weeks between two matchdays. Default 1. */
    gint week_gap;
    /** Here we store intervals of fixtures during which
	there should be two matches in a week instead of one. */
    GArray *two_match_weeks[2];
    /** How many round robins are played. Important for
	small leagues with 10 teams or so. Default: 2. */
    gint round_robins;
    /** Number of weeks between the parts of a round robin. */
    GArray *rr_breaks;
    /** Number of yellow cards until a player gets banned. 
	Default 1000 (which means 'off', basically). */
    gint yellow_red;
    /** Average talent for the first season. Default: -1. */
    gfloat average_talent;
    /** Array of teams in the league.
	@see Team */
    GArray *teams;
    /** List of leagues joined fixture-wise to this one.
	@see JoinedLeague */
    GArray *joined_leagues;
    /** League tables. Usually only one, sometimes more than one is created.
	@see Table */
    GArray *tables;
    /** Array holding NewTable elements specifying when to add
        a new table to the tables array. */
    GArray *new_tables;
    /** The fixtures of a season for the league. */
    GArray *fixtures;
    /** A gchar pointer array of properties (like "inactive"). */
    GPtrArray *properties;
    /** Array of custom breaks in schedule. */
    GArray *week_breaks;
    /** The current league statistics. */
    LeagueStat stats;
    /** Pointer array with the sids of competitions that
        the fixtures of which should be avoided when scheduling
        the league fixtures. */
    GPtrArray *skip_weeks_with;
} League;

#endif
