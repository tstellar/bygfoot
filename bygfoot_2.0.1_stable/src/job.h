/*
   job.h

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

#ifndef JOB_H
#define JOB_H

#include "bygfoot.h"
#include "job_struct.h"
#include "league_struct.h"
#include "team_struct.h"
#include "user_struct.h"

void
job_update(void);

void
job_add_new_international(gint num_of_new);

gint
job_country_is_in_list(const gchar *country_file, 
		       const Country *countries, gint len);

void
job_add_new_national(void);

gint
job_team_is_on_list(gint team_id);

Team*
job_get_team(const Job *job);


gboolean
query_job_application_successful(const Job *job, const User *user);

void
job_remove(Job *job, gboolean free_tm);

void
job_pick_team_from_country(const Country *cntry, Team **tm, League **league);

gint
job_team_is_in_cup(const gchar *team_name);

void
job_change_country(Job *job);

#endif
