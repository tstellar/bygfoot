/*
   cup.h

   Bygfoot Football Manager -- a small and simple GTK2-based
   football management game.

   http://bygfoot.sourceforge.net

   Copyright (C) 2005  Gyözö Both (gyboth@bygfoot.com)

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

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
#define query_cup_hide(clid) query_cup_has_property(clid, "hide")

Cup
cup_new(gboolean new_id);

CupChooseTeam
cup_choose_team_new(void);

CupRound
cup_round_new(void);

void
cup_reset(Cup *cup);

void
cup_get_team_pointers(Cup *cup, gint round);

void
cup_load_choose_team_generate(Cup *cup, CupRound *cup_round, const CupChooseTeam *ct);

void
cup_load_choose_team(Cup *cup, GPtrArray *teams, const CupChooseTeam *ct);

gint
cup_get_first_week_of_cup_round(const Cup *cup, gint cup_round);

gint
cup_get_matchdays_in_cup_round(const Cup *cup, gint round);

gint
cup_round_get_number_of_teams(const Cup *cup, gint round);

gint
cup_round_get_byes(const Cup *cup, gint round);

gint
cup_round_get_new_teams(const CupRound *cup_round);

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

gint
cup_compare_success(gconstpointer a, gconstpointer b, gpointer data);

gint
cup_compare_success_knockout(const Team *tm1, const Team *tm2, const GArray *fixtures);

gint
cup_get_round_reached(const Team *tm, const GArray *fixtures);

gboolean
query_cup_begins(const Cup *cup);

void
cup_get_choose_team_league_cup(const CupChooseTeam *ct, 
			       const League **league, const Cup **cup);

gint
cup_get_last_week_from_first(const Cup *cup, gint first_week);

void
cup_get_round_name(const Cup *cup, gint round, gchar *buf);

gint
cup_has_tables(gint clid);

Team*
cup_get_winner(const Cup *cup);

gboolean
query_cup_transfer(void);

gchar*
cup_get_highlight_colour(const Cup *cup);

#endif
