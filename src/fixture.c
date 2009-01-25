/*
   fixture.c

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
#include "fixture.h"
#include "free.h"
#include "league.h"
#include "main.h"
#include "maths.h"
#include "misc.h"
#include "option.h"
#include "user.h"
#include "table.h"
#include "team.h"
#include "variables.h"

/** Write the fixtures for the given league
    at the beginning of a new season. 
    @param league The league we write the fixtures for. */
void
fixture_write_league_fixtures(League *league)
{
#ifdef DEBUG
    printf("fixture_write_league_fixtures\n");
#endif

    gint i,j;
    gint max_rr;
    gint joined_clids[league->joined_leagues->len];
    gint joined_rrs[league->joined_leagues->len];
    GPtrArray *teams;
    
    max_rr = league->round_robins;
    teams = team_get_pointers_from_array(league->teams, NULL);

    /** Add all teams to the same pointer array. */
    for(i = 0; i < league->joined_leagues->len; i++)
    {
        joined_rrs[i] = g_array_index(league->joined_leagues, JoinedLeague, i).rr;
        joined_clids[i] = league_index_from_sid(g_array_index(league->joined_leagues, JoinedLeague, i).sid);
        max_rr = MAX(joined_rrs[i], max_rr);
        teams = team_get_pointers_from_array(lig(joined_clids[i]).teams, teams);
    }
        
    /** Write fixtures for as many round robins as required by the maximum number of rrs given. */
    for(i = 0; i < max_rr;)
    {
    	fixture_write_round_robin((gpointer)league, -1, misc_copy_ptr_array(teams),
                                  (i == max_rr - 1), -1, league->rr_breaks, i - 1);
    	i += (i < max_rr - 1) ? 2 : 1;
    }

    g_ptr_array_free(teams, TRUE);
    g_array_sort_with_data(league->fixtures, fixture_compare_func, 
			   GINT_TO_POINTER(FIXTURE_COMPARE_DATE + 100));

    if(league->joined_leagues->len > 0)
    {
        /** Remove fixtures involving only teams from the joined leagues and
            for joined leagues with a smaller number of rrs than the maximum. */
        for(i = 0; i < league->joined_leagues->len; i++)
        {
            if(joined_rrs[i] < max_rr)
                fixture_remove_rrs(league->fixtures, league->id, lig(joined_clids[i]).id, max_rr - joined_rrs[i]);

            for(j = i; j < league->joined_leagues->len; j++)
                fixture_remove_rrs(league->fixtures, lig(joined_clids[i]).id, lig(joined_clids[j]).id, max_rr);
        }

        /** Remove fixtures for reduced number of rrs. */
        if(league->round_robins < max_rr)
            fixture_remove_rrs(league->fixtures, league->id, league->id, max_rr - league->round_robins);

        /** Move matches to earlier weeks if possible. */
        fixtures_condense(league->fixtures);
    }
}

/** Write the fixtures for the given cup
    at the beginning of a new season. 
    @param cup The cup we write the fixtures for. */
gboolean
fixture_write_cup_fixtures(Cup *cup)
{
#ifdef DEBUG
    printf("fixture_write_cup_fixtures\n");
#endif

    gint i;
    GPtrArray *teams_sorted = NULL;
    
    /* Store the order of teams in case the cup
       uses teams from its previous incarnation (previous season). */
    if(query_cup_self_referential(cup))
        teams_sorted = cup_get_teams_sorted(cup);

    cup_reset(cup);

    for(i=0;i<cup->rounds->len;i++)
        cup_get_team_pointers(cup, i, teams_sorted, TRUE);

    if(teams_sorted != NULL)
        g_ptr_array_free(teams_sorted, TRUE);

    if(g_array_index(cup->rounds, CupRound, 0).round_robin_number_of_groups > 0)
	fixture_write_cup_round_robin(
	    cup, 0, misc_copy_ptr_array(
		g_array_index(cup->rounds, CupRound, 0).team_ptrs));
    else
	fixture_write_knockout_round(
	    cup, 0, misc_copy_ptr_array(
		g_array_index(cup->rounds, CupRound, 0).team_ptrs));

    return cup_check_fixtures(cup);
}

/** Update the fixtures for the given cup. 
    @param cup The cup we update. */
void
fixture_update(Cup *cup)
{
#ifdef DEBUG
    printf("fixture_update\n");
#endif

    gint i;
    GArray *fixtures = cup->fixtures;
    gint round = g_array_index(fixtures, Fixture, fixtures->len - 1).round;
    gint replay = g_array_index(cup->rounds, CupRound, round).replay;
    GPtrArray *teams = NULL;
    const CupRound *new_round = NULL;

    if(debug > 90)
	g_print("fixture_update: cup %s (id %d)\n", cup->name, cup->id);

    if(replay != 0 && 
       g_array_index(fixtures, Fixture, fixtures->len - 1).replay_number < replay && 
       fixture_update_write_replays(cup))
	    return;
    
    /** Still waiting for other cups. */
    if(round + 1 < cup->rounds->len &&
       cup_round_check_waits(&g_array_index(cup->rounds, CupRound, round + 1)))
       return;

    teams = fixture_get_cup_round_winners(cup);

    if(round == cup->rounds->len - 1 && teams->len < 2)
    {
	g_ptr_array_free(teams, TRUE);
        return;
    }

    if(round + 1 > cup->rounds->len - 1)
    {
	g_ptr_array_free(teams, TRUE);
	main_exit_program(
	    EXIT_CUP_ROUND_ERROR, 
	    "fixture_update: round index %d too high for round array (%d) in cup %s\n",
	    round + 1, cup->rounds->len - 1, cup->name);
    }

    new_round = &g_array_index(cup->rounds, CupRound, round + 1);
    cup_get_team_pointers(cup, round + 1, NULL, FALSE);

    for(i=0;i<new_round->team_ptrs->len;i++)
	g_ptr_array_add(teams, g_ptr_array_index(new_round->team_ptrs, i));
    
    if(teams->len < 2)
    {
	g_ptr_array_free(teams, TRUE);
	return;
    }
    
    if(cup->bye != NULL && cup->bye->len != 0)
    {
	for(i=0;i<cup->bye->len;i++)
	    g_ptr_array_add(teams, g_ptr_array_index(cup->bye, i));

	free_g_ptr_array(&cup->bye);
    }

    if(new_round->round_robin_number_of_groups > 0)
	fixture_write_cup_round_robin(cup, round + 1, teams);
    else
	fixture_write_knockout_round(cup, round + 1, teams);
}

/** Return the teams that advance to the next cup round.
    @param fixtures The fixtures array of the cup.
    @return A team pointer array. */
GPtrArray*
fixture_get_cup_round_winners(const Cup *cup)
{
#ifdef DEBUG
    printf("fixture_get_cup_round_winners\n");
#endif

    gint i;
    GPtrArray *array = NULL;
    GArray *fixtures = cup->fixtures;
    gint round = g_array_index(fixtures, Fixture, fixtures->len - 1).round;
    const CupRound *cupround = &g_array_index(cup->rounds, CupRound, round);

    if(cupround->round_robin_number_of_groups > 0)
	return fixture_get_round_robin_advance(cup, round);

    array = g_ptr_array_new();

    if(cupround->replay != 0)
    {
	for(i=0;i<fixtures->len;i++)
	    if(g_array_index(fixtures, Fixture, i).round == round &&
	       (g_array_index(fixtures, Fixture, i).result[0][0] != 
		g_array_index(fixtures, Fixture, i).result[1][0] ||
		g_array_index(fixtures, Fixture, i).result[0][1] +
		g_array_index(fixtures, Fixture, i).result[0][2] +
		g_array_index(fixtures, Fixture, i).result[1][1] +
		g_array_index(fixtures, Fixture, i).result[1][2] != 0))
		g_ptr_array_add(array, fixture_winner_of(&g_array_index(fixtures, Fixture, i), FALSE));

	return array;
    }

    if(!cupround->home_away)
    {
	for(i=0;i<fixtures->len;i++)
	    if(g_array_index(fixtures, Fixture, i).round == round)
		g_ptr_array_add(array, fixture_winner_of(&g_array_index(fixtures, Fixture, i), FALSE));
	
	return array;
    }

    for(i=0;i<fixtures->len;i++)
	if(g_array_index(fixtures, Fixture, i).round == round &&
	   g_array_index(fixtures, Fixture, i).second_leg)
	    g_ptr_array_add(array, fixture_winner_of(&g_array_index(fixtures, Fixture, i), FALSE));

    return array;
}

/** Return an array of teams advancing from a round robin
    stage of a cup. 
    @param cup The cup.
    @param round The cup round.
    @return The pointers to the teams. */
GPtrArray*
fixture_get_round_robin_advance(const Cup *cup, gint round)
{
#ifdef DEBUG
    printf("fixture_get_round_robin_advance\n");
#endif

    const CupRound *cupround = &g_array_index(cup->rounds, CupRound, round);
    GPtrArray *teams = cup_get_teams_sorted(cup);
    gint i, remove = teams->len -
	(cupround->round_robin_number_of_groups *
	 cupround->round_robin_number_of_advance) -
	cupround->round_robin_number_of_best_advance;  

    for(i=0;i<remove;i++)
	g_ptr_array_remove_index(teams, teams->len - 1);    

    return teams;
}

/** Return the pointer of the team that won the encounter.
    @param fix The fixture we examine.
    @param team_name Whether to return a team pointer or the
    team id of the winner.
    @return A team pointer or a coded integer. */
gpointer
fixture_winner_of(const Fixture *fix, gboolean team_id)
{
#ifdef DEBUG
    printf("fixture_winner_of\n");
#endif

    gint winner_idx = -1;
    const Fixture *first_leg = NULL;
    const CupRound *cupround = NULL;

    if(fix->clid < ID_CUP_START)
    {
	winner_idx = (fix->result[0][0] < fix->result[1][0]);
	if(team_id)
	    return GINT_TO_POINTER(fix->team_ids[winner_idx]);
	else
	    return (gpointer)fix->teams[winner_idx];
    }

    cupround = &g_array_index(cup_from_clid(fix->clid)->rounds, 
			      CupRound, fix->round);

    if(cupround->replay != 0 ||
       !cupround->home_away ||
       cupround->round_robin_number_of_groups > 0)
	winner_idx = (math_sum_int_array(&(fix->result[0][0]), 3) <
		      math_sum_int_array(&(fix->result[1][0]), 3));
    else
    {
	first_leg = fixture_get_first_leg(fix, FALSE);
	
	if(fix->result[0][2] + fix->result[1][2] != 0)
	    winner_idx = (fix->result[0][2] < fix->result[1][2]);
	else if(fix->result[0][0] + fix->result[0][1] +
		 first_leg->result[1][0] + first_leg->result[1][1] !=
		 fix->result[1][0] + fix->result[1][1] +
		first_leg->result[0][0] + first_leg->result[0][1])
	    winner_idx = (fix->result[0][0] + fix->result[0][1] +
			  first_leg->result[1][0] + first_leg->result[1][1] <
			  fix->result[1][0] + fix->result[1][1] +
			  first_leg->result[0][0] + first_leg->result[0][1]);
	else
	    winner_idx = (fix->result[1][0] + fix->result[1][1] >
			  first_leg->result[1][0] + first_leg->result[1][1]);
    }

    if(team_id)
	return GINT_TO_POINTER(fix->team_ids[winner_idx]);
    else
	return (gpointer)fix->teams[winner_idx];
}

/** Write replay matches for the cup.
    @param cup The cup.
    @return TRUE if fixtures were written, FALSE otherwise. */
gboolean
fixture_update_write_replays(Cup *cup)
{
#ifdef DEBUG
    printf("fixture_update_write_replays\n");
#endif

    gint i;
    GArray *fixtures = cup->fixtures;
    gint replay_number = g_array_index(fixtures, Fixture, fixtures->len - 1).replay_number + 1;
    gint round = g_array_index(fixtures, Fixture, fixtures->len - 1).round;
    gboolean return_value = FALSE;

    for(i=0;i<fixtures->len;i++)
	if(g_array_index(fixtures, Fixture, i).round == round &&
	   g_array_index(fixtures, Fixture, i).replay_number == replay_number - 1 &&
	   g_array_index(fixtures, Fixture, i).result[0][0] == 
	   g_array_index(fixtures, Fixture, i).result[1][0])
	    fixture_write(fixtures, g_array_index(fixtures, Fixture, i).teams[1],
			  g_array_index(fixtures, Fixture, i).teams[0], week,
			  fixture_get_free_round(week, NULL, g_array_index(fixtures, Fixture, i).teams[0]->id,
						 g_array_index(fixtures, Fixture, i).teams[1]->id),
			  cup->id, round, replay_number,
			  !g_array_index(cup->rounds, CupRound, round).neutral,
			  FALSE, (g_array_index(cup->rounds, CupRound, round).replay == replay_number));

    if(g_array_index(fixtures, Fixture, fixtures->len - 1).replay_number == replay_number)
    {
	return_value = TRUE;
	cup->next_fixture_update_week_round = 
	    g_array_index(fixtures, Fixture, fixtures->len - 1).week_round_number;
    }

    return return_value;
}

/** Write round robins for the given cup and cup round.
    @param cup The cup the fixtures are for.
    @param cup_round The cup round. */
void
fixture_write_cup_round_robin(Cup *cup, gint cup_round, GPtrArray *teams)
{
#ifdef DEBUG
    printf("fixture_write_cup_round_robin\n");
#endif

    gint i, j, cnt;
    CupRound *cupround = &g_array_index(cup->rounds, CupRound, cup_round);
    gint number_of_groups = cupround->round_robin_number_of_groups;
    gint team_num = (teams->len - (teams->len % number_of_groups)) / number_of_groups;
    gint plus_div = teams->len % number_of_groups;
    GPtrArray *teams_group[number_of_groups];
    Table table_group[number_of_groups];
    TableElement new_table_element;

    if(debug > 100)
        g_print("fixture_write_cup_round_robin: %s round %d teamlen %d\n", cup->name, cup_round, teams->len);

    if(teams->len < number_of_groups)
	main_exit_program(EXIT_FIXTURE_WRITE_ERROR, 
			  "fixture_write_cup_round_robin: cup %s round %d: number of teams (%d) less than number of groups (%d)\n", 
			  cup->name, cup_round, teams->len, number_of_groups);

    if(cupround->randomise_teams || opt_int("int_opt_randomise_teams"))
	g_ptr_array_sort_with_data(teams, (GCompareDataFunc)team_compare_func,
			       GINT_TO_POINTER(TEAM_COMPARE_AV_SKILL));

    for(i=0;i<number_of_groups;i++)
    {
	table_group[i].name = NULL;
	misc_string_assign(&table_group[i].name, cup->name);
	table_group[i].clid = cup->id;
	table_group[i].round = cup_round;
	table_group[i].elements = g_array_new(FALSE, FALSE, sizeof(TableElement));    
	teams_group[i] = g_ptr_array_new();
    }
    
    cnt = 0;
    if(!cupround->randomise_teams && !opt_int("int_opt_randomise_teams"))
    {
	for(i=0;i<number_of_groups;i++)
	    for(j=0;j<team_num + (i < plus_div);j++)
	    {
		g_ptr_array_add(teams_group[i],
				g_ptr_array_index(teams, cnt));
		new_table_element = 
		    table_element_new((Team*)g_ptr_array_index(teams, cnt), 
				      table_group[i].elements->len);
		g_array_append_val(table_group[i].elements, new_table_element);
		cnt++;
	    }	
    }
    else
    {
	for(i=0;i<teams->len;i++)
	{
	    g_ptr_array_add(teams_group[cnt],
			    g_ptr_array_index(teams, i));
	    new_table_element = 
		table_element_new((Team*)g_ptr_array_index(teams, i), 
				  table_group[cnt].elements->len);
	    g_array_append_val(table_group[cnt].elements, new_table_element);
	    cnt = (cnt + 1) % number_of_groups;
	}
    }

    for(i=0;i<number_of_groups;i++)
    {
	g_array_append_val(cupround->tables, table_group[i]);

        for(j = 0; j < cupround->round_robins;)
        {
            fixture_write_round_robin((gpointer)cup, cup_round, 
                                      misc_copy_ptr_array(teams_group[i]),
                                      (j == cupround->round_robins - 1),
                                      (j == 0) ? -1 : g_array_index(cup->fixtures, Fixture, cup->fixtures->len - 1).week_number + 
                                      g_array_index(cupround->rr_breaks, gint, j - 1),
                                      cupround->rr_breaks, j - 1);
            j += (j < cupround->round_robins - 1) ? 2 : 1;
        }
        g_ptr_array_free(teams_group[i], TRUE);
    }

    g_ptr_array_free(teams, TRUE);

    g_array_sort_with_data(cup->fixtures, fixture_compare_func,
			   GINT_TO_POINTER(FIXTURE_COMPARE_DATE + 100));

    cup->next_fixture_update_week = (cup_round < cup->rounds->len - 1) ?
	g_array_index(cup->fixtures, Fixture, cup->fixtures->len - 1).week_number : 1000;
    cup->next_fixture_update_week_round = (cup_round < cup->rounds->len - 1) ?
	g_array_index(cup->fixtures, Fixture, cup->fixtures->len - 1).week_round_number : 1000;
}

/** Write round robin fixtures for the teams in the array.
    The way it's done is difficult to explain, so there
    won't be many comments below.
    @param league_cup The pointer to the cup or league the fixtures
    belong to.
    @param cup_round The round of the cup the fixtures belong to
    or -1 if we have a league.
    @param teams The pointer array containing pointers
    to the teams. 
    @param one_round Whether a team plays each other team twice or only once. */
void
fixture_write_round_robin(gpointer league_cup, gint cup_round, 
			  GPtrArray *teams, gboolean one_round, 
                          gint first_week, GArray *rr_breaks,
                          gint rr_break_idx)
{
#ifdef DEBUG
    printf("fixture_write_round_robin\n");
#endif

    gint i, j;
    gint week_gap, week_number, 
	week_round_number, clid, first_fixture;
    gboolean home_advantage;
    League *league = NULL;
    Cup *cup = NULL;
    gint len = teams->len;
    GArray *fixtures = NULL;
    GArray **two_match_weeks;
    Team team_temp;
    gboolean odd_fixtures = FALSE;

    teams = misc_randomise_g_pointer_array(teams);
    
    if(cup_round == -1)
    {
	league = (League*)league_cup;
	fixtures = league->fixtures;
	two_match_weeks = league->two_match_weeks;
	clid = league->id;
	first_week = (fixtures->len == 0) ? league->first_week : 
	    g_array_index(fixtures, Fixture, fixtures->len - 1).week_number + 
            g_array_index(rr_breaks, gint, rr_break_idx);
	week_gap = league->week_gap;
	home_advantage = TRUE;
    }
    else
    {
	cup = (Cup*)league_cup;
        if(first_week == -1)
            first_week = cup_get_first_week_of_cup_round(cup, cup_round, TRUE);
	week_gap = cup->week_gap;
	fixtures = cup->fixtures;
	two_match_weeks = g_array_index(cup->rounds, CupRound, cup_round).two_match_weeks;
	clid = cup->id;
	home_advantage = (!g_array_index(cup->rounds, CupRound, cup_round).neutral);
    }   

    first_fixture = fixtures->len;

    if(first_week < 1)
	main_exit_program(EXIT_FIXTURE_WRITE_ERROR, 
			  "fixture_write_round_robin: first week of %s is not positive (%d).\nPlease lower the week gap or set a later last week.\n", 
			  league_cup_get_name_string(clid), first_week);
    
    if(len % 2 != 0)
    {
	team_temp = team_new(FALSE);
	odd_fixtures = TRUE;
	g_ptr_array_add(teams, &team_temp);
	len++;
    }

    /* Special rule for cups that have to wait for other cups. */
    if(cup_round != -1 && first_week < week)
        first_week = week;

    /* first half of fixtures */
    week_number = league_cup_get_week_with_break(clid, first_week);
    for(i=0;i<len - 1;i++)
    {
	if(i > 0 && !query_league_cup_matchday_in_two_match_week(two_match_weeks,
								 fixture_count_matchdays(fixtures) + 1))
	    week_number = league_cup_get_week_with_break(clid, week_number + week_gap);

	fixture_write_round_robin_matchday(fixtures, cup_round, teams, i,
					   week_number, clid, home_advantage);
    }

    if(!one_round)
    {
	/* second half of fixtures */
        week_number = league_cup_get_week_with_break(clid, week_number + g_array_index(rr_breaks, gint, rr_break_idx + 1));

	for(i = 0; i < len - 1; i++)
	{
	    if(i > 0 && !query_league_cup_matchday_in_two_match_week(two_match_weeks, 
								     fixture_count_matchdays(fixtures) + 1))
                week_number = league_cup_get_week_with_break(clid, week_number + week_gap);
	       
	    week_round_number = 
		fixture_get_free_round(week_number, teams, -1, -1);

	    for(j = 0; j < len / 2; j++)
		fixture_write(fixtures,
			      g_array_index(fixtures, Fixture, 
					    first_fixture + i * (len / 2) + j).teams[1],
			      g_array_index(fixtures, Fixture, 
					    first_fixture + i * (len / 2) + j).teams[0],
			      week_number, week_round_number,
			      clid, cup_round, 0, home_advantage, FALSE, FALSE);
	}
    }

    g_ptr_array_free(teams, TRUE);

    if(odd_fixtures)
    {
	for(i=fixtures->len - 1; i>=0; i--)
	    if(g_array_index(fixtures, Fixture, i).team_ids[0] == -1 ||
	       g_array_index(fixtures, Fixture, i).team_ids[1] == -1)
		g_array_remove_index(fixtures, i);

	free_team(&team_temp);
    }
}

/** Write one matchday of round robin games.
    @param fixtures The fixture array we add the fixtures to.
    @param cup_round The round of the cup the fixtures belong to
    or -1 if we have a league.
    @param teams The array containing the pointers
    of the teams participating.
    @param special The special team.
    @param week_number The week the matchday takes place.
    @param week_round_number The week_round the matchday takes place.
    @param clid The id of the league or cup.
    @param home_advantage Whether there's home advantage. */
void
fixture_write_round_robin_matchday(GArray *fixtures, gint cup_round, GPtrArray *teams,
				   gint special, gint week_number,
				   gint clid, gboolean home_advantage)
{
#ifdef DEBUG
    printf("fixture_write_round_robin_matchday\n");
#endif

    gint i;
    gint len = teams->len / 2;
    gpointer home[len], away[len];
    gint week_round_number = fixture_get_free_round(week_number, teams, -1, -1);


    home[0] = g_ptr_array_index(teams, len * 2 - 1);
    away[0] = g_ptr_array_index(teams, special);

    for(i=1;i<len;i++)
    {
	home[i] = g_ptr_array_index(teams, (special + i) % (len * 2 - 1));
	away[i] = 
	    g_ptr_array_index(teams, (special + i + (len - 1 - i) * 2 + 1) % (len * 2 - 1));
    }

    if(special % 2)
	for(i=0;i<len;i++)
	    misc_swap_gpointer(&(home[i]), &(away[i]));

    for(i=0;i<len;i++)
	fixture_write(fixtures, (Team*)home[i], (Team*)away[i], week_number, 
		      week_round_number, clid, cup_round,
		      0, home_advantage, FALSE, FALSE);
}

/** Write fixtures for a knockout round, e.g. home/away games.
    @param teams The teams participating.
    @param cup The cup the fixtures belong to.
    @param cup_round The index of the cup round. */
void
fixture_write_knockout_round(Cup *cup, gint cup_round, GPtrArray *teams)
{
#ifdef DEBUG
    printf("fixture_write_knockout_round\n");
#endif

    gint i, len = teams->len;
    gint first_week = cup_get_first_week_of_cup_round(cup, cup_round, TRUE);
    gint week_number, week_round_number;
    CupRound *round = &g_array_index(cup->rounds, CupRound, cup_round);
    gint bye_len = (round->byes == -1) ?
	math_get_bye_len(len) : round->byes;

    if(debug > 60)
	g_print("fixture_write_knockout_round: %s %d byelen %d\n",
	       cup->name, cup_round, bye_len);

    if(bye_len != 0)
    {
	cup->bye = g_ptr_array_new();

	g_ptr_array_sort_with_data(teams, team_compare_func,
				   GINT_TO_POINTER(TEAM_COMPARE_LEAGUE_LAYER));
	
	for(i=0;i<bye_len;i++)
	{
	    g_ptr_array_add(cup->bye, g_ptr_array_index(teams, 0));
	    g_ptr_array_remove_index(teams, 0);
	}

	teams = misc_randomise_g_pointer_array(teams);
    }
    else if(round->randomise_teams)
	teams = misc_randomise_g_pointer_array(teams);

    /* Special rule for cups that have to wait for other cups. */
    if(first_week < week)
        first_week = week + 1;

    week_number = league_cup_get_week_with_break(cup->id, first_week);
    week_round_number =
	fixture_get_free_round(week_number, teams, -1, -1);
    for(i=0; i<=(teams->len - 2) / 2; i++)	
	if(!round->home_away && query_league_cup_has_property(cup->id, "weak_at_home") &&
	   league_from_clid(((Team*)g_ptr_array_index(teams, 2 * i))->clid)->layer <
	   league_from_clid(((Team*)g_ptr_array_index(teams, 2 * i + 1))->clid)->layer)
	    fixture_write(cup->fixtures, (Team*)g_ptr_array_index(teams, 2 * i + 1),
			  (Team*)g_ptr_array_index(teams, 2 * i), week_number,
			  week_round_number, cup->id, cup_round, 0,
			  !round->neutral, FALSE, 
			  (!round->home_away && round->replay == 0));
	else
	    fixture_write(cup->fixtures, (Team*)g_ptr_array_index(teams, 2 * i),
			  (Team*)g_ptr_array_index(teams, 2 * i + 1), week_number,
			  week_round_number, cup->id, cup_round, 0,
			  !round->neutral, FALSE, 
			  (!round->home_away && round->replay == 0));


    if(round->home_away)
    {
	week_number = (round->two_match_week) ? 
	    week_number :
            league_cup_get_week_with_break(cup->id, week_number + cup->week_gap);

	week_round_number =
	    fixture_get_free_round(week_number, teams, -1, -1);
	for(i=0; i<=(teams->len - 2) / 2; i++)
	    fixture_write(cup->fixtures, (Team*)g_ptr_array_index(teams, 2 * i + 1),
			  (Team*)g_ptr_array_index(teams, 2 * i), week_number,
			  week_round_number, cup->id, cup_round, 0,
			  !round->neutral, TRUE, TRUE);
    }

    g_array_sort_with_data(cup->fixtures, fixture_compare_func,
			   GINT_TO_POINTER(FIXTURE_COMPARE_DATE + 100));

    cup->next_fixture_update_week = 
	(cup_round < cup->rounds->len - 1 || round->replay > 0) ?
	g_array_index(cup->fixtures, Fixture, cup->fixtures->len - 1).week_number : 1000;
    
    cup->next_fixture_update_week_round = 
	(cup_round < cup->rounds->len - 1 || round->replay > 0) ?
	g_array_index(cup->fixtures, Fixture, cup->fixtures->len - 1).week_round_number : 1000;
    
    g_ptr_array_free(teams, TRUE);
}

/** Write a fixture and append it to a fixture array.
    @param fixtures The fixture array.
    @param home_team The pointer of the home team.
    @param away_team The pointer of the away team.
    @param week_number The week it takes place.
    @param week_round_number The week_round it takes place.
    @param clid The id of the league or cup.
    @param cup_round The index of the cup round or -1 if it's a league.
    @param replay_number Whether this is a replay match and the how 'manyth' it is.
    @param home_advantage Whether there's home advantage.
    @param second_leg Whether this is the second leg of a round.
    @param decisive Whether the match has to have a winner. */
void
fixture_write(GArray *fixtures, Team *home_team, Team *away_team, gint week_number,
	      gint week_round_number, gint clid, gint cup_round, gint replay_number,
	      gboolean home_advantage, gboolean second_leg, gboolean decisive)
{
#ifdef DEBUG
    printf("fixture_write\n");
#endif

    gint i;
    Fixture new;

    new.id = fixture_id_new;
    new.clid = clid;
    new.round = cup_round;
    new.replay_number = replay_number;
    new.week_number = week_number;
    new.week_round_number = week_round_number;
    new.teams[0] = home_team;
    new.teams[1] = away_team;
    new.team_ids[0] = home_team->id;
    new.team_ids[1] = away_team->id;
    new.live_game = NULL;
    
    for(i=0;i<3;i++)
	new.result[0][i] = new.result[1][i] = 0;

    new.home_advantage = home_advantage;
    new.second_leg = second_leg;
    new.decisive = decisive;
    new.attendance = -1;

    g_array_append_val(fixtures, new);
}

/** Return the round of the week which doesn't contain
    any matches yet featuring teams from the team list or
    with the given ids.
    @param week_number The week we examine.
    @param teams The participating in the round or NULL.
    @param team_id1, team_id1 Ids of the teams (if the team list is NULL) playing
    against each other.
    @return A round number. If the clid belongs to a league this is always 1. */
gint
fixture_get_free_round(gint week_number, const GPtrArray *teams, gint team_id1, gint team_id2)
{
#ifdef DEBUG
    printf("fixture_get_free_round\n");
#endif

    gint i, j;
    gint max_round = 0;

    for(i=0;i<ligs->len;i++)
	for(j=0;j<lig(i).fixtures->len;j++)
	    if(g_array_index(lig(i).fixtures, Fixture, j).week_number == week_number &&
	       (stat5 != STATUS_GENERATE_TEAMS ||
		g_array_index(lig(i).fixtures, Fixture, j).attendance == -1) &&
	       ((teams == NULL &&
		(query_fixture_team_involved((&g_array_index(lig(i).fixtures, Fixture, j)), team_id1) ||
		 query_fixture_team_involved((&g_array_index(lig(i).fixtures, Fixture, j)), team_id2))) || 
		(teams != NULL &&
		 (query_team_is_in_teams_array(g_array_index(lig(i).fixtures, Fixture, j).teams[0], teams) ||
		  query_team_is_in_teams_array(g_array_index(lig(i).fixtures, Fixture, j).teams[1], teams)))) &&
	       g_array_index(lig(i).fixtures, Fixture, j).week_round_number > max_round)
		max_round = g_array_index(lig(i).fixtures, Fixture, j).week_round_number;
    
    for(i=0;i<acps->len;i++)
	for(j=0;j<acp(i)->fixtures->len;j++)
	    if(g_array_index(acp(i)->fixtures, Fixture, j).week_number == week_number &&
	       (stat5 != STATUS_GENERATE_TEAMS ||
		g_array_index(acp(i)->fixtures, Fixture, j).attendance == -1) &&
	       ((teams == NULL &&
		(query_fixture_team_involved((&g_array_index(acp(i)->fixtures, Fixture, j)), team_id1) ||
		 query_fixture_team_involved((&g_array_index(acp(i)->fixtures, Fixture, j)), team_id2))) || 
		(teams != NULL &&
		 (query_team_is_in_teams_array(g_array_index(acp(i)->fixtures, Fixture, j).teams[0], teams) ||
		  query_team_is_in_teams_array(g_array_index(acp(i)->fixtures, Fixture, j).teams[1], teams)))) &&
	       g_array_index(acp(i)->fixtures, Fixture, j).week_round_number > max_round)
		max_round = g_array_index(acp(i)->fixtures, Fixture, j).week_round_number;
    
    return max_round + 1;
}

/** Check whether the date of a fixture is earlier 
    than of a second one.
    @param fix1 The first fixture.
    @param fix2 The second fixture.
    @return TRUE if fix1 is earlier than fix2, FALSE otherwise. */
gboolean
query_fixture_is_earlier(const Fixture *fix1, const Fixture *fix2)
{
#ifdef DEBUG
    printf("query_fixture_is_earlier\n");
#endif

    return (fix1->week_number < fix2->week_number ||
	    (fix1->week_number == fix2->week_number && 
	     fix1->week_round_number < fix2->week_round_number));
}

/** Check whether the date of a fixture is later 
    than of a second one.
    @param fix1 The first fixture.
    @param fix2 The second fixture.
    @return TRUE if fix1 is later than fix2, FALSE otherwise. */
gboolean
query_fixture_is_later(const Fixture *fix1, const Fixture *fix2)
{
#ifdef DEBUG
    printf("query_fixture_is_later\n");
#endif

    return (fix1->week_number > fix2->week_number ||
	    (fix1->week_number == fix2->week_number && 
	     fix1->week_round_number > fix2->week_round_number));
}

/** Find out whether the current result of the fixture
    is a draw. This is a bit tricky because of second-leg games.
    In general (as a definition), fixtures with 'decisive' FALSE
    are never drawn.
    @param fix The pointer to the fixture.
    @return TRUE or FALSE. */
gboolean
query_fixture_is_draw(const Fixture *fix)
{
#ifdef DEBUG
    printf("query_fixture_is_draw\n");
#endif

    const Fixture *first_leg;

    if(!fix->decisive)
	return FALSE;

    if(!fix->second_leg)
	return (fix->result[0][0] + fix->result[0][1] ==
		fix->result[1][0] + fix->result[1][1]);

    first_leg = fixture_get_first_leg(fix, FALSE);

    return (fix->result[0][1] + fix->result[1][1] == 0 &&
	    fix->result[0][0] == first_leg->result[0][0] &&
	    fix->result[1][0] == first_leg->result[1][0]);
}

/** Check whether a user's team participates.
    @param fix The fixture we check.
    @return Index of the user or -1. */
gint
fixture_user_team_involved(const Fixture *fix)
{
#ifdef DEBUG
    printf("fixture_user_team_involved\n");
#endif

    if(team_is_user(fix->teams[0]) != -1)
	return team_is_user(fix->teams[0]);

    if(team_is_user(fix->teams[1]) != -1)
	return team_is_user(fix->teams[1]);
    
    return -1;
}

/** Check whether we have to update a table
    after calculating the fixture.
    @param fix The fixture that got calculated.
    @return TRUE or FALSE. */
gboolean
query_fixture_has_tables(const Fixture *fix)
{
#ifdef DEBUG
    printf("query_fixture_has_tables\n");
#endif

    return (fix->clid < ID_CUP_START ||
	    cup_has_tables(fix->clid) == fix->round);
}

/** Find out whether there were games in the specified league
    or cup at the specified time.
    @param clid The cup or league id.
    @param week_number The week number.
    @param week_round_number The week round. 
    @return TRUE or FALSE. */
gboolean
query_fixture_in_week_round(gint clid, gint week_number, gint week_round_number)
{
#ifdef DEBUG
    printf("query_fixture_in_week_round\n");
#endif

    gint i;
    GArray *fixtures = league_cup_get_fixtures(clid);

    for(i=0;i<fixtures->len;i++)
	if(g_array_index(fixtures, Fixture, i).week_number == week_number &&
	   g_array_index(fixtures, Fixture, i).week_round_number == week_round_number)
	    return TRUE;

    return FALSE;
}

/** Return a pointer to the first leg going with
 the fixture.
 @param fix The second leg fixture.
 @param silent Whether to complain about a missing first leg.
 @return A fixture pointer or NULL if failed. */
Fixture*
fixture_get_first_leg(const Fixture *fix, gboolean silent)
{
#ifdef DEBUG
    printf("fixture_get_first_leg\n");
#endif

    gint i;
    Fixture *first_leg = NULL;
    const GArray *fixtures = cup_from_clid(fix->clid)->fixtures;

    if(g_array_index(cup_from_clid(fix->clid)->rounds, CupRound, fix->round).round_robin_number_of_groups == 0)
        for(i=0;i<fixtures->len;i++)
            if(g_array_index(fixtures, Fixture, i).round == fix->round &&
               g_array_index(fixtures, Fixture, i).team_ids[0] == fix->team_ids[1] &&
               g_array_index(fixtures, Fixture, i).team_ids[1] == fix->team_ids[0])
                first_leg = &g_array_index(fixtures, Fixture, i);

    if(first_leg == NULL && !silent)
	g_warning("fixture_get_first_leg: didn't find first leg match; cup %s round %d\n",
		  cup_from_clid(fix->clid)->name, fix->round);

    return first_leg;
}

/** Return a list of fixture pointers.
    @param clid The cup or league id.
    @param week_number The week number of the fixtures.
    @param week_round_number The week round of the fixtures. */
GPtrArray*
fixture_get_week_list_clid(gint clid, gint week_number, gint week_round_number)
{
#ifdef DEBUG
    printf("fixture_get_week_list_clid\n");
#endif

    gint i;
    GArray *fixtures = league_cup_get_fixtures(clid);
    GPtrArray *fixtures_array = g_ptr_array_new();

    for(i=0;i<fixtures->len;i++)
	if(g_array_index(fixtures, Fixture, i).week_number == week_number &&
	   g_array_index(fixtures, Fixture, i).week_round_number == week_round_number)
	    g_ptr_array_add(fixtures_array, &g_array_index(fixtures, Fixture, i));

    return fixtures_array;
}

/** Return an array with all fixtures in a given week. */
GPtrArray*
fixture_get_week_list(gint week_number, gint week_round_number)
{
#ifdef DEBUG
    printf("fixture_get_week_list\n");
#endif

    gint i, j;
    GPtrArray *fixtures = g_ptr_array_new();

    for(i=0;i<ligs->len;i++)
	for(j=0;j<lig(i).fixtures->len;j++)
	    if(g_array_index(lig(i).fixtures, Fixture, j).week_number == week_number &&
	       g_array_index(lig(i).fixtures, Fixture, j).week_round_number == week_round_number)
		g_ptr_array_add(fixtures, &g_array_index(lig(i).fixtures, Fixture, j));

    for(i=0;i<acps->len;i++)
	for(j=0;j<acp(i)->fixtures->len;j++)
	    if(g_array_index(acp(i)->fixtures, Fixture, j).week_number == week_number &&
	       g_array_index(acp(i)->fixtures, Fixture, j).week_round_number == week_round_number)
		g_ptr_array_add(fixtures, &g_array_index(acp(i)->fixtures, Fixture, j));

    return fixtures;
}

/** Print the result of the fixture into a buffer.
    @param swap Whether to swap the two teams' goals. */
void
fixture_result_to_buf(const Fixture *fix, gchar *buf, gboolean swap)
{
#ifdef DEBUG
    printf("fixture_result_to_buf\n");
#endif

    gchar local_buf[SMALL];
    gint idx0 = 0, idx1 = 1;

    if(swap)
    {
	idx0 = 1;
	idx1 = 0;
    }

    if(fix->attendance < 0)
	strcpy(buf, "-- : --");
    else
    {
	sprintf(local_buf, "%d - %d", math_sum_int_array(fix->result[idx0], 3),
		math_sum_int_array(fix->result[idx1], 3));
	if(fix->result[idx0][2] + fix->result[idx1][2] != 0)
	    /* Game was decided in penalty shoot-out. */
	    strcat(local_buf, _(" p."));
	else if(fix->result[idx0][1] + fix->result[idx1][1] != 0)
	    /* Game was decided in extra time. */
	    strcat(local_buf, _(" e.t."));

	if(fix->second_leg)
	    sprintf(buf, "%s (%d - %d)", local_buf,
		    fixture_get_first_leg(fix, FALSE)->result[idx1][0],
		    fixture_get_first_leg(fix, FALSE)->result[idx0][0]);
	else
	    strcpy(buf, local_buf);
    }
}

/** Return the number of fixtures in a given week round.
    @param week_number The week number the fixtures should have.
    @param week_round_number The week round number the fixtures should have.
    @return The number of matches. */
gint
fixture_get_number_of_matches(gint week_number, gint week_round_number)
{
#ifdef DEBUG
    printf("fixture_get_number_of_matches\n");
#endif

    gint i, j;
    gint sum = 0;

    for(i=0;i<acps->len;i++)
	for(j=0;j<acp(i)->fixtures->len;j++)
	    if(g_array_index(acp(i)->fixtures, Fixture, j).week_number == week_number &&
	       g_array_index(acp(i)->fixtures, Fixture, j).week_round_number == week_round_number)
		sum++;

    for(i=0;i<ligs->len;i++)
	if(week_round_number == 1)
	{
	    for(j=0;j<lig(i).fixtures->len;j++)
		if(g_array_index(lig(i).fixtures, Fixture, j).week_number == week_number &&
		   g_array_index(lig(i).fixtures, Fixture, j).week_round_number == week_round_number)
		    sum++;
	}
    
    return sum;
}

/** Return a fixture from the following match-day.
    @param clid The cup/league id.
    @param week_number The current week.
    @param week_round_number The current round.
    @return A fixture pointer. */
Fixture*
fixture_get_next(gint clid, gint week_number, gint week_round_number)
{
#ifdef DEBUG
    printf("fixture_get_next\n");
#endif

    gint i;
    GArray *fixtures = league_cup_get_fixtures(clid);

    for(i=0;i<fixtures->len;i++)
	if(g_array_index(fixtures, Fixture, i).week_number > week_number ||
	   (g_array_index(fixtures, Fixture, i).week_number == week_number &&
	    g_array_index(fixtures, Fixture, i).week_round_number > week_round_number))
	    return &g_array_index(fixtures, Fixture, i);

    return &g_array_index(fixtures, Fixture, 0);
}

/** Return a fixture from the previous match-day.
    @param clid The cup/league id.
    @param week_number The current week.
    @param week_round_number The current round.
    @return A fixture pointer. */
Fixture*
fixture_get_previous(gint clid, gint week_number, gint week_round_number)
{
#ifdef DEBUG
    printf("fixture_get_previous\n");
#endif

    gint i;
    GArray *fixtures = league_cup_get_fixtures(clid);

    for(i=fixtures->len - 1;i>=0;i--)
	if(g_array_index(fixtures, Fixture, i).week_number < week_number ||
	   (g_array_index(fixtures, Fixture, i).week_number == week_number &&
	    g_array_index(fixtures, Fixture, i).week_round_number < week_round_number))
	    return &g_array_index(fixtures, Fixture, i);

    return &g_array_index(fixtures, Fixture, fixtures->len - 1);
}

/** Find a certain fixture for displaying in the right treeview.
    @param type Type telling us how to look for the fixture (@see #ShowFixType)
    @param clid The id of the current league/cup we're showing.
    @param week_number The week we're showing.
    @param week_round_number The round we're showing.
    @param tm A team pointer (for the case SHOW_TEAM).
    @return A fixture pointer or NULL. */
Fixture*
fixture_get(gint type, gint clid, gint week_number, 
	    gint week_round_number, const Team *tm)
{
#ifdef DEBUG
    printf("fixture_get\n");
#endif

    Fixture *fix = NULL;
    gint new_clid = -1;

    if(type == SHOW_TEAM)
    {
	fix = team_get_fixture(tm, TRUE);
	if(fix == NULL)
	    fix = team_get_fixture(tm, FALSE);
    }
    else if(type == SHOW_CURRENT)
    {
	fix = league_cup_get_previous_fixture(clid, week_number, week_round_number);
	if(fix == NULL)
	    fix = league_cup_get_next_fixture(clid, week_number, week_round_number);
    }
    else if(type == SHOW_NEXT)
	fix = fixture_get_next(clid, week_number, week_round_number);
    else if(type == SHOW_PREVIOUS)
	    fix = fixture_get_previous(clid, week_number, week_round_number);
    else if(type == SHOW_NEXT_LEAGUE ||
	    type == SHOW_PREVIOUS_LEAGUE)
    {
	    new_clid = (type == SHOW_NEXT_LEAGUE) ?
		league_cup_get_next_clid(clid, FALSE):
		league_cup_get_previous_clid(clid, FALSE);

	    fix = fixture_get(SHOW_CURRENT, new_clid, week, week_round, NULL);
    }

    if(fix == NULL)
	g_warning("fixture_get: no fixture found for type %d clid %d (%s) week %d round %d\n",
		  type, clid, league_cup_get_name_string(clid), week_number, week_round_number);

    return fix;
}

/** Compare two fixtures. */
gint
fixture_compare_func(gconstpointer a, gconstpointer b, gpointer data)
{
#ifdef DEBUG
    printf("fixture_compare_func\n");
#endif

    gint local_data = GPOINTER_TO_INT(data);
    const Fixture *fix1 = (local_data >= 100) ? 
	(const Fixture*)a : *(const Fixture**)a,
	*fix2 = (local_data >= 100) ? 
	(const Fixture*)b : *(const Fixture**)b;
    gint type = local_data % 100;
    gint return_value = 0;

    switch(type)
    {
	default:
	    g_warning("fixture_compare_func: unknown type %d.\n", type);
	    break;
	case FIXTURE_COMPARE_DATE:
	    if(query_fixture_is_earlier(fix1, fix2))
		return_value = -1;
	    else
		return_value = 1;
	    break;
    }

    return return_value;
}

/** Return an array with the last fixtures of the team. */
GPtrArray*
fixture_get_latest(const Team *tm, gboolean with_cups)
{
#ifdef DEBUG
    printf("fixture_get_latest\n");
#endif

    gint i, j;
    GPtrArray *latest = g_ptr_array_new();

    for(i=0;i<ligs->len;i++)
	if(lig(i).id == tm->clid)
	{
	    for(j=0;j<lig(i).fixtures->len;j++)
		if(g_array_index(lig(i).fixtures, Fixture, j).attendance == -1)
		    break;
		else if(g_array_index(lig(i).fixtures, Fixture, j).teams[0] == tm ||
			g_array_index(lig(i).fixtures, Fixture, j).teams[1] == tm)
		    g_ptr_array_add(latest, &g_array_index(lig(i).fixtures, Fixture, j));
	}

    if(with_cups)
    {
	for(i=0;i<acps->len;i++)
	{
	    for(j=0;j<acp(i)->fixtures->len;j++)
		if(g_array_index(acp(i)->fixtures, Fixture, j).attendance == -1)
		    break;
		else if(g_array_index(acp(i)->fixtures, Fixture, j).teams[0] == tm ||
			g_array_index(acp(i)->fixtures, Fixture, j).teams[1] == tm)
		    g_ptr_array_add(latest, &g_array_index(acp(i)->fixtures, Fixture, j));
	}	
    }

    g_ptr_array_sort_with_data(latest, fixture_compare_func,
			       GINT_TO_POINTER(FIXTURE_COMPARE_DATE));

    return latest;
}

/** Return an array with the next fixtures of the team. */
GPtrArray*
fixture_get_coming(const Team *tm)
{
#ifdef DEBUG
    printf("fixture_get_coming\n");
#endif

    gint i, j;
    GPtrArray *coming = g_ptr_array_new();

    for(i=0;i<ligs->len;i++)
	if(lig(i).id == tm->clid)
	{
	    for(j=lig(i).fixtures->len - 1; j >= 0; j--)
		if(g_array_index(lig(i).fixtures, Fixture, j).attendance != -1)
		    break;
		else if(g_array_index(lig(i).fixtures, Fixture, j).teams[0] == tm ||
			g_array_index(lig(i).fixtures, Fixture, j).teams[1] == tm)
		    g_ptr_array_add(coming, &g_array_index(lig(i).fixtures, Fixture, j));
	}
    
    for(i=0;i<acps->len;i++)
    {
	for(j=acp(i)->fixtures->len - 1; j >= 0; j--)
	    if(g_array_index(acp(i)->fixtures, Fixture, j).attendance != -1)
		break;
	    else if(g_array_index(acp(i)->fixtures, Fixture, j).teams[0] == tm ||
		    g_array_index(acp(i)->fixtures, Fixture, j).teams[1] == tm)
		g_ptr_array_add(coming, &g_array_index(acp(i)->fixtures, Fixture, j));
    }

    g_ptr_array_sort_with_data(coming, fixture_compare_func, 
			       GINT_TO_POINTER(FIXTURE_COMPARE_DATE));

    return coming;
}

/** Return a pointer array with all the matches featuring the two teams. */
GPtrArray*
fixture_get_matches(const Team *tm1, const Team *tm2)
{
#ifdef DEBUG
    printf("fixture_get_matches\n");
#endif

    gint i, j;
    GPtrArray *matches = g_ptr_array_new();

    for(i=0;i<ligs->len;i++)
	if(lig(i).id == tm1->clid)
	    for(j=0;j<lig(i).fixtures->len;j++)
	    {
		if(g_array_index(lig(i).fixtures, Fixture, j).attendance == -1)
		    break;
		else if((g_array_index(lig(i).fixtures, Fixture, j).teams[0] == tm1 &&
			 g_array_index(lig(i).fixtures, Fixture, j).teams[1] == tm2) ||
			(g_array_index(lig(i).fixtures, Fixture, j).teams[0] == tm2 &&
			 g_array_index(lig(i).fixtures, Fixture, j).teams[1] == tm1))
		    g_ptr_array_add(matches, &g_array_index(lig(i).fixtures, Fixture, j));
	    }

    for(i=0;i<acps->len;i++)
	for(j=0;j<acp(i)->fixtures->len;j++)
	{
	    if(g_array_index(acp(i)->fixtures, Fixture, j).attendance == -1)
		break;
	    else if((g_array_index(acp(i)->fixtures, Fixture, j).teams[0] == tm1 &&
		     g_array_index(acp(i)->fixtures, Fixture, j).teams[1] == tm2) ||
		    (g_array_index(acp(i)->fixtures, Fixture, j).teams[0] == tm2 &&
		     g_array_index(acp(i)->fixtures, Fixture, j).teams[1] == tm1))
		g_ptr_array_add(matches, &g_array_index(acp(i)->fixtures, Fixture, j));
	}

    return matches;
}

/** Return the fixture going with the id. */
Fixture*
fixture_from_id(gint id, gboolean abort_program)
{
#ifdef DEBUG
    printf("fixture_from_id\n");
#endif

    gint i, j;

    for(i=0;i<ligs->len;i++)
	for(j=0;j<lig(i).fixtures->len;j++)
	    if(g_array_index(lig(i).fixtures, Fixture, j).id == id)
		return &g_array_index(lig(i).fixtures, Fixture, j);

    for(i=0;i<cps->len;i++)
	for(j=0;j<cp(i).fixtures->len;j++)
	    if(g_array_index(cp(i).fixtures, Fixture, j).id == id)
		return &g_array_index(cp(i).fixtures, Fixture, j);

    if(abort_program)
	main_exit_program(EXIT_POINTER_NOT_FOUND, 
			  "fixture_from_id: fixture with id %d found \n", id);

    return NULL;
}

/** Return the last week round number of the specified week. */
gint
fixture_get_last_week_round(gint week_number)
{
#ifdef DEBUG
    printf("fixture_get_last_week_round\n");
#endif

    gint i, j;
    gint week_round_number = 1;

    for(i=0;i<acps->len;i++)
	for(j=0;j<acp(i)->fixtures->len;j++)
	    if(g_array_index(acp(i)->fixtures, Fixture, j).week_number == week_number)
		week_round_number = MAX(week_round_number, 
					g_array_index(acp(i)->fixtures, Fixture, j).week_round_number);

    return week_round_number;
}

/** Get the next week and round there are fixtures from
    the specified ones onwards and write it into the variables. */
void
fixture_get_next_week(gint *week_number, gint *week_round_number)
{
#ifdef DEBUG
    printf("fixture_get_next_week\n");
#endif

    gint i;
    gint local_week = *week_number,
	local_round = *week_round_number;
    Fixture *fix = NULL;

    *week_number = *week_round_number = 1000;

    for(i=0;i<ligs->len;i++)
	if(query_league_active(&lig(i)))
	{
	    fix = fixture_get_next(lig(i).id, local_week, local_round);
	    if((fix->week_number > local_week ||
		(fix->week_number == local_week && 
		 fix->week_round_number > local_round)) &&
	       (fix->week_number < *week_number ||
		(fix->week_number == *week_number && 
		 fix->week_round_number < *week_round_number)) &&
	       (fix->clid == current_user.tm->clid || 
		opt_user_int("int_opt_user_show_all_leagues")))
	    {
		*week_number = fix->week_number;
		*week_round_number = fix->week_round_number;
	    }	    
	}

    for(i=0;i<acps->len;i++)
    {
	fix = fixture_get_next(acp(i)->id, local_week, local_round);
	if((fix->week_number > local_week ||
	   (fix->week_number == local_week && fix->week_round_number > local_round)) &&
	   (fix->week_number < *week_number ||
	    (fix->week_number == *week_number && fix->week_round_number < *week_round_number)))
	{
	    *week_number = fix->week_number;
	    *week_round_number = fix->week_round_number;
	}
    }

    if(*week_number == 1000)
	*week_number = *week_round_number = 1;
}

/** Get the previous week and round there are fixtures
    and write it into the variables. */
void
fixture_get_previous_week(gint *week_number, gint *week_round_number)
{
#ifdef DEBUG
    printf("fixture_get_previous_week\n");
#endif

    gint i;
    gint local_week = *week_number,
	local_round = *week_round_number;
    Fixture *fix = NULL;

    *week_number = *week_round_number = -1;

    for(i=0;i<ligs->len;i++)
	if(query_league_active(&lig(i)))
	{
	    fix = fixture_get_previous(lig(i).id, local_week, local_round);
	    if((fix->week_number < local_week ||
		(fix->week_number == local_week && 
		 fix->week_round_number < local_round)) &&
	       (fix->week_number > *week_number ||
		(fix->week_number == *week_number && 
		 fix->week_round_number > *week_round_number)) &&
	       (fix->clid == current_user.tm->clid || 
		opt_user_int("int_opt_user_show_all_leagues")))
	    {
		*week_number = fix->week_number;
		*week_round_number = 1;
	    }
	}

    for(i=0;i<acps->len;i++)
    {
	fix = fixture_get_previous(acp(i)->id, local_week, local_round);
	if((fix->week_number < local_week ||
	   (fix->week_number == local_week && fix->week_round_number < local_round)) &&
	   (fix->week_number > *week_number ||
	    (fix->week_number == *week_number && fix->week_round_number > *week_round_number)))
	{
	    *week_number = fix->week_number;
	    *week_round_number = fix->week_round_number;
	}
    }

    if(*week_number == -1)
    {
	*week_number = local_week;
	*week_round_number = local_round;
    }
}

/** Return an array with the user's fixtures in this season. */
GPtrArray*
fixture_get_season_results(void)
{
#ifdef DEBUG
    printf("fixture_get_season_results\n");
#endif

    gint i, j;
    GPtrArray *results = g_ptr_array_new();
    GArray *fixtures = NULL;
    
    if(query_league_active(league_from_clid(current_user.tm->clid)))
    {
	fixtures = league_from_clid(current_user.tm->clid)->fixtures;
	for(i=0;i<fixtures->len;i++)
	    if(query_fixture_team_involved((&g_array_index(fixtures, Fixture, i)),
					   current_user.tm->id))
		g_ptr_array_add(results, &g_array_index(fixtures, Fixture, i));
    }

    for(i=0;i<acps->len;i++)
	if(query_team_is_in_cup(current_user.tm, acp(i)))
	{
	    fixtures = acp(i)->fixtures;
	    for(j=0;j<fixtures->len;j++)
		if(query_fixture_team_involved((&g_array_index(fixtures, Fixture, j)),
					       current_user.tm->id))
		    g_ptr_array_add(results, &g_array_index(fixtures, Fixture, j));
	}

    g_ptr_array_sort_with_data(results, fixture_compare_func,
			       GINT_TO_POINTER(FIXTURE_COMPARE_DATE));

    return results;
}

/** Calculate how many goals the team has to score to
    win the match (also taking into account second leg stuff). */
gint
fixture_get_goals_to_win(const Fixture *fix, const Team *tm)
{
#ifdef DEBUG
    printf("fixture_get_goals_to_win\n");
#endif

    const Fixture *first_leg = NULL;
    gint idx = (tm == fix->teams[1]), return_value = -100;
    gint res[2] = {math_sum_int_array(fix->result[0], 2),
		   math_sum_int_array(fix->result[1], 2)};
    gint tmp;

    if(!query_fixture_team_involved(fix, tm->id))
    {
	g_warning("fixture_get_goals_to_win: team %s doesn't participate in fixture given (%s)\n",
		  tm->name, league_cup_get_name_string(fix->clid));
	return -100;
    }

    if(!fix->second_leg)
	return res[!idx] - res[idx] + 1;

    first_leg = fixture_get_first_leg(fix, FALSE);

    if(res[0] + first_leg->result[1][0] ==
       res[1] + first_leg->result[0][0])
    {
	if(idx == 0)
	    return_value = (res[1] >= first_leg->result[1][0]) ?
		1 : 0;
	else
	    return_value = (res[1] > first_leg->result[1][0]) ?
		0 : 1;
    }
    else if(res[0] + first_leg->result[1][0] >
       res[1] + first_leg->result[0][0])
    {
	tmp = res[1];
	while(res[0] + first_leg->result[1][0] >
	      res[1] + first_leg->result[0][0])
	    res[1]++;

	if(idx == 0)
	    return_value = (res[1] >= first_leg->result[1][0]) ?
		tmp - res[1] + 1 : tmp - res[1];
	else
	    return_value = (res[1] > first_leg->result[1][0]) ?
		res[1] - tmp : res[1] - tmp + 1;
    }
    else
    {
	tmp = res[0];
	while(res[0] + first_leg->result[1][0] <
	      res[1] + first_leg->result[0][0])
	    res[0]++;

	if(idx == 0)
	    return_value = (res[1] >= first_leg->result[1][0]) ?
		res[0] - tmp + 1 : res[0] - tmp;
	else
	    return_value = (res[1] > first_leg->result[1][0]) ?
		tmp - res[0] : tmp - res[0] + 1;
    }

    return return_value;
}

/** Count how many matchdays there are in 
    a fixture array. */
gint
fixture_count_matchdays(const GArray *fixtures)
{
#ifdef DEBUG
    printf("fixture_count_matchdays\n");
#endif

    gint i;
    gint count = 0;

    for(i=0;i<fixtures->len - 1;i++)
    {
	if((g_array_index(fixtures, Fixture, i).week_number != 
	    g_array_index(fixtures, Fixture, i + 1).week_number) ||
	   (g_array_index(fixtures, Fixture, i).week_round_number != 
	    g_array_index(fixtures, Fixture, i + 1).week_round_number))
	    count++;
    }

    return count + (fixtures->len != 0);
}

/** Return the last week there are fixtures written for. */
gint
fixture_get_last_scheduled_week(void)
{
#ifdef DEBUG
    printf("fixture_get_last_scheduled_week\n");
#endif

    gint i;
    gint week_nr = 0;

    for(i=0;i<acps->len;i++)
        week_nr = MAX(week_nr, g_array_index(acp(i)->fixtures, Fixture, acp(i)->fixtures->len - 1).week_number);

    return week_nr;
}

/** Remove a number of round robin from a fixture array.
    @fixtures: The fixture array.
    @clid1: The league id for one team.
    @clid2: The league id for the second team.
    @to_remove: How many matches to remove per pairing. */
void
fixture_remove_rrs(GArray *fixtures, gint clid1, gint clid2, gint to_remove)
{
#ifdef DEBUG
    printf("fixture_remove_rrs\n");
#endif

    gint i, j, k;
    gint removed;
    gint start;
    League *league1, *league2;

    league1 = league_from_clid(clid1);
    league2 = league_from_clid(clid2);

    for(i = 0; i < league1->teams->len; i++)
    {
        start = (clid1 == clid2) ? i + 1 : 0;
        for(j = start; j < league2->teams->len; j++)
        {
            removed = 0;
            for(k = fixtures->len - 1; k >= 0; k--)
                if((g_array_index(fixtures, Fixture, k).teams[0]->id == g_array_index(league1->teams, Team, i).id &&
                    g_array_index(fixtures, Fixture, k).teams[1]->id == g_array_index(league2->teams, Team, j).id) ||
                   (g_array_index(fixtures, Fixture, k).teams[1]->id == g_array_index(league1->teams, Team, i).id &&
                    g_array_index(fixtures, Fixture, k).teams[0]->id == g_array_index(league2->teams, Team, j).id))
                {
                    g_array_remove_index(fixtures, k);
                    removed++;
                    
                    if(removed == to_remove)
                        break;
                }
        }
    }
}

/** Move matches in the fixture array to earlier dates if possible. */
void
fixtures_condense(GArray *fixtures)
{
#ifdef DEBUG
    printf("fixtures_condense\n");
#endif

    gint i, j, k, l;
    GArray *matchday_teams[1000][5];
    Fixture *fix;
    gboolean fix_moved;

    fix_moved = TRUE;
    
    while(fix_moved)
    {
        fix_moved = FALSE;

        for(i = 0; i < 1000; i++)
            for(j = 0; j < 5; j++)
                matchday_teams[i][j] = NULL;

        for(i = 0; i < ligs->len; i++)
            for(j = 0; j < lig(i).fixtures->len; j++)
            {
                fix = &g_array_index(lig(i).fixtures, Fixture, j);
                if(matchday_teams[fix->week_number - 1][fix->week_round_number - 1] == NULL)
                    matchday_teams[fix->week_number - 1][fix->week_round_number - 1] = g_array_new(FALSE, FALSE, sizeof(gint));

                g_array_append_val(matchday_teams[fix->week_number - 1][fix->week_round_number - 1], fix->teams[0]->id);
                g_array_append_val(matchday_teams[fix->week_number - 1][fix->week_round_number - 1], fix->teams[1]->id);
            }
    

        for(i = fixtures->len - 1; i >= 0; i--)
        {
            fix = &g_array_index(fixtures, Fixture, i);
            for(j = 0; j <= fix->week_number - 2; j++)
            {
                for(k = 0; k < 5; k++)
                {
                    if(matchday_teams[j][k] != NULL)
                    {
                        for(l = 0; l < matchday_teams[j][k]->len; l++)
                            if(g_array_index(matchday_teams[j][k], gint, l) == fix->teams[0]->id ||
                               g_array_index(matchday_teams[j][k], gint, l) == fix->teams[1]->id)
                                break;

                        if(l == matchday_teams[j][k]->len)
                        {
                            fix_moved = TRUE;
                            fix->week_number = j + 1;
                            fix->week_round_number = k + 1;
                            g_array_append_val(matchday_teams[j][k], fix->teams[0]->id);
                            g_array_append_val(matchday_teams[j][k], fix->teams[1]->id);
                        }
                    }
                }
            }
        }

        for(i = 0; i < 1000; i++)
            for(j = 0; j < 5; j++)
                if(matchday_teams[i][j] != NULL)
                    g_array_free(matchday_teams[i][j], TRUE);

        g_array_sort_with_data(fixtures, fixture_compare_func,
                               GINT_TO_POINTER(FIXTURE_COMPARE_DATE + 100));
    }
}

/** Return the name of a cup round, e.g. 'round robin'
    or 'final' or so.
    @param fix A fixture belonging to the round.
    @param buf The buffer we write the name into*/
void
fixture_get_cup_round_name(const Fixture *fix, gchar *buf)
{
#ifdef DEBUG
    printf("cup_round_name\n");
#endif

    const Cup *cup = cup_from_clid(fix->clid);
    const CupRound *cup_round = 
	&g_array_index(cup->rounds, CupRound, fix->round);

    cup_get_round_name(cup, fix->round, buf);

    if(cup_round->home_away && cup_round->round_robin_number_of_groups == 0)
    {
	if(fix->second_leg)
	    strcat(buf, _(" -- Second leg"));
	else
	    strcat(buf, _(" -- First leg"));
    }
    else if(fix->replay_number > 0)
	strcat(buf, _(" -- Replay matches"));
}

/* Reset the team pointers from the team ids in the fixtures. */
void
fixture_refresh_team_pointers(GArray *fixtures)
{
    gint i, j;

    for(i = 0; i < fixtures->len; i++)
    {
        for(j = 0; j < 2; j++)
            g_array_index(fixtures, Fixture, i).teams[j] =
                team_of_id(g_array_index(fixtures, Fixture, i).team_ids[j]);
    }
}
