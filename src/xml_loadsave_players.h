#ifndef XML_LOADSAVE_PLAYERS_H
#define XML_LOADSAVE_PLAYERS_H

#include "bygfoot.h"
#include "team_struct.h"

void
xml_loadsave_players_text(gchar *text);

void
xml_loadsave_players_end_element(gint tag, GArray *players);

void
xml_loadsave_players_start_element(gint tag, Team *tm);

void
xml_loadsave_players_write_player(FILE *fil, const Player *pl);

void
xml_loadsave_players_write(FILE *fil, const GArray *players);

#endif

