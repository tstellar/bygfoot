#ifndef PLAYER_H
#define PLAYER_H

#include "bygfoot.h"
#include "player_struct.h"
#include "team_struct.h"

enum PlayerCompareAttrib
{
    PLAYER_COMPARE_ATTRIBUTE_GAME_SKILL = 0,
    PLAYER_COMPARE_ATTRIBUTE_POS,
    PLAYER_COMPARE_ATTRIBUTE_LEAGUE_GOALS,
    PLAYER_COMPARE_ATTRIBUTE_END
};

#define query_player_is_youth(pl) (pl->age <= const_float("float_player_age_lower"))

Player
player_new(Team *tm, gfloat average_skill, gboolean new_id);

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

gint
player_all_games_goals(const Player *pl, gint type);

gint
player_all_cards(const Player *pl);

Player*
player_of_idx_team(const Team *tm, gint number);

Player*
player_of_id_team(const Team *tm, gint id);

gint
player_compare_func(gconstpointer a, gconstpointer b, gpointer data);

gint
player_compare_substitute_func(gconstpointer a, gconstpointer b, gpointer data);

void
player_move(Team *tm1, gint player_number, Team *tm2, gint insert_at);

void
player_swap(Team *tm1, gint player_number1, Team *tm2, gint player_number2);

gfloat
player_get_cskill(const Player *pl, gint position, gboolean check_health);

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
player_games_goals_set(Player *pl, gint clid, gint type, gint value);

gint
player_games_goals_get(const Player *pl, gint clid, gint type);

void
player_update_weekly(Player *pl);

void
player_remove_contract(Player *pl);

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

void
player_copy(Player *pl, Team *tm, gint insert_at);

gchar*
player_injury_to_char(gint injury_type);

gboolean
query_player_id_in_team(gint player_id, const Team *tm);

void
player_season_start(Player *pl, gfloat skill_change);

gchar*
player_get_last_name(const gchar *name);

void
player_complete_def(Player *pl, gfloat average_skill);

GPtrArray*
player_get_pointers_from_array(const GArray *players);

void
player_move_to_ya(gint idx);

void
player_move_from_ya(gint idx);

#endif
