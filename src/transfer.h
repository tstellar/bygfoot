#ifndef TRANSFER_H
#define TRANSFER_H

#include "bygfoot.h"
#include "team.h"

#define CONSTANT_TRANSFER_MAX_PLAYERS 20
#define CONSTANT_TRANSFER_DEADLINE 35

/** Structure representing a player on the transfer list. */
typedef struct
{
    /** Cup or league id, team id, player id.*/
    gint clid, team_nid, id;
    /** Estimated wage and value. */
    gint ewage, evalue;
    /** Time until player gets removed from the list. */
    gint time;
} TransferPlayer;

#endif
