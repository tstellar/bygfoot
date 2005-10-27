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

#ifndef BET_H
#define BET_H

#include "bet_struct.h"
#include "bygfoot.h"
#include "fixture_struct.h"

BetMatch*
bet_from_fixture(const Fixture *fix);

void
bet_update_user_bets(void);

void
bet_get_odds(BetMatch *bet);

void
bet_update(void);

gfloat
bet_round_odd(gfloat odd);

BetUser*
bet_is_user(const BetMatch *bet);

void
bet_place(const Fixture *fix, gint outcome, gint wager);

void
bet_remove(const Fixture *fix);

#endif
