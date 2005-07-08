#ifndef YOUTH_ACADEMY_H
#define YOUTH_ACADEMY_H

#include "bygfoot.h"
#include "youth_academy_struct.h"

YouthAcademy
youth_academy_new(Team *tm);

void
youth_academy_add_new_player(YouthAcademy *youth_academy);

void
youth_academy_update_weekly(void);

#endif

