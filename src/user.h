#ifndef USER_H
#define USER_H

#include "bygfoot.h"
#include "user_struct.h"
#include "variables.h"

/** Convenience abbrevs. */
#define usr(i) g_array_index(users, User, i)

void
user_set_up_team_new_game(User *user);

void
user_set_up_team(User *user);

void
user_set_up_finances(User *user);

User
user_new(void);

void
user_remove(gint idx, gboolean regenerate_team);

void
user_set_player_list_attributes(const User *user, PlayerListAttribute *attribute, gint list_number);

#endif
