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

enum PlayerValue
{
    PLAYER_VALUE_CARD_YELLOW = 0,
    PLAYER_VALUE_CARD_RED, 
    PLAYER_VALUE_GAMES,
    PLAYER_VALUE_GOALS,
    PLAYER_VALUE_SHOTS,
    PLAYER_VALUE_END
};

Player
player_new(Team *tm, gfloat average_skill);

gint
player_new_id(const GArray *players);

gint
player_get_position_from_structure(gint structure, gint player_number);

gfloat
player_new_talent(gfloat skill);

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

gfloat
player_get_cskill(const Player *pl, gint position);

gint
player_is_banned(const Player *pl);

gboolean
player_substitution_good_structure(gint old_structure, gint old_pos, gint player_pos);

gint
player_id_index(const Team *tm, gint player_id);

gfloat
player_get_game_skill(const Player *pl, gboolean skill);

void
player_decrease_fitness(Player *pl);

gint
player_card_get(const Player *pl, gint clid, gint card_type);

void
player_card_set(Player *pl, gint clid, gint card_type, gint value, gboolean diff);

void
player_games_goals_set(Player *pl, gint clid, gint type, gint value, gboolean diff);

gint
player_games_goals_get(const Player *pl, gint clid, gint type);

void
player_update_weekly(Team *tm, gint idx);

void
player_remove_contract(Team *tm, gint idx);

void
player_update_skill(Player *pl);

void
player_update_fitness(Player *pl);

void
player_update_post_match(Player *pl, gint clid);

void
player_replace_by_new(Player *pl, gboolean free_player);

void
player_remove_from_team(Team *tm, gint idx);

void
player_update_week_roundly(Team *tm, gint idx);

void
player_update_injury(Player *pl);

#endif
