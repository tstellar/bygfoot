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
    PLAYER_POS_ANY,
    PLAYER_POS_END
};

/** Streaks a player can go on. */
enum PlayerStreak
{
    PLAYER_STREAK_COLD = -1,
    PLAYER_STREAK_NONE,
    PLAYER_STREAK_HOT
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
    PLAYER_INJURY_CONCUSSION,
    PLAYER_INJURY_PULLED_MUSCLE,
    PLAYER_INJURY_HAMSTRING,
    PLAYER_INJURY_GROIN,
    PLAYER_INJURY_FRAC_ANKLE,
    PLAYER_INJURY_RIB,
    PLAYER_INJURY_LEG,
    PLAYER_INJURY_BROK_ANKLE,
    PLAYER_INJURY_ARM,
    PLAYER_INJURY_SHOULDER,
    PLAYER_INJURY_LIGAMENT,
    PLAYER_INJURY_CAREER_STOP,
    PLAYER_INJURY_END
};

/** Enum for different player data. */
enum PlayerValue
{
    PLAYER_VALUE_GAMES = 0,
    PLAYER_VALUE_GOALS,
    PLAYER_VALUE_SHOTS,
    PLAYER_VALUE_CARD_YELLOW,
    PLAYER_VALUE_CARD_RED, 
    PLAYER_VALUE_END
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
	health, /**< Health. An integer signifying an injury or
		   good health. @see #PlayerInjury */
	recovery, /**< Weeks until the player gets healthy. */
	id, /**< Id of the player within the team. */
	value, /**< Value of the player. */
	wage, /**< Wage of the player. */
	offers, /**< Number of times the player received a contract offer. */
	streak; /**< The streak the player is on. */
		
    gfloat skill, /**< Skill. Between 0 and a constant
		     (specified in the constants file). */
	cskill, /**< Current Skill. */
	talent, /**< Talent. The peak ability (which isn't always reached). */
	etal[QUALITY_END], /**< Estimated talent (the user never sees the actual talent).
			      Depends on scout quality. */
	fitness, /**< Fitness. Between 0 and 1. */
	lsu, /**< Last skill update. Number of weeks since the player
		skill was last updated. */
	age, /**< Age in years. */
	peak_age, /**< Age at which the player reaches his peak ability. */
	peak_region, /**< Region around the peak age during which the player's
			ability is at the peak (in years). */
	contract, /**< The years until the player's contract expires. */
	streak_prob, /**< This number determines how probable it is that a player
			goes on a hot/cold streak. Between -1 and 1. */
	streak_count; /**< How many weeks the streak goes (or how
			 long a new streak may not begin if the value
			 is negative). */

    /** Whether the player participated in the team's last match. */
    gboolean participation;

    /** Array of games and goals; one item per league and cup.
	@see PlayerGamesGoals */
    GArray *games_goals;

    /** Array of cards; one item per league and cup.
	@see PlayerCard*/
    GArray *cards;

    /** Career goals, games etc. */
    gint career[PLAYER_VALUE_END];

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
    PLAYER_LIST_ATTRIBUTE_SHOTS,
    PLAYER_LIST_ATTRIBUTE_GOALS,
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

enum PlayerInfoAttributeValue
{
    PLAYER_INFO_ATTRIBUTE_NAME = 0,
    PLAYER_INFO_ATTRIBUTE_POS,
    PLAYER_INFO_ATTRIBUTE_CPOS,
    PLAYER_INFO_ATTRIBUTE_SKILL,
    PLAYER_INFO_ATTRIBUTE_CSKILL,
    PLAYER_INFO_ATTRIBUTE_FITNESS,
    PLAYER_INFO_ATTRIBUTE_ETAL,
    PLAYER_INFO_ATTRIBUTE_AGE,
    PLAYER_INFO_ATTRIBUTE_HEALTH,
    PLAYER_INFO_ATTRIBUTE_VALUE,
    PLAYER_INFO_ATTRIBUTE_WAGE,
    PLAYER_INFO_ATTRIBUTE_CONTRACT,
    PLAYER_INFO_ATTRIBUTE_GAMES_GOALS,
    PLAYER_INFO_ATTRIBUTE_YELLOW_CARDS,
    PLAYER_INFO_ATTRIBUTE_BANNED,
    PLAYER_INFO_ATTRIBUTE_STREAK,
    PLAYER_INFO_ATTRIBUTE_CAREER,
    PLAYER_INFO_ATTRIBUTE_OFFERS,
    PLAYER_INFO_ATTRIBUTE_END
};

/** A struct telling us which player attributes to show in a player list.
    @see #PlayerListAttributeValue*/
typedef struct
{
    gboolean on_off[PLAYER_LIST_ATTRIBUTE_END];
} PlayerListAttribute;

#endif
