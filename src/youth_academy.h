#ifndef YOUTH_ACADEMY_H
#define YOUTH_ACADEMY_H

#include "bygfoot.h"
#include "youth_academy_struct.h"

void
youth_academy_new(User *user);

void
youth_academy_add_new_player(YouthAcademy *youth_academy);

void
youth_academy_update_weekly(void);

#endif

