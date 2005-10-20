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

#ifndef MATHS_H
#define MATHS_H

#include <math.h>

#include "bygfoot.h"

/**
   Macros for random number generation (#rnd for float, #rndi and #gauss_disti for integer).
*/
#define math_rnd(lower,upper) g_rand_double_range(rand_generator, lower, upper)
#define math_rndi(lower,upper) g_rand_int_range(rand_generator, lower, upper + 1)
#define math_gauss_disti(lower, upper) ((gint)rint( math_gauss_dist((gfloat)lower - 0.499, (gfloat)upper + 0.499)))

gdouble
math_gaussrand(void);

gdouble
math_gauss_dist(gdouble lower, gdouble upper);

gint
math_get_place(gint value, gint place);

gint
math_round_integer(gint number, gint places);

void
math_generate_permutation(gint *array, gint start, gint end);

gint
math_get_bye_len(gint number);

gint
math_sum_int_array(const gint *array, gint max);

#endif
