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
    LG_TOKEN_PLAYER1,
    LG_TOKEN_PLAYER2,
    LG_TOKEN_ATTENDANCE,
    LG_TOKEN_RESULT,
    LG_TOKEN_MINUTE,
    LG_TOKEN_EXTRA,
    LG_TOKEN_END
};

gchar*
lg_commentary_get_player_name(const LiveGameUnit *unit, const Fixture *fix, gint player);

void
lg_commentary_set_strings(gchar *strings[][2], const LiveGameUnit *unit, const Fixture *fix);

void
lg_commentary_generate(LiveGameUnit *unit, const Fixture *fix);

GPtrArray*
lg_commentary_get_commentaries(gint event_type);

gchar*
lg_commentary_get_extra_data(const LiveGameUnit *unit, const Fixture *fix);

gboolean
lg_commentary_replace_tokens(const gchar *string, gchar *strings[][2], gchar *dest);

#endif

