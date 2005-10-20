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

#ifndef OPTION_H
#define OPTION_H

#include <math.h>
#include "bygfoot.h"
#include "option_struct.h"

/** Convenience abbrevs. */
#define option_set_float(name, option_array, value) option_set_int(name, option_array, (gint)rint(value * 1000))

#define opt_str(name) option_string(name, &options)
#define opt_strp(name) option_string_pointer(name, &options)
#define opt_int(name) option_int(name, &options)
#define opt_intp(name) option_int_pointer(name, &options)
#define opt_float(name) option_float(name, &options)

#define opt_set_int(name, value) option_set_int(name, &options, value)
#define opt_set_str(name, value) option_set_string(name, &options, value)
#define opt_set_float(name, value) option_set_float(name, &options, value)

#define opt_user_str(name) option_string(name, &current_user.options)
#define opt_user_strp(name) option_string_pointer(name, &current_user.options)
#define opt_user_int(name) option_int(name, &current_user.options)
#define opt_user_intp(name) option_int_pointer(name, &current_user.options)
#define opt_user_float(name) option_float(name, &current_user.options)

#define opt_user_set_int(name, value) option_set_int(name, &current_user.options, value)
#define opt_user_set_str(name, value) option_set_string(name, &current_user.options, value)
#define opt_user_set_float(name, value) option_set_float(name, &current_user.options, value)

#define const_app(name) option_string(name, &constants_app)
#define const_str(name) option_string(name, &constants)
#define const_int(name) option_int(name, &constants)
#define const_float(name) option_float(name, &constants)

#define sett_int(name) option_int(name, &settings)
#define sett_set_int(name, value) option_set_int(name, &settings, value)

gfloat
option_float(const gchar *name, OptionList *optionlist);

gint
option_int(const gchar *name, OptionList *optionlist);

gint*
option_int_pointer(const gchar *name, OptionList *optionlist);

gchar*
option_string(const gchar *name, OptionList *optionlist);

gchar**
option_string_pointer(const gchar *name, OptionList *optionlist);

void
option_set_string(const gchar *name, OptionList *optionlist, const gchar *new_value);

void
option_set_int(const gchar *name, OptionList *optionlist, gint new_value);

void
option_add(OptionList *optionlist, const gchar *name, gint int_value, const gchar *string_value);

#endif
