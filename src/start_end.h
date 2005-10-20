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

#ifndef START_END_H
#define START_END_H

#include "bygfoot.h"

void
start_new_game(void);

void
start_new_season(void);

void
start_write_variables(void);

void
start_generate_league_teams(void);

void
end_week_round(void);

void
end_week_round_results(void);

void
end_week_round_sort_tables(void);

void
end_week_round_update_fixtures(void);

void
start_week_round(void);

void
start_week(void);

void
start_week_update_user_teams(void);

void
start_week_round_update_user_teams(void);

void
start_week_update_user_finances(void);

void
start_week_update_users(void);

void
end_week_round_autosave(void);

gboolean
query_start_end_season_end(void);

void
start_new_season_league_changes(void);

void
end_week(void);

void
end_season(void);

void
start_week_add_cups(void);

void
end_week_hide_cups(void);

#endif
