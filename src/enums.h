#ifndef ENUMS_H
#define ENUMS_H

/** Some counters. */
enum Counters
{
    COUNT_AUTOSAVE = 0,
    COUNT_AUTOSAVE_FILE,
    COUNT_TEAM_ID,
    COUNT_PLAYER_ID,
    COUNT_CUP_ID,
    COUNT_PROM_CUP_ID,
    COUNT_SUPERCUP_ID,
    COUNT_LEAGUE_ID,
    COUNT_END
};

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

/** Possible values for a status variable. */
enum Status0Value
{
    STATUS_NONE = 0,
    STATUS_MAIN,
    STATUS_SHOW_LIVE_GAME,
    STATUS_LIVE_GAME_PAUSE,
    STATUS_SHOW_TEAM_LIST,
    STATUS_SHOW_PLAYER_INFO,
    STATUS_BROWSE_TEAMS,
    STATUS_TEAM_SELECTION,
    STATUS_SHOW_LAST_MATCH,
    STATUS_SHOW_LAST_MATCH_STATS,
    STATUS_SHOW_FIXTURES,
    STATUS_SHOW_TABLES,
    STATUS_SHOW_FINANCES,
    STATUS_SHOW_TRANSFER_LIST,
    STATUS_GET_LOAN,
    STATUS_PAY_LOAN,
    STATUS_SHOW_EVENT,
    STATUS_JOB_OFFER_SUCCESS,
    STATUS_JOB_OFFER_FIRE_FINANCE,
    STATUS_JOB_OFFER_FIRE_FAILURE,    
    STATUS_TRANSFER_OFFER,
    STATUS_CUSTOM_STRUCTURE,
    STATUS_SHOW_LEAGUE_RESULTS,
    STATUS_CONTRACT_OFFER,
    STATUS_SHOW_PLAYER_LIST,
    STATUS_FIRE_PLAYER,
    STATUS_USER_MANAGEMENT,
    STATUS_SHOW_PREVIEW,
    STATUS_SAVE_GAME,
    STATUS_LOAD_GAME,
    STATUS_LOAD_GAME_TEAM_SELECTION,
    STATUS_QUERY_UNFIT,
    STATUS_QUERY_QUIT,
    STATUS_END
};

#endif
