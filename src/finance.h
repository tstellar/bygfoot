#ifndef FINANCE_H
#define FINANCE_H

#include "bygfoot.h"
#include "variables.h"
#include "user_struct.h"

void
finance_update_user_weekly(User *user);

gfloat
finance_wage_unit(const Team *tm);

gint
finance_team_drawing_credit_loan(const Team *tm, gboolean loan);

#endif
