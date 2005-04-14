#ifndef CUP_H
#define CUP_H

#include "bygfoot.h"
#include "cup_struct.h"
#include "fixture_struct.h"
#include "league_struct.h"

#define query_cup_is_prom(clid) (clid >= ID_PROM_CUP_START && clid < ID_SUPERCUP_START)

Cup
cup_new(gboolean new_id);

CupChooseTeam
cup_choose_team_new(void);

CupRound
cup_round_new(void);

void
cup_load_choose_teams(Cup *cup);

void
cup_load_choose_team_user(Cup *cup);

void
cup_choose_team_abort(const Cup *cup, const CupChooseTeam *choose_team, gboolean user);

GPtrArray*
cup_get_team_pointers(const Cup *cup);

gint
cup_get_first_week_of_cup_round(const Cup *cup, gint cup_round);

gint
cup_get_matchdays_in_cup_round(const Cup *cup, gint cup_round);

gint
cup_round_get_number_of_teams(const Cup *cup, gint cup_round);

Cup*
cup_from_clid(gint clid);

void
cup_round_name(const Fixture *fix, gchar *buf);

GPtrArray*
cup_get_teams_sorted(const Cup *cup);

GPtrArray*
cup_get_teams_from_names(GPtrArray *team_names);

gint
cup_compare_success(gconstpointer a, gconstpointer b, gpointer data);

gint
cup_get_round_reached(const Team *tm, const GArray *fixtures);

GPtrArray*
cup_get_choose_teams_pointers(Cup *cup);

gboolean
query_cup_supercup_begins(const Cup *supercup);

void
cup_get_choose_team_league_cup(const CupChooseTeam *ct, 
			       League **league, Cup **cup);

gint
cup_get_last_week_from_first(const Cup *cup, gint first_week);

#endif
