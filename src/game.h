#ifndef GAME_H
#define GAME_H

#include "bygfoot.h"
#include "fixture_struct.h"
#include "player_struct.h"

/** Influence of the fitness on the player contribution.
    The higher the worse. @see game_get_player_contribution() */
#define CONSTANT_GAME_PLAYER_FITNESS_EXPONENT 0.25
/* #define CONSTANT_GAME_HOME_ADVANTAGE_LOWER 0.04 */
/* #define CONSTANT_GAME_HOME_ADVANTAGE_UPPER 0.08 */
#define CONSTANT_GAME_HOME_ADVANTAGE_LOWER 0.04
#define CONSTANT_GAME_HOME_ADVANTAGE_UPPER 0.08

/** Indices for the team_value array.
    @see game_get_values() */
enum GameTeamValue
{
    GAME_TEAM_VALUE_ATTACK = 0,
    GAME_TEAM_VALUE_DEFEND,
    GAME_TEAM_VALUE_GOALIE,
    GAME_TEAM_VALUE_END
};

/** Player types. @see game_get_player() */
enum GamePlayerType
{
    GAME_PLAYER_TYPE_ATTACKER = 0,
    GAME_PLAYER_TYPE_ATTACKING,
    GAME_PLAYER_TYPE_DEFENDER,
    GAME_PLAYER_TYPE_PENALTY,
    GAME_PLAYER_TYPE_INJURY,
    GAME_PLAYER_TYPE_END
};

void
game_get_values(const Fixture *fix, gfloat team_values[][GAME_TEAM_VALUE_END],
		gfloat home_advantage);

gfloat
game_get_player_contribution(const Player *pl, gboolean attack);

gint
game_get_player(const Team *tm, gint player_type, gint number_of_penalty, gint not_this_one);

void
game_get_player_probs(GArray *players, gfloat *probs, gfloat *weights);

#endif
