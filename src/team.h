#ifndef TEAM_H
#define TEAM_H

#include "bygfoot.h"
#include "cup_struct.h"
#include "fixture_struct.h"
#include "team_struct.h"

/** Possibilities for team comparison. */
enum TeamCompare
{
    TEAM_COMPARE_RANK = 0,
    TEAM_COMPARE_END
};

Team
team_new(void);

gint
team_assign_playing_structure(void);

gint
team_assign_playing_style(void);

Stadium
team_stadium_new(void);

gboolean
query_is_my_team(const Team *tm);

void
team_generate_players_stadium(Team *tm);

gint
team_return_league_cup_value_int(const Team *tm, gint value_type);

void
team_get_league_cup_string(const Team *tm, gint value_type, gchar *buf);

void
team_copy(const Team *source, Team *dest);

void
team_append_to_array(const Team *tm, GArray *teams_array);

void
team_append_to_array_with_ids(const Team *tm, GArray *teams_array, gint clid, gint id);

gboolean
query_is_in_international_cups(const Team *tm);

gboolean
query_is_in_cup(const Team *tm, const Cup *cup);

GPtrArray*
team_get_pointers_from_array(const GArray *teams);

GPtrArray*
team_get_pointers_from_choose_teams(const GArray *choose_teams);

Team*
team_get_pointer_from_ids(gint clid, gint id);

GPtrArray*
team_get_player_pointers(const Team *tm);

Fixture*
team_get_fixture(const Team *tm, gboolean last_fixture);

gfloat
team_get_average_skill(const Team *tm, gboolean cskill);

gint
team_is_user(const Team *tm);

gint
team_rank(const Team *tm, gint clid);

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
team_update_user_team_weekly(Team *tm);

void
team_update_cpu_team(Team *tm, gboolean reset_fitness);

void
team_update_post_match(Team *tm, gint clid);

void
team_update_cpu_corrections(Team *tm, gboolean reset_fitness);

void
team_update_cpu_subs(Team *tm);

void
team_update_cpu_structure(Team *tm);

void
team_update_cpu_new_players(Team *tm);

void
team_update_user_team_week_roundly(Team *tm);

Team*
team_get_new(const Team *tm, gboolean fire);

GPtrArray*
team_get_sorted(GCompareDataFunc compare_function, gint type);

gint
team_compare_func(gconstpointer a, gconstpointer b, gpointer data);

#endif
