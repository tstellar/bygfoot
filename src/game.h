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

#ifndef GAME_H
#define GAME_H

#include "bygfoot.h"
#include "fixture_struct.h"
#include "player_struct.h"
#include "live_game_struct.h"

/** Player types. @see game_get_player() */
enum GamePlayerType
{
    GAME_PLAYER_TYPE_DEFEND = LIVE_GAME_UNIT_AREA_DEFEND,
    GAME_PLAYER_TYPE_MIDFIELD = LIVE_GAME_UNIT_AREA_MIDFIELD,
    GAME_PLAYER_TYPE_ATTACK = LIVE_GAME_UNIT_AREA_ATTACK,
    GAME_PLAYER_TYPE_PENALTY,
    GAME_PLAYER_TYPE_END
};

void
game_get_values(const Fixture *fix, gfloat team_values[][GAME_TEAM_VALUE_END],
		gfloat home_advantage);

gfloat
game_get_player_contribution(const Player *pl, gboolean attack);

gint
game_get_player(const Team *tm, gint player_type, 
		gint number_of_penalty, gint not_this_one,
		gboolean skills);

void
game_get_player_probs(GArray *players, gfloat *probs, gfloat *weights, gboolean skills);

gint
game_get_penalty_taker(const Team *tm, gint last_penalty);

void
game_initialize(Fixture *fix);

void
game_save_team_states(void);

gboolean
game_check_live_game_resume_state(void);

void
game_get_subs(gint team_number, gint *subs_in, gint *subs_out);

gint
game_substitute_player(Team *tm, gint player_number);

void
game_player_injury(Player *pl);

gfloat
game_get_foul_possession_factor(gboolean boost1, gboolean boost2);

gint
game_find_to_substitute(gint clid, const Team *tm);

void
game_substitute_player_send_off(gint clid, Team *tm, gint player_number, 
				gint *to_substitute, gint *substitute);
void
game_decrease_fitness(const Fixture *fix);

gint
game_player_get_ban_duration(void);

void
game_update_stats(LiveGame *lg, const LiveGameUnit *unit);

void
game_update_stats_player(LiveGame *lg, const LiveGameUnit *unit);

void
game_post_match(Fixture *fix);

void
game_assign_attendance(Fixture *fix);

void
game_assign_attendance_neutral(Fixture *fix);

void
game_stadium_event(Stadium *stadium, gint type);

void
game_get_max_values(gfloat max_values[3]);

void
game_reset_players(gint idx);

gint
game_get_default_penalty_shooter(const Team *tm);

/* void */
/* game_check_cpu_strategy(LiveGame *lg); */

#endif
