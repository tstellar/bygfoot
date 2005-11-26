/*
   fixture_struct.h

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

#ifndef FIXTURE_STRUCT_H
#define FIXTURE_STRUCT_H

#include "bygfoot.h"
#include "team_struct.h"

/** Structure representing a fixture, or, in other words,
    a match. */
typedef struct
{
    /** The cup or league the fixture belongs to. */
    gint clid;
    /** The unique id of the fixture. */
    gint id;
    /** The round (in a cup) the fixture belongs to. */
    gint round;    
    /** The replay number (ie. how often the match was repeated because of a draw). */
    gint replay_number;
    /** When it takes place. */
    gint week_number, week_round_number;
    /** The teams involved. */
    Team *teams[2];
    /** Ids of the teams. Needed when the team
	pointers get invalid (e.g. after promotion/relegation). */
    gint team_ids[2];
    /** The number of goals for each team in
	regulation, extra time and penalty shoot-out. */
    gint result[2][3];
    /** Whether there's home advantage, this is second leg,
	or the game has to be decided. */
    gboolean home_advantage, second_leg, decisive;
    /** How many people attended and whether there were
	special events. */
    gint attendance;
} Fixture;

#endif
