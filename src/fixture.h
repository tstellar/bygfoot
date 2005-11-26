/*
   fixture.h

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

#ifndef FIXTURE_H
#define FIXTURE_H

#include "bygfoot.h"
#include "cup_struct.h"
#include "fixture_struct.h"
#include "league_struct.h"

enum FixtureCompare
{
    FIXTURE_COMPARE_DATE = 0,
    FIXTURE_COMPARE_END
};

/** Check whether the team with given id participates in the fixture. */
#define query_fixture_team_involved(fix, team_id) (fix->team_ids[0] == team_id || fix->team_ids[1] == team_id)

void
fixture_write_league_fixtures(League *league);

void
fixture_write_cup_fixtures(Cup *cup);

void
fixture_write_cup_round_robin(Cup *cup, gint cup_round, GPtrArray *teams);

void
fixture_write_round_robin(gpointer league_cup, gint cup_round, GPtrArray *teams, gboolean one_round);

void
fixture_write_round_robin_matchday(GArray *fixtures, gint cup_round, GPtrArray *teams,
				   gint special, gint week_number,
				   gint clid, gboolean home_advantage);

void
fixture_write_knockout_round(Cup *cup, gint cup_round, GPtrArray *teams);

void
fixture_write(GArray *fixtures, Team *home_team, Team *away_team, gint week_number,
	      gint week_round_number, gint clid, gint cup_round, gint replay_number,
	      gboolean home_advantage, gboolean second_leg, gboolean decisive);

void
fixture_update(Cup *cup);

gboolean
fixture_update_write_replays(Cup *cup);

GPtrArray*
fixture_get_cup_round_winners(const Cup *cup);

GPtrArray*
fixture_get_round_robin_advance(const Cup *cup, gint round);

gpointer
fixture_winner_of(const Fixture *fix, gboolean team_id);

gint
fixture_get_free_round(gint week_number, const GPtrArray *teams, 
		       gint team_id1, gint team_id2);

gboolean
query_fixture_is_earlier(const Fixture *fix1, const Fixture *fix2);

gboolean
query_fixture_is_later(const Fixture *fix1, const Fixture *fix2);

gboolean
query_fixture_is_draw(const Fixture *fix);

gint
fixture_user_team_involved(const Fixture *fix);

gboolean
query_fixture_has_tables(const Fixture *fix);

gboolean
query_fixture_in_week_round(gint clid, gint week_number, gint week_round_number);

Fixture*
fixture_get_first_leg(const Fixture *fix);

GPtrArray*
fixture_get_week_list_clid(gint clid, gint week_number, gint week_round_number);

void
fixture_result_to_buf(const Fixture *fix, gchar *buf, gboolean swap);

gint
fixture_get_number_of_matches(gint week_number, gint week_round_number);

Fixture*
fixture_get(gint type, gint clid, gint week_number, gint week_round_number, const Team *tm);

Fixture*
fixture_get_next(gint clid, gint week_number, gint week_round_number);

Fixture*
fixture_get_previous(gint clid, gint week_number, gint week_round_number);

GPtrArray*
fixture_get_latest(const Team *tm);

gint
fixture_compare_func(gconstpointer a, gconstpointer b, gpointer data);

GPtrArray*
fixture_get_matches(const Team *tm1, const Team *tm2);

GPtrArray*
fixture_get_coming(const Team *tm);

Fixture*
fixture_from_id(gint id);

gint
fixture_get_last_week_round(gint week_number);

void
fixture_get_next_week(gint *week_number, gint *week_round_number);

void
fixture_get_previous_week(gint *week_number, gint *week_round_number);

GPtrArray*
fixture_get_week_list(gint week_number, gint week_round_number);

GPtrArray*
fixture_get_season_results(void);

gint
fixture_get_goals_to_win(const Fixture *fix, const Team *tm);

#endif
