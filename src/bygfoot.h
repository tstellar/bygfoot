#ifndef BYGFOOT_H
#define BYGFOOT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "gettext.h"

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
    EXIT_DIR_OPEN, /**< Exit when the $HOME/.bygfoot/definitions directory can't be opened. */
    EXIT_PRINT_ERROR, /**< Exit when the print_error function is called on a set error.*/
    EXIT_END
};

/**
 * A struct representing a country.
 */
typedef struct
{
    GString *name, /**< Name of the country. */
	*symbol, /**< Symbol of the country, e.g. a flag pixmap. */
	*id; /**< Id of the country, e.g. 'england'. */

    /** Leagues, cups and supercups arrays.  */
    GArray *leagues,
	*cups,
	*supercups;
} Country;

/** Convenience abbreviation. */
#define ligs country.leagues
/** Convenience abbreviation. */
#define lig(i) g_array_index(country.leagues, League, i)

/** Convenience abbreviation. */
#define cps country.cups
/** Convenience abbreviation. */
#define cp(i) g_array_index(country.cups, Cup, i)

gint my_team;
/** The font used in treeviews. */
GString *font_name;

/** The pointer to the main window of the game. */
GtkWidget *main_window;

#endif
