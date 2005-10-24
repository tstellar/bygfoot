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

#ifndef FREE_H
#define FREE_H

#include "bygfoot.h"
#include "cup_struct.h"
#include "league_struct.h"
#include "live_game_struct.h"
#include "name_struct.h"
#include "player_struct.h"
#include "team_struct.h"
#include "user_struct.h"

#define free_gchar_ptr(string) {g_free(string); string = NULL;}

void
free_g_array(GArray **array);

void
free_g_ptr_array(GPtrArray **array);

void
free_g_string(GString **string);

void
free_memory(void);

void
free_users(gboolean reset);

void
free_user(User *user);

void
free_country(gboolean reset);

void
free_leagues_array(GArray **leagues, gboolean reset);

void
free_league(League *league);

void
free_teams_array(GArray **teams, gboolean reset);

void
free_team(Team *team);

void
free_cups_array(GArray **cups, gboolean reset);

void
free_cup(Cup *cup);

void
free_cup_choose_team(CupChooseTeam *cup_choose_team);

void
free_variables(void);

void
free_gchar_array(GPtrArray **array);

void
free_player_array(GArray **players);

void
free_player(Player *pl);

void
free_live_game(LiveGame *match);

void
free_lg_commentary(gboolean reset);

void
free_option_list(OptionList *optionlist, gboolean reset);

void
free_event(Event *event);

void
free_support_dirs(void);

void
free_table(Table *table);

void
free_league_stats(LeagueStat *stats);

void
free_season_stats(gboolean reset);

void
free_transfer_list(void);

void
free_name_list(NameList *namelist, gboolean reset);

void
free_names(gboolean reset);

void
free_mmatches(GArray **mmatches, gboolean reset);

void
free_strategies(void);

#endif
