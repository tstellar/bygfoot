#include "bygfoot.h"
#include "enums.h"
#include "live_game_struct.h"
#include "option_struct.h"
#include "player_struct.h"
#include "team_struct.h"
#include "user_struct.h"

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

/** The array containing players to be transfered.
    @see TransferPlayer */
GArray *transfer_list;

/** Some counters we use. */
gint counters[COUNT_END];

/** These help us keep track of what's happening. */
gint status[6];

/** A pointer we store temporary stuff in. */
gpointer statp;

/** The currently selected rows in the treeviews. */
gint selected_row[2];

/** An array of player names that we keep in memory. */
GPtrArray *player_names;

/** The struct containing the window pointers. */
Windows window;

/** With this we keep track of the number of popup
    windows and make the main window sensitive or
    insensitive, depending. */
gint popups_active;

/** The variable for non-user games (which aren't shown). */
LiveGame live_game_temp;

/** The index of the current user in the #users array. */
gint cur_user;

gint timeout_id;

GRand *rand_generator;
/**
   The list of directories the file_find_support_file() function
   searches for support files (e.g. pixmaps or text files).
   @see file_find_support_file()
   @see file_add_support_directory_recursive()
*/
GList *support_directories;

/** The name of the current save file (gets updated when a game is
    saved or loaded).  */
GString *save_file;
