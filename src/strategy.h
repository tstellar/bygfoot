/*
   strategy.h

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

#ifndef STRATEGY_H
#define STRATEGY_H

#include "bygfoot.h"
#include "player_struct.h"
#include "strategy_struct.h"
#include "team_struct.h"
#include "variables.h"

void
strategy_update_team_pre_match(Team *tm);

gint
strategy_compare_players(gconstpointer a,
			 gconstpointer b,
			 gpointer user_data);
gchar*
strategy_get_random(void);

Strategy*
strategy_from_sid(const gchar *sid);

gboolean
query_strategy_formation_possible(const GPtrArray *players, 
				  const StrategyPrematch *prematch,
				  gint formation);

void
strategy_update_lineup(Team *tm, const GPtrArray *players,
		       const StrategyPrematch *prematch, gint formation);

void
strategy_repair_player(Player *pl);

void
strategy_repair_players(GPtrArray *players, 
			const StrategyPrematch *prematch);

void
strategy_set_tokens(const Team *tm, const Fixture *fix);

void
strategy_free_tokens(void);

void
strategy_apply_prematch(Team *tm, const StrategyPrematch *prematch);

void
strategy_live_game_set_tokens(const LiveGame *match, gint team_idx);

void
strategy_live_game_check(LiveGame *match, gint team_idx);

void
strategy_live_game_apply_action(LiveGame *match, gint team_idx,
				const StrategyMatchAction *action);

gint
strategy_compare_players_sub(gconstpointer a,
			     gconstpointer b,
			     gpointer user_data);

gint
strategy_get_sub(const Team *tm, gint position, 
		 gint property, gboolean sub_in);

gint
strategy_compare_positions(gconstpointer a,
			   gconstpointer b,
			   gpointer user_data);

#endif
