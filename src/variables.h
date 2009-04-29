/*
   variables.h

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

#include "bygfoot.h"
#include "enums.h"
#include "live_game_struct.h"
#include "news_struct.h"
#include "option_struct.h"

/**
 * The main variable of the game.
 * @see Country
 */
Country country;

/** The array of human players. @see #User */
GArray *users;

/** The season, week and week round numbers. 
    We keep track of the time in the game with these variables. */
gint season, week, week_round;

/** Array of options that get read from
    bygfoot.conf. */
OptionList options;
/** Array of constants that get read from the constants
    file specified in bygfoot.conf. */
OptionList constants;
/** Array of constants affecting game appearance rather than
    behaviour. */
OptionList constants_app;
/** Array with internal settings. */
OptionList settings;
/* Array holding string replacement tokens. */
OptionList tokens;

/** The array containing the live game commentary strings. */
GArray *lg_commentary[LIVE_GAME_EVENT_END];

/** The array containing the news article strings. */
GArray *news[NEWS_ARTICLE_TYPE_END];

/** Newspaper containing the news articles. */
NewsPaper newspaper;

/** The array containing players to be transfered.
    @see TransferPlayer */
GArray *transfer_list;

/** Array with season statistics (updated at the
    end of each season. */
GArray *season_stats;

/** Array of available CPU strategies. */
GArray *strategies;

/** Array of current and recent bets. */
GArray *bets[2];

/** Loan interest for the current week. */
gfloat current_interest;

/** Array of jobs in the job exchange and
    teams going with the international jobs. */
GArray *jobs, *job_teams;

/** Some counters we use. */
gint counters[COUNT_END];

/** These help us keep track of what's happening. */
gint status[6];

/** A pointer we store temporary stuff in. */
gpointer statp;

/** The currently selected row in the treeview. */
gint selected_row;

/** An array of name lists. */
GArray *name_lists;

/** The struct containing the window pointers. */
Windows window;

/** The variables for non-user live games (which aren't shown). */
GArray *live_games;

/** The index of the current user in the #users array. */
gint cur_user;

gint timeout_id;

GRand *rand_generator;

/** Debug information. */
gint debug_level, debug_output;

/**
   The list of directories the file_find_support_file() function
   searches for support files (e.g. pixmaps or text files).
   @see file_find_support_file()
   @see file_add_support_directory_recursive()
*/
GList *support_directories;

/**
 * The list of root defintions directories found (ending in definitions)
 */ 
GList *root_definitions_directories;

/**
 * The list of defintions directories found
 */ 
GList *definitions_directories;

/** The name of the current save file (gets updated when a game is
    saved or loaded).  */
gchar *save_file;

/** Whether we are using a Unix system or Windows. */
gboolean os_is_unix;

/** The hints displayed in the splash screen. */
OptionList hints;
