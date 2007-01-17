/*
   table.h

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

#ifndef TABLE_H
#define TABLE_H

#include "bygfoot.h"
#include "fixture_struct.h"
#include "table_struct.h"

Table
table_new(void);

TableElement
table_element_new(Team *team, gint old_rank);

void
table_update(const Fixture *fix);

void
table_update_get_elements(TableElement **elements, const Fixture *fix);

gint
table_element_compare_func(gconstpointer a,
			   gconstpointer b,
			   gpointer clid_round);

gboolean
query_tables_in_country(void);

#endif

