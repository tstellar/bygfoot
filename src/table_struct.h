/*
   table_struct.h

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

#ifndef TABLE_STRUCT_H
#define TABLE_STRUCT_H

#include "bygfoot.h"
#include "team_struct.h"

/**
   Table element values.
   @see TableElement
   @see Table
*/
enum TableElementValues
{
    TABLE_PLAYED = 0,
    TABLE_WON,
    TABLE_DRAW,
    TABLE_LOST,
    TABLE_GF,
    TABLE_GA,
    TABLE_GD,
    TABLE_PTS,
    TABLE_END
};

/**
   An element representing a team in the tables.
   @see Table
   @see #TableElementValues
*/
typedef struct
{
    Team *team;
    gint team_id;
    /** The rank of the element before the last update. 
	Used to display an arrow if the rank changed. */
    gint old_rank;
    gint values[TABLE_END];
} TableElement;

/**
   A table belonging to a league or a cup with round robin.
   @see TableElement
*/
typedef struct
{
    gchar *name;
    gint clid;
    /** The cup round (or -1 if it's a league). */
    gint round;
    GArray *elements;
} Table;

#endif
