#ifndef FREE_H
#define FREE_H

#include "bygfoot.h"
#include "league.h"
#include "variables.h"

void
free_memory(void);

void
free_country(void);

void
free_league(League *league);

void
free_team(Team *team);

#endif
