#ifndef LIVE_GAME_H
#define LIVE_GAME_H

#include "bygfoot.h"
#include "live_game_struct.h"

void
live_game_calculate_fixture(Fixture *fix);

void
live_game_initialize(Fixture *fix);

void
live_game_post_match(void);

gboolean
query_live_game_event_is_break(gint minute, gint time);

gboolean
query_live_game_penalties_over(void);

gint
live_game_get_break(void);

gint
live_game_get_time(const LiveGameUnit *unit);

gint
live_game_get_minute(void);

void
live_game_create_unit(void);

void
live_game_fill_new_unit(LiveGameUnit *new);

void
live_game_create_start_unit(void);

void
live_game_evaluate_unit(LiveGameUnit *unit);

void
live_game_event_foul(void);

void
live_game_event_lost_possession(void);

void
live_game_event_injury(gint team, gint player, gboolean create_new);

void
live_game_event_stadium(void);

void
live_game_event_scoring_chance(void);

void
live_game_event_penalty();

void
live_game_event_general(gboolean create_new);

void
live_game_event_general_get_players(void);

void
live_game_event_free_kick(void);

void
live_game_event_send_off(gint team, gint player, gboolean second_yellow);

void
live_game_event_duel(void);

gboolean
query_live_game_second_yellow(gint team, gint player);

void
live_game_reset(LiveGame *live_game, Fixture *fix, gboolean free_variable);

gint
live_game_unit_get_minute(const LiveGameUnit *unit);

gint
live_game_get_area(const LiveGameUnit *unit);

gfloat
live_game_pit_teams(const LiveGameUnit *unit, gfloat exponent);

void
live_game_injury_get_player(void);

void
live_game_resume(void);

void
live_game_event_substitution(gint team_number, gint sub_in, gint sub_out);

void
live_game_event_team_change(gint team_number, gint event_type);

void
live_game_finish_unit(void);

LiveGameUnit*
live_game_unit_before(const LiveGameUnit* unit, gint gap);

void
live_game_set_match(LiveGame *live_game);

gint
live_game_event_get_verbosity(gint event_type);

#endif
