#ifndef PLAYER_H
#define PLAYER_H

#include "bygfoot.h"
#include "player_struct.h"
#include "team_struct.h"

/** Highest skill and talent a player can have.  */
#define CONSTANT_PLAYER_MAX_SKILL 99

Player
player_new(const Team *tm, gint average_skill);

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

#endif
