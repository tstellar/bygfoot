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

#ifndef LANGUAGE_H
#define LANGUAGE_H

#include "bygfoot.h"
#include "option.h"

#include <locale.h>

#define language_get_current_index() (strlen(opt_str("string_opt_language_code")) > 0) ? language_get_code_index(opt_str("string_opt_language_code")) : -1

gint
language_get_code_index(const gchar *code);

void
language_set(gint index);

#ifndef G_OS_UNIX
extern int _nl_msg_cat_cntr;
#endif

#endif
