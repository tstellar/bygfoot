#ifndef LEAGUE_STRUCT_H
#define LEAGUE_STRUCT_H

#include "bygfoot.h"
#include "table_struct.h"

/**
   An element representing a promotion or relegation rule.
   This means, a PromRelElement specifies a range of teams
   that get promoted or relegated to a given league.
   @see PromRel
*/
typedef struct
{
    gint ranks[2]; /**< The range of teams; default 0 and 0 */
    GString *dest_sid; /**< The id of the destination league. Default "" */
} PromRelElement;

/**
   This structure specifies how promotion and relegation is handled in a league.
   It contains promotion and relegation rules in an array and possibly also
   a rule about promotion games to be played.
   @see PromRelElement
*/
typedef struct
{
    /** The id of the league the promotion games winner gets promoted to. Default "" */
    GString *prom_games_dest_sid;
    /** The id of the cup that specifies the promotion games format.
	We regard the promotion games as a national cup like any other cup.
	@see Cup
    */
    GString *prom_games_cup_sid; /* "" */

    /** Array with promotion/relegation rules.
	@see PromRelElement
    */
    GArray *elements;
} PromRel;

/**
   Representation of a league.
   @see PromRel
   @see Table
*/
typedef struct
{
    /** Default value "" */
    GString *name, *short_name, *sid, *symbol;
    /** @see PromRel */
    PromRel prom_rel;
    /** Numerical id, as opposed to the string id 'sid'. */
    gint id;
    /** The first week games are played. Default 1. */
    gint first_week;
    /** Weeks between two matchdays. Default 1. */
    gint week_gap;
    /** Number of yellow cards until a player gets banned. 
	Default 1000 (which means 'off', basically). */
    gint yellow_red;
    /** Average skill for the first season. Default: -1. */
    gint average_skill;
    /** Average stadium capacity. Default: -1. */
    gint average_capacity;
    /** Array of teams in the league.
	@see Team */
    GArray *teams;
    /** League table.
	@see Table */
    Table table;
    /** The fixtures of a season for the league. */
    GArray *fixtures;
} League;

#endif
