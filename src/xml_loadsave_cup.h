#ifndef XML_LOADSAVE_CUP_H
#define XML_LOADSAVE_CUP_H

#include "bygfoot.h"
#include "cup_struct.h"

void
xml_loadsave_cup_read(const gchar *filename, Cup *cup);

void
xml_loadsave_cup_write(const gchar *prefix, const Cup *cup);

void
xml_loadsave_cup_write_round(FILE *fil, const CupRound *cup_round);

void
xml_loadsave_cup_write_choose_team(FILE *fil, const CupChooseTeam *choose_team);

#endif
