#include "bygfoot.h"
#include "enums.h"
#include "live_game_struct.h"
#include "player_struct.h"
#include "team_struct.h"

/**
 * The main variable of the game.
 * @see Country
 */
Country country;

/** The id of the league or cup of the user's team and its numerical id. */
Team *my_team;

/** The season, week and week round numbers. 
    We keep track of the time in the game with these variables. */
gint season, week, week_round;

/** The user's scout and physio qualities.
    @see #Quality */
gint scout, physio;

/** The user's finances. @see #FinanceValue */
gint finances[FIN_END];
/** The user's settings. @see #OptionValue */
gint options[OPT_END];
/** The attributes shown in the player lists.
    @see #PlayerListAttribute
    @see #PlayerListAttributeValue */
PlayerListAttribute player_list_attributes[3];

/** Some counters. @see #CounterValue */
gint counters[COUNT_END];

/** The array containing players to be transfered.
    @see TransferPlayer */
GArray *transfer_list;

/** The font used in treeviews. */
GString *font_name;

/** Whether we are in debug mode. */
gboolean debug;

/** These help us keep track of what's happening. */
gint status, status2, status3, status4, status5;

/** An array of player names that we keep in memory. */
GPtrArray *player_names;

/** The pointer to the main window of the game. */
GtkWidget *main_window;

/** With this we keep track of the number of popup
    windows and make the main window sensitive or
    insensitive, depending. */
gint popups_active;

/** The variable for the latest user live game. @see #Game */
LiveGame live_game;

/** The variable for non-user games (which aren't shown). */
LiveGame live_game_temp;
