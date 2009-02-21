/*
   team.h

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

#ifndef TEAM_H
#define TEAM_H

#include "bygfoot.h"
#include "cup_struct.h"
#include "fixture_struct.h"
#include "team_struct.h"

/** Possibilities for team comparison. */
enum TeamCompare
{
    TEAM_COMPARE_LEAGUE_RANK = 0,
    TEAM_COMPARE_LEAGUE_LAYER,
    TEAM_COMPARE_UNSORTED,
    TEAM_COMPARE_AV_SKILL,
    TEAM_COMPARE_OFFENSIVE,
    TEAM_COMPARE_DEFENSE,
    TEAM_COMPARE_END
};

Team
team_new(gboolean new_id);

gboolean
query_is_my_team(const Team *tm);

void
team_generate_players_stadium(Team *tm, gfloat av_talent);

gboolean
query_team_is_in_cups(const Team *tm, gint group);

gboolean
query_team_is_in_cup(const Team *tm, const Cup *cup);

GPtrArray*
team_get_pointers_from_array(const GArray *teams, GPtrArray *team_ptrs);

Team*
team_of_id(gint id);

Fixture*
team_get_fixture(const Team *tm, gboolean last_fixture);

gfloat
team_get_average_skill(const Team *tm, gboolean cskill);

gfloat
team_get_average_talent(const Team *tm);

gint
team_name_is_user(const gchar *team_name);

gint
team_is_user(const Team *tm);

gint
team_get_cup_rank(const Team *tm, const CupRound *cupround, gboolean abort);

gint
team_get_league_rank(const Team *tm, gint clid);

void
team_change_structure(Team *tm, gint new_structure);

gint
team_find_appropriate_structure(const Team *tm);

void
team_rearrange(Team *tm);

gchar*
team_attribute_to_char(gint attribute, gint value);

void
team_change_attribute_with_message(Team * tm, gint attribute, gint new_value);

void
team_update_team_weekly(Team *tm);

void
team_update_post_match(Team *tm, const Fixture *fix);

void
team_update_cpu_new_players(Team *tm);

void
team_update_team_week_roundly(Team *tm);

Team*
team_get_new(const Team *tm, gboolean fire);

GPtrArray*
team_get_sorted(GCompareDataFunc compare_function, gint type, gboolean cup);

gint
team_compare_func(gconstpointer a, gconstpointer b, gpointer data);

gint
team_get_index(const Team *tm);

gfloat
team_get_average_talents(const GArray *teams);

gboolean
query_team_plays(const Team *tm, gint week_number, gint week_round_number);

gint
team_get_table_value(const Team *tm, gint type);

void
team_write_results(const Team *tm, gchar *result_buf, gchar *goals_buf);

void
team_write_overall_results(const Team *tm, gint clid, gchar *results);

void
team_write_own_results(const Team *tm, gchar *buf, gboolean sort, gboolean cup_matches);

gboolean
query_team_is_in_teams_array(const Team *tm, const GPtrArray *teams);

gchar*
team_has_def_file(const Team *tm);

void
team_complete_def(Team *tm);

void
team_complete_def_sort(Team *tm);

gint
team_get_table_clid(const Team *tm);

#endif
