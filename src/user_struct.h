#ifndef USER_STRUCT_H
#define USER_STRUCT_H

#include "bygfoot.h"
#include "enums.h"
#include "team_struct.h"
#include "live_game_struct.h"

/** A structure representing a human player. */
typedef struct
{
    /** Username. */
    GString *name;
    /** The team the user manages. */
    Team *tm;
    /** User options. */
    GArray *options;
    /** The user's finances. @see #FinanceValue */
    gint finances[FIN_END];
    /** The attributes shown in the player lists.
	@see #PlayerListAttribute
	@see #PlayerListAttributeValue */
    PlayerListAttribute player_list_attributes[3];
    /** The user's scout and physio qualities.
	@see #Quality */
    gint scout, physio;
    /** The font used in treeviews. */
    GString *font_name;
    /** The variable for the latest user live game. @see #Game */
    LiveGame live_game;
} User;

#endif
