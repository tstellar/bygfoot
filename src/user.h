/*
   user.h

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

#ifndef USER_H
#define USER_H

#include "bygfoot.h"
#include "user_struct.h"
#include "variables.h"

/** Convenience abbrevs. */
#define usr(i) g_array_index(users, User, i)
#define query_user_games_this_week_round() query_user_games_in_week_round(week, week_round)
#define current_user usr(cur_user)

void
user_set_up_team_new_game(User *user);

void
user_set_up_team(User *user);

void
user_set_up_finances(User *user);

User
user_new(void);

void
user_remove(gint idx, gboolean regenerate_team);

void
user_set_player_list_attributes(const User *user, PlayerListAttribute *attribute, gint list_number);

gboolean
query_user_game_in_week_round(gint usr_idx, gint week_number, gint week_round_number);

gboolean
query_user_games_in_week_round(gint week_number, gint week_round_number);

User*
user_from_team(const Team *tm);

void
user_weekly_update_counters(User *user);

Event
user_event_new(void);

void
user_event_add(User *user, gint type, gint value1, gint value2, 
	       gpointer value_pointer, gchar *format, ...)
__attribute__ ((format (printf, 6, 7)));

void
user_event_remove(User *user, gint idx);

void
user_event_show_next(void);

void
user_change_team(User *user, Team *tm);

void
user_set_up_counters(User *user);

gboolean
query_user_teams_have_unfit(void);

void
user_history_add(User *user, gint type, gint team_id, 
		 gint value1, gint value2, gchar *string);

void
user_history_to_string(const UserHistory *history, gchar *buf);

gint
user_history_compare(gconstpointer a, gconstpointer b);

void
user_job_offer(User *user);

gboolean
query_user_no_turn(void);

void
user_show_sponsors(void);

UserSponsor
user_get_sponsor(const User *user);

void
user_show_sponsor_continue(void);

void
user_mm_set_filename(const gchar *filename, gchar *dest);

void
user_mm_save_file(const gchar *filename, const GArray *mmatches);

void
user_mm_add_last_match(gboolean load_file, gboolean save_file);

void
user_mm_import_file(const gchar *filename);

void
user_mm_load_file(const gchar *filename, GArray *mmatches);

void
user_mm_export_file(const gchar *filename);

#endif
