#ifndef LEAUGUE_H
#define LEAUGUE_H

#include "bygfoot.h"
#include "team.h"

/**
   Table element values.
   @see TableElement
   @see Table
*/
enum TableElementValues
{
    TABLE_PLAYED = 0,
    TABLE_WON,
    TABLE_DRAW,
    TABLE_LOST,
    TABLE_GF,
    TABLE_GA,
    TABLE_PTS,
    TABLE_END
};

/**
   An element representing a team in the tables.
   @see Table
   @see #TableElementValues
*/
typedef struct
{
    gint team_id;
    gint values[TABLE_END];
} TableElement;

/**
   A table belonging to a league or a cup with round robin.
   @see TableElement
*/
typedef struct
{
    GString *name;
    GString *league_id;
    GArray *elements;
} Table;

/**
   An element representing a promotion or relegation rule.
   This means, a PromRelElement specifies a range of teams
   that get promoted or relegated to a given league.
   @see PromRel
*/
typedef struct
{
    gint ranks[2]; /**< The range of teams; default 0 and 0 */
    GString *dest_id; /**< The id of the destination league. Default "" */
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
    GString *prom_games_dest_id;
    /** The id of the cup that specifies the promotion games format.
	We regard the promotion games as a national cup like any other cup.
	@see Cup
    */
    GString *prom_games_cup_id; /* "" */

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
    GString *name, *short_name, *id, *symbol;
    /** @see PromRel */
    PromRel prom_rel;
    /** Numerical id, as opposed to the string id. */
    gint nid;
    /** The first week games are played. Default 1. */
    gint first_week;
    /** Weeks between two matchdays. Default 1. */
    gint week_gap;
    /** Number of yellow cards until a player gets banned. 
	Default 1000 (which means 'off', basically). */
    gint yellow_red;
    /** Average skill and talent values for the first season. */
    gint average_skill, average_talent;
    /** Array of teams in the league.
	@see Team */
    GArray *teams;
    /** League table.
	@see Table */
    Table table;
} League;


League
league_new(void);

PromRelElement
prom_rel_element_new(void);

#endif
