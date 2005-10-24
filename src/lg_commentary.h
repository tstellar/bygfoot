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

#ifndef LG_COMMENTARY_H
#define LG_COMMENTARY_H

#include "bygfoot.h"
#include "fixture_struct.h"
#include "lg_commentary_struct.h"
#include "live_game_struct.h"

void
lg_commentary_post_match(void);

void
lg_commentary_initialize(const Fixture *fix);

gboolean
lg_commentary_check_commentary(const LGCommentary *commentary, gchar *dest);

gboolean
query_lg_commentary_is_repetition(gint id);

void
lg_commentary_get_order(const GArray *commentaries, gint *order);

void
lg_commentary_set_basic_tokens(const LiveGameUnit *unit, const Fixture *fix);;

void
lg_commentary_set_player_tokens(const LiveGameUnit *unit, const Fixture *fix);

void
lg_commentary_set_team_tokens(const LiveGameUnit *unit, const Fixture *fix);

void
lg_commentary_set_stats_tokens(const LiveGameStats *stats);

void
lg_commentary_generate(const LiveGame *live_game, LiveGameUnit *unit);

gchar*
lg_commentary_get_extra_data(const LiveGameUnit *unit, const Fixture *fix);

void
lg_commentary_load_commentary_file_from_option(void);

void
lg_commentary_load_commentary_file(const gchar *commentary_file, gboolean abort);

#endif
