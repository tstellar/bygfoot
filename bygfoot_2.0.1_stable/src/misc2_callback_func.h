/*
   misc2_callback_func.h

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

#ifndef MISC2_CALLBACK_FUNC_H
#define MISC2_CALLBACK_FUNC_H

#include "bygfoot.h"

void
misc2_callback_transfer_cpu_player(void);

void
misc2_callback_transfer_user_player(void);

gboolean
misc2_callback_change_structure(gint structure);

void
misc2_callback_contract_offer(void);

void
misc2_callback_add_user(void);

void
misc2_callback_mmatches_button_press(GtkWidget *widget, gint row_num, gint col_num);

gboolean
misc2_callback_evaluate_job_application(Job *job, User *user);

#endif

