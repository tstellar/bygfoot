#include "bygfoot.h"

/**
 * The main variable of the game.
 * @see Country
 */
Country country;

/** The id of the league or cup of the user's team and its numerical id. */
gint my_team_clid, my_team_id;

/** The season, week and week round numbers. 
    We keep track of the time in the game with these variables. */
gint season, week, week_round;

/** The user's scout and physio qualities.
    @see #Quality */
gint scout, physio;

/** The array containing the fixtures.
    @see Fixture */
GArray *fixtures;

/** The array containing players to be transfered.
    @see TransferPlayer */
GArray *transfer_list;

/** The font used in treeviews. */
GString *font_name;

/** An array of player names that we keep in memory. */
GPtrArray *player_names;

/** The pointer to the main window of the game. */
GtkWidget *main_window;

/** All the open windows in the game. 
    @see window_create()
    @see window_destroy() */
GList *windows;
