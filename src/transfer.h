/*
   transfer.h

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

#ifndef TRANSFER_H
#define TRANSFER_H

#include "bygfoot.h"
#include "player_struct.h"
#include "team_struct.h"
#include "transfer_struct.h"
#include "variables.h"

/** Convenience abbrev. */
#define trans(i) g_array_index(transfer_list, Transfer, i)
#define transoff(i, j) g_array_index(g_array_index(transfer_list, Transfer, i).offers, TransferOffer, j)

void
transfer_update(void);

void
transfer_add_cpu_offers(void);

void
transfer_evaluate_offers(void);

void
transfer_add_new_players(void);

Player*
transfer_player_get_new(gboolean cup);

void
transfer_add_player(Player *pl, gint time);

gint
transfer_get_deadline(void);

void
transfer_remove_player(gint idx);

gboolean
query_transfer_player_is_on_list(const Player *pl);

gboolean
transfer_add_offer(gint idx, Team *tm, gint fee, gint wage);

gboolean
transfer_remove_offer(gint idx, const Team *tm);

gint
transfer_offer_compare_func(gconstpointer a, gconstpointer b);

void
transfer_remove_player_ptr(const Player *pl);

Team*
transfer_team_get_new(void);

void
transfer_add_remove_user_player(Player *pl);

void
transfer_offers_notify(Transfer *trans, gboolean sort);

const TransferOffer*
transfer_player_has_offer(const Player *pl);

gboolean
query_transfer_current_team_offer(const Player *pl);

gboolean
transfer_offers_pending(void);

void
transfer_get_previous_offer(const Transfer *tr, const Team *tm, gint *fee, gint *wage);

gboolean
transfer_new_star_balks(const Transfer *tr, const TransferOffer *troff);

#endif
