/*
   youth_academy_struct.h

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

#ifndef YOUTH_ACADEMY_STRUCT_H
#define YOUTH_ACADEMY_STRUCT_H

#include "bygfoot.h"

/** The data about a user's youth academy is bundled
    here. */
typedef struct
{
    /** Pointer to the team the academy belongs to. */
    Team *tm;
    /** Position preference of the user. */
    gint pos_pref;
    /** Coach quality, like scout or physio. */
    gint coach;
    /** Percentage of income the user devotes to the youth
	academy. */
    gint percentage;
    /** Average coach and percentage values; these are used
	when a new youth comes into the academy to determine
	the quality of the youth. */
    gfloat av_coach, av_percentage;
    /** When this counter is <= 0, a new youth appears. */
    gfloat counter_youth;
    /** The youths are simply young players. */
    GArray *players;
} YouthAcademy;

#endif
