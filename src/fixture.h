#ifndef FIXTURE_H
#define FIXTURE_H

#include "bygfoot.h"
#include "cup_struct.h"
#include "fixture_struct.h"
#include "league_struct.h"

void
fixture_write_league_fixtures(League *league);

void
fixture_write_cup_fixtures(Cup *cup);

void
fixture_write_cup_round_robin(Cup *cup, gint cup_round);

void
fixture_write_round_robin(gpointer league_cup, gint cup_round, GPtrArray *teams);

void
fixture_write_round_robin_matchday(GArray *fixtures, gint cup_round, GPtrArray *teams,
				   gint special, gint week_number, gint week_round_number,
				   gint clid, gboolean home_advantage, gboolean second_leg,
				   gboolean decisive);

void
fixture_write_knockout_round(GPtrArray *teams, Cup *cup, gint cup_round);

void
fixture_write(GArray *fixtures, Team *home_team, Team *away_team, gint week_number,
	      gint week_round_number, gint clid, gint cup_round, gboolean home_advantage,
	      gboolean second_leg, gboolean decisive);

gint
fixture_get_free_round(gint week_number, gint clid);

gboolean
query_fixture_is_earlier(const Fixture *fix1, const Fixture *fix2);

gboolean
query_fixture_is_draw(const Fixture *fix);

gboolean
query_my_team_involved(const Fixture *fix);

#endif
