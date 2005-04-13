#ifndef BYGFOOT_H
#define BYGFOOT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "gettext_macros.h"

/**
 * Program version number.
 */
#define VERS "1.9.0"

/** Home dir name */
#define HOMEDIRNAME ".bygfoot-1.9"

/**
 * Convenience macros, used for string sizes (typically buf[SMALL]).
 */
#define SMALL 10000
#define BIG 1000000

/** Starting numbers of league, cup and supercup numerical ids. */
#define ID_LEAGUE_START 1000
#define ID_CUP_START 2000
#define ID_PROM_CUP_START 3000

/** Convenience abbreviation. */
#define ligs country.leagues
/** Convenience abbreviation. */
#define lig(i) g_array_index(country.leagues, League, i)

/** Convenience abbreviation. */
#define cps country.cups
/** Convenience abbreviation. */
#define cp(i) g_array_index(country.cups, Cup, i)

/** Convenience abbreviation. */
#define player_name(i) ((GString*)g_ptr_array_index(player_names, i))->str;

/** Convenience abbrevs. */
#define stat0 status[0]
#define stat1 status[1]
#define stat2 status[2]
#define stat3 status[3]
#define stat4 status[4]
#define old_stat status[5]

#define player_id_new (counters[COUNT_PLAYER_ID]++)
#define team_id_new (counters[COUNT_TEAM_ID]++)

#define debug opt_int("int_opt_debug")

/**
 * Exit codes.
 */
enum ExitCodes
{
    EXIT_OK = 0, /**< Normal exit. */
    EXIT_NO_COUNTRY_FILES, /**< Exit when no country files have been found. */
    EXIT_FILE_OPEN_FAILED, /** A file could not be opened. */
    EXIT_PRINT_ERROR, /**< Exit when the print_error function is called on a set error.*/
    EXIT_NO_LEAGUES, /**< The game must contain at least one league. */
    EXIT_CHOOSE_TEAM_ERROR, /**< There was a problem loading the choose_teams. @see cup_load_choose_teams() */
    EXIT_FIXTURE_WRITE_ERROR, /** There was an error writing the fixtures. */
    EXIT_USER_FIRED,
    EXIT_CUP_LAST_ROUND,
    EXIT_END
};

/**
 * A struct representing a country.
 */
typedef struct
{
    GString *name, /**< Name of the country. */
	*symbol, /**< Symbol of the country, eg a flag pixmap. */
	*sid; /**< Id of the country, eg 'england'. */

    /** Leagues and cups arrays. */
    GArray *leagues, *cups;
} Country;

/** Struct used for having all the windows
    in one place. */
typedef struct
{
    GtkWidget *main,
	*startup,
	*live,
	*warning,
	*progress,
	*digits,
	*stadium,
	*job_offer,
	*yesno,
	*options,
	*font_sel,
	*file_sel,
	*contract,
	*menu_player,
	*user_management;
} Windows;
#endif
