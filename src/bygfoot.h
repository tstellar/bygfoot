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

/**
 * Convenience macros, used for string sizes (typically buf[SMALL]).
 */
#define SMALL 10000
#define BIG 1000000

/**
 * Exit codes.
 */
enum ExitCodes
{
    EXIT_OK = 0, /**< Normal exit. */
    EXIT_DIR_OPEN_FAILED, /**< Exit when the $HOME/.bygfoot/definitions directory can't be opened. */
    EXIT_FILE_OPEN_FAILED,
    EXIT_PRINT_ERROR, /**< Exit when the print_error function is called on a set error.*/
    EXIT_NO_LEAGUES,
    EXIT_CHOOSE_TEAM_ERROR,
    EXIT_END
};

/** Scout and physio qualities. */
enum Quality
{
    QUALITY_BEST = 1,
    QUALITY_GOOD,
    QUALITY_AVERAGE,
    QUALITY_BAD,
    QUALITY_END
};

/** Starting numbers of league, cup and supercup numerical ids. */
#define ID_LEAGUE_START 1000
#define ID_CUP_START 2000

/** The player names file. */
#define PLAYER_NAMES_FILE "player_names.xml"

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

#endif
