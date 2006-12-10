/*
   team_struct.h

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

#ifndef TEAM_STRUCT_H
#define TEAM_STRUCT_H

#include "bygfoot.h"

/** @see team_return_league_cup_value_int() */
enum LeagueCupValue
{
    LEAGUE_CUP_VALUE_NAME = 0,
    LEAGUE_CUP_VALUE_SHORT_NAME,
    LEAGUE_CUP_VALUE_SID,
    LEAGUE_CUP_VALUE_SYMBOL,
    LEAGUE_CUP_VALUE_ID,
    LEAGUE_CUP_VALUE_FIRST_WEEK,
    LEAGUE_CUP_VALUE_LAST_WEEK,
    LEAGUE_CUP_VALUE_WEEK_GAP,
    LEAGUE_CUP_VALUE_YELLOW_RED,
    LEAGUE_CUP_VALUE_AVERAGE_SKILL,
    LEAGUE_CUP_VALUE_AVERAGE_CAPACITY,
    LEAGUE_CUP_VALUE_SKILL_DIFF,
    LEAGUE_CUP_VALUE_END
};

/** Some team attributes. */
enum TeamAttribute
{
    TEAM_ATTRIBUTE_STYLE = 0,
    TEAM_ATTRIBUTE_BOOST,
    TEAM_ATTRIBUTE_END
};

/** The stadium of a team. */
typedef struct
{    
    gchar *name;
    gint capacity, /**< How many people fit in. Default: -1 (depends on league). */
	average_attendance, /**< How many people watched on average. Default: 0. */
	possible_attendance, /**< How many people would've watched if every game had been
				sold out. We need this only to compute the average attendance in percentage
				of the capacity. Default: 0. */
	games; /**< Number of games. Default: 0. */
    gfloat safety; /**< Safety percentage between 0 and 100. Default: randomized. */

} Stadium;

/** Structure representing a team.
    @see Player */
typedef struct
{
    gchar *name, *symbol;
    /** File the team takes the 
	player names from. */
    gchar *names_file;
    gchar *def_file;
    /** The sid of the strategy if it's a CPU team. */
    gchar *strategy_sid;

    gint clid, /**< Numerical id of the league or cup the team belongs to. */
	id, /**< Id of the team. */
	structure, /**< Playing structure. @see team_assign_playing_structure() */
	style, /**< Playing style. @see team_assign_playing_style() */
	boost; /**< Whether player boost or anti-boost is switched on. */

    /** Average talent of the players at generation. */
    gfloat average_talent;

    /** A value that influences scoring chances etc.
     If > 1, the team's lucky, if < 1, it's unlucky.
     Only used for users' teams. */
    gfloat luck;    

    Stadium stadium;
    /**
       Array of players.
    */
    GArray *players;
} Team;

#endif
