#ifndef FINANCE_H
#define FINANCE_H

#include "bygfoot.h"
#include "user_struct.h"

/** The amount of money the user can spend. */
#define BUDGET(idx) (finance_team_drawing_credit_loan(usr(idx).tm, FALSE) + usr(idx).money)

void
finance_update_user_weekly(User *user);

gfloat
finance_wage_unit(const Team *tm);

gint
finance_team_drawing_credit_loan(const Team *tm, gboolean loan);

void
finance_pay_loan(gint value);

void
finance_get_loan(gint value);

gint
finance_get_stadium_improvement_cost(gfloat value, gboolean capacity);

gint
finance_get_stadium_improvement_duration(gfloat value, gboolean capacity);

#endif
