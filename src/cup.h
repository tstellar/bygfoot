#ifndef CUP_H
#define CUP_H

#include "bygfoot.h"
#include "cup_struct.h"
#include "fixture_struct.h"
#include "league_struct.h"

#define cup_get_last_tables_round(clid) &g_array_index(cup_from_clid(clid)->rounds, CupRound, cup_has_tables(clid))
#define cup_get_last_tables(clid) g_array_index(cup_from_clid(clid)->rounds, CupRound, cup_has_tables(clid)).tables

#define query_cup_has_property(clid, string) query_misc_string_in_array(string, cup_from_clid(clid)->properties)

#define query_cup_is_international(clid) query_cup_has_property(clid, "international")
#define query_cup_is_national(clid) query_cup_has_property(clid, "national")
#define query_cup_is_supercup(clid) query_cup_has_property(clid, "supercup")
#define query_cup_is_promotion(clid) query_cup_has_property(clid, "promotion")

Cup
cup_new(gboolean new_id);

CupChooseTeam
cup_choose_team_new(void);

CupRound
cup_round_new(void);

void
cup_reset(Cup *cup);

GPtrArray*
cup_get_team_pointers(Cup *cup, gint round);

void
cup_load_choose_team_generate(Cup *cup, CupRound *cup_round, const CupChooseTeam *ct);

void
cup_load_choose_team(Cup *cup, GPtrArray *teams, const CupChooseTeam *ct);

gint
cup_get_first_week_of_cup_round(const Cup *cup, gint cup_round);

gint
cup_get_matchdays_in_cup_round(const Cup *cup, gint cup_round);

gint
cup_round_get_number_of_teams(const Cup *cup, gint cup_round);

Cup*
cup_from_clid(gint clid);

Cup*
cup_from_sid(const gchar* sid);

void
cup_round_name(const Fixture *fix, gchar *buf);

GPtrArray*
cup_get_teams_sorted(const Cup *cup);

gint
cup_compare_success_tables(const Team *tm1, const Team *tm2, const Cup *cup, gint round);

GPtrArray*
cup_get_teams_from_names(GPtrArray *team_names);

gint
cup_compare_success(gconstpointer a, gconstpointer b, gpointer data);

gint
cup_get_round_reached(const Team *tm, const GArray *fixtures);

GPtrArray*
cup_get_choose_teams_pointers(Cup *cup);

gboolean
query_cup_begins(const Cup *cup);

void
cup_get_choose_team_league_cup(const CupChooseTeam *ct, 
			       League **league, Cup **cup);

gint
cup_get_last_week_from_first(const Cup *cup, gint first_week);

void
cup_get_round_name(const Cup *cup, gint round, gchar *buf);

gint
cup_has_tables(gint clid);

Team*
cup_get_winner(const Cup *cup);

gint
cup_count_international(void);

#endif
