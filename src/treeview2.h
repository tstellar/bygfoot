/*
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

#ifndef TREEVIEW2_H
#define TREEVIEW2_H

#include "bygfoot.h"

enum
{
    TREEVIEW_MMATCH_COL_REPLAY = 6,
    TREEVIEW_MMATCH_COL_REMOVE,
    TREEVIEW_MMATCH_COL_EXPORT,
    TREEVIEW_MMATCH_COL_END
};

void
treeview2_create_mmatches(GtkListStore *ls);

void
treeview2_set_up_mmatches(GtkTreeView *treeview);

void
treeview2_show_mmatches(void);


void
treeview2_create_season_results(GtkListStore *ls);

void
treeview2_set_up_season_results(GtkTreeView *treeview);

void
treeview2_show_season_results(void);

void
treeview2_create_bets(GtkListStore *ls);

void
treeview2_set_up_bets(GtkTreeView *treeview);

void
treeview2_show_bets(void);

#endif
