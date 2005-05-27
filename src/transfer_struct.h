#ifndef TRANSFER_STRUCT_H
#define TRANSFER_STRUCT_H

#include "bygfoot.h"
#include "player_struct.h"
#include "team_struct.h"

/** Structure representing a player on the transfer list. */
typedef struct
{
    /** Team of the player. */
    Team *tm;
    /** Id of player in the team. */
    gint id;
    /** Time until player gets removed from the list. */
    gint time;
    /** Estimated fees and wages for different scout qualities. */
    gint fee[QUALITY_END], wage[QUALITY_END];
    /** Offers for the player. */
    GArray *offers;
    /** This is true if an accepted offer is pending. */
    gboolean locked;
} Transfer;

typedef struct
{
    /** The team that makes the offer. */
    Team *tm;
    /** Transfer fee and wage offer. */
    gint fee, wage;

} TransferOffer;

#endif
