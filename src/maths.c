/*
   maths.c

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

#include "maths.h"
#include "misc.h"
#include "variables.h"

/**
   Generate a Gauss-distributed (pseudo)random number.
   "By Box and Muller, and recommended by Knuth".
   @return A Gauss-distributed random number.
*/
gdouble
math_gaussrand(void)
{
#ifdef DEBUG
    printf("math_gaussrand\n");
#endif

    static gdouble V1, V2, S;
    static gint phase = 0;
    gdouble X;

    if(phase == 0) {
	do {
	    gdouble U1 = g_rand_double(rand_generator);
	    gdouble U2 = g_rand_double(rand_generator);

	    V1 = 2 * U1 - 1;
	    V2 = 2 * U2 - 1;
	    S = V1 * V1 + V2 * V2;
	} while(S >= 1 || S == 0);

	X = V1 * sqrt(-2 * log(S) / S);
    } else
	X = V2 * sqrt(-2 * log(S) / S);

    phase = 1 - phase;

    return X;
}

/**
   Generate a Gauss-distributed random number within given boundaries
   using math_gaussrand().
   Expectation value of the distribution is (upper + lower) / 2,
   the variance is so that the number is between the boundaries with probability
   99,7 %. If the number isn't between the boundaries, we cut off.
   @param lower Lower cutoff boundary.
   @param upper Upper cutoff boundary.
   @return A Gauss-distributed number 
*/
gdouble
math_gauss_dist(gdouble lower, gdouble upper)
{
#ifdef DEBUG
    printf("math_gauss_dist\n");
#endif

 						   
    gdouble result;				   
     	
    result = (upper - lower) / 6 * math_gaussrand()
	+ (upper + lower) / 2;

    if(result < lower)
	result = lower;

    if(result > upper)
	result = upper;

    return result;
}

/**
   Get a certain part of an integer number.

   If 'place' is between 1 and 9, the 'place'th digit beginning
   from the right is returned, e.g. if the number = 1234 and
   place = 2, the function returns 3.

   If 'place' is between 10 and 19, say 10 + x, the first
   'x' digits are returned, e.g. number = 8765 and place = 12 leads to
   return value 87.

   If 'place' is between 20 and 29, say 20 + x, the last
   'x' digits are returned, e.g. number = 4869 and place = 22
   leads to return value 69.

   @param value The number which gets scrutinized.
   @param place The number telling the function which part of 'value' to return.
   @return A part of the integer 'value'.
 */
gint
math_get_place(gint value, gint place)
{
#ifdef DEBUG
    printf("math_get_place\n");
#endif

    if(place < 10)
	return (value % (gint)powf(10, place) -
		value % (gint)powf(10, place - 1)) /
	    (gint)powf(10, place - 1);

    else if(place < 20)
    {
	while(value >= (gint)powf(10, place % 10))
	    value = (value - value % 10) / 10;
	
	return value;
    }
    
    return value % (gint)powf(10, place % 10);
}

/**
   Round an integer with given precision.

   If places > 0, round with precision 'places', e.g. 
   number = 124566 and places = 2 leads to return value
   124600.

   If places < 0, precision is length of 'number' minus
   'places', e.g. number = 654987 and places = -2 leads to return
   value 65000.

   @param number The number to be rounded.
   @param places The precision.
   @return The rounded integer.
*/
gint
math_round_integer(gint number, gint places)
{
#ifdef DEBUG
    printf("math_round_integer\n");
#endif

    gint length = 0;
    gfloat copy = (gfloat)number;

    if(places > 0)
	return (gint)rint( (gfloat)number / powf(10, places) ) *
	    powf(10, places);

    while(copy >= 1)
    {
	copy /= 10;
	length++;
    }

    return (gint)rint( (gfloat)number / powf(10, length + places) ) *
	powf(10, length + places);
}

/** Generate a permutation of integers and write it to 'array'.
    @param array The integer array we store the permutation in. 
    It must have size at least end - start - 1.
    @param start The integer to start with.
    @param end The integer to end with. */
void
math_generate_permutation(gint *array, gint start, gint end)
{
#ifdef DEBUG
    printf("math_generate_permutation\n");
#endif

    gint i;

    for(i = start; i < end + 1; i++)
	array[i - start] = i;

    for(i=0;i<end - start;i++)
	misc_swap_int(&array[i], &array[math_rndi(i, end - start)]);
}

/** This function tells us how many teams from 'number' teams
    have to be left away to obtain a power of 2. */
gint
math_get_bye_len(gint number)
{
#ifdef DEBUG
    printf("math_get_bye_len\n");
#endif

    gint i;

    for(i=0;i<20;i++)
	if((gint)powf(2, i) >= number)
	    break;

    return (gint)powf(2, i) - number;
}

/** Return the sum of the integers in the array.
    @param array The integer array.
    @param max The size of the array.
    @return The sum of all the integers in the array. */
gint
math_sum_int_array(const gint *array, gint max)
{
#ifdef DEBUG
    printf("math_sum_int_array\n");
#endif

    gint i, sum = 0;

    for(i=0;i<max;i++)
	sum += array[i];

    return sum;
}
