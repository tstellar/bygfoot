#ifndef TEAM_H
#define TEAM_H

#include "bygfoot.h"
#include "cup_struct.h"
#include "fixture_struct.h"
#include "team_struct.h"

/** Maximum number of players in a team. */
#define CONSTANT_TEAM_MAX_PLAYERS 20

Team
team_new(void);

gint
team_assign_playing_structure(void);

gint
team_assign_playing_style(void);

Stadium
team_stadium_new(void);

gboolean
query_is_my_team(const Team *tm);

void
team_generate_players(Team *tm);

gint
team_return_league_cup_value_int(const Team *tm, gint value_type);

void
team_get_league_cup_string(const Team *tm, gint value_type, gchar *buf);

void
team_copy(const Team *source, Team *dest);

void
team_append_to_array(const Team *tm, GArray *teams_array);

void
team_append_to_array_with_ids(const Team *tm, GArray *teams_array, gint clid, gint id);

gboolean
query_is_in_international_cups(const Team *tm);

gboolean
query_is_in_cup(const Team *tm, const Cup *cup);

GPtrArray*
team_get_pointers_from_array(const GArray *teams);

GPtrArray*
team_get_pointers_from_choose_teams(const GArray *choose_teams);

Team*
team_get_pointer_from_ids(gint clid, gint id);

GPtrArray*
team_get_player_pointers(const Team *tm);

Fixture*
team_get_next_fixture(const Team *tm);

gfloat
team_average_cskill(const Team *tm);

#endif
