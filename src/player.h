#ifndef PLAYER_H
#define PLAYER_H

#include "bygfoot.h"
#include "player_struct.h"
#include "team_struct.h"

Player
player_new(Team *tm, gint average_skill);

gint
player_new_id(const GArray *players);

gint
player_get_position_from_structure(gint structure, gint player_number);

gint
player_new_talent(gint skill);

gint
player_estimate_talent(const Player *pl);

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
player_all_games_goals(const Player *pl, gboolean goals);

gint
player_all_cards(const Player *pl);

Player*
player_of_ids(gint clid, gint team_id, gint id);

Player*
player_of(const Team *tm, gint number);

Player*
player_of_id(const Team *tm, gint id);

#endif
