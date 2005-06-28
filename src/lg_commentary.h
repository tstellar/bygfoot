#ifndef LG_COMMENTARY_H
#define LG_COMMENTARY_H

#include "bygfoot.h"
#include "fixture_struct.h"
#include "live_game_struct.h"

/** Tokens that get replaced in the commentary text. */
enum LGTokens
{
    LG_TOKEN_TEAM_HOME = 0,
    LG_TOKEN_TEAM_AWAY,
    LG_TOKEN_TEAM_IN_POSS,
    LG_TOKEN_TEAM_NOT_IN_POSS,
    LG_TOKEN_TEAM_LOSING,
    LG_TOKEN_TEAM_WINNING,
    LG_TOKEN_TEAM,
    LG_TOKEN_ATTENDANCE,
    LG_TOKEN_PLAYER1,
    LG_TOKEN_PLAYER2,
    LG_TOKEN_RESULT,
    LG_TOKEN_MINUTE,
    LG_TOKEN_GOALS_HOME,
    LG_TOKEN_GOALS_AWAY,
    LG_TOKEN_GOAL_DIFF,
    LG_TOKEN_EXTRA,
    LG_TOKEN_END
};

#define lg_token(name) option_string(name, &lg_tokens)

gchar*
lg_commentary_get_player_name(const LiveGameUnit *unit, const Fixture *fix, gint player);

gboolean
lg_commentary_replace_tokens(const gchar *string, const GPtrArray *strings, gchar *dest);

void
lg_commentary_set_strings(GPtrArray *strings, const LiveGameUnit *unit, const Fixture *fix);

void
lg_commentary_generate(LiveGameUnit *unit, const Fixture *fix);

gchar*
lg_commentary_get_extra_data(const LiveGameUnit *unit, const Fixture *fix);

void
lg_commentary_load_commentary_file_from_option(void);

void
lg_commentary_load_commentary_file(const gchar *commentary_file, gboolean abort);

#endif
