/*
   callback_func.h

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

#ifndef CALLBACK_FUNC_H
#define CALLBACK_FUNC_H

#include "bygfoot.h"
#include "live_game_struct.h"

void
callback_show_next_live_game(void);

void
callback_player_clicked(gint idx, GdkEventButton *event);

void
callback_player_activate(gint idx);

void
callback_show_last_match(gboolean start, LiveGame *lg);

void
callback_show_fixtures(gint type);

void
callback_show_tables(gint type);

void
callback_pay_loan(void);

void
callback_get_loan(void);

void
callback_transfer_list_clicked(gint button, gint idx);

void
callback_offer_new_contract(gint idx);

void
callback_show_team(gint type);

void
callback_show_player_list(gint type);

void
callback_fire_player(gint idx);

void
callback_show_last_match_stats(void);

void
callback_show_fixtures_week(gint type);

void
callback_show_league_stats(gint type);

void
callback_show_season_history(gint type);

void
callback_show_next_opponent(void);

void
callback_show_player_team(void);

void
callback_show_youth_academy(void);

#endif
