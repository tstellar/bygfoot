#ifndef PLAYER_STRUCT_H
#define PLAYER_STRUCT_H

/**
   Player attribute indices.
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
    gint league_cup_id;
    /** Number of yellow cards. */
    gint yellow;
    /** Number of weeks the player is banned. */
    gint red;
} PlayerCard;

/**
   Representation of a player.
   @see #PlayerAttributes
*/
typedef struct
{
    GString *name;   
    
    gint pos, /**< Position. @see #PlayerPos */
	cpos, /**< Current position. @see #PlayerPos */
	skill, /**< Skill. Between 0 and CONSTANT_PLAYER_MAX_SKILL. */
	cskill, /**< Current Skill. */
	age, /**< Age in weeks */
	peak_age, /**< Age at which the player reaches his peak ability. */
	talent, /**< Talent. The peak ability (which isn't always reached). */
	etal, /**< Estimated talent (the user never sees the actual talent). */
	fitness, /**< Fitness. Between 0 and 99. */
	health, /**< Health. An integer signifying an injury or good health. */
	recovery, /**< Weeks until the player gets healthy. */
	clid, /**< Cup or league id of the player's team. */
	team_id, /**< Id of the player's team. */
	id, /**< Id of the player within the team. */
	value, /**< Value of the player. */
	wage, /**< Wage of the player. */
	contract, /**< The number of weeks until the player's contract expires. */
	lsu, /**< Last skill update. Number of weeks since the player skill was last updated. */
	goals, /**< Number of goals (scored for field players or conceded for goalies). */
	games; /**< Number of games the player played. */

    /** array of cards; one item per league and cup.
	@see PlayerCard*/
    GArray *cards;
    /** Player history. To be specified. */
    GArray *history;
} Player;

#endif
