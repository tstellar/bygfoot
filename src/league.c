/*
   league.c

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

#include "cup.h"
#include "free.h"
#include "league.h"
#include "main.h"
#include "maths.h"
#include "misc.h"
#include "option.h"
#include "player.h"
#include "stat.h"
#include "table.h"
#include "team.h"
#include "user.h"
#include "variables.h"

/**
   Create a new league with some default values.
   @return The league we created.
   @see League
*/
League
league_new(gboolean new_id)
{
#ifdef DEBUG
    printf("league_new\n");
#endif

    League new;

    new.name = NULL;
    new.names_file = g_strdup(opt_str("string_opt_player_names_file"));
    new.sid = NULL;
    new.short_name = NULL;
    new.symbol = NULL;

    new.id = (new_id) ? league_id_new : -1;
    new.layer = -1;

    new.average_talent = 0;

    new.prom_rel.elements = g_array_new(FALSE, FALSE, sizeof(PromRelElement));
    new.prom_rel.prom_games = g_array_new(FALSE, FALSE, sizeof(PromGames));

    new.teams = g_array_new(FALSE, FALSE, sizeof(Team));
    new.fixtures = g_array_new(FALSE, FALSE, sizeof(Fixture));
    new.joined_leagues = g_array_new(FALSE, FALSE, sizeof(JoinedLeague));
    new.new_tables = g_array_new(FALSE, FALSE, sizeof(NewTable));
    new.tables = g_array_new(FALSE, FALSE, sizeof(Table));
    new.properties = g_ptr_array_new();
    new.skip_weeks_with = g_ptr_array_new();
    new.rr_breaks = g_array_new(FALSE, FALSE, sizeof(gint));
    new.week_breaks = g_array_new(FALSE, FALSE, sizeof(WeekBreak));

    new.first_week = new.week_gap = 1;
    new.two_match_weeks[0] = g_array_new(FALSE, FALSE, sizeof(gint));
    new.two_match_weeks[1] = g_array_new(FALSE, FALSE, sizeof(gint));
    new.round_robins = 2;
    new.yellow_red = 1000;

    new.stats = stat_league_new("", "");
    
    return new;
}

/**
   Create a new PromRelElement with default values.
   @return The PromRelElement we created.
   @see PromRelElement
*/
PromRelElement
prom_rel_element_new(void)
{
#ifdef DEBUG
    printf("prom_rel_element_new\n");
#endif

    PromRelElement new;

    new.ranks[0] = new.ranks[1] =
        new.from_table = new.num_teams = 0;
    new.dest_sid = NULL;
    new.type = PROM_REL_NONE;

    return new;
}

/**
   Create a new PromGames with default values.
   @see PromGames
*/
PromGames
prom_games_new(void)
{
#ifdef DEBUG
    printf("prom_games_new\n");
#endif

    PromGames new;

    new.dest_sid = NULL;
    new.loser_sid = NULL;
    new.cup_sid = NULL;
    new.number_of_advance = 1;
    new.ranks[0] = new.ranks[1] = 0;

    return new;
}

/** Get the array index of the given league or cup id.
    @param clid The id of the league or cup.
    @return The index in the leagues or cups array. */
gint
league_cup_get_index_from_clid(gint clid)
{
#ifdef DEBUG
    printf("league_cup_get_index_from_clid\n");
#endif

    gint i;
    gint index = -1;

    if(clid < ID_CUP_START)
    {
    for(i=0;i<ligs->len;i++)
        if(lig(i).id == clid)
        {
        index = i;
        break;
        }
    }
    else
    for(i=0;i<cps->len;i++)
        if(cp(i).id == clid)
        {
        index = i;
        break;
        }

    if(index == -1)
    main_exit_program(EXIT_POINTER_NOT_FOUND, 
              "league_cup_get_index_from_clid: couldn't find league or cup with id %d\n", 
              clid);

    return index;
}

/** Return the league pointer belonging to the id.
    @param clid The id we look for.
    @return The league pointer or NULL if failed. */
League*
league_from_clid(gint clid)
{
#ifdef DEBUG
    printf("league_from_clid\n");
#endif

    gint i;

    for(i=0;i<ligs->len;i++)
    if(lig(i).id == clid)
        return &lig(i);

    main_exit_program(EXIT_POINTER_NOT_FOUND, 
              "league_from_clid: didn't find league with id %d\n", 
              clid);

    return NULL;
}

/** Return the id of the next league (or cup if we are
    at the end of the leagues array).
    @param clid The id of the current league or cup. 
    @param count_inactive Whether inactive leagues count. */
gint
league_cup_get_next_clid(gint clid, gboolean count_inactive)
{
#ifdef DEBUG
    printf("league_cup_get_next_clid\n");
#endif

    gint i, return_value = -1;

    if(clid < ID_CUP_START)
    {
    for(i=0;i<ligs->len;i++)
        if(lig(i).id == clid)
        break;

    if(i != ligs->len - 1)
    {
        if(query_league_active(&lig(i + 1)) || count_inactive)
        return_value = lig(i + 1).id;
        else
        return_value = league_cup_get_next_clid(lig(i + 1).id, count_inactive);
    }
    else if(acps->len > 0)
        return_value = acp(0)->id;
    else
    {
        if(query_league_active(&lig(0)) || count_inactive)
        return_value = lig(0).id;
        else
        return_value = league_cup_get_next_clid(lig(0).id, count_inactive);
    }
    }
    else
    {
    for(i=0;i<acps->len;i++)
        if(acp(i)->id == clid)
        break;    

    if(i != acps->len - 1)
        return_value = acp(i + 1)->id;
    else
    {
        if(query_league_active(&lig(0)) || count_inactive)
        return_value = lig(0).id;
        else
        return_value = league_cup_get_next_clid(lig(0).id, count_inactive);
    }
    }

    return return_value;
}


/** Return the id of the previous league or cup.
    @param clid The id of the current league or cup.
    @param count_inactive Whether inactive leagues count. */
gint
league_cup_get_previous_clid(gint clid, gboolean count_inactive)
{
#ifdef DEBUG
    printf("league_cup_get_previous_clid\n");
#endif

    gint i, return_value = -1;

    if(clid < ID_CUP_START)
    {
    for(i=ligs->len - 1;i>=0;i--)
        if(lig(i).id == clid)
        break;

    if(i != 0)
    {
        if(query_league_active(&lig(i - 1)) || count_inactive)
        return_value = lig(i - 1).id;
        else
        return_value = league_cup_get_previous_clid(lig(i - 1).id, count_inactive);
    }
    else if(acps->len > 0)
        return_value = acp(acps->len - 1)->id;
    else
    {
        if(query_league_active(&lig(ligs->len - 1)) || count_inactive)
        return_value = lig(ligs->len - 1).id;
        else
        return_value = league_cup_get_previous_clid(lig(ligs->len - 1).id, count_inactive);
    }
    }
    else
    {
    for(i=acps->len - 1;i>=0;i--)
        if(acp(i)->id == clid)
        break;

    if(i != 0)
        return_value = acp(i - 1)->id;
    else
    {
        if(query_league_active(&lig(ligs->len - 1)) || count_inactive)
        return_value = lig(ligs->len - 1).id;
        else
        return_value = league_cup_get_previous_clid(lig(ligs->len - 1).id, count_inactive);
    }
    }

    return return_value;
}

/** Return the fixture coming after the specified week and round. 
    @param league The league the fixtures of which we examine. 
    @return A fixture pointer or NULL. */
Fixture*
league_cup_get_next_fixture(gint clid, gint week_number, gint week_round_number)
{
#ifdef DEBUG
    printf("league_cup_get_next_fixture\n");
#endif

    gint i;
    GArray *fixtures = league_cup_get_fixtures(clid);

    for(i=0;i<fixtures->len;i++)
    if(g_array_index(fixtures, Fixture, i).week_number > week_number ||
       (g_array_index(fixtures, Fixture, i).week_number == week_number &&
        g_array_index(fixtures, Fixture, i).week_round_number >= week_round_number))
        return &g_array_index(fixtures, Fixture, i);

    return NULL;
}

/** Return the fixture coming just before the specified week and round. 
    @param league The league the fixtures of which we examine. 
    @return A fixture pointer or NULL. */
Fixture*
league_cup_get_previous_fixture(gint clid, gint week_number, gint week_round_number)
{
#ifdef DEBUG
    printf("league_cup_get_previous_fixture\n");
#endif

    gint i;
    GArray *fixtures = league_cup_get_fixtures(clid);

    for(i=fixtures->len - 1;i>=0;i--)
    if(g_array_index(fixtures, Fixture, i).week_number < week_number ||
       (g_array_index(fixtures, Fixture, i).week_number == week_number &&
        g_array_index(fixtures, Fixture, i).week_round_number < week_round_number))
        return &g_array_index(fixtures, Fixture, i);

    return NULL;
}


/** Return the average stadium capacity of cpu teams
    in the specified league or cup. */
gint
league_cup_average_capacity(gint clid)
{
#ifdef DEBUG
    printf("league_cup_average_capacity\n");
#endif

    gint i, cnt = 0;
    gfloat sum = 0;
    const GArray *teams = NULL;
    const GPtrArray *teamsp = NULL;

    if(clid < ID_CUP_START)
    {    
    teams = (GArray*)league_cup_get_teams(clid);
    for(i=0;i<teams->len;i++)
        sum += g_array_index(teams, Team, i).stadium.capacity;
        cnt++;
    }
    else
    {
    teamsp = (GPtrArray*)league_cup_get_teams(clid);
    for(i=0;i<teamsp->len;i++)
        sum += ((Team*)g_ptr_array_index(teamsp, i))->stadium.capacity;
        cnt++;
    }
    
    return sum / (gfloat)cnt;
}

/** Get the index of the league with the specified string id. */
gint
league_index_from_sid(const gchar *sid)
{
#ifdef DEBUG
    printf("league_index_from_sid\n");
#endif

    gint i;

    for(i=0;i<ligs->len;i++)
    if(strcmp(lig(i).sid, sid) == 0)
        return i;

    main_exit_program(EXIT_INT_NOT_FOUND, 
              "league_index_from_sid: no index found for sid '%s'.\n", sid);

    return -1;
}

/** Remove the team with the specified id from the teams
    array without freeing the memory (used in promotion/relegation). */
void
league_remove_team_with_id(League *league, gint id)
{
#ifdef DEBUG
    printf("league_remove_team_with_id\n");
#endif

    gint i;

    for(i=0;i<league->teams->len;i++)
    if(g_array_index(league->teams, Team, i).id == id)
    {
        g_array_remove_index(league->teams, i);
        return;
    }

    debug_print_message("league_remove_team_with_id: team with id %d in league %s not found\n",
          id, league->name);
}




/** Nullify league stuff at the beginning of a season. */
void
league_season_start(League *league)
{
#ifdef DEBUG
    printf("league_season_start\n");
#endif

    gint i, j;
    gint idx = league_index_from_sid(league->sid);
    gboolean user_champ = 
    (team_is_user(
        team_of_id(
        g_array_index(league_table((&lig(0)))->elements, TableElement, 0).team_id)) != -1);
    gboolean league_above_talent =
    (team_get_average_talents(league->teams) > league->average_talent *
     const_float("float_season_end_league_above_talent_factor") && !user_champ);
    gboolean team_is_top = FALSE;
    Team *tm = NULL;
    
    gfloat team_change_lower = 
    const_float("float_season_end_team_change_lower"),
    user_champ_addition = 
    const_float("float_season_end_user_champ_addition"),
    user_champ_best_teams_addition = 
    const_float("float_season_end_user_champ_best_teams_addition"),
    team_change_upper = 
    const_float("float_season_end_team_change_upper");

    gint user_champ_best_teams_limit =
    const_int("int_season_end_user_champ_best_teams_limit");

    gfloat team_change_factor = 0;

    if(user_champ)
    {
    tm = team_of_id(
        g_array_index(league_table((&lig(0)))->elements, TableElement, 0).team_id);
    tm->luck = MAX(tm->luck * const_float("float_season_end_user_champ_luck_factor"),
               const_float("float_luck_limit"));
    }
    else
    for(i=0;i<users->len;i++)
        if(usr(i).tm->luck < 1)
        usr(i).tm->luck = 
            MIN(usr(i).tm->luck * const_float("float_season_end_user_champ_luck_factor_regen"), 1);

    /** Reset tables */
    for(i = league->tables->len - 1; i >= 0; i--)
    {
        g_array_free(g_array_index(league->tables, Table, i).elements, TRUE);
        g_array_remove_index(league->tables, i);
    }
    league_add_table(league);

    /*d*/
/*     if(league == &lig(0)) */
/*     printf("league %s av %.2f ab %d\n", league->name, */
/*            team_get_average_talents(league->teams), league_above_talent); */

    for(i=0;i<league->teams->len;i++)
    {
    team_is_top = 
        (team_get_league_rank(&g_array_index(league->teams, Team, i), -1) <= 
         user_champ_best_teams_limit && idx == 0 && user_champ);

    team_change_factor = 
        (team_is_user(&g_array_index(league->teams, Team, i)) == -1) *
        math_rnd(team_change_lower + user_champ * user_champ_addition +
             team_is_top * user_champ_best_teams_addition -
             league_above_talent * (user_champ_addition / 2),
             team_change_upper + user_champ * user_champ_addition +
             team_is_top * user_champ_best_teams_addition -
             league_above_talent * (user_champ_addition / 2));

    for(j=0;j<g_array_index(league->teams, Team, i).players->len;j++)
        player_season_start(
        &g_array_index(
            g_array_index(
            league->teams, Team, i).players, Player, j), team_change_factor);

    g_array_index(league->teams, Team, i).stadium.average_attendance =
        g_array_index(league->teams, Team, i).stadium.games = 
        g_array_index(league->teams, Team, i).stadium.possible_attendance = 0;
    }

/*     if(league == &lig(0)) */
/*     printf("2 league %s av %.2f\n", league->name, */
/*            team_get_average_talents(league->teams)); */
}

/** Find out whether the team with specified rank in the league
    would participate in promotion games were the season to end. */
gboolean
query_league_rank_in_prom_games(const League *league, gint rank)
{
#ifdef DEBUG
    printf("query_league_rank_in_prom_games\n");
#endif

    gint i, j, k, l;
    const Cup *cup = NULL;
    const CupRound *cup_round = NULL;

    for(i=0;i<ligs->len;i++)
    if(query_league_has_prom_games((&lig(i))))
    {
            for(l = 0; l < lig(i).prom_rel.prom_games->len; l++)
            {
                cup = cup_from_sid(g_array_index(lig(i).prom_rel.prom_games, PromGames, l).cup_sid);
                for(k=0;k<cup->rounds->len;k++)
                {
                    cup_round = &g_array_index(cup->rounds, CupRound, k);
                    for(j=0;j<cup_round->choose_teams->len;j++)
                    {
                        if(strcmp(g_array_index(cup_round->choose_teams, CupChooseTeam, j).sid,
                                  league->sid) == 0 &&
                           ((rank >= g_array_index(cup_round->choose_teams,
                                                   CupChooseTeam, j).start_idx &&
                             rank <= g_array_index(cup_round->choose_teams, 
                                                   CupChooseTeam, j).end_idx && 
                             g_array_index(cup_round->choose_teams, 
                                           CupChooseTeam, j).randomly) ||
                            (rank >= g_array_index(cup_round->choose_teams, 
                                                   CupChooseTeam, j).start_idx &&
                             rank < g_array_index(cup_round->choose_teams, 
                                                  CupChooseTeam, j).start_idx + 
                             g_array_index(cup_round->choose_teams, 
                                           CupChooseTeam, j).number_of_teams &&
                             !g_array_index(cup_round->choose_teams, 
                                            CupChooseTeam, j).randomly)))
                            return TRUE;
                    }
                }                
            }
    }

    return FALSE;
}

/** Find out whether there are/were league matches. */
gboolean
query_league_matches_in_week(const League *league, gint week_number)
{
#ifdef DEBUG
    printf("query_league_matches_in_week\n");
#endif

    gint i;

    for(i=0;i<league->fixtures->len;i++)
    if(g_array_index(league->fixtures, Fixture, i).week_number == week_number)
        return TRUE;

    return FALSE;
}

static gint
league_get_max_movements(const League *league, enum PromRelType type)
{
    unsigned i;
    const GArray *elements = league->prom_rel.elements;
    unsigned max = 0;
    for (i = 0; i < elements->len; i++) {
        const PromRelElement *element = &g_array_index(elements,
                                                       PromRelElement, i);
        if (element->type != type)
            continue;
        max+= element->num_teams;
    }

    for (i = 0; i < league->prom_rel.prom_games->len; i++) {
        const PromGames *prom_games =
            &g_array_index(league->prom_rel.prom_games, PromGames, i);
        const Cup *cup;
        const GPtrArray *teams;
        if (type == PROM_REL_PROMOTION) {
            max += prom_games->number_of_advance;
            continue;
        }

        /* PROM_REL_RELEGATION */
        cup = cup_from_sid(prom_games->cup_sid);
        teams = cup_get_teams_sorted(cup);
        max += (teams->len - prom_games->number_of_advance);
    }

    return max;
}

/**
 * This function will handle the case were a reserve team needs to be
 * relegated, because it's first team or a higher reserve team has been
 * relegated into its division.  For Example:
 *
 * League 1:
 * Blue
 * Red
 * ------
 * Green [Relegated]
 *
 * League 2:
 * Orange
 * Green B [Needs to be relegated since first team is joining League 2]
 * ------
 * Red B
 *
 */
static void
handle_required_reserve_relegation(const TeamMove *move, GArray *team_movements)
{
    gint i, j;
    for (i = 0; i < move->dest_idcs->len; i++) {
        gint idx = g_array_index(move->dest_idcs, gint, i);
        const League *dest_league = &g_array_index(country.leagues,
                                                   League, idx);
        /* If the destination league (league 2 in example above) allows
         * multiple reserve teams than we don't need to relegate reserve
         * teams (Green B in example above) that are already in it. */
        if (league_allows_multiple_reserve_teams(dest_league))
                continue;

        /* Search through the destination league (League 2 in example above)
         * to see if there are any teams with the same first team of the
         * team (Green in the example above) we are relegating. */
        for (j = 0; j < dest_league->teams->len; j++) {
            gint k;
            TeamMove new_move;
            const Team *reserve_team = &g_array_index(dest_league->teams,
                                                      Team, j);
            if (move->tm.first_team_id != reserve_team->first_team_id)
                continue; /* Teams do not share the same first team. */

            /* We have found a reserve team (Green B in the example above)
             * that shares a first team with the team we are relegating (Green
             * in the example above).  So no we need to also relegate this
             * reserve team. */
            new_move.tm = *reserve_team;
            new_move.prom_rel_type = PROM_REL_RELEGATION;
            new_move.dest_assigned = FALSE;

            /* Search for a place to insert the new relegation into
             * team_movements, we want it to be added at the end of this
             * leagues relegation list so it is preferred over the other
             * relegations. Also, if we were already planning to relegate
             * this team, delete the relegation. */
            for (k = team_movements->len - 1; k >= 0; k--) {
                const TeamMove *move = &g_array_index(team_movements, TeamMove, k);
                if (move->prom_rel_type != PROM_REL_RELEGATION)
                    continue;
                if (move->tm.id == reserve_team->id) {
                    /* Found another relegation for this team, so we need to
                     * remove it. */
                    g_array_remove_index(team_movements, k);
                    continue;
                }

                if (move->tm.clid == reserve_team->clid) {
                    /* We have found the last relegation for this league, so
                     * insert the new_move here. */
                    gint insert_index = k + 1;
#ifdef GLIB_VERSION_2_62
                    new_move.dest_idcs = g_array_copy(move->dest_idcs);
#else
                    new_move.dest_idcs = g_array_sized_new(FALSE, FALSE,
                                                           sizeof(gint),
                                                           move->dest_idcs->len);
                    g_array_append_vals(new_move.dest_idcs,
                                        move->dest_idcs->data,
                                        move->dest_idcs->len);
#endif

                    if(debug > 70) {
                        printf("Adding relegation of %s\n", reserve_team->name);
                    }

                    if (insert_index == team_movements->len)
                        g_array_append_val(team_movements, new_move);
                    else
                        g_array_insert_val(team_movements, insert_index, new_move);
                    break;
                }
            }

            /* TODO: There is one case we aren't handling yet, and that is
             * where a reserve team (Green B in the example above), needs to
             * be relegated becasue its first team (Green in the example above)
             * is relegated to its league *and* the league the reserve team
             * (Green B) is being relegated to has no teams eligible for
             * promotion for example:
             *
             * League 3:
             * Green C
             * Orange B
             * Red C
             *
             * None of theses teams would be elegible, since they all have
             * first teams in League 2.  So, if we were to relegated (Green B),
             * then we'd end up with an extra team in League 3.
             * I'm not sure how to handle this, so for now, when this scenario
             * happens, we just league the reserve team (Green B) in the league
             * with its first team (Green). */
        }
    }
}

static gboolean
league_team_is_top_reserve_team(const League *league, const Team *team)
{
    gint reserve_level, i;

    for (i = 0; i < league->teams->len; i++) {
        gint other_level;
        const Team *other_team = &g_array_index(league->teams, Team, i);
        if (team->first_team_id != other_team->first_team_id)
            continue;

        if (team->reserve_level > other_team->reserve_level)
            return FALSE;
    }
    return TRUE;
}

static void
get_next_movement_search(gint *current_layer, enum PromRelType *type)
{
    if (*type == PROM_REL_PROMOTION) {
        *current_layer = *current_layer - 1;
        *type = PROM_REL_RELEGATION;
    } else if (*type == PROM_REL_RELEGATION) {
        *current_layer = *current_layer + 2;
        *type = PROM_REL_PROMOTION;
   }
}

static void
country_filter_promotions(const Country *country, GArray *team_movements,
                          GArray *move_summaries, gint current_layer)
{
    gint i = 0, j;
    while (i < team_movements->len) {
        gint additions = 0;
        const TeamMove *move = &g_array_index(team_movements, TeamMove, i++);
        const League *league = league_from_clid(move->tm.clid);
        gint league_idx = league_index_from_sid(league->sid);
        MoveSummary *summary = &g_array_index(move_summaries, MoveSummary, league_idx);
        gint dest_idx;
        gpointer hash_value;

        if (move->prom_rel_type != PROM_REL_PROMOTION)
            continue;

        if (league->layer != current_layer)
            continue;

        if(debug > 70) {
            gint dest_idx = g_array_index(move->dest_idcs, gint, 0);
            const League *dest_league = &g_array_index(country->leagues,
                                                       League, dest_idx);

            printf("Looking at promotion of %s from %s to %s.\n", move->tm.name,
                   league->name, dest_league->name);
        }
        if (summary->max_promotions && summary->num_promotions_from == summary->max_promotions)
            goto remove_promotion;

        /* If there are other reserve teams with the same first team that
         * are a higher level, then we can't promote this team. */
        if (!league_team_is_top_reserve_team(league, &move->tm))
            goto remove_promotion;

        for (j = 0; j < move->dest_idcs->len; j++) {
            gint idx = g_array_index(move->dest_idcs, gint, j);
            const League *dest_league = &g_array_index(country->leagues,
                                                       League, idx);
            if (!league_can_accept_promoted_team(dest_league, &move->tm, team_movements))
                break;
        }
        if (j < move->dest_idcs->len) {
            /* This means we found a destination league that we can't promote to,
             * so we can't promote this team. */
            goto remove_promotion;
        }

        /* We can promote this team */
        /* FIXME: How to handle multiple dest_idcs */
        dest_idx = g_array_index(move->dest_idcs, gint, 0);
        summary->num_promotions_from++;
        g_array_index(move_summaries, MoveSummary, dest_idx).num_promotions_to++;

        continue;
remove_promotion:
        if(debug > 70) {
	        printf("Removing the promotion.\n");
        }
        i--;
        g_array_remove_index(team_movements, i);
    }
}
static void
country_filter_relegations(const Country *country, GArray *team_movements,
                           GArray *move_summaries, gint current_layer)
{
    gint i = team_movements->len - 1;
    while (i >= 0) {
        const TeamMove *move = &g_array_index(team_movements, TeamMove, i--);
        const League *league = league_from_clid(move->tm.clid);
        gint league_idx = league_index_from_sid(league->sid);
        MoveSummary *summary = &g_array_index(move_summaries, MoveSummary, league_idx);
        const Team *first_team;
        gint reserve_level;
        gint j;

        if (move->prom_rel_type != PROM_REL_RELEGATION)
            continue;

        if (league->layer != current_layer)
            continue;

        if(debug > 70) {
            gint dest_idx = g_array_index(move->dest_idcs, gint, 0);
            const League *dest_league = &g_array_index(country->leagues,
                                                       League, dest_idx);

            printf("Looking at relegation of %s from %s to %s.\n", move->tm.name,
                   league->name, dest_league->name);
        }
        if (summary->num_relegations_from == summary->num_promotions_to) {
            if(debug > 70)
                printf("Removing the relegation.\n");
            /* We can't relegate this team */
            g_array_remove_index(team_movements, i + 1);
            continue;
        }

        summary->num_relegations_from++;
    }

    /* Next check if any of our relegations would cause a lower level reserve
     * team to also be relegated. */
    for (i = 0; i < team_movements->len; i++) {
        const TeamMove *move = &g_array_index(team_movements, TeamMove, i);
        const League *league = league_from_clid(move->tm.clid);
        if (move->prom_rel_type != PROM_REL_RELEGATION)
            continue;
        if (league->layer != current_layer)
            continue;
        handle_required_reserve_relegation(move, team_movements);
    }
}

/**
 * Return the maximum league layer for \p country.  This function does not
 * assume that the last league has the  maximum layer even though this is
 * usually the case.
 */
static gint
country_get_max_layer(const Country *country)
{
    gint i;
    gint max_layer = 0;

    for (i = 0; i < country->leagues->len; i++) {
        const League *league = &g_array_index(country->leagues, League, i);
        max_layer = MAX(league->layer, max_layer);
    }
    return max_layer;
}

/** Filter out movemnts that may not be allowed due to league rules,
  * e.g. promotion of reserve teams. */
void
country_apply_reserve_prom_rules(const Country *country, GArray *team_movements)
{
    gint num_promotions;
    GArray *move_summaries;
    gint i = 0, j = 0;
    gint current_layer, max_layer;
    enum PromRelType current_type;

    if (country->reserve_promotion_rules != RESERVE_PROM_RULES_DEFAULT)
        return;

    move_summaries = g_array_sized_new(FALSE, TRUE, sizeof(MoveSummary),
                                       country->leagues->len);
    g_array_set_size(move_summaries, country->leagues->len);

    /* Initialize the move summaries */
    for (i = 0; i < move_summaries->len; i++) {
        const League *league = &g_array_index(country->leagues, League, i);
        MoveSummary *summary = &g_array_index(move_summaries, MoveSummary, i);
        summary->max_promotions = league_get_max_movements(league, PROM_REL_PROMOTION);
    }


    /* We need to analyze the relegations and promotions in a specific order
     * due to the dependencies between relegations in promotions in different
     * leagues.  You can model the dependencies as a graph that looks like this:
     *
     *   -----1-- (2P) --3-----
     *   |                    |
     *  \/                   \/
     * (1R) --4-----------> (3P)--3----> (4P)
     *   |                    |          ^ |
     *   |                    1   ---4---| 1
     *   |                    |  /         |
     *   |                   \/ /         \/
     *   -----2-----------> (2R) --2---> (3R)
     *
     * Where the number is the league layer, and P stands for promotion and R
     * stands for relegation.  The numbers describe why there is a dependeny:
     *
     * 1. Normal promotion / relegation dependency.  The number of relegations
     *    depends on the number of promotions from the lower league.
     *
     * 2. Forced relegation of reserve team when first team is relegated into
     *    its division.  This means that the relegations of a lower league
     *    depend on the relegations of the higher league.
     *
     * 3. A first team being promoted out of a league that allows the reserve
     *    team to be promoted into it.  This means that promotions from a lower
     *    league depend on the promotions from the league abvoe it.
     *
     * 4. A first team being relegated to a lower league at the same time
     *    a reserve team is being promoted into that league.  This means
     *    that promotions from a league depend an relegations from a league
     *    2 levels above it.
     *
     * This graph helps to illistrate the correct in order in which we should
     * determine promotions and relegations in this country so that all
     * dependencies are resolved before we start analyzing promotions and
     * relegations for a specific league.
     *
     * In this case, the correct order is:
     *
     * 2P -> 1R -> 3P -> 2R -> 4P -> 3R
     */

    max_layer = country_get_max_layer(country);
    current_layer = 2;
    current_type = PROM_REL_PROMOTION;

    /* Loop through the team_movements in the order from the comment above.
     * We are making the following assumptions about the list of team_movements:
     * 1. Movements from the same league are listed in order of the teams rank
     *    in the fixtures.
     * 2. The leagues have 'normal' promotions e.g. layer 2 promotes to layer 1
     * and relegates to layer 3, etc.
     *
     * We are not making the assumption that the list is ordered based on the layer
     * of the team's league (even though it likely is).
     */
    do {
        if (current_type == PROM_REL_PROMOTION)
            country_filter_promotions(country, team_movements,
                                      move_summaries, current_layer);
        else if (current_type == PROM_REL_RELEGATION)
            country_filter_relegations(country, team_movements,
                                       move_summaries, current_layer);

        get_next_movement_search(&current_layer, &current_type);
    } while(current_layer <= max_layer);


    /* Next handle relegations. */

    /* Iterate in reverse order to ensure that the lowest ranked teams are
     * preferred for relegation. */

    g_array_unref(move_summaries);
}

/** Add the teams to promote/relegate (from the prom_rel elements)
    from the league to the array. */
void
league_get_team_movements_prom_rel(const League *league, GArray *team_movements)
{
#ifdef DEBUG
    printf("league_get_team_movements_prom_rel\n");
#endif

    gint i, j, k;
    TeamMove new_move;
    const GArray *elements = league->prom_rel.elements;
    PromRelElement *elem;
    GArray *dest_idcs = NULL;
    GPtrArray *dest_sids = NULL;

    for(i=0;i<elements->len;i++)
    {
        elem = &g_array_index(elements, PromRelElement, i);
    dest_sids = misc_separate_strings(
        elem->dest_sid);
    gint dest_idcs_int[dest_sids->len];
    gint dest_idcs_order[dest_sids->len];

    for(j=0;j<dest_sids->len;j++)
        dest_idcs_int[j] = 
        league_index_from_sid((gchar*)g_ptr_array_index(dest_sids, j));

    for(j=elem->ranks[0];
        j<=elem->ranks[1]; j++)
    {
        dest_idcs = g_array_new(FALSE, FALSE, sizeof(gint));
        math_generate_permutation(dest_idcs_order, 0, dest_sids->len - 1);

        for(k=0;k<dest_sids->len;k++)        
        g_array_append_val(dest_idcs, dest_idcs_int[dest_idcs_order[k]]);
        
        new_move.tm = *(g_array_index(g_array_index(league->tables, Table, elem->from_table).elements, TableElement, j - 1).team);
        new_move.prom_rel_type = elem->type;
        new_move.dest_idcs = dest_idcs;
        new_move.dest_assigned = FALSE;
        g_array_append_val(team_movements, new_move);
    }

    free_gchar_array(&dest_sids);
    }
}


/** Add the team movements from the promotion games
    to the array. */
void
league_get_team_movements_prom_games(const League *league, const PromGames *prom_games,
                                     GArray *team_movements, const GPtrArray *prom_games_teams,
                                     gboolean up)
{
#ifdef DEBUG
    printf("league_get_team_movements_prom_games\n");
#endif

    gint i, j;
    TeamMove new_move;
    GPtrArray *dest_sids = (up) ?
    misc_separate_strings(prom_games->dest_sid) :
    misc_separate_strings(prom_games->loser_sid);
    GArray *dest_idcs = NULL;
    gint dest_idcs_int[dest_sids->len];
    gint dest_idcs_order[dest_sids->len];
    gint start_idx = prom_games->ranks[0] - 1,
    end_idx = prom_games->ranks[1];
    gint prom_type = PROM_REL_PROMOTION;

    if(!up)
    {
    start_idx = prom_games->number_of_advance;
    end_idx = prom_games_teams->len;
    prom_type = PROM_REL_RELEGATION;
    }

    for(i=0;i<dest_sids->len;i++)
    dest_idcs_int[i] = 
        league_index_from_sid((gchar*)g_ptr_array_index(dest_sids, i));

    for(i=start_idx;i<end_idx;i++)
    {        
    dest_idcs = g_array_new(FALSE, FALSE, sizeof(gint));
    math_generate_permutation(dest_idcs_order, 0, dest_sids->len - 1);
    for(j=0;j<dest_sids->len;j++)
        g_array_append_val(dest_idcs, dest_idcs_int[dest_idcs_order[j]]);

    new_move.tm = *((Team*)g_ptr_array_index(prom_games_teams, i));
    new_move.prom_rel_type = prom_type;
    new_move.dest_idcs = dest_idcs;
    new_move.dest_assigned = FALSE;
    g_array_append_val(team_movements, new_move);
    }

    free_gchar_array(&dest_sids);
}


/** Add the teams to promote/relegate from the league to the array
    and remove them from the leagues. */
void
league_get_team_movements(League *league, GArray *team_movements)
{
#ifdef DEBUG
    printf("league_get_team_movements\n");
#endif

    gint i;
    GPtrArray *prom_games_teams = NULL;
    const Cup *prom_cup = NULL;

    league_get_team_movements_prom_rel(league, team_movements);

    if(query_league_has_prom_games(league))
    {
        for(i = 0; i < league->prom_rel.prom_games->len; i++)
        {
            prom_cup = cup_from_sid(g_array_index(league->prom_rel.prom_games, PromGames, i).cup_sid);
    
            prom_games_teams = cup_get_teams_sorted(prom_cup);

            league_get_team_movements_prom_games(league, &g_array_index(league->prom_rel.prom_games, PromGames, i),
                                                 team_movements, prom_games_teams, TRUE);

            if(g_array_index(league->prom_rel.prom_games, PromGames, i).loser_sid != NULL)
                league_get_team_movements_prom_games(league, &g_array_index(league->prom_rel.prom_games, PromGames, i),
                                                     team_movements, prom_games_teams, FALSE);
    
            g_ptr_array_free(prom_games_teams, TRUE);
        }
    }

    g_array_sort_with_data(league->teams, team_compare_func,
               GINT_TO_POINTER(TEAM_COMPARE_LEAGUE_RANK + 100));
}

/** Find out whether there are unassigned TeamMoves in the array. */
gboolean
query_league_team_movements_unassigned(const GArray *team_movements)
{
#ifdef DEBUG
    printf("query_league_team_movements_unassigned\n");
#endif

    gint i;

    for(i=0;i<team_movements->len;i++)
    if(!g_array_index(team_movements, TeamMove, i).dest_assigned)
        return TRUE;

    return FALSE;
}

/** Find out whether there are unassigned team moves with a single
    destination value. */
gboolean
query_league_team_movements_unassigned_single(const GArray *team_movements)
{
#ifdef DEBUG
    printf("query_league_team_movements_unassigned_single\n");
#endif

    gint i;

    for(i=0;i<team_movements->len;i++)
    if(!g_array_index(team_movements, TeamMove, i).dest_assigned &&
       g_array_index(team_movements, TeamMove, i).dest_idcs->len == 1)
        return TRUE;

    return FALSE;
}

/** Print out the movements to the console (debug function). */
void
league_team_movements_print(const GArray *team_movements, 
                const gint *league_size, const gint *league_cur_size)
{
#ifdef DEBUG
    printf("league_team_movements_print\n");
#endif

    gint i, j;
    const TeamMove *tmove = NULL;

    g_print("%-25s Dest \t\t Possible\n\n", "Team");
    for(i=0;i<team_movements->len;i++)
    {
    tmove = &g_array_index(team_movements, TeamMove, i);
    if(tmove->dest_assigned)
        g_print("%-25s (%d) %s \t\t", tmove->tm.name,
           league_from_clid(tmove->tm.clid)->layer,
           lig(g_array_index(tmove->dest_idcs, gint, 0)).name);
    else
        g_print("%-25s (%d) UNASSIGNED \t\t", tmove->tm.name,
           league_from_clid(tmove->tm.clid)->layer);
    for(j=0;j<tmove->dest_idcs->len;j++)
        g_print("%d ", g_array_index(tmove->dest_idcs, gint, j));
    g_print("\n");
    }

    g_print("%-20s Size Cursize\n", "League");
    for(i=0;i<ligs->len;i++)
    g_print("%-20s %d %d\n", lig(i).name, league_size[i],
           league_cur_size[i]);
}

/** Compare two leagues when doing promotion/relegation. The league
    that has fewer teams is preferred. */
gint
league_team_movements_compare_dest_idcs(gconstpointer a, gconstpointer b, 
                    gpointer data)
{
#ifdef DEBUG
    printf("league_team_movements_compare_dest_idcs\n");
#endif

    gint league_idx1 = *(gint*)a,
    league_idx2 = *(gint*)b;
    const gint *league_cur_size = (const gint*)data;

    if(league_cur_size[league_idx1] < league_cur_size[league_idx2])
    return -1;
    else if(league_cur_size[league_idx1] > league_cur_size[league_idx2])
    return 1;

    return 0;
}

/** Assign a random destination for the team move with given index
    and remove the destination from all other unassigned moves if
    the dest league is full. */
void
league_team_movements_assign_dest(GArray *team_movements, gint idx,
                  const gint *league_size, gint *league_cur_size)
{
#ifdef DEBUG
    printf("league_team_movements_assign_dest\n");
#endif

    gint i, j, dest_idx;
    TeamMove *tmove = &g_array_index(team_movements, TeamMove, idx);

    if(debug > 60)
    g_print("league_team_movements_assign_dest %s\n", tmove->tm.name);

    if(tmove->dest_idcs->len == 1)
    dest_idx = g_array_index(tmove->dest_idcs, gint, 0);
    else
    {
    g_array_sort_with_data(tmove->dest_idcs,
                   (GCompareDataFunc)league_team_movements_compare_dest_idcs,
                   (gpointer)league_cur_size);
    dest_idx = g_array_index(tmove->dest_idcs, gint, 0);
    }

    league_cur_size[dest_idx]++;

    if(league_cur_size[dest_idx] > league_size[dest_idx])
    main_exit_program(EXIT_PROM_REL, 
              "league_team_movements_assign_dest: no room in league %s for team %s.",
              lig(dest_idx).name, tmove->tm.name);

    tmove->dest_assigned = TRUE;

    if(debug > 60)
    g_print("%s  %d -> %d\n", tmove->tm.name,
        league_from_clid(tmove->tm.clid)->layer,
        league_from_clid(lig(dest_idx).id)->layer);

    if(league_cur_size[dest_idx] == league_size[dest_idx])
    for(i=0;i<team_movements->len;i++)
    {
        if(!g_array_index(team_movements, TeamMove, i).dest_assigned)
        {
        tmove = &g_array_index(team_movements, TeamMove, i);
        for(j=tmove->dest_idcs->len - 1; j>= 0; j--)
            if(g_array_index(tmove->dest_idcs, gint, j) == dest_idx)
            g_array_remove_index(tmove->dest_idcs, j);

        if(tmove->dest_idcs->len == 0)
            main_exit_program(EXIT_PROM_REL, 
                      "league_team_movements_assign_dest: no destinations left for team %s.",
                      tmove->tm.name);
        }
    }

    if(team_is_user(&tmove->tm) != -1)
    {
    if(tmove->prom_rel_type == PROM_REL_PROMOTION)
        user_history_add(&usr(team_is_user(&tmove->tm)),
                 USER_HISTORY_PROMOTED, tmove->tm.name,
                 lig(g_array_index(tmove->dest_idcs, gint, 0)).name,
                 NULL, NULL);
    else
        user_history_add(&usr(team_is_user(&tmove->tm)),
                 USER_HISTORY_RELEGATED, tmove->tm.name,
                 lig(g_array_index(tmove->dest_idcs, gint, 0)).name,
                 NULL, NULL);
    }
}

/** Assign all unassigned TeamMoves in the array that only have one
    destination value.
    @param league_size The size of all leagues (i.e. how many teams they
    should contain after promotion/relegation is finished).
    @param league_cur_size The current size of all leagues (taking into
    account the assigned team moves). */
void
league_team_movements_prune(GArray *team_movements, const gint *league_size,
                gint *league_cur_size)
{
#ifdef DEBUG
    printf("league_team_movements_prune\n");
#endif

    gint i;

    if(debug > 60)
    g_print("league_team_movements_prune\n");

    while(query_league_team_movements_unassigned_single(team_movements))
    {
    for(i=0;i<team_movements->len;i++)
        if(!g_array_index(team_movements, TeamMove, i).dest_assigned &&
           g_array_index(team_movements, TeamMove, i).dest_idcs->len == 1)
        league_team_movements_assign_dest(team_movements, i, 
                          league_size, league_cur_size);
    }
}

/** Try to assign destinations for the team movements based on the
    array of possible destinations.
    @param league_size The size of all leagues (i.e. how many teams they
    should contain after promotion/relegation is finished). */
void
league_team_movements_destinations(GArray *team_movements, const gint *league_size)
{
#ifdef DEBUG
    printf("league_team_movements_destinations\n");
#endif

    gint i;
    gint league_cur_size[ligs->len];

    if(debug > 60)
    g_print("league_team_movements_destinations\n");

    for(i=0;i<ligs->len;i++)
    league_cur_size[i] = lig(i).teams->len;

    if(debug > 65)
    league_team_movements_print(team_movements, league_size, league_cur_size);

    while(query_league_team_movements_unassigned(team_movements))
    {
    league_team_movements_prune(team_movements, league_size, league_cur_size);
    for(i=0;i<team_movements->len;i++)
        if(!g_array_index(team_movements, TeamMove, i).dest_assigned)
        league_team_movements_assign_dest(team_movements, i, 
                          league_size, league_cur_size);
    }

    if(debug > 65)
    league_team_movements_print(team_movements, league_size, league_cur_size);
}

/** Find out whether there are any active leagues in the country. */
gboolean
query_leagues_active_in_country(void)
{
#ifdef DEBUG
    printf("query_leagues_active_in_country\n");
#endif

    gint i;

    for(i=0;i<ligs->len;i++)
    if(query_league_active(&lig(i)))
        return TRUE;

    return FALSE;
}

/** Find out whether a given matchday should occur in the same week
    as the one before or a full week later. */
gboolean
query_league_cup_matchday_in_two_match_week(GArray **two_match_weeks, gint matchday)
{
#ifdef DEBUG
    printf("query_league_cup_matchday_in_two_match_week\n");
#endif

    gint i;

    for(i=0;i<two_match_weeks[0]->len;i++)
    {
    if(g_array_index(two_match_weeks[0], gint, i) < matchday &&
       matchday <= g_array_index(two_match_weeks[1], gint, i) &&
       (matchday - g_array_index(two_match_weeks[0], gint, i)) % 2 == 1)
        return TRUE;
    }

    return FALSE;
}

/** Add a new table to the league tables if specified
    in the new_tables array. */
void
league_check_new_tables(League *league)
{
#ifdef DEBUG
    printf("league_check_new_tables\n");
#endif

    gint i;
    Table new_table;

    for(i = 0; i < league->new_tables->len; i++)
        if(g_array_index(league->new_tables, NewTable, i).add_week == week)
        {
            /** Create cumulative table if necessary. */
            if(league->tables->len == 1 && week > 1)
            {
                new_table = table_copy(league_table(league));
                g_array_append_val(league->tables, new_table);
            }

            league_add_table(league);
            misc_string_assign(&league_table(league)->name,
                               g_array_index(league->new_tables, NewTable, i).name);
        }
}

/** Add an initialized table to the league. */
void
league_add_table(League *league)
{
#ifdef DEBUG
    printf("league_add_table\n");
#endif

    gint i;
    Table new_table;
    TableElement new_table_element;
 
    new_table = table_new();
    new_table.clid = league->id;
    new_table.name = g_strdup(league->name);

    for(i = 0; i < league->teams->len; i++)
    {
        new_table_element = 
            table_element_new(
                &g_array_index(league->teams, Team, i), i);
        g_array_append_val(new_table.elements, new_table_element);
    }

    g_array_append_val(league->tables, new_table);
}

gboolean
query_league_cup_has_property(gint clid, const gchar *property)
{
    const GPtrArray *properties = league_cup_get_properties(clid);
    return query_misc_string_in_array(property, properties);
}

GPtrArray*
league_cup_get_teams(gint clid)
{
    return (clid < ID_CUP_START) ?
    (GPtrArray*)league_from_clid(clid)->teams :
    cup_from_clid(clid)->teams;
}

GPtrArray*
league_cup_get_properties(gint clid)
{
    return (clid < ID_CUP_START) ? 
    league_from_clid(clid)->properties :
    cup_from_clid(clid)->properties;
}

/** Synchronise the number of league breaks with the number of 
    round robins in the league. */
void
league_cup_adjust_rr_breaks(GArray *rr_breaks, gint round_robins, gint week_gap)
{
#ifdef DEBUG
    printf("league_cup_adjust_rr_breaks\n");
#endif

    gint i;
    gint default_break;

    /* Remove superfluous breaks. */
    for(i = rr_breaks->len - 1; i >= round_robins - 1; i--)
        g_array_remove_index(rr_breaks, i);

    /* Add more breaks if necessary. */
    if(rr_breaks->len == 0)
        default_break = week_gap;
    else
        default_break = g_array_index(rr_breaks, gint, rr_breaks->len - 1);

    for(i = rr_breaks->len; i < round_robins - 1; i++)
        g_array_append_val(rr_breaks, default_break);
}

/** Fill the breaks array from a comma separated string of integers. */
void
league_cup_fill_rr_breaks(GArray *rr_breaks, const gchar *breaks)
{
#ifdef DEBUG
    printf("league_cup_fill_rr_breaks\n");
#endif

    gint i = 0;
    gchar **breaks_arr = g_strsplit(breaks, ",", 0);
    gint new_break;

    while(breaks_arr[i] != NULL)
    {
        new_break = (gint)g_ascii_strtod(breaks_arr[i], NULL);
        g_array_append_val(rr_breaks, new_break);
        i++;
    }

    g_strfreev(breaks_arr);
}

/** Set the values of the week breaks to the week gap of the league or
    cup if necessary. */
void
league_cup_adjust_week_breaks(GArray *week_breaks, gint week_gap)
{
    gint i;

    for(i = 0; i < week_breaks->len; i++)
        if(g_array_index(week_breaks, WeekBreak, i).length == -1000)
            g_array_index(week_breaks, WeekBreak, i).length = week_gap;
}

/** Return the week number with a possible schedule break adjustment. */
gint
league_cup_get_week_with_break(gint clid, gint week_number)
{
    gint i;
    const GArray *week_breaks;

    week_breaks = (clid >= ID_CUP_START) ?
        cup_from_clid(clid)->week_breaks :
        league_from_clid(clid)->week_breaks;

    for(i = 0; i < week_breaks->len; i++)
        if(g_array_index(week_breaks, WeekBreak, i).week_number == week_number)
            return week_number + g_array_index(week_breaks, WeekBreak, i).length;            

    return week_number;
}

/** Lookup the first_team_id from the first_team_sid for each team in the
 * country.  This will allow faster lookups of the first team.  This needs to
 * be called after both xml_country_read() and team_generate_players_stadium()
 * have been called, since those functions are what complete loads all the teams
 * in a country. */
void
country_lookup_first_team_ids(const Country *country)
{
    gint i,j;
    for (i = 0; i < country->leagues->len; i++) {
        const League *league = &g_array_index(country->leagues, League, i);
        for (j = 0; j < league->teams->len; j++) {
            Team *team = &g_array_index(league->teams, Team, j);
            if (!team_is_reserve_team(team)) {
                team->first_team_id = team->id;
                continue;
            }

            /* Check if we have already computed the first_team ID */
            if (team->first_team_id != 0)
                continue;

            const Team *first_team = team_of_sid(team->first_team_sid, country);
            team->first_team_id = first_team->id;
        }
    }
}

gboolean
league_can_accept_promoted_team(const League *league, const Team *tm,
                                const GArray *team_movements)
{
    gint i, j;

    /* Reserve teams are currently the only kind of teams that have restrictions
     * around promotion. */
    if (!team_is_reserve_team(tm))
        return TRUE;

    if (!league_allows_reserve_teams(league))
        return FALSE;

    /* If the first team or a higher reserve team is in the league, then we
     * can't promote the reserve team.
     */
    for (i = 0; i < league->teams->len; i++) {
        gboolean upper_team_promoted = FALSE;
        const Team *upper_team = &g_array_index(league->teams, Team, i);
        if (tm->first_team_id != upper_team->first_team_id)
            continue;

        /* If the upper team is going to be promoted, then it is ok to
         * promote the reserve team.
         * This code assumes than team_movements is ordered so that higher
         * divisions come first and have already been filtered for
         * ineligible promotions. */
        for (j = 0; j < team_movements->len; j++) {
            const TeamMove *move = &g_array_index(team_movements, TeamMove, j);
            if (move->prom_rel_type != PROM_REL_PROMOTION)
                continue;
            if (move->tm.id == upper_team->id) {
                upper_team_promoted = TRUE;
                break;
            }
        }
        /* The upper team has not been promoted. */
        if (!upper_team_promoted)
            return FALSE;
    }

    /* If the first team or a higher reserve team is going to be relegated into
     * this league, then we can't promote this team. */
    for (i = 0; i < team_movements->len; i++) {
        const TeamMove *move = &g_array_index(team_movements, TeamMove, i);
        if (move->prom_rel_type != PROM_REL_RELEGATION)
            continue;

        if (tm->first_team_id != move->tm.first_team_id)
            continue;

        for (j = 0; j < move->dest_idcs->len; j++) {
            gint idx = g_array_index(move->dest_idcs, gint, j);
            const League *dest_league = &g_array_index(country.leagues,
                                                       League, idx);
            if(dest_league->id == league->id)
                return FALSE;
        }
    }

    /* The league has no upper teams, so we can continue. */
    return TRUE;
}

gboolean
league_allows_reserve_teams(const League *league)
{
    return league->layer != 1;
}

gboolean
league_allows_multiple_reserve_teams(const League *league)
{
    return lig(ligs->len - 1).layer == league->layer;
}

/**
 * This is a helper function to return a GPtrArray* of teams in the league.
 * This is the recommended way to get the list of teams in the league, and
 * should be stable across possible internal changes like creating a unique
 * pointer for each team or implmenting alternative backends.
 *
 * @returns A GPtrArray* containing Team* for each team in the league.  The
 *          caller should call g_ptr_array_unref() when done using the pointer.
 */
GPtrArray*
league_get_teams(const League *league)
{
    GPtrArray *teams;
    int i;
    for (i = 0; i < league->teams->len; i++)
    {
        g_ptr_array_add(teams, &g_array_index(league->teams, Team, i));
    }
    return teams;
}
