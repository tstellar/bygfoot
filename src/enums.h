#ifndef ENUMS_H
#define ENUMS_H

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

/** Indices for the #counters variable. */
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

/** Indices for the #options variable. */
enum OptionValue
{
    OPT_BOOL_CONF_NEW_ROUND = 0, /**< Whether beginning of a new week round has to be confirmed. */
    OPT_BOOL_CONF_UNFIT, /**< Unfit player confirmation.  */
    OPT_BOOL_CONF_QUIT, /**< Quit confirmation.  */
    OPT_BOOL_OVERWRITE, /**< Clicking on save overwrites current save game. */
    OPT_BOOL_SHOW_LIVE, /**< Whether live game is shown. */
    OPT_LIVE_SPEED, /**< Live game speed. */
    OPT_BOOL_LIVE_TENDENCY, /**< Tendency bar. */
    OPT_BOOL_MAXIMIZE, /**< Whether to start maximized. */
    OPT_BOOL_NOTIFY, /**< Whether transfer list notification is on. */
    OPT_NOTIFY_POS, /**< Whether we look for a certain position. */
    OPT_NOTIFY_LEAGUE_UPPER, /**< League with lowest index for notification. */
    OPT_NOTIFY_LEAGUE_LOWER, /**< League with highest index for notification. */
    OPT_BOOL_NOTIFY_CUPS, /**< Notify when player from international cups. */
    OPT_NOTIFY_AGE_UPPER, /**< Upper age bound. */
    OPT_NOTIFY_AGE_LOWER, /**< Lower age bound. */
    OPT_NOTIFY_SKILL_UPPER, /**< Skill bound. */
    OPT_NOTIFY_SKILL_LOWER, /**< Skill bound. */
    OPT_NOTIFY_ETAL_UPPER, /**< Etal bound. */
    OPT_NOTIFY_ETAL_LOWER, /**< Etal bound. */
    OPT_NOTIFY_VALUE, /**< Upper value bound. */
    OPT_BOOL_JOBS, /**< Whether job offers are shown. */
    OPT_MESS, /**< Message style. */
    OPT_BOOL_PREFER_MESS, /**< Messages instead of popups when possible. */
    OPT_BOOL_AUTOSAVE, /**< Whether autosave is on. */
    OPT_AUTOSAVE_INTERVAL, /**< How often to autosave. */
    OPT_BOOL_SORT_TRANSFERS, /**< Whether to sort transfers. */    
    OPT_SORT_TRANSFERS_ATTRIBUTE, /**< Which attribute to sort. */
    OPT_BOOL_SORT_TRANSFERS_DESCENDING, /**< Whether we sort descending. */
    OPT_BOOL_REARRANGE, /**< Whether the players get sorted when a new structure is given. */
    OPT_BOOL_SWAP, /**< Whether the players get sorted when two players are swapped. */
    OPT_HISTORY_TEAM_INTERVAL, /**< Interval of team history update. */
    OPT_HISTORY_PLAYER_INTERVAL, /**< Interval of player history update. */
    OPT_HISTORY_TEAM_MAX, /**< Maximum length of team history. */
    OPT_HISTORY_PLAYER_MAX, /**< Maximum length of player history. */
    OPT_BOOL_HISTORY_TEAM_DELETE, /**< Whether team histories get deleted each new season. */
    OPT_BOOL_HISTORY_PLAYER_DELETE, /**< Whether player histories get deleted each new season. */
    OPT_BOOL_BOOST, /**< Whether player boost is on. */
    OPT_BOOL_COMPRESS, /**< Whether to zip-compress savegames. */
    OPT_BOOL_OBJECTIVE, /**< Whether season objective is on. */
    OPT_PENALTY_SHOOTER, /**< The id of the penalty shooter. */
    OPT_END
};

#endif
