#ifndef GAME_GUI_H
#define GAME_GUI_H

#include "bygfoot.h"
#include "live_game.h"
#include "variables.h"

void
game_gui_live_game_show_unit(const LiveGameUnit *unit);

void
game_gui_live_game_set_hscale(const LiveGameUnit *unit, GtkHScale *hscale);

#endif
