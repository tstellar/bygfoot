#ifndef LIVE_GAME_H
#define LIVE_GAME_H

#include "bygfoot.h"
#include "live_game_struct.h"

void
live_game_calculate_fixture(Fixture *fix);

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
live_game_event_foul(gboolean general);

void
live_game_event_lost_possession(void);

void
live_game_event_injury(gint player, gboolean create_new);

void
live_game_event_stadium(void);

void
live_game_event_scoring_chance(void);

void
live_game_event_penalty();

void
live_game_event_general(gboolean create_new);

void
live_game_event_free_kick(void);

void
live_game_event_send_off(gint player);

void
live_game_generate_commentary(LiveGameUnit *unit);

void
live_game_event_duel(void);

gboolean
query_live_game_second_yellow(gint team, gint player);

void
live_game_reset(Fixture *fix);

gint
live_game_unit_get_minute(const LiveGameUnit *unit);

void
live_game_create_stats(void);

#endif
