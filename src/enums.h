#ifndef ENUMS_H
#define ENUMS_H

/** Scout and physio qualities. */
enum Quality
{
    QUALITY_BEST = 0,
    QUALITY_GOOD,
    QUALITY_AVERAGE,
    QUALITY_BAD,
    QUALITY_END
};

/** Indices for the #finances variable. */
enum FinanceValue
{
    FIN_PRIZE = 0, /**< Prize money at the end of the season. */
    FIN_DEBTS, /**< User's debts. */
    FIN_MONEY, /**< User's money. */
    FIN_TICKETS, /**< Ticket income (weekly).  */
    FIN_JOURNEY, /**< Journey costs (weekly). */
    FIN_WAGES, /**< Wages (weekly). */
    FIN_SCOUT, /**< Scout wage (weekly). */
    FIN_PHYSIO, /**< Physio wage (weekly). */
    FIN_TRANSFERS_IN, /**< Transfer fees income (weekly). */
    FIN_TRANSFERS_OUT, /**< Transfer fees spent (weekly). */
    FIN_STADIUM, /**< Stadium improvement (weekly). */
    FIN_STAD_BILLS, /**< Bills for riots or fire or so (weekly). */
    FIN_END
};

/** Indices for the counters variable in #User. */
enum CounterValue
{
    COUNT_LOAN = 0, /** How many weeks until user has to pay back his loan. */
    COUNT_OVERDRAWN, /**< How often the user overdrew his bank account. */
    COUNT_POSITIVE, /**< How many weeks until the bank account has to be positive
		       or at least not overdrawn). */
    COUNT_SUCCESS, /**< How successful the user is. */
    COUNT_WARNING, /**< Whether there was already a warning about rumours (new coach). */
    COUNT_INC_CAP, /**< How many weeks until the stadium capacity is increased. */
    COUNT_INC_SAF, /**< How often the stadium safety was increased (in a week). */
    COUNT_AUTOSAVE, /**< How often the game is autosaved. */
    COUNT_OLD_WAGE1,
    COUNT_OLD_WAGE2,
    COUNT_OLD_WAGE3,
    COUNT_OLD_WAGE4,
    COUNT_OLD_WAGE5,
    COUNT_END
};

/** Possible values for a status variable. */
enum Status0Value
{
    STATUS_NONE = 0,
    STATUS_MAIN,
    STATUS_SHOW_LIVE_GAME,
    STATUS_LIVE_GAME_PAUSE,
    STATUS_BROWSE_TEAMS,
    STATUS_TEAM_SELECTION,
    STATUS_SHOW_LAST_MATCH,
    STATUS_SHOW_LAST_MATCH_STATS,
    STATUS_SHOW_FIXTURES,
    STATUS_SHOW_TABLES,
    STATUS_END
};

/** Enum used to determine what kind of fixtures or
    tables to show (e.g. when an arrow is clicked on). */
enum ShowType
{
    SHOW_TEAM = 0,
    SHOW_CURRENT,
    SHOW_NEXT,
    SHOW_PREVIOUS,
    SHOW_NEXT_LEAGUE,
    SHOW_PREVIOUS_LEAGUE,
    SHOW_END
};

#endif
