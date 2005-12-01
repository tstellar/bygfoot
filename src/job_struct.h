/*
   job_struct.h

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

#ifndef JOB_STRUCT_H
#define JOB_STRUCT_H

/** Enumeration describing the type of a job. */
enum
{
    /** Job offer by a team from the country
	the user's playing. */
    JOB_TYPE_NATIONAL = 0,
    /** Job offer by a team from a different country
	than user's playing. */
    JOB_TYPE_INTERNATIONAL,
    /** Job offer by a team from a different country
	than user's playing; the team participates
	in an international cup. */
    JOB_TYPE_CUP,
    JOB_TYPE_END
};

/** A structure representing a job in the job exchange. */
typedef struct
{
    /** Whether it's an international job or a national one. */
    gint type;
    /** How many weeks remaining the offer is on the list. */
    gint time;
    /** Only relevant for international jobs. **/
    gchar *country_file, *country_name, *league_name;
    /** Only relevant for international jobs. **/
    gint league_layer;
    /** Only relevant for international jobs. **/
    gint country_rating;
    /** Average talent compared to the league average in percent. */
    gint talent_percent;
    /** The id of the team the job describes. */
    gint team_id;
} Job;

#endif
