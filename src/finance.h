/*
   finance.h

   Bygfoot Football Manager -- a small and simple GTK2-based
   football management game.

   http://bygfoot.sourceforge.net

   Copyright (C) 2005  Gyözö Both (gyboth@bygfoot.com)

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

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

void
finance_assign_game_money(const Fixture *fix);

#endif
