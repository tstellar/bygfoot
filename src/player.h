#ifndef PLAYER_H
#define PLAYER_H

#include "variables.h"
#include "bygfoot.h"

/**
   Player attribute indices.
*/
enum PlayerAttributes
{
    PLAYER_POS = 0, /**< Position. 0 Goalie, 1 Defender, 2 Midfielder, 3 Forward. */
    PLAYER_CPOS, /**< Current position. */
    PLAYER_SKILL, /**< Skill. Between 0 and 99. */
    PLAYER_CSKILL, /**< Current Skill. */
    PLAYER_AGE, /**< Age in weeks */
    PLAYER_PEAK_AGE, /**< Age at which the player reaches his peak ability. */
    PLAYER_TALENT, /**< Talent. The peak ability (which isn't always reached). */
    PLAYER_ETAL, /**< Estimated talent (the user never sees the actual talent). */
    PLAYER_FITNESS, /**< Fitness. Between 0 and 99. */
    PLAYER_HEALTH, /**< Health. An integer signifying an injury or good health. */
    PLAYER_RECOVERY, /**< Weeks until the player gets healthy. */
    PLAYER_TEAMID, /**< Id of the player's team. */
    PLAYER_VALUE, /**< Value of the player. */
    PLAYER_WAGE, /**< Wage of the player. */
    PLAYER_LSU, /**< Last skill update. Number of weeks since the player skill was last updated. */
    PLAYER_GOALS, /**< Number of goals (scored for field players or conceded for goalies). */
    PLAYER_GAMES, /**< Number of games the player played. */
    PLAYER_END
};

/**
   Representation of a player.
   @see #PlayerAttributes
*/
typedef struct
{
    GString *name;   
    gint values[PLAYER_END];
    /** Array of cards; one item per league and cup.
	@see PlayerCard*/
    GArray *cards;
    /** Player history. To be specified. */
    GArray *history;
} Player;

/**
   Cards in different cups are counted separately for players;
   for each league or cup the cards are stored in such a struct.
*/
typedef struct
{
    /** Numerical id of the league or cup. */
    gint league_cup_id;
    /** Number of yellow cards. */
    gint yellow;
    /** Number of weeks the player is banned. */
    gint red;
} PlayerCard;

#endif
