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

/** A struct needed when managing promotions
    and relegations. */
typedef struct
{
    Team tm; /**< The team we move. */
    gint league_idx; /** The index of the league that the team gets moved to. */

} TeamMove;

Team
team_new(gboolean new_id);

gint
team_assign_playing_structure(void);

gint
team_assign_playing_style(void);

gboolean
query_is_my_team(const Team *tm);

void
team_generate_players_stadium(Team *tm);

gboolean
query_team_is_in_international_cups(const Team *tm, gint group);

gboolean
query_team_is_in_cup(const Team *tm, const Cup *cup);

GPtrArray*
team_get_pointers_from_array(const GArray *teams);

Team*
team_of_id(gint id);

GPtrArray*
team_get_player_pointers(const Team *tm);

Fixture*
team_get_fixture(const Team *tm, gboolean last_fixture);

gfloat
team_get_average_skill(const Team *tm, gboolean cskill);

gint
team_is_user(const Team *tm);

gint
team_get_cup_rank(const Team *tm, const CupRound *cupround);

gint
team_get_league_rank(const Team *tm);

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
team_get_sorted(GCompareDataFunc compare_function, gint type, gboolean cup);

gint
team_compare_func(gconstpointer a, gconstpointer b, gpointer data);

gint
team_get_index(const Team *tm);

gfloat
team_get_average_skills(const GArray *teams);

gboolean
query_team_plays(const Team *tm, gint week_number, gint week_round_number);

gint
team_get_table_value(const Team *tm, gint type);

void
team_write_results(const Team *tm, gchar *result_buf, gchar *goals_buf);

void
team_write_own_results(const Team *tm, gchar *buf, gboolean sort);

gboolean
query_team_is_in_teams_array(const Team *tm, const GPtrArray *teams);

gchar*
team_has_def_file(const Team *tm);

void
team_complete_def(Team *tm, gfloat average_skill);

void
team_complete_def_sort(Team *tm);

#endif
