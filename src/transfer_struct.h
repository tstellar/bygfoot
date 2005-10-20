/*
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
} Transfer;

typedef struct
{
    /** The team that makes the offer. */
    Team *tm;
    /** Transfer fee and wage offer. */
    gint fee, wage;
    /** Whether the offer got accepted or rejected etc. */
    gint status;
} TransferOffer;

enum TransferOfferStatus
{
    TRANSFER_OFFER_NOT_CONSIDERED = 0,
    TRANSFER_OFFER_ACCEPTED,
    TRANSFER_OFFER_REJECTED,
    TRANSFER_OFFER_REJECTED2,
    TRANSFER_OFFER_END
};

#endif
