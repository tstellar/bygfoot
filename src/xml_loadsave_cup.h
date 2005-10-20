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

#ifndef XML_LOADSAVE_CUP_H
#define XML_LOADSAVE_CUP_H

#include "bygfoot.h"
#include "cup_struct.h"

void
xml_loadsave_cup_read(const gchar *filename, Cup *cup);

void
xml_loadsave_cup_write(const gchar *prefix, const Cup *cup);

void
xml_loadsave_cup_write_round(FILE *fil, const gchar *prefix, const Cup *cup, gint round);

void
xml_loadsave_cup_write_choose_team(FILE *fil, const CupChooseTeam *choose_team);

#endif
