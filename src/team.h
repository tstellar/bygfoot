#ifndef TEAM_H
#define TEAM_H

#include "bygfoot.h"
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
is_my_team(const Team *tm);

void
team_generate_players(Team *tm);

gint
team_return_league_cup_value_int(const Team *tm, gint value_type);

void
team_copy(const Team *source, Team *dest);

void
team_append_to_array(const Team *tm, GArray *teams_array);

void
team_append_to_array_with_ids(const Team *tm, GArray *teams_array, gint clid, gint id);

gboolean
is_in_international_cups(const Team *tm);

#endif
