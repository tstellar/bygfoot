#ifndef USER_STRUCT_H
#define USER_STRUCT_H

#include "bygfoot.h"
#include "enums.h"
#include "team_struct.h"
#include "live_game_struct.h"

/** Indices for the money_in array. */
enum MonIn
{
    MON_IN_PRIZE = 0,
    MON_IN_TICKET,
    MON_IN_TRANSFERS,
    MON_IN_END
};

/** Indices for the money_out array. */
enum MonOut
{
    MON_OUT_WAGE = 0,
    MON_OUT_PHYSIO,
    MON_OUT_SCOUT,
    MON_OUT_JOURNEY,
    MON_OUT_STADIUM_IMPROVEMENT,
    MON_OUT_STADIUM_BILLS,
    MON_OUT_TRANSFERS,
    MON_OUT_END
};

/** Indices for the counters variable in #User. */
enum CounterValue
{
    COUNT_USER_LOAN = 0, /** How many weeks until user has to pay back his loan. */
    COUNT_USER_OVERDRAWN, /**< How often the user overdrew his bank account. */
    COUNT_USER_POSITIVE, /**< How many weeks until the bank account has to be positive
		       or at least not overdrawn). */
    COUNT_USER_SUCCESS, /**< How successful the user is. */
    COUNT_USER_WARNING, /**< Whether there was already a warning about rumours (new coach). */
    COUNT_USER_INC_CAP, /**< How many weeks until the stadium capacity is increased. */
    COUNT_USER_INC_SAF, /**< How often the stadium safety was increased (in a week). */
    COUNT_USER_AUTOSAVE, /**< How often the game is autosaved. */
    COUNT_USER_STADIUM_CAPACITY, /**< Counter for building stadium seats. */
    COUNT_USER_STADIUM_SAFETY, /**< Counter for increasing stadium safety. */
    COUNT_USER_OLD_WAGE1,
    COUNT_USER_OLD_WAGE2,
    COUNT_USER_OLD_WAGE3,
    COUNT_USER_OLD_WAGE4,
    COUNT_USER_OLD_WAGE5,
    COUNT_USER_END
};

/** A structure representing a human player. */
typedef struct
{
    /** Username. */
    GString *name;
    /** The team the user manages. */
    Team *tm;
    /** User options. */
    GArray *options;
    /** User counters (not changeable by the user),
	like number of weeks until debt has to be paid back. */
    gint counters[COUNT_USER_END];
    /** The user's money, debt, income and expenses.
	We have double arrays to store information about
	the current and the past week. */
    gint money, debt, money_in[2][MON_IN_END],
	money_out[2][MON_OUT_END];
    /** The attributes shown in the player lists.
	@see #PlayerListAttribute
	@see #PlayerListAttributeValue */
    PlayerListAttribute player_list_attributes[3];
    /** The user's scout and physio qualities.
	@see #Quality */
    gint scout, physio;
    /** The font used in treeviews. */
    GString *font_name;
    /** The variable for the latest user live game. @see #Game */
    LiveGame live_game;
} User;

#endif
