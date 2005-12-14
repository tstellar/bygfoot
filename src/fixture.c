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
    gint round_robins = league->round_robins;
    GPtrArray *teams = NULL;
    
    g_array_free(league->fixtures, TRUE);
    league->fixtures = g_array_new(FALSE, FALSE, sizeof(Fixture));

    while(round_robins > 0)
    {
	teams = team_get_pointers_from_array(league->teams);
	fixture_write_round_robin((gpointer)league, -1, teams, (round_robins == 1));
	round_robins -= (round_robins > 1) ? 2 : 1;
    }

    g_array_sort_with_data(league->fixtures, fixture_compare_func, 
			   GINT_TO_POINTER(FIXTURE_COMPARE_DATE + 100));
}

/** Write the fixtures for the given cup
    at the beginning of a new season. 
    @param cup The cup we write the fixtures for. */
void
fixture_write_cup_fixtures(Cup *cup)
{
    gint i;

    for(i=0;i<cup->rounds->len;i++)
	cup_get_team_pointers(cup, i);

    if(g_array_index(cup->rounds, CupRound, 0).round_robin_number_of_groups > 0)
	fixture_write_cup_round_robin(
	    cup, 0, misc_copy_ptr_array(
		g_array_index(cup->rounds, CupRound, 0).team_ptrs));
    else
	fixture_write_knockout_round(
	    cup, 0, misc_copy_ptr_array(
		g_array_index(cup->rounds, CupRound, 0).team_ptrs));
}

/** Update the fixtures for the given cup. 
    @param cup The cup we update. */
void
fixture_update(Cup *cup)
{
    gint i;
    GArray *fixtures = cup->fixtures;
    gint round = g_array_index(fixtures, Fixture, fixtures->len - 1).round;
    gint replay = g_array_index(cup->rounds, CupRound, round).replay;
    GPtrArray *teams = NULL;
    const CupRound *new_round = NULL;

    if(replay != 0 && 
       g_array_index(fixtures, Fixture, fixtures->len - 1).replay_number < replay && 
       fixture_update_write_replays(cup))
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
	main_exit_program(EXIT_CUP_ROUND_ERROR, 
			  "fixture_update: round index %d too high for round array (%d) in cup %s\n",
			  round + 1, cup->rounds->len - 1, cup->name);
    }

    new_round = &g_array_index(cup->rounds, CupRound, round + 1);

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

    if(cupround->replay != 0 || !cupround->home_away)
	winner_idx = (math_sum_int_array(&(fix->result[0][0]), 3) <
		      math_sum_int_array(&(fix->result[1][0]), 3));
    else
    {
	first_leg = fixture_get_first_leg(fix);
	
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
    gint i, j, cnt;
    CupRound *cupround = &g_array_index(cup->rounds, CupRound, cup_round);
    gint number_of_groups = cupround->round_robin_number_of_groups;
    gint team_num = (teams->len - (teams->len % number_of_groups)) / number_of_groups;
    gint plus_div = teams->len % number_of_groups;
    GPtrArray *teams_group[number_of_groups];
    Table table_group[number_of_groups];
    TableElement new_table_element;

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
		    table_element_new((Team*)g_ptr_array_index(teams, cnt), table_group[i].elements->len);
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
		table_element_new((Team*)g_ptr_array_index(teams, i), table_group[cnt].elements->len);
	    g_array_append_val(table_group[cnt].elements, new_table_element);
	    cnt = (cnt + 1) % number_of_groups;
	}
    }

    for(i=0;i<number_of_groups;i++)
    {
	g_array_append_val(cupround->tables, table_group[i]);
	fixture_write_round_robin((gpointer)cup, cup_round, teams_group[i], !cupround->home_away);	
    }

    g_ptr_array_free(teams, TRUE);

    g_array_sort_with_data(cup->fixtures, fixture_compare_func,
			   GINT_TO_POINTER(FIXTURE_COMPARE_DATE + 100));

    cup->next_fixture_update_week = (cup_round < cup->rounds->len - 1) ?
	g_array_index(cup->fixtures, Fixture, cup->fixtures->len - 1).week_number : -1;
    cup->next_fixture_update_week_round = (cup_round < cup->rounds->len - 1) ?
	g_array_index(cup->fixtures, Fixture, cup->fixtures->len - 1).week_round_number : -1;
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
fixture_write_round_robin(gpointer league_cup, gint cup_round, GPtrArray *teams, gboolean one_round)
{
    gint i, j;
    gint first_week, week_gap, week_round_number,
	clid, first_fixture;
    gboolean home_advantage;
    League *league = NULL;
    Cup *cup = NULL;
    gint len = teams->len;
    GArray *fixtures = NULL;
    Team team_temp;
    gboolean odd_fixtures = FALSE;

    teams = misc_randomise_g_pointer_array(teams);
    
    if(cup_round == -1)
    {
	league = (League*)league_cup;
	fixtures = league->fixtures;
	clid = league->id;
	first_week = (fixtures->len == 0) ? 
	    league->first_week : g_array_index(fixtures, Fixture, fixtures->len - 1).week_number + 1;
	week_gap = league->week_gap;
	home_advantage = TRUE;
    }
    else
    {
	cup = (Cup*)league_cup;
	first_week = cup_get_first_week_of_cup_round(cup, cup_round);
	week_gap = cup->week_gap;
	fixtures = cup->fixtures;
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

    /* first half of fixtures */
    for(i=0;i<len - 1;i++)
	fixture_write_round_robin_matchday(fixtures, cup_round, teams, i,
					   first_week + i * week_gap,
					   clid, home_advantage);

    if(!one_round)
    {
	/* second half of fixtures */
	for(i = 0; i < len - 1; i++)
	{
	    week_round_number = (cup_round == -1) ?
		1 : fixture_get_free_round(first_week + (len - 1 + i) * week_gap, teams, -1, -1);

	    for(j = 0; j < len / 2; j++)
		fixture_write(fixtures, g_array_index(fixtures, Fixture, first_fixture + i * (len / 2) + j).teams[1],
			      g_array_index(fixtures, Fixture, first_fixture + i * (len / 2) + j).teams[0],
			      first_week + (len - 1 + i) * week_gap, week_round_number,			      
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
    gint i;
    gint len = teams->len / 2;
    gpointer home[len], away[len];
    gint week_round_number = (cup_round == -1) ?
	1 : fixture_get_free_round(week_number, teams, -1, -1);


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
    gint i, len = teams->len;
    gint first_week = cup_get_first_week_of_cup_round(cup, cup_round);
    gint week_round_number;
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

    week_round_number =
	fixture_get_free_round(first_week, teams, -1, -1);
    for(i=0; i<=(teams->len - 2) / 2; i++)
	fixture_write(cup->fixtures, (Team*)g_ptr_array_index(teams, 2 * i),
		      (Team*)g_ptr_array_index(teams, 2 * i + 1), first_week,
		      week_round_number, cup->id, cup_round, 0,
		      !round->neutral, FALSE, (!round->home_away && round->replay == 0));

    if(round->home_away)
    {
	week_round_number =
	    fixture_get_free_round(first_week + cup->week_gap, teams, -1, -1);
	for(i=0; i<=(teams->len - 2) / 2; i++)
	    fixture_write(cup->fixtures, (Team*)g_ptr_array_index(teams, 2 * i + 1),
			  (Team*)g_ptr_array_index(teams, 2 * i), 
			  first_week + cup->week_gap,
			  week_round_number, cup->id, cup_round, 0,
			  !round->neutral, TRUE, TRUE);
    }

    g_array_sort_with_data(cup->fixtures, fixture_compare_func,
			   GINT_TO_POINTER(FIXTURE_COMPARE_DATE + 100));

    cup->next_fixture_update_week = 
	(cup_round < cup->rounds->len - 1 || round->replay > 0) ?
	g_array_index(cup->fixtures, Fixture, cup->fixtures->len - 1).week_number : -1;
    
    cup->next_fixture_update_week_round = 
	(cup_round < cup->rounds->len - 1 || round->replay > 0) ?
	g_array_index(cup->fixtures, Fixture, cup->fixtures->len - 1).week_round_number : -1;
    
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
    const Fixture *first_leg;

    if(!fix->decisive)
	return FALSE;

    if(!fix->second_leg)
	return (fix->result[0][0] + fix->result[0][1] ==
		fix->result[1][0] + fix->result[1][1]);

    first_leg = fixture_get_first_leg(fix);

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
 @return A fixture pointer or NULL if failed. */
Fixture*
fixture_get_first_leg(const Fixture *fix)
{
    gint i;
    Fixture *first_leg = NULL;

    for(i=0;i<cup_from_clid(fix->clid)->fixtures->len;i++)
	if(g_array_index(cup_from_clid(fix->clid)->fixtures, Fixture, i).round == fix->round &&
	   g_array_index(cup_from_clid(fix->clid)->fixtures, Fixture, i).team_ids[0] == fix->team_ids[1] &&
	   g_array_index(cup_from_clid(fix->clid)->fixtures, Fixture, i).team_ids[1] == fix->team_ids[0])
	    first_leg = &g_array_index(cup_from_clid(fix->clid)->fixtures, Fixture, i);

    if(first_leg == NULL)
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
		    fixture_get_first_leg(fix)->result[idx1][0],
		    fixture_get_first_leg(fix)->result[idx0][0]);
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
fixture_get_latest(const Team *tm)
{
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
    
    for(i=0;i<acps->len;i++)
    {
	for(j=0;j<acp(i)->fixtures->len;j++)
	    if(g_array_index(acp(i)->fixtures, Fixture, j).attendance == -1)
		break;
	    else if(g_array_index(acp(i)->fixtures, Fixture, j).teams[0] == tm ||
		    g_array_index(acp(i)->fixtures, Fixture, j).teams[1] == tm)
		g_ptr_array_add(latest, &g_array_index(acp(i)->fixtures, Fixture, j));
    }

    g_ptr_array_sort_with_data(latest, fixture_compare_func,
			       GINT_TO_POINTER(FIXTURE_COMPARE_DATE));

    return latest;
}

/** Return an array with the next fixtures of the team. */
GPtrArray*
fixture_get_coming(const Team *tm)
{
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
fixture_from_id(gint id)
{
    gint i, j;

    for(i=0;i<ligs->len;i++)
	for(j=0;j<lig(i).fixtures->len;j++)
	    if(g_array_index(lig(i).fixtures, Fixture, j).id == id)
		return &g_array_index(lig(i).fixtures, Fixture, j);

    for(i=0;i<cps->len;i++)
	for(j=0;j<cp(i).fixtures->len;j++)
	    if(g_array_index(cp(i).fixtures, Fixture, j).id == id)
		return &g_array_index(cp(i).fixtures, Fixture, j);

    main_exit_program(EXIT_POINTER_NOT_FOUND, 
		      "fixture_from_id: fixture with id %d found \n", id);

    return NULL;
}

/** Return the last week round number of the specified week. */
gint
fixture_get_last_week_round(gint week_number)
{
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
    gint i;
    gint local_week = *week_number,
	local_round = *week_round_number;
    Fixture *fix = NULL;

    *week_number = *week_round_number = 1000;

    for(i=0;i<ligs->len;i++)
	if(lig(i).active)
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
		*week_round_number = 1;
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
    gint i;
    gint local_week = *week_number,
	local_round = *week_round_number;
    Fixture *fix = NULL;

    *week_number = *week_round_number = -1;

    for(i=0;i<ligs->len;i++)
	if(lig(i).active)
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
    gint i, j;
    GPtrArray *results = g_ptr_array_new();
    GArray *fixtures = NULL;
    
    if(league_from_clid(current_user.tm->clid)->active)
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

    first_leg = fixture_get_first_leg(fix);

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
