#ifndef FREE_H
#define FREE_H

#include "bygfoot.h"
#include "cup_struct.h"
#include "league_struct.h"
#include "live_game_struct.h"
#include "player_struct.h"
#include "team_struct.h"

void
free_g_array(GArray **array);

void
free_g_ptr_array(GPtrArray **array);

void
free_g_string(GString **string);

void
free_memory(void);

void
free_country(void);

void
free_leagues_array(GArray **leagues);

void
free_league(League *league);

void
free_teams_array(GArray **teams);

void
free_team(Team *team);

void
free_cups_array(GArray **cups);

void
free_cup(Cup *cup);

void
free_cup_tables(GArray *tables);

void
free_cup_choose_team(CupChooseTeam *cup_choose_team);

void
free_variables(void);

void
free_g_string_array(GPtrArray **array);

void
free_player(Player *pl);

void
free_live_game(LiveGame *match);

#endif
