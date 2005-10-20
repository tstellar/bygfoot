/*
   Bygfoot Football Manager -- a small and simple GTK2-based
   football management game.

   http://bygfoot.sourceforge.net

   Copyright (C) 2005  Gyözö Both (gyboth@bygfoot.com)

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

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
    COUNT_LEAGUE_ID,
    COUNT_FIX_ID,
    COUNT_LG_COMM_ID,
    COUNT_SHOW_DEBUG,
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
    STATUS_LIVE_GAME_CHANGE,
    STATUS_SHOW_TEAM_LIST,
    STATUS_SHOW_PLAYER_INFO,
    STATUS_BROWSE_TEAMS,
    STATUS_TEAM_SELECTION,
    STATUS_SHOW_LAST_MATCH,
    STATUS_SHOW_LAST_MATCH_PAUSE,
    STATUS_SHOW_LAST_MATCH_ABORT,
    STATUS_SHOW_LAST_MATCH_STATS,
    STATUS_SHOW_FIXTURES,
    STATUS_SHOW_FIXTURES_WEEK,
    STATUS_SHOW_TABLES,
    STATUS_SHOW_FINANCES,
    STATUS_SHOW_TRANSFER_LIST,
    STATUS_SHOW_USER_HISTORY,
    STATUS_GET_LOAN,
    STATUS_PAY_LOAN,
    STATUS_SHOW_EVENT,
    STATUS_JOB_OFFER_SUCCESS,
    STATUS_JOB_OFFER_FIRE_FINANCE,
    STATUS_JOB_OFFER_FIRE_FAILURE,    
    STATUS_TRANSFER_OFFER_USER,
    STATUS_TRANSFER_OFFER_CPU,
    STATUS_CUSTOM_STRUCTURE,
    STATUS_SHOW_LEAGUE_RESULTS,
    STATUS_SHOW_SEASON_RESULTS,
    STATUS_SHOW_LEAGUE_STATS,
    STATUS_SHOW_SEASON_HISTORY,
    STATUS_SHOW_PLAYER_LIST,
    STATUS_FIRE_PLAYER,
    STATUS_USER_MANAGEMENT,
    STATUS_SHOW_PREVIEW,
    STATUS_SAVE_GAME,
    STATUS_LOAD_GAME,
    STATUS_LOAD_GAME_TEAM_SELECTION,
    STATUS_QUERY_UNFIT,
    STATUS_QUERY_QUIT,
    STATUS_QUERY_USER_NO_TURN,
    STATUS_GENERATE_TEAMS,
    STATUS_SPONSOR_CONTINUE,
    STATUS_SHOW_YA,
    STATUS_SET_YA_PERCENTAGE,
    STATUS_QUERY_KICK_YOUTH,
    STATUS_SELECT_MM_FILE_LOAD,
    STATUS_SELECT_MM_FILE_ADD,
    STATUS_SELECT_MM_FILE_IMPORT,
    STATUS_SELECT_MM_FILE_EXPORT,
    STATUS_END
};

#endif
