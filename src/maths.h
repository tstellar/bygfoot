#ifndef MATH_H
#define MATH_H

#include <math.h>
#include "bygfoot.h"
#include "variables.h"

/**
   Macros for random number generation (#rnd for float and #rndi for integer).
*/
#define rnd(lower,upper) ((gfloat)random()/(gfloat)0x7fffffff*((upper)-(lower))+(lower))
#define rndi(lower,upper) ((gint)rint( rnd((gfloat)(lower) - 0.499, (gfloat)(upper) + 0.499) ))

gfloat
gaussrand(void);

gfloat
gauss_dist(gfloat lower, gfloat upper);

gint
get_place(gint value, gint place);

gint
round_integer(gint number, gint places);

#endif
