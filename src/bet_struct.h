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

#ifndef BET_STRUCT_H
#define BET_STRUCT_H

#include "bygfoot.h"
#include "fixture_struct.h"

/** A struct representing a betting possibility: a fixture plus odds. */
typedef struct
{
    /** The match the bet is about. */
    const Fixture *fix;
    /** The odds for a win, draw or loss of the first team. */
    gfloat odds[3];
    
} BetMatch;

/** A struct representing a bet by a user. */
typedef struct
{
    /** Match the user betted on. */
    const Fixture *fix;
    /** Which outcome he picked. */
    gint outcome;
    /** How much he wagered. */
    gint wager;
} BetUser;

#endif
