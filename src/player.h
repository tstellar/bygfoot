#ifndef PLAYER_H
#define PLAYER_H

#include "bygfoot.h"
#include "player_struct.h"
#include "team_struct.h"

enum PlayerCompareAttrib
{
    PLAYER_COMPARE_ATTRIBUTE_GAME_SKILL = 0,
    PLAYER_COMPARE_ATTRIBUTE_POS,
    PLAYER_COMPARE_ATTRIBUTE_END
};

Player
player_new(Team *tm, gint average_skill);

gint
player_new_id(const GArray *players);

gint
player_get_position_from_structure(gint structure, gint player_number);

gint
player_new_talent(gint skill);

void
player_estimate_talent(Player *pl);

gint
player_assign_value(const Player *pl);

gint
player_assign_wage(const Player *pl);

void
player_copy(const Player *source, Player *dest);

void
player_append_to_array(const Player *pl, Team *tm);

void
player_remove_from_team(Team *tm, gint player_number);

gint
player_all_games_goals(const Player *pl, gint type);

gint
player_all_cards(const Player *pl);

Player*
player_of_ids(gint clid, gint team_id, gint id);

Player*
player_of(const Team *tm, gint number);

Player*
player_of_id(const Team *tm, gint id);

gint
player_compare_func(gconstpointer a, gconstpointer b, gpointer data);

gint
player_compare_substitute_func(gconstpointer a, gconstpointer b, gpointer data);

void
player_move(Team *tm1, gint player_number, Team *tm2, gint insert_at);

void
player_swap(Team *tm1, gint player_number1, Team *tm2, gint player_number2);

gint
player_get_cskill(const Player *pl, gint position);

gint
player_is_banned(const Player *pl);

gboolean
player_substitution_good_structure(gint old_structure, gint old_pos, gint player_pos);

gint
player_id_index(const Team *tm, gint player_id);

gfloat
player_get_game_skill(const Player *pl, gboolean skill);

#endif
