/*
   lg_commentary_struct.h

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

#ifndef LG_COMMENTARY_STRUCT_H
#define LG_COMMENTARY_STRUCT_H

/** Structure describing a commentary
    for the live game.  */
typedef struct
{
    /** The commentary text (possibly containing tokens). */
    gchar *text;
    /** A condition (if not fulfilled, the commentary doesn't get
	shown). */
    gchar *condition;
    /** Priority of the commentary (compared to
	the other ones for the same event).
	The higher the priority the higher the
	probability that the commentary gets picked. */
    gint priority;
    /** An id to keep track of already used commentaries in the
	live game (so as not to use the same one too frequently). */
    gint id;
} LGCommentary;

#endif

