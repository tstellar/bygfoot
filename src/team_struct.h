#ifndef TEAM_STRUCT_H
#define TEAM_STRUCT_H

#include "bygfoot.h"

/**
   Playing styles for teams.
   @see The match result calculating functions.
*/
enum TeamPlayingStyle
{
    PLAYING_STYLE_ALL_OUT_DEFEND = -2,
    PLAYING_STYLE_DEFEND,
    PLAYING_STYLE_BALANCED,
    PLAYING_STYLE_ATTACK,
    PLAYING_STYLE_ALL_OUT_ATTACK,
    PLAYING_STYLE_END
};

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
    LEAGUE_CUP_VALUE_CUP_TYPE,
    LEAGUE_CUP_VALUE_SKILL_DIFF,
    LEAGUE_CUP_VALUE_END
};

/** The stadium of a team. */
typedef struct
{    
    gint capacity, /**< How many people fit in. Default: -1 (depends on league). */
	safety, /**< Safety percentage between 0 and 100. Default: randomized. */
	average_attendance, /**< How many people watched on average. Default: 0. */
	possible_attendance, /**< How many people would've watched if every game had been
				sold out. We need this only to compute the average attendance in percentage
				of the capacity. Default: 0. */
	games; /**< Number of games. Default: 0. */
} Stadium;

/** Structure representing a team.
    @see Player */
typedef struct
{
    GString *name;
    GString *symbol;
    
    gint clid, /**< Numerical id of the league or cup the team belongs to. */
	id, /**< Id of the team. */
	structure, /**< Playing structure. @see team_assign_playing_structure() */
	style, /**< Playing style. @see team_assign_playing_style() */
	boost; /**< Whether player boost or anti-boost is switched on. */
    
    Stadium stadium;
    /**
       Array of players.
    */
    GArray *players;
} Team;

#endif
