#ifndef PLAYER_STRUCT_H
#define PLAYER_STRUCT_H

#include "bygfoot.h"
#include "enums.h"
#include "team_struct.h"

/**
   Player positions.
*/
enum PlayerPos
{
    PLAYER_POS_GOALIE = 0,
    PLAYER_POS_DEFENDER,
    PLAYER_POS_MIDFIELDER,
    PLAYER_POS_FORWARD,
    PLAYER_POS_END
};

/**
   Cards in different cups are counted separately for players;
   for each league or cup the cards are stored in such a struct.
*/
typedef struct
{
    /** Numerical id of the league or cup. */
    gint clid;
    /** Number of yellow cards. */
    gint yellow;
    /** Number of weeks the player is banned. */
    gint red;
} PlayerCard;

/**
   Goals and games in different leagues and cups are counted separately for players.
*/
typedef struct
{
    /** Numerical id of the league or cup. */
    gint clid;
    /** Number of games the player played. */
    gint games;
    /** Number of goals (scored for field players or conceded for goalies). */
    gint goals;
    /** Number of shots (taken or faced). */
    gint shots;
} PlayerGamesGoals;

enum PlayerInjury
{
    PLAYER_INJURY_NONE = 0,
    PLAYER_INJURY_END
};

/**
   Representation of a player.
   @see #PlayerAttributes
*/
typedef struct
{
    GString *name;   
    
    gint pos, /**< Position. @see #PlayerPos */
	cpos, /**< Current position. @see #PlayerPos */
	skill, /**< Skill. Between 0 and a constant (specified in the constants file). */
	cskill, /**< Current Skill. */
	age, /**< Age in weeks */
	peak_age, /**< Age at which the player reaches his peak ability. */
	talent, /**< Talent. The peak ability (which isn't always reached). */
	etal[QUALITY_END], /**< Estimated talent (the user never sees the actual talent).
			      Depends on scout quality. */
	fitness, /**< Fitness. Between 0 and 9900. */
	health, /**< Health. An integer signifying an injury or good health. @see #PlayerInjury */
	recovery, /**< Weeks until the player gets healthy. */
	id, /**< Id of the player within the team. */
	value, /**< Value of the player. */
	wage, /**< Wage of the player. */
	contract, /**< The number of weeks until the player's contract expires. */
	lsu; /**< Last skill update. Number of weeks since the player skill was last updated. */

    /** Array of games and goals; one item per league and cup.
	@see PlayerGamesGoals */
    GArray *games_goals;

    /** Array of cards; one item per league and cup.
	@see PlayerCard*/
    GArray *cards;

    /** Pointer to the player's team. */
    Team *team;
} Player;

/** Enum for player attributes that can be shown in a player list. */
enum PlayerListAttributeValue
{
    PLAYER_LIST_ATTRIBUTE_NAME = 0,
    PLAYER_LIST_ATTRIBUTE_CPOS,
    PLAYER_LIST_ATTRIBUTE_POS,
    PLAYER_LIST_ATTRIBUTE_CSKILL,
    PLAYER_LIST_ATTRIBUTE_SKILL,
    PLAYER_LIST_ATTRIBUTE_FITNESS,
    PLAYER_LIST_ATTRIBUTE_GAMES,
    PLAYER_LIST_ATTRIBUTE_GOALS,
    PLAYER_LIST_ATTRIBUTE_SHOTS,
    PLAYER_LIST_ATTRIBUTE_STATUS,
    PLAYER_LIST_ATTRIBUTE_CARDS,
    PLAYER_LIST_ATTRIBUTE_AGE,
    PLAYER_LIST_ATTRIBUTE_ETAL,
    PLAYER_LIST_ATTRIBUTE_VALUE,
    PLAYER_LIST_ATTRIBUTE_WAGE,
    PLAYER_LIST_ATTRIBUTE_CONTRACT,
    PLAYER_LIST_ATTRIBUTE_TEAM,
    PLAYER_LIST_ATTRIBUTE_LEAGUE_CUP,
    PLAYER_LIST_ATTRIBUTE_END
};

/** A struct telling us which player attributes to show in a player list.
    @see #PlayerListAttributeValue*/
typedef struct
{
    gboolean on_off[PLAYER_LIST_ATTRIBUTE_END];
} PlayerListAttribute;

#endif
