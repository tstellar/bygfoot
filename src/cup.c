/*
   cup.c

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
#include "fixture.h"
#include "league.h"
#include "main.h"
#include "maths.h"
#include "misc.h"
#include "option.h"
#include "table.h"
#include "team.h"
#include "variables.h"
#include "xml_league.h"

/**
   Create and return a new cup with default values.
   @return The new cup.
   @see #Cup
*/
Cup
cup_new(gboolean new_id)
{
#ifdef DEBUG
    printf("cup_new\n");
#endif

    Cup new;

    new.name = NULL;
    new.short_name = NULL;
    new.symbol = NULL;
    new.sid = NULL;

    new.id = (new_id) ? cup_id_new : -1;
    new.group = -1;
    new.add_week = 0;
    new.last_week = -1;
    new.week_gap = 1;

    new.yellow_red = 1000;
    new.talent_diff = 0;

    new.rounds = g_array_new(FALSE, FALSE, sizeof(CupRound));
    new.teams = g_ptr_array_new();
    new.team_names = g_ptr_array_new();
    new.fixtures = g_array_new(FALSE, FALSE, sizeof(Fixture));
    new.week_breaks = g_array_new(FALSE, FALSE, sizeof(WeekBreak));
    new.skip_weeks_with = g_ptr_array_new();
    new.bye = g_ptr_array_new();

    new.properties = g_ptr_array_new();

    new.next_fixture_update_week = -1;
    new.next_fixture_update_week_round = -1;

    new.history = g_ptr_array_new();
    return new;
}

/** Return a CupChooseTeam with default values. */
CupChooseTeam
cup_choose_team_new(void)
{
#ifdef DEBUG
    printf("cup_choose_team_new\n");
#endif

#ifdef DEBUG
    printf("cup_choose_team_new\n");
#endif

    CupChooseTeam new;

    new.sid = NULL;
    new.number_of_teams = -1;
    new.start_idx = new.end_idx = -1;
    new.randomly = FALSE;
    new.generate = FALSE;
    new.skip_group_check = FALSE;
    new.from_table = 0;
    new.preload = TRUE;
    new.optional = FALSE;
    new.next = NULL;

    return new;
}

/** Return a CupRound with default values. */
CupRound
cup_round_new(void)
{
#ifdef DEBUG
    printf("cup_round_new\n");
#endif

    CupRound new;

    new.name = NULL;
    new.new_teams = 0;
    new.byes = -1;
    new.home_away = TRUE;
    new.replay = 0;
    new.neutral = FALSE;
    new.randomise_teams = TRUE;
    new.delay = 0;
    new.round_robin_number_of_groups = 0;
    new.round_robin_number_of_advance = 0;
    new.round_robin_number_of_best_advance = 0;
    new.round_robins = 2;
    new.rr_breaks = g_array_new(FALSE, FALSE, sizeof(gint));
    new.two_match_weeks[0] = g_array_new(FALSE, FALSE, sizeof(gint));
    new.two_match_weeks[1] = g_array_new(FALSE, FALSE, sizeof(gint));
    new.two_match_week = FALSE;
    new.tables = g_array_new(FALSE, FALSE, sizeof(Table));
    new.choose_teams = g_array_new(FALSE, FALSE, sizeof(CupChooseTeam));
    new.waits = g_array_new(FALSE, FALSE, sizeof(CupRoundWait));
    new.team_ptrs = g_ptr_array_new();

    return new;
}

/** Reset some arrays at the beginning of a new season. */
void
cup_reset(Cup *cup)
{
#ifdef DEBUG
    printf("cup_reset\n");
#endif

    gint i;
    
    free_gchar_array(&cup->team_names);
    cup->team_names = g_ptr_array_new();

    if(cup->teams->len > 0)
    {
        /* Save this season's results. */
        GPtrArray *sorted_teams = cup_get_teams_sorted(cup);
	g_ptr_array_add(cup->history, sorted_teams);
	g_ptr_array_free(cup->teams, TRUE);
	cup->teams = g_ptr_array_new();
    }

    g_array_free(cup->fixtures, TRUE);
    cup->fixtures = g_array_new(FALSE, FALSE, sizeof(Fixture));

    free_g_ptr_array(&cup->bye);

    for(i=0;i<cup->rounds->len;i++)
    {
	CupRound *cup_round = &g_array_index(cup->rounds, CupRound, i);
	if(cup_round->tables->len > 0)
	{
            free_tables(&cup_round->tables);
            cup_round->tables = g_array_new(FALSE, FALSE, sizeof(Table));
	}
	g_ptr_array_free(cup_round->team_ptrs, TRUE);
	cup_round->team_ptrs = g_ptr_array_new();
    }
}

/** Find out whether a choose_team definition refers to a 
    league in the country. If so, no teams get loaded for that
    choose_team. */
gboolean
query_cup_choose_team_is_league(const gchar *sid)
{
#ifdef DEBUG
    printf("query_cup_choose_team_is_league\n");
#endif

    gint i;

    for(i=0;i<ligs->len;i++)
	if(strcmp(lig(i).sid, sid) == 0)
	    return TRUE;

    return FALSE;
}

/** @return TRUE if the team(s) referenced by \p sid need to be
 *          generated.
 */
static gboolean
cup_choose_team_should_generate(const CupChooseTeam *ct)
{
    if (g_str_has_prefix(ct->sid, "LEAGUE") || g_str_has_prefix(ct->sid, "CUP"))
        return FALSE;

    return !country_get_league_sid(&country, ct->sid) &&
           !country_get_cup_sid(&country, ct->sid);
}

gint
cup_choose_team_compute_start_idx(const CupChooseTeam *ct)
{
    if (ct->number_of_teams == -1)
        return 0;

    return ct->start_idx - 1;
}

gint
cup_choose_team_compute_end_idx(const CupChooseTeam *ct, gint num_teams)
{
    if (ct->number_of_teams == -1)
        return num_teams - 1;

    return ct->end_idx - 1;
}

/** Write the cup or league of the chooseteam into the appropriate pointer
    and return TRUE; return FALSE if no cup/league is found. */
void
cup_get_choose_team_league_cup(const CupChooseTeam *ct, 
			       const League **league, const Cup **cup)
{
#ifdef DEBUG
    printf("cup_get_choose_team_league_cup\n");
#endif

    gint i, idx;
    gchar trash[SMALL];
    gchar prefix[SMALL];

    *league = NULL;
    *cup = NULL;

    sscanf(ct->sid, "%[^0-9]%d", trash, &idx);

    if(g_str_has_prefix(ct->sid, "LEAGUE") && idx >=0 && idx <= ligs->len)
    {
	*league = &lig(idx - 1);
	*cup = NULL;
    }
    else if(g_str_has_prefix(ct->sid, "CUP") && idx >=0 && idx <= cps->len)
    {
	*cup = &cp(idx - 1);
	*league = NULL;
    }
    else
    {
        if(g_str_has_suffix(ct->sid, "*"))
            g_utf8_strncpy(prefix, ct->sid, g_utf8_strlen(ct->sid, -1) - 1);
        else
            strcpy(prefix, "NONAME");

	for(i=0;i<ligs->len;i++)
	{
	    if(strcmp(lig(i).sid, ct->sid) == 0 ||
               g_str_has_prefix(lig(i).sid, prefix))
	    {
		*league = &lig(i);
		*cup = NULL;
		break;
	    }
	}

	for(i=0;i<cps->len;i++)
	{
	    if(strcmp(cp(i).sid, ct->sid) == 0 ||
               g_str_has_prefix(cp(i).sid, prefix))
	    {
		*cup = &cp(i);
		*league = NULL;
		break;
	    }
	}
    }

    if(*league == NULL && *cup == NULL)
	main_exit_program(EXIT_CHOOSE_TEAM_ERROR, 
			  "cup_get_choose_team_league_cup: no league nor cup found for chooseteam %s",
			  ct->sid);
    if(debug > 100)
    {
        if(*league == NULL)
            printf("cup_get_choose_team_league_cup: sid %s cup %s\n", ct->sid, (*cup)->name);
        else
            printf("cup_get_choose_team_league_cup: sid %s league %s\n", ct->sid, (*league)->name);   
    }
}

/** Load the pointers to the teams participating in the 
    cup round. If necessary, teams are generated and stored in the teams
    array of the cup round. */
void
cup_get_team_pointers(Cup *cup, gint round, gboolean preload)
{
#ifdef DEBUG
    printf("cup_get_team_pointers\n");
#endif

    gint i, existing_teams;
    CupRound *cup_round = &g_array_index(cup->rounds, CupRound, round);
    GPtrArray *teams = cup_round->team_ptrs;

    if(debug > 60)
	g_print("cup_get_team_pointers %s round %d\n", cup->name, round);

    existing_teams = cup_round->team_ptrs->len;
    
    for(i=0;i<cup_round->choose_teams->len;i++) {
        const CupChooseTeam *choose_team =
	    &g_array_index(cup_round->choose_teams, CupChooseTeam, i);

        if(choose_team->preload == preload)
        {
            if(cup_choose_team_should_generate(choose_team))
                cup_load_choose_team_generate(cup, cup_round, choose_team);
            else
                cup_load_choose_team(cup, teams, choose_team);
        }
    }

    for(i=existing_teams;i<cup_round->team_ptrs->len;i++)
    {
        Team *team = g_ptr_array_index(cup_round->team_ptrs, i);
	g_ptr_array_add(cup->teams, team);
    }

    country_lookup_first_team_ids(&country);

    if(debug > 70)
	for(i=0;i<teams->len;i++)
	    g_print("cup_get_team_pointers: %s (%s) round %d team %d %s (clid %d)\n", 
                    cup->name, cup->sid, round, i,
                    ((Team*)g_ptr_array_index(teams, i))->name,
                    ((Team*)g_ptr_array_index(teams, i))->clid);
}

/** Get the pointers to the teams (already generated, in one of the leagues or cups)
    specified in the chooseteam. Add them to the 'teams' pointer array. */
void
cup_load_choose_team(Cup *cup, GPtrArray *teams, const CupChooseTeam *ct)
{
#ifdef DEBUG
    printf("cup_load_choose_team\n");
#endif

    gint i;
    gint debug_num = teams->len;
    const League *league = NULL;
    const Cup *cup_temp = NULL;

    if(debug > 60)
	g_print("cup_load_choose_team: %s, %s, teams %d to %d, random: %d \n", cup->name,
                ct->sid, ct->start_idx, ct->end_idx, ct->randomly);

    cup_get_choose_team_league_cup(ct, &league, &cup_temp);

    if(cup_temp == NULL)
        cup_load_choose_team_from_league(cup, league, teams, ct);
    else
        cup_load_choose_team_from_cup(cup, cup_temp, teams, ct);

    if(debug > 80)
	for(i=debug_num;i<teams->len;i++)
	    g_print("cup_load_choose_team: %d %s \n", i, ((Team*)g_ptr_array_index(teams, i))->name);
}

void
cup_load_choose_team_from_cup(Cup *cup, const Cup *cup_temp, GPtrArray *teams, const CupChooseTeam *ct)
{
#ifdef DEBUG
    printf("cup_load_choose_team_from_cup\n");
#endif

    gint i;
    gint start, end;
    gint number_of_teams;
    GPtrArray *cup_teams_sorted;

    number_of_teams = 0;
    cup_teams_sorted = NULL;

    if(season == 1 && week == 1 && cup->add_week == 0)
    {
        if(lig(0).teams->len < ct->number_of_teams)
            main_exit_program(EXIT_CHOOSE_TEAM_ERROR, 
                              "cup_load_choose_team_from_cup: not enough teams in league 0 for chooseteam %s (%d; required: %d) in cup %s\n",
                              ct->sid, lig(0).teams->len, 
                              ct->number_of_teams, cup->name);

        gint permutation[lig(0).teams->len];
        math_generate_permutation(permutation, 0, lig(0).teams->len - 1);

        for(i = ct->start_idx - 1; i <= ct->end_idx - 1; i++)
        {
            gint team_idx = permutation[i - ct->start_idx + 1];
            Team *team = g_ptr_array_index(lig(0).teams, team_idx);
            if(ct->skip_group_check ||
               !query_team_is_in_cups(team, cup->group))
            {
                g_ptr_array_add(teams, team);
                g_ptr_array_add(cup->team_names, g_strdup(team->name));
                number_of_teams++;
            }
		
            if(number_of_teams == ct->number_of_teams)
                break;
        }
	    
        if(number_of_teams != ct->number_of_teams) {
	    if (ct->next)
	        return cup_load_choose_team(cup, teams, ct->next);

            main_exit_program(EXIT_CHOOSE_TEAM_ERROR, 
                              "cup_load_choose_team_from_cup (2): not enough teams found in league 0 for chooseteam %s (%d; required: %d) in cup %s (group %d)\n",
                              ct->sid, number_of_teams, 
                              ct->number_of_teams, cup->name, cup->group);
        }
    }
    else
    {
        /* Self-referential cup or no? */
        cup_teams_sorted = (cup == cup_temp) ? 
            cup_get_last_season_results(cup) :
            cup_get_teams_sorted(cup_temp);

        start = cup_choose_team_compute_start_idx(ct);
        end = cup_choose_team_compute_end_idx(ct, cup_teams_sorted->len) + 1;

        for(i = start; i < end && i < cup_teams_sorted->len; i++)
        {
            if(ct->skip_group_check ||
               !query_team_is_in_cups(
                   (Team*)g_ptr_array_index(cup_teams_sorted, i), cup->group))
            {
                g_ptr_array_add(teams, g_ptr_array_index(cup_teams_sorted, i));
                g_ptr_array_add(cup->team_names, 
                                g_strdup(((Team*)g_ptr_array_index(cup_teams_sorted, i))->name));
                number_of_teams++;

                if(number_of_teams == ct->number_of_teams)
                    break;
            }
        }
	
        if(cup != cup_temp)
            g_ptr_array_free(cup_teams_sorted, TRUE);

        if(ct->number_of_teams != -1 &&
           number_of_teams != ct->number_of_teams) {
	    if (ct->next)
	        return cup_load_choose_team(cup, teams, ct->next);
	    if (ct->optional)
	        return;
            main_exit_program(EXIT_CHOOSE_TEAM_ERROR, 
                              "cup_load_choose_team_from_cup (3): not enough teams (that don't participate in international cups yet) found in chooseteam %s for cup %s (%d specified, %d found) cup group %d.\n ",
                              ct->sid, cup->name,
                              ct->number_of_teams, number_of_teams, cup->group);
	}
    }
}

void
cup_load_choose_team_from_league(Cup *cup, const League *league,
                                 GPtrArray *teams, const CupChooseTeam *ct)
{
#ifdef DEBUG
    printf("cup_load_choose_team_from_league\n");
#endif

    gint start, end;
    gint number_of_teams;
    gint j;
    Table *table;

    number_of_teams = 0;
    if(league->tables->len == 0)
        table = &g_array_index(league->tables, Table, 0);
    else
        table = &g_array_index(league->tables, Table, ct->from_table);

    if(ct->number_of_teams == -1)
    {
        for(j=0;j<table->elements->len;j++)
        {
	    Team *team = g_array_index(table->elements, TableElement, j).team;
	    if (team_is_reserve_team(team) &&
	        !query_league_cup_has_property(cup->id, "include_reserve_teams"))
		continue;
            g_ptr_array_add(teams, team);
            g_ptr_array_add(cup->team_names, g_strdup(team->name));
        }
    }
    else
    {
        start = ct->start_idx - 1;
        end = ct->end_idx - start;

        gint order[end];
        for(j=0;j<end;j++)
            order[j] = j + start;

        if(ct->randomly)
            math_generate_permutation(order, start, start + end - 1);

        for(j = 0; j < end; j++)
        {
	    Team *team = g_array_index(table->elements, TableElement, order[j]).team;
	    if (team_is_reserve_team(team) &&
	        !query_league_cup_has_property(cup->id, "include_reserve_teams"))
		continue;
            if(debug > 80)
                g_print("j %d order %d team %s isinint %d numteams %d\n",
                        j, order[j],
                        g_array_index(table->elements,
                                      TableElement, order[j]).team->name,
                        query_team_is_in_cups(
                            g_array_index(table->elements,
                                          TableElement, order[j]).team,
                            cup->group),
                        number_of_teams);

            if(ct->skip_group_check || !query_team_is_in_cups(team, cup->group))
            {
                g_ptr_array_add(teams, team);
                g_ptr_array_add(cup->team_names, g_strdup(team->name));
                number_of_teams++;

                if(number_of_teams == ct->number_of_teams)
                    break;
            }
        }
    }

    if(ct->number_of_teams != -1 &&
       number_of_teams != ct->number_of_teams)
        main_exit_program(EXIT_CHOOSE_TEAM_ERROR, 
                          "cup_load_choose_team_from_league (1): not enough teams (that don't participate in international cups yet) found in chooseteam %s for cup %s (%d specified, %d found) cup group %d.\n ",
                          ct->sid, cup->name, ct->number_of_teams, 
                          number_of_teams, cup->group);    
}

/** This function is used when a CupChooseTeam object references a cup
 * that is outside of the users's league.  It returns a list of teamas
 * that would participate in this cup.  This is a best effort
 * function and the goal is to just get enough teams to satisfy the
 * requirements of the CupChooseTeam object, so it may omit some
 * teams especially if the cup definition is complex.
 */
static void
cup_generate_team_list(const Cup *cup, GPtrArray *teams, gboolean league_talents)
{
    int i;
    const League *top_league = NULL;
    for (i = 0; i < cup->rounds->len; i++) {
        const CupRound *round = &g_array_index(cup->rounds, CupRound, i);
        int j;
        for (j = 0; j < round->choose_teams->len; j++) {
            const CupChooseTeam *ct = &g_array_index(round->choose_teams, CupChooseTeam, j);
            const League *league = bygfoot_get_league_sid(ct->sid);
            if (!league)
                continue;
            if (!top_league || (league->layer < top_league->layer))
                top_league = league;
            if (league->layer == 1)
                break;
        }
    }
    if (!top_league)
        return;

    for (i = 0; i < top_league->teams->len; i++) {
        Team *team = g_ptr_array_index(top_league->teams, i);
	if (league_talents)
            team->average_talent = top_league->average_talent;
        g_ptr_array_add(teams, team);
    }
}

/** Load the teams specified in the chooseteam from a non-country league. */
void
cup_load_choose_team_generate(Cup *cup, CupRound *cup_round, const CupChooseTeam *ct)
{
#ifdef DEBUG
    printf("cup_load_choose_team_generate\n");
#endif

    gint j, k;
    gint number_of_teams, end_idx = -1;
    GPtrArray *teams_local = NULL;
    GPtrArray *sids = NULL;

    if(debug > 60)
	g_print("cup_load_choose_team_generate: %s, (%s) %s \n", cup->name, cup->sid,
	       ct->sid);

    teams_local = g_ptr_array_new();
    sids = misc_randomise_g_pointer_array(misc_separate_strings(ct->sid));

    for(j=0;j<sids->len;j++)
    {
	const gchar *sid = g_ptr_array_index(sids, j);
        Cup *generate_cup = bygfoot_get_cup_sid(sid);
        if (generate_cup) {
            cup_generate_team_list(generate_cup, teams_local,
                                   query_league_cup_has_property(cup->id, "league_talents"));
            continue;
        }

	if(!query_cup_choose_team_is_league(sid))
	{
            const League *league = bygfoot_get_league_sid(sid);
            if (!league) {
                g_warning("Cup: %s choose_team_sid %s not found.",
                          cup->sid, sid);
                continue;
            }

	    for(k=0; k < league->teams->len; k++)
            {
                Team *team = g_ptr_array_index(league->teams, k);
                if(query_league_cup_has_property(cup->id, "league_talents"))
                {        
                    team->average_talent = league->average_talent;
                }
                g_ptr_array_add(teams_local, team);
            }
	}
    }

    free_gchar_array(&sids);

    /** No teams found. */
    if(teams_local->len == 0)
    {
        g_ptr_array_free(teams_local, TRUE);
	return;
    }
    
    gint permutation[teams_local->len];
    for(j=0;j<teams_local->len;j++)
	permutation[j] = j;

    if(ct->randomly && teams_local->len > 0)
    {
	if(ct->start_idx == -1)
	    math_generate_permutation(permutation, 0, teams_local->len - 1);
	else
	    math_generate_permutation(permutation, ct->start_idx - 1, ct->end_idx - 1);
    } else {
        /* I'm doing this in the else here to avoid breaking other code
         * paths, since I don't really understand well enough what they
         * are doing.
         */
        for (j = 0; j < teams_local->len; j++) {
            permutation[j] = ct->start_idx + j - 1;
        }
    }

    number_of_teams = 0;
    if(ct->start_idx == -1)
	end_idx = teams_local->len;
    else
	end_idx = ct->end_idx -
	    ct->start_idx + 1;

    if(teams_local->len < end_idx)
    {
	main_exit_program(EXIT_CHOOSE_TEAM_ERROR, 
			  "cup_load_choose_team_generate: not enough teams (%d) in chooseteam %s in cup %s (%d are specified) \n",
			  teams_local->len, ct->sid, 
			  cup->name, end_idx);
    }

    for(j = 0; j < end_idx; j++)
    {
        Team *team = g_ptr_array_index(teams_local, permutation[j]);
	if(ct->skip_group_check ||
           !query_team_is_in_cups(team, cup->group))
	{
	    //g_array_append_val(cup_round->teams, g_array_index(teams_local, Team, permutation[j]));
	    //g_array_index(cup_round->teams, Team, cup_round->teams->len - 1).clid = cup->id;
	    team->clid = cup->id;
	    g_ptr_array_add(cup->team_names, g_strdup(team->name));
	    g_ptr_array_add(cup_round->team_ptrs, team);

	    number_of_teams++;
	}

	if(number_of_teams == ct->number_of_teams)
	    break;
    }

    if(((ct->number_of_teams != -1 && number_of_teams != ct->number_of_teams) ||
	(ct->number_of_teams == -1 && number_of_teams != teams_local->len)) &&
       teams_local->len > 0) {
        /* TODO: handle number_of_teams > 0 */
	g_ptr_array_free(teams_local, TRUE);
	if (ct->next)
            return cup_load_choose_team_generate(cup, cup_round, ct->next);
	if (ct->optional)
	    return;
	main_exit_program(EXIT_CHOOSE_TEAM_ERROR,
			  "cup_load_choose_team_generate: not enough teams (that don't participate in international cups yet) found in chooseteam %s for cup %s (%d specified, %d found).\n ", 
			  ct->sid, cup->name, 
			  ct->number_of_teams, number_of_teams);
    }
    g_ptr_array_free(teams_local, teams_local->len > 0);
}

/** Return a pointer array of teams ordered corresponding to
    their success in the cup. */
GPtrArray*
cup_get_teams_sorted(const Cup *cup)
{
#ifdef DEBUG
    printf("cup_get_teams_sorted\n");
#endif

    gint i, j;
    GPtrArray *teams = g_ptr_array_new();
    
    for(i=0;i<cup->fixtures->len;i++) {
        const Fixture *fixture = &g_array_index(cup->fixtures, Fixture, i);
	for(j=0;j<2;j++) {
            Team *team = fixture->teams[j];
            if (misc_g_ptr_array_find(teams, team, NULL))
                continue;
            g_ptr_array_add(teams, team);
        }
    }
    g_ptr_array_sort_with_data(teams, cup_compare_success, (gpointer)cup);

    return teams;
}

/** Compare two teams in a pointer array taking into account
    their success in a cup. */
gint
cup_compare_success(gconstpointer a, gconstpointer b, gpointer data)
{
#ifdef DEBUG
    printf("cup_compare_success\n");
#endif

    const Cup *cup = (const Cup*)data;
    const CupRound *cupround = NULL;
    const GArray *fixtures = cup->fixtures;
    const Team *team1 = *(const Team**)a;
    const Team *team2 = *(const Team**)b;
    gint round_reached1 = cup_get_round_reached(team1, fixtures),
	round_reached2 = cup_get_round_reached(team2, fixtures);
    gint return_value = 0;

    if(team1 == team2)
	return_value = 0;
    else if(round_reached1 < round_reached2)
	return_value = 1;
    else if(round_reached1 > round_reached2)
	return_value = -1;
    else
    {
	cupround = &g_array_index(cup->rounds, CupRound, round_reached1);
	
	if(cupround->tables->len > 0)
	    return_value = 
		cup_compare_success_tables(team1, team2, cup, round_reached1);
	else
	    return_value = cup_compare_success_knockout(team1, team2, cup->fixtures);
    }

    return return_value;
}

/** Compare two teams that reached the same knockout round. */
gint
cup_compare_success_knockout(const Team *tm1, const Team *tm2, const GArray *fixtures)
{
#ifdef DEBUG
    printf("cup_compare_success_knockout\n");
#endif

    gint i;
    gint winner1 = -1, winner2 = -1;
    gint return_value = 0;

    for(i=fixtures->len - 1; i>=0; i--)
    {
	if(winner1 == -1 &&
	   query_fixture_team_involved((&g_array_index(fixtures, Fixture, i)), tm1->id))
	    winner1 = GPOINTER_TO_INT(
		fixture_winner_of(&g_array_index(fixtures, Fixture, i), TRUE));

	if(winner2 == -1 &&
	   query_fixture_team_involved((&g_array_index(fixtures, Fixture, i)), tm2->id))
	    winner2 = GPOINTER_TO_INT(
		fixture_winner_of(&g_array_index(fixtures, Fixture, i), TRUE));
	
	if(winner1 != -1 && winner2 != -1)
	    break;
    }

    if(winner1 == tm1->id && winner2 != tm2->id)
	return_value = -1;
    else if(winner1 != tm1->id && winner2 == tm2->id)
	return_value = 1;

    return return_value;
}


/** Compare two teams in cup tables. */
gint
cup_compare_success_tables(const Team *tm1, const Team *tm2, const Cup *cup, gint round)
{
#ifdef DEBUG
    printf("cup_compare_success_tables\n");
#endif

    gint i, j;
    gint return_value = 0;
    const CupRound *cupround = &g_array_index(cup->rounds, CupRound, round);
    const TableElement *elem1 = NULL, *elem2 = NULL;

    if(team_get_cup_rank(tm1, cupround, TRUE) > 
       team_get_cup_rank(tm2, cupround, TRUE))
	return_value = 1;
    else if(team_get_cup_rank(tm1, cupround, TRUE) < 
	    team_get_cup_rank(tm2, cupround, TRUE))
	return_value = -1;
    else
    {
	for(i=0;i<cupround->tables->len;i++)
	    for(j=0;j<g_array_index(cupround->tables, Table, i).elements->len;j++)
		if(g_array_index(g_array_index(cupround->tables, Table, i).elements, TableElement, j).team == tm1)
		    elem1 = &g_array_index(g_array_index(cupround->tables, Table, i).elements, TableElement, j);
		else if(g_array_index(g_array_index(cupround->tables, Table, i).elements, TableElement, j).team == tm2)
		    elem2 = &g_array_index(g_array_index(cupround->tables, Table, i).elements, TableElement, j);

	return_value = table_element_compare_func(elem1, elem2, GINT_TO_POINTER(cup->id));
    }

    return return_value;
}

/** Return the cup round that the team reached in the cup.
    @param fixtures The fixtures array of the cup. */
gint
cup_get_round_reached(const Team *tm, const GArray *fixtures)
{
#ifdef DEBUG
    printf("cup_get_round_reached\n");
#endif

    gint round = -1;
    gint i;

    for(i=0;i<fixtures->len;i++)
	if(g_array_index(fixtures, Fixture, i).teams[0] == tm ||
	   g_array_index(fixtures, Fixture, i).teams[1] == tm)
	    round = MAX(round, g_array_index(fixtures, Fixture, i).round);

    return round;
}

/** Calculate the week number the first matchday of the given
    cup round takes place.
    @param cup The cup we examine.
    @param cup_round The index of the cup round in the cup.rounds array.
    @return A week number. */
gint
cup_get_first_week_of_cup_round(Cup *cup, gint cup_round, gboolean with_delay)
{
#ifdef DEBUG
    printf("cup_get_first_week_of_cup_round\n");
#endif

    gint week_number;

    if(cup_round == cup->rounds->len - 1)
	week_number = cup->last_week - 
	    (cup_get_matchdays_in_cup_round(cup, cup_round) - 1) * cup->week_gap;
    else
	week_number = cup_get_first_week_of_cup_round(cup, cup_round + 1, FALSE) -
	    cup_get_matchdays_in_cup_round(cup, cup_round) * cup->week_gap;

    if(with_delay)
        week_number += g_array_index(cup->rounds, CupRound, cup_round).delay;

    if(week_number <= 0)
    {
	debug_print_message("cup_get_first_week_of_cup_round: First week of cup %s, cup round %d is not positive (%d). Please correct the cup definition file!!!\n",
		  cup->name, cup_round, week_number);
	
	if(cup->week_gap > 1)
	{
	    cup->week_gap--;
	    debug_print_message("Lowering week gap to %d and trying again.\n",
		      cup->week_gap);
	}
	else
	{
	    cup->last_week++;
	    debug_print_message("Increasing last week to %d and trying again.\n",
		      cup->last_week);
	}

	return cup_get_first_week_of_cup_round(cup, cup_round, with_delay);
    }
    
    return week_number;
}

/** Calculate the last week of a cup if we only know the first week. */
gint
cup_get_last_week_from_first(const Cup *cup, gint first_week)
{
#ifdef DEBUG
    printf("cup_get_last_week_from_first\n");
#endif

    gint i;
    gint matchdays = 0;

    for(i=0;i<cup->rounds->len;i++)
	matchdays += cup_get_matchdays_in_cup_round(cup, i);

    return first_week + (matchdays - 1) * cup->week_gap;
}

/** Return the number of matchdays for a given cup round.
    @param cup The cup we examine.
    @param cup_round The index of the cup round.
    @return The number of matchdays, mostly 1 or 2. */
gint
cup_get_matchdays_in_cup_round(const Cup *cup, gint round)
{
#ifdef DEBUG
    printf("cup_get_matchdays_in_cup_round\n");
#endif

    gint i;
    const CupRound *cup_round = &g_array_index(cup->rounds, CupRound, round);    
    gint number_of_teams = -1;
    gint number_of_matchdays = -1;
    gint diff;
    
    if(cup_round->round_robin_number_of_groups > 0)
    {
	number_of_teams = cup_round_get_number_of_teams(cup, round);
	number_of_teams += cup_round->round_robin_number_of_groups - 1;
	number_of_teams /= cup_round->round_robin_number_of_groups;
	/* Now, number_of_teams = number of teams in largest group */
	if (number_of_teams % 2 == 0)
	    number_of_matchdays = number_of_teams - 1;
	else
	    number_of_matchdays = number_of_teams;
	
        number_of_matchdays *= cup_round->round_robins;

	for(i=0;i<cup_round->two_match_weeks[0]->len;i++)
	{
	    diff = g_array_index(cup_round->two_match_weeks[1], gint, i) - 
		g_array_index(cup_round->two_match_weeks[0], gint, i);	    
	    number_of_matchdays -= ((diff + diff % 2) / 2);
	}
    }
    else if(cup_round->home_away)
	number_of_matchdays = 2 - cup_round->two_match_week;
    else
	number_of_matchdays = 1;

    return number_of_matchdays;
}

/** Return the number of teams playing in
    the given cup round.
    @param cup The cup we examine.
    @param cup_round The index of the cup round.
    @return The number teams in the round. */
gint
cup_round_get_number_of_teams(const Cup *cup, gint round)
{
#ifdef DEBUG
    printf("cup_round_get_number_of_teams\n");
#endif

    const CupRound *cup_round = &g_array_index(cup->rounds, CupRound, round);
    gint number_of_teams = 0;

    if(round == 0)
	number_of_teams = cup_round_get_new_teams(cup_round);
    else if(g_array_index(cup->rounds, CupRound, round - 1).round_robin_number_of_groups > 0)
    {
	number_of_teams = 
	    (g_array_index(cup->rounds, CupRound, round - 1).round_robin_number_of_groups *
	     g_array_index(cup->rounds, CupRound, round - 1).round_robin_number_of_advance) +
	    g_array_index(cup->rounds, CupRound, round - 1).round_robin_number_of_best_advance +
	    cup_round_get_new_teams(cup_round) + cup_round_get_byes(cup, round - 1);
    }
    else
	number_of_teams = cup_round_get_number_of_teams(cup, round - 1) / 2 +
	    cup_round_get_new_teams(cup_round) + cup_round_get_byes(cup, round - 1);

    return number_of_teams;
}

/** Get the number of byes in the given cup round. */
gint
cup_round_get_byes(const Cup *cup, gint round)
{
#ifdef DEBUG
    printf("cup_round_get_byes\n");
#endif

    const CupRound *cup_round = &g_array_index(cup->rounds, CupRound, round);
    gint number_of_byes = 0, new_teams = 0;

    if(cup_round->byes != -1)
	number_of_byes = cup_round->byes;
    else
    {
	if(round == 0)
	    new_teams = cup_round_get_new_teams(cup_round);
	else
	    new_teams = cup_round_get_number_of_teams(cup, round);

	number_of_byes = (cup_round->round_robin_number_of_groups == 0) ?
	    math_get_bye_len(new_teams) : 0;

/* 	if(cup_round->round_robin_number_of_groups == 0) */
/* 	    number_of_byes = math_get_bye_len(new_teams); */
/* 	else */
/* 	{ */
/* 	    while(new_teams % cup_round->round_robin_number_of_groups != 0) */
/* 	    { */
/* 		new_teams--; */
/* 		number_of_byes++; */
/* 	    } */
/* 	} */
    }

    return number_of_byes;
}

/** Return the number of new teams that come into the
    cup in the given cup round. */
gint
cup_round_get_new_teams(const CupRound *cup_round)
{
#ifdef DEBUG
    printf("cup_round_get_new_teams\n");
#endif

    gint i, new_teams = 0;
    const Cup *cup_temp = NULL;
    const League *league = NULL;
    GPtrArray *teams_sorted = NULL;

    if(cup_round->new_teams != 0)
	new_teams = cup_round->new_teams;
    else
    {
	for(i=0;i<cup_round->choose_teams->len;i++)
	{
	    if(g_array_index(cup_round->choose_teams, CupChooseTeam, i).
	       number_of_teams != -1)
		new_teams += 
		    g_array_index(cup_round->choose_teams, CupChooseTeam, i).
		    number_of_teams;
	    else
	    {
		cup_get_choose_team_league_cup(
		    &g_array_index(cup_round->choose_teams, CupChooseTeam, i),
		    &league, &cup_temp);
		if(cup_temp == NULL)
		    new_teams += league->teams->len;
		else
		{
		    teams_sorted = cup_get_teams_sorted(cup_temp);
		    new_teams += teams_sorted->len;
		    g_ptr_array_free(teams_sorted, TRUE);
		}
	    }
	}
    }

    return new_teams;
}

/** Return the cup pointer belonging to the id.
    @param clid The id we look for.
    @return The cup pointer or NULL if failed. */
Cup*
cup_from_clid(gint clid)
{
#ifdef DEBUG
    printf("cup_from_clid\n");
#endif

    gint i;

    for(i=0;i<cps->len;i++)
	if(cp(i).id == clid)
	    return &cp(i);

    main_exit_program(EXIT_POINTER_NOT_FOUND, 
		      "cup_from_clid: didn't find cup with id %d\n", clid);

    return NULL;
}

/** Find the cup with the given sid. */
Cup*
cup_from_sid(const gchar *sid)
{
#ifdef DEBUG
    printf("cup_from_sid\n");
#endif

    gint i;

    for(i=0;i<cps->len;i++)
	if(strcmp(cp(i).sid, sid) == 0)
	    return &cp(i);

    main_exit_program(EXIT_POINTER_NOT_FOUND, 
		      "cup_from_sid: didn't find cup with sid %s \n", sid);

    return NULL;
}

/** Return a standard cup round name. */
void
cup_get_round_name(const Cup *cup, gint round, gchar *buf)
{
#ifdef DEBUG
    printf("cup_get_round_name\n");
#endif

    const CupRound *cup_round = 
	&g_array_index(cup->rounds, CupRound, round);

    if(cup_round->round_robin_number_of_groups > 0)
    {
	strcpy(buf, _("Round robin"));
	return;
    }

    switch(cup->rounds->len - round)
    {
	default:
	    /* A cup stage, e.g. Last 32 when there are only 32 teams left. */
	    sprintf(buf, _("Last %d"), (gint)rint(powf(2, cup->rounds->len - round)));
	    break;
	case 1:
	    strcpy(buf, _("Final"));
	    break;
	case 2:
	    strcpy(buf, _("Semi-final"));
	    break;
	case 3:
	    strcpy(buf, _("Quarter-final"));
	    break;
    }
}


/** Find out whether the cup contains tables
    that can be displayed. Returns -1 if false
    and the number of the cup round with tables
    otherwise. */
gint
cup_has_tables(gint clid)
{
#ifdef DEBUG
    printf("cup_has_tables\n");
#endif

    const Cup *cup = cup_from_clid(clid);
    gint i;

    for(i=cup->rounds->len - 1;  i>=0; i--)
	if(g_array_index(cup->rounds, CupRound, i).tables->len > 0)
	    return i;

    return -1;
}

/** Return the team that won the cup. */
Team*
cup_get_winner(const Cup *cup)
{
#ifdef DEBUG
    printf("cup_get_winner\n");
#endif

    GPtrArray *teams_sorted = cup_get_teams_sorted(cup);
    Team *tm = (Team*)g_ptr_array_index(teams_sorted, 0);
    
    g_ptr_array_free(teams_sorted, TRUE);

    return tm;
}

/** Find out whether a cup with add_week 1000 should begin now. */
gboolean
query_cup_begins(const Cup *cup)
{
#ifdef DEBUG
    printf("query_cup_begins\n");
#endif

    gint i, j;
    const League *league = NULL;
    const Cup *cup_temp = NULL;
    const CupRound *cup_round = NULL;

    for(j=0;j<cup->rounds->len;j++)
    {
	cup_round = &g_array_index(cup->rounds, CupRound, j);

	for(i=0;i<cup_round->choose_teams->len;i++)
	    if(!cup_choose_team_should_generate(&g_array_index(cup_round->choose_teams,CupChooseTeam, i)))
	    {		
	        const Fixture *last_fixture;
		cup_get_choose_team_league_cup(
		    &g_array_index(cup_round->choose_teams,
				   CupChooseTeam, i), &league, &cup_temp);
		
		if(cup_temp == NULL &&
		    query_league_active(league) &&
		    g_array_index(league->fixtures, Fixture,
				  league->fixtures->len - 1).attendance == -1)
		    return FALSE;

		/* Handle cups */
		if (!cup_temp)
		    continue;

		if (!cup_temp->fixtures->len)
		    return FALSE;

		last_fixture = &g_array_index(cup_temp->fixtures, Fixture,
				              cup_temp->fixtures->len - 1);

		/* attendance == 1 means there are scheduled games that have
		 * not been played yet.  If the last fixture is not for the
		 * last round of the cup that means there are still games
		 * that have not been scheduled yet.
		 */
		if (last_fixture->attendance == -1 ||
		    last_fixture->round != cup_temp->rounds->len - 1)
		    return FALSE;
	    }
    }

    return TRUE;
}

/** Return the number of international cups in the country. */
gboolean
query_cup_transfer(void)
{
#ifdef DEBUG
    printf("query_cup_transfer\n");
#endif

    gint i;

    for(i=0;i<acps->len;i++)
	if(acp(i)->teams->len > 0)
	    return TRUE;

    return FALSE;
}

/** Find out whether the cup has a highlight property
    and return the highlight colour. */
gchar*
cup_get_highlight_colour(const Cup *cup)
{
#ifdef DEBUG
    printf("cup_get_highlight_colour\n");
#endif

#ifdef DEBUG
    printf("cup_get_highlight_colour\n");
#endif

    gint i;
    gchar buf[SMALL];

    for(i=0;i<cup->properties->len;i++)
	if(g_str_has_prefix((gchar*)g_ptr_array_index(cup->properties, i), "highlight"))
	{
	    sprintf(buf, "string_cup_%s",
		    (gchar*)g_ptr_array_index(cup->properties, i));
	    return const_app(buf);
	}

    return NULL;
}

/** Check the cup fixtures for suspicious entries. */
gboolean
cup_check_fixtures(const Cup *cup)
{
#ifdef DEBUG
    printf("cup_check_fixtures\n");
#endif

    gint i;

    for(i = 0; i < cup->fixtures->len; i++)
    {
        if(g_array_index(cup->fixtures, Fixture, i).teams[0] ==
           g_array_index(cup->fixtures, Fixture, i).teams[1])
        {
            if(!query_league_cup_has_property(cup->id, "silent_on_fixture_error"))
                debug_print_message("cup_check_fixture: bad fixture found in cup %s; cup will be disabled\n", cup->name);

            return FALSE;
        }
    }

    return TRUE;
}

/** Check whether we have to wait for other cups
    before we can write fixtures for the cup round. */
gboolean
cup_round_check_waits(const CupRound *cup_round)
{
    gint i, j, k;
    gchar prefix[SMALL];

    for(i = 0; i < cup_round->waits->len; i++)
    {
        if(g_str_has_suffix(g_array_index(cup_round->waits, CupRoundWait, i).cup_sid, "*"))
            g_utf8_strncpy(prefix, g_array_index(cup_round->waits, CupRoundWait, i).cup_sid,
                           g_utf8_strlen(g_array_index(cup_round->waits, CupRoundWait, i).cup_sid, -1) - 1);
        else
            strcpy(prefix, "NONAME");        

        for(j = 0; j < acps->len; j++)
        {
            if(strcmp(acp(j)->sid, g_array_index(cup_round->waits, CupRoundWait, i).cup_sid) == 0 ||
               g_str_has_prefix(acp(j)->sid, prefix))
            {
                /* Cup round we're waiting for isn't even reached. */
                if(g_array_index(acp(j)->fixtures, Fixture, acp(j)->fixtures->len - 1).round < 
                   g_array_index(cup_round->waits, CupRoundWait, i).cup_round)
                    return TRUE;

                for(k = acp(j)->fixtures->len - 1; k >= 0; k--)
                {
                    /* Cup round we've been waiting for is finished,
                       we're not waiting anymore. */
                    if(g_array_index(acp(j)->fixtures, Fixture, k).round > 
                       g_array_index(cup_round->waits, CupRoundWait, i).cup_round)
                        break;

                    /* Still waiting for matches to be calculated. */
                    if(g_array_index(acp(j)->fixtures, Fixture, k).round ==
                       g_array_index(cup_round->waits, CupRoundWait, i).cup_round &&
                       g_array_index(acp(j)->fixtures, Fixture, k).attendance == -1)
                        return TRUE;
                }

                break;
            }
        }
    }

    return FALSE;
}

/** Find out whether the cup chooses teams from itself
    (e.g. the defending champion from last season). */
gboolean
query_cup_self_referential(const Cup *cup)
{
    gint i, j;

    for(i = 0; i < cup->rounds->len; i++)
        for(j = 0; j < g_array_index(cup->rounds, CupRound, i).choose_teams->len; j++)
            if(strcmp(g_array_index(g_array_index(cup->rounds, CupRound, i).choose_teams, CupChooseTeam, j).sid, cup->sid) == 0)
                return TRUE;

    return FALSE;
}

/** Find out if the cup is part of the array of
    cups that are displayed in the game. */
gboolean
query_cup_hidden(const Cup *cup)
{
    gint i;

    for(i = 0; i < acps->len; i++)
        if(acp(i) == cup)
            return FALSE;

    return TRUE;
}

/** @return a GPtrArray with Team* sorted by their ranking in last seasons's
 * cup. */
GPtrArray *
cup_get_last_season_results(const Cup *cup)
{
    return g_ptr_array_index(cup->history, cup->history->len - 1);
}
