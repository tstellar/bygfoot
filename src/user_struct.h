#ifndef USER_STRUCT_H
#define USER_STRUCT_H

#include "bygfoot.h"
#include "enums.h"
#include "live_game_struct.h"
#include "option_struct.h"
#include "player_struct.h"
#include "team_struct.h"

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
    MON_OUT_COMPENSATIONS,
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
    COUNT_USER_STADIUM_CAPACITY, /**< Counter for building stadium seats. */
    COUNT_USER_STADIUM_SAFETY, /**< Counter for increasing stadium safety. */
    COUNT_USER_SHOW_RES, /**< Whether the latest result is shown when the main window gets refreshed. */
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
    OptionList options;
    /** Events shown each week. */
    GArray *events;
    /** User counters (not changeable by the user),
	like number of weeks until debt has to be paid back. */
    gint counters[COUNT_USER_END];
    /** The user's money, debt, income and expenses.
	We have double arrays to store information about
	the current and the past week. */
    gint money, debt, money_in[2][MON_IN_END],
	money_out[2][MON_OUT_END];
    /** The user's scout and physio qualities.
	@see #Quality */
    gint scout, physio;
    /** The variable for the latest user live game. @see #Game */
    LiveGame live_game;
} User;

enum EventType
{
    EVENT_TYPE_WARNING = 0,
    EVENT_TYPE_PLAYER_LEFT,
    EVENT_TYPE_PAYBACK,
    EVENT_TYPE_OVERDRAW,
    EVENT_TYPE_JOB_OFFER,
    EVENT_TYPE_FIRE_FINANCE,
    EVENT_TYPE_FIRE_UNSUCCESSFUL,
    EVENT_TYPE_TRANSFER_OFFER,
    EVENT_TYPE_TRANSFER_OFFER_REJECTED,
    EVENT_TYPE_TRANSFER_OFFER_MONEY,
    EVENT_TYPE_TRANSFER_OFFER_ROSTER,
    EVENT_TYPE_TRANSFER_OFFER_ACCEPTED,
    EVENT_TYPE_TRANSFER_OFFER_OUTBID,
    EVENT_TYPE_PLAYER_CAREER_STOP,
    EVENT_TYPE_END
};

/** A structure representing an event for a user. This is used
    to show information like a successful transfer or a job offer. */
typedef struct
{
    /** Pointer to the user the event belongs to. */
    User *user;
    /** Type of the event. See #EventType. */
    gint type;
    /** Some values that are used for different purposes. */
    gint value1, value2;
    /** A pointer for different purposes. */
    gpointer value_pointer;
    /** A string for different purposes. */
    GString *value_string;

} Event;

#endif
