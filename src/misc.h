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

#ifndef MISC_H
#define MISC_H

#include "bygfoot.h"

#define misc_int_to_char(number) g_strdup_printf("%d", number)

void
misc_print_error(GError **error, gboolean abort_program);

void
misc_swap_int(gint *first, gint *second);

void
misc_swap_gpointer(gpointer *first, gpointer *second);

GPtrArray*
misc_separate_strings(gchar *string);

GPtrArray*
misc_randomise_g_pointer_array(GPtrArray *array);

void
misc_print_grouped_int(gint number, gchar *buf);

gboolean
query_integer_is_in_array(gint item, gint *array, gint max);

gint
misc_int_compare(gint first, gint second);

gint
misc_float_compare(gfloat first, gfloat second);

gboolean
query_misc_string_contains(const gchar *string, const gchar *text);

gboolean
query_misc_string_in_array(const gchar *string, GPtrArray *array);

gboolean
query_misc_integer_is_in_g_array(gint item, GArray *array);

void
misc_string_replace_token(gchar *string, const gchar *token, const gchar *replacement);

void
misc_replace_sums(gchar *string);

gfloat
misc_get_age_from_birth(gint birth_year, gint birth_month);

const gchar*
misc_skip_spaces(const gchar* s);

const gchar*
misc_parse_value(const gchar *s, gint *value);

const gchar*
misc_parse_expression(const gchar *s, gint *result);

const gchar*
misc_parse_comparison(const gchar *s, gint *result);

const gchar*
misc_parse_and(const gchar *s, gint *result);

const gchar*
misc_parse(const gchar *s, gint *result);

GPtrArray*
misc_copy_ptr_array(const GPtrArray *array);

void
misc_string_assign(gchar **string, const gchar *contents);

#endif
