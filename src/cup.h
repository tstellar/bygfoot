#ifndef CUP_H
#define CUP_H

#include "bygfoot.h"
#include "cup_struct.h"

Cup
cup_new(void);

gint
cup_new_id(void);

CupChooseTeam
cup_choose_team_new(void);

CupRound
cup_round_new(void);

void
cup_load_choose_teams(Cup *cup);

void
cup_load_choose_team_user(Cup *cup);

void
cup_choose_team_abort(const Cup *cup, const CupChooseTeam *choose_team, gboolean user);

#endif
