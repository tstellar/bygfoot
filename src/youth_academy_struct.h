#ifndef YOUTH_ACADEMY_STRUCT_H
#define YOUTH_ACADEMY_STRUCT_H

#include "bygfoot.h"

/** The data about a user's youth academy is bundled
    here. */
typedef struct
{
    /** Pointer to the team the academy belongs to. */
    Team *tm;
    /** Position preference of the user. */
    gint pos_pref;
    /** Coach quality, like scout or physio. */
    gint coach;
    /** Percentage of income the user devotes to the youth
	academy. */
    gint percentage;
    /** Average coach and percentage values; these are used
	when a new youth comes into the academy to determine
	the quality of the youth. */
    gfloat av_coach, av_percentage;
    /** When this counter is <= 0, a new youth appears. */
    gfloat counter_youth;
    /** The youths are simply young players. */
    GArray *players;
} YouthAcademy;

#endif
