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
lg_commentary_replace_tokens(gchar *commentary_text);

void
lg_commentary_replace_expressions(gchar *commentary_text);

gboolean
lg_commentary_parse_condition(const gchar *condition);

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
