#ifndef GAME_H
#define GAME_H

#include "bygfoot.h"
#include "fixture_struct.h"
#include "player_struct.h"

/** Indices for the team_value array.
    @see game_get_values() */
enum GameTeamValue
{
    GAME_TEAM_VALUE_DEFEND = 0,
    GAME_TEAM_VALUE_MIDFIELD,
    GAME_TEAM_VALUE_ATTACK,
    GAME_TEAM_VALUE_GOALIE,
    GAME_TEAM_VALUE_END
};

/** Player types. @see game_get_player() */
enum GamePlayerType
{
    GAME_PLAYER_TYPE_ATTACK = 0,
    GAME_PLAYER_TYPE_MIDFIELD,
    GAME_PLAYER_TYPE_DEFEND,
    GAME_PLAYER_TYPE_PENALTY,
    GAME_PLAYER_TYPE_END
};

/** @see game_player_increase() */
enum GamePlayerIncreaseType
{
    GAME_PLAYER_INCREASE_SHOTS = 0,
    GAME_PLAYER_INCREASE_GOALS,
    GAME_PLAYER_INCREASE_GAMES,
    GAME_PLAYER_INCREASE_YELLOW,
    GAME_PLAYER_INCREASE_END
};

void
game_get_values(const Fixture *fix, gfloat team_values[][GAME_TEAM_VALUE_END],
		gfloat home_advantage);

gfloat
game_get_player_contribution(const Player *pl, gboolean attack);

gint
game_get_player(const Team *tm, gint player_type, 
		gint number_of_penalty, gint not_this_one,
		gboolean skills);

void
game_get_player_probs(GArray *players, gfloat *probs, gfloat *weights, gboolean skills);

gint
game_get_penalty_taker(const Team *tm, gint last_penalty);

void
game_initialize(Fixture *fix);

void
game_save_team_states(void);

gboolean
game_check_live_game_resume_state(void);

void
game_get_subs(gint team_number, gint *subs_in, gint *subs_out);

gint
game_substitute_player(Team *tm, gint player_number);

void
game_player_increase(gint clid, Player *pl, gint type);

void
game_player_injury(Player *pl);

gfloat
game_get_foul_possession_factor(gboolean boost1, gboolean boost2);

gint
game_find_to_substitute(const Team *tm);

void
game_substitute_player_send_off(Team *tm, gint player_number, 
				gint *to_substitute, gint *substitute);
void
game_decrease_fitness(const Fixture *fix);

gint
game_player_get_ban_duration(void);

void
game_create_stats(gpointer live_game);

void
game_create_stats_players(gpointer live_game);

#endif
