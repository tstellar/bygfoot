#include "cup.h"
#include "fixture.h"
#include "free.h"
#include "league.h"
#include "main.h"
#include "maths.h"
#include "misc.h"
#include "table.h"
#include "team.h"
#include "variables.h"

/** Write the fixtures for the given league
    at the beginning of a new season. 
    @param league The league we write the fixtures for. */
void
fixture_write_league_fixtures(League *league)
{
    /*d*/
    gint i;
    Fixture *fix = NULL;
    GPtrArray *teams = NULL;
    
    teams = team_get_pointers_from_array(league->teams);

    fixture_write_round_robin((gpointer)league, -1, teams);

    /*d*/
/*     printf("\n%s\n", league->name->str); */
/*     for(i=0;i<league->fixtures->len;i++) */
/*     { */
/* 	fix = &g_array_index(league->fixtures, Fixture, i); */
/* 	if(i > 0 && */
/* 	   g_array_index(league->fixtures, Fixture, i).week_number != */
/* 	   g_array_index(league->fixtures, Fixture, i - 1).week_number) */
/* 	    printf("\nweek %d\n", g_array_index(league->fixtures, Fixture, i).week_number); */

/* 	printf("%s -- %s round %d week %d wr %d res %d %d home %d sec %d dec %d\n", */
/* 	       fix->teams[0]->name->str, fix->teams[1]->name->str, */
/* 	       fix->round, */
/* 	       fix->week_number, fix->week_round_number, */
/* 	       fix->result[0][0], fix->result[1][0], */
/* 	       fix->home_advantage, fix->second_leg, */
/* 	       fix->decisive); */
	       
/*     } */
}

/** Write the fixtures for the given cup
    at the beginning of a new season. 
    @param cup The cup we write the fixtures for. */
void
fixture_write_cup_fixtures(Cup *cup)
{
    /*d*/
    gint i;
    Fixture *fix = NULL;
    GPtrArray *teams = NULL;

    if(g_array_index(cup->rounds, CupRound, 0).
       round_robin_number_of_groups > 0)
	fixture_write_cup_round_robin(cup, 0, NULL);
    else
    {	
	if(cup->type == CUP_TYPE_INTERNATIONAL)
	    teams = cup_get_team_pointers(cup);
	else
	    teams = team_get_pointers_from_choose_teams(cup->choose_teams);

	fixture_write_knockout_round(cup, 0, teams);
    }

    /*d*/
/*     printf("\n%s\n", cup->name->str); */
/*     for(i=0;i<cup->fixtures->len;i++) */
/*     { */
/* 	fix = &g_array_index(cup->fixtures, Fixture, i); */
/* 	if(i > 0 && */
/* 	   g_array_index(cup->fixtures, Fixture, i).week_number != */
/* 	   g_array_index(cup->fixtures, Fixture, i - 1).week_number) */
/* 	    printf("\nweek %d\n", g_array_index(cup->fixtures, Fixture, i).week_number); */

/* 	printf("%2d %s -- %s round %d week %d wr %d res %d %d home %d sec %d dec %d\n", i, */
/* 	       fix->teams[0]->name->str, */
/* 	       fix->teams[1]->name->str, */
/* 	       fix->round, */
/* 	       fix->week_number, fix->week_round_number, */
/* 	       fix->result[0][0], fix->result[1][0], */
/* 	       fix->home_advantage, fix->second_leg, */
/* 	       fix->decisive); */
	       
/*     } */
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
    const CupRound *new_round = &g_array_index(cup->rounds, CupRound, round + 1);

    if(replay != 0)
    {
	if(g_array_index(fixtures, Fixture, fixtures->len - 1).replay_number < replay && 
	   fixture_update_write_replays(cup))
	{
	    cup->next_fixture_update_week_round++;
	    return;
	}
    }
    
    teams = fixture_get_cup_round_winners(cup);

    if(teams->len < 2)
    {
	g_warning("fixture_update_cup_fixtures: found less than 2 teams for cup %s\n",
		  cup->name->str);
	return;
    }

    if(round == 0 && cup->bye != NULL && cup->bye->len != 0)
    {
	for(i=0;i<cup->bye->len;i++)
	    g_ptr_array_add(teams, g_ptr_array_index(cup->bye, i));

	free_g_ptr_array(&cup->bye);
    }

    /*d*/
    printf("\n");
    for(i=0;i<teams->len;i++)
	printf("%d adv %s\n", i, ((Team*)g_ptr_array_index(teams, i))->name->str);
    printf("\n");

    if(new_round->round_robin_number_of_groups > 0)
	fixture_write_cup_round_robin(cup, round + 1, teams);
    else
	fixture_write_knockout_round(cup, round + 1, teams);

    for(i=0;i<cup->fixtures->len;i++)
	if(g_array_index(cup->fixtures, Fixture, i).round == round + 1)
	    printf("%d %d %d %25s - %-25s\n", 
		   g_array_index(cup->fixtures, Fixture, i).week_number,
		   g_array_index(cup->fixtures, Fixture, i).week_round_number,
		   g_array_index(cup->fixtures, Fixture, i).clid,
		   g_array_index(cup->fixtures, Fixture, i).teams[0]->name->str,
		   g_array_index(cup->fixtures, Fixture, i).teams[1]->name->str);
}

/** Return the teams that advance to the next cup round.
    @param fixtures The fixtures array of the cup.
    @return A team pointer array. */
GPtrArray*
fixture_get_cup_round_winners(const Cup *cup)
{
    gint i;
    GPtrArray *array;
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
		g_ptr_array_add(array, fixture_winner_of(&g_array_index(fixtures, Fixture, i)));

	return array;
    }

    if(!cupround->home_away)
    {
	for(i=0;i<fixtures->len;i++)
	    if(g_array_index(fixtures, Fixture, i).round == round)
		g_ptr_array_add(array, fixture_winner_of(&g_array_index(fixtures, Fixture, i)));
	
	return array;
    }

    for(i=0;i<fixtures->len;i++)
	if(g_array_index(fixtures, Fixture, i).round == round &&
	   g_array_index(fixtures, Fixture, i).second_leg)
	    g_ptr_array_add(array, fixture_winner_of(&g_array_index(fixtures, Fixture, i)));

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
    gint i, j;
    GPtrArray *array = g_ptr_array_new();
    const CupRound *cupround = &g_array_index(cup->rounds, CupRound, round);
    GArray *best_advance = g_array_new(FALSE, FALSE, sizeof(TableElement));
    
    for(i=0;i<cup->tables->len;i++)
	for(j=0;j<g_array_index(cup->tables, Table, i).elements->len;j++)
	    if(j < cupround->round_robin_number_of_advance)
		g_ptr_array_add(array, g_array_index(
				    g_array_index(cup->tables, Table, i).elements,
				    TableElement, j).team);
	    else
		g_array_append_val(best_advance,
				   g_array_index(g_array_index(cup->tables, Table, i).elements,
						 TableElement, j));
    
    g_array_sort_with_data(best_advance,
			   (GCompareDataFunc)table_element_compare_func,
			   GINT_TO_POINTER(cup->id));
    
    for(i=0;i<cupround->round_robin_number_of_best_advance;i++)
	g_ptr_array_add(array, g_array_index(best_advance, TableElement, i).team);

    return array;
}

/** Return the pointer of the team that won the encounter.
    @param fix The fixture we examine.
    @return A team pointer. */
Team*
fixture_winner_of(const Fixture *fix)
{
    gint i;
    const Fixture *first_leg;
    const CupRound *cupround = 
	&g_array_index(cup_from_clid(fix->clid)->rounds, CupRound, fix->round);

    if(cupround->replay != 0 || !cupround->home_away)
	return fix->teams[(math_sum_int_array(&(fix->result[0][0]), 3) <
			   math_sum_int_array(&(fix->result[1][0]), 3))];

    first_leg = fixture_get_first_leg(fix);

    printf("win_of %s %d - %d %s\n",
	   fix->teams[0]->name->str, fix->result[0][0],
	   fix->result[1][0],
	   fix->teams[1]->name->str);
    printf("win_of %s %d - %d %s\n", first_leg->teams[0]->name->str, first_leg->result[0][0],
	   first_leg->result[1][0],
	   first_leg->teams[1]->name->str);
    printf("\n");
    
    if(fix->result[0][0] + first_leg->result[1][0] >
       fix->result[1][0] + first_leg->result[0][0])
	return fix->teams[0];

    if(fix->result[1][0] > first_leg->result[1][0])
	return fix->teams[1];

    return fix->teams[(fix->result[0][1] + fix->result[0][2] <
		       fix->result[1][1] + fix->result[1][2])];
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
    gboolean return_value;

    for(i=0;i<fixtures->len;i++)
	if(g_array_index(fixtures, Fixture, i).round == round &&
	   g_array_index(fixtures, Fixture, i).replay_number == replay_number - 1 &&
	   g_array_index(fixtures, Fixture, i).result[0][0] == 
	   g_array_index(fixtures, Fixture, i).result[1][0])
	    fixture_write(fixtures, g_array_index(fixtures, Fixture, i).teams[1],
			  g_array_index(fixtures, Fixture, i).teams[0], week, week_round + 1,
			  cup->id, round, replay_number, 
			  !g_array_index(cup->rounds, CupRound, round).neutral,
			  FALSE, (g_array_index(cup->rounds, CupRound, round).replay == replay_number));

    return_value = (g_array_index(fixtures, Fixture, fixtures->len - 1).replay_number == replay_number);

    printf("retval %d\n", return_value);
    return return_value;
}

/** Write round robins for the given cup and cup round.
    @param cup The cup the fixtures are for.
    @param cup_round The cup round. */
void
fixture_write_cup_round_robin(Cup *cup, gint cup_round, GPtrArray *teams)
{
    gint i, j;
    gint number_of_groups =
	g_array_index(cup->rounds, CupRound, cup_round).round_robin_number_of_groups;
    GPtrArray *teams_group[number_of_groups];
    Table new_table;
    TableElement new_table_element;

    if(teams == NULL)
	teams = misc_randomise_g_pointer_array(cup_get_team_pointers(cup));

    if(teams->len % number_of_groups != 0)
    {
	g_warning("fixture_write_cup_round_robin: number of teams not divisable by number of groups\ncup %s cup_round %d\n",
		  cup->name->str, cup_round);
	main_exit_program(EXIT_FIXTURE_WRITE_ERROR, NULL);
    }

    free_cup_tables(cup->tables);
    cup->tables = g_array_new(FALSE, FALSE, sizeof(Table));

    for(i=0;i<number_of_groups;i++)
    {
	new_table.name = g_string_new(cup->name->str);
	new_table.clid = cup->id;
	new_table.round = cup_round;
	new_table.elements = g_array_new(FALSE, FALSE, sizeof(TableElement));
    
	teams_group[i] = g_ptr_array_new();

	for(j=0;j<teams->len / number_of_groups;j++)
	{
	    g_ptr_array_add(teams_group[i], g_ptr_array_index(teams, j + i * number_of_groups));
	    new_table_element = 
		table_element_new((Team*)g_ptr_array_index(teams, j + i * number_of_groups));
	    g_array_append_val(new_table.elements, new_table_element);
	}

	g_array_append_val(cup->tables, new_table);

	fixture_write_round_robin((gpointer)cup, cup_round, teams_group[i]);
    }

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
    to the teams. */
void
fixture_write_round_robin(gpointer league_cup, gint cup_round, GPtrArray *teams)
{
    gint i, j;
    gint first_week, week_gap, clid, first_fixture;
    gboolean home_advantage;
    League *league = NULL;
    Cup *cup = NULL;
    gint len = teams->len;
    GArray *fixtures = NULL;

    teams = misc_randomise_g_pointer_array(teams);
    
    if(cup_round == -1)
    {
	league = (League*)league_cup;
	first_week = league->first_week;
	week_gap = league->week_gap;
	fixtures = league->fixtures;
	clid = league->id;
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
    {
	g_warning("fixture_write_round_robin: first week is not positive: %d\n", first_week);
	main_exit_program(EXIT_FIXTURE_WRITE_ERROR, NULL);
    }

    if(len % 2 != 0)
	main_exit_program(EXIT_FIXTURE_WRITE_ERROR,
			  "fixture_write_round_robin: round robin for an odd number of teams is not supported.\n");

    /* first half of fixtures */
    for(i=0;i<len - 1;i++)
	fixture_write_round_robin_matchday(fixtures, cup_round, teams, i,
					   first_week + i * week_gap,
					   fixture_get_free_round(first_week + i * week_gap, clid),
					   clid, home_advantage);

    /* second half of fixtures */
    for(i = 0; i < len - 1; i++)
	for(j = 0; j < len / 2; j++)
	    fixture_write(fixtures, g_array_index(fixtures, Fixture, first_fixture + i * (len / 2) + j).teams[1],
			  g_array_index(fixtures, Fixture, first_fixture + i * (len / 2) + j).teams[0],
			  first_week + (len - 1 + i) * week_gap, 
			  fixture_get_free_round(first_week + (len - 1 + i) * week_gap, clid),
			  clid, cup_round, 0, home_advantage, FALSE, FALSE);
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
				   gint special, gint week_number, gint week_round_number,
				   gint clid, gboolean home_advantage)
{
    gint i;
    gint len = teams->len / 2;
    gpointer home[len], away[len];

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
	fixture_write(fixtures, (Team*)home[i], (Team*)away[i], week_number, week_round_number,
		      clid, cup_round, 0, home_advantage, FALSE, FALSE);
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
    CupRound *round = &g_array_index(cup->rounds, CupRound, cup_round);
    gint bye_len = math_get_bye_len(teams->len);

    if(len % 2 != 0)
	main_exit_program(EXIT_FIXTURE_WRITE_ERROR,
			  "fixture_write_knockout_round: odd number of teams.\n");

    teams = misc_randomise_g_pointer_array(teams);

    if(bye_len != 0)
	for(i=0;i<bye_len;i++)
	{
	    /*d*/
/* 	    printf("bye %d %s\n", i, ((Team*)g_ptr_array_index(teams, len - bye_len + i))->name->str); */
	    g_ptr_array_add(cup->bye, g_ptr_array_index(teams, len - bye_len + i));
	}
    else
	free_g_ptr_array(&cup->bye);

    for(i=0; i<(len - bye_len) / 2; i++)
	fixture_write(cup->fixtures, (Team*)g_ptr_array_index(teams, i),
		      (Team*)g_ptr_array_index(teams, i + (len - bye_len) / 2), first_week,
		      fixture_get_free_round(first_week, cup->id), cup->id, cup_round, 0,
		      !round->neutral, FALSE, (!round->home_away && round->replay == 0));/*todo: adjust
											   round->replay */
    if(round->home_away)
	for(i=0; i<(len - bye_len) / 2; i++)
	    fixture_write(cup->fixtures, (Team*)g_ptr_array_index(teams, i + (len - bye_len) / 2),
			  (Team*)g_ptr_array_index(teams, i), first_week + cup->week_gap,
			  fixture_get_free_round(first_week + cup->week_gap, cup->id), cup->id, 
			  cup_round, 0, !round->neutral, TRUE, TRUE);

    cup->next_fixture_update_week = (cup_round < cup->rounds->len - 1) ?
	g_array_index(cup->fixtures, Fixture, cup->fixtures->len - 1).week_number : -1;
    cup->next_fixture_update_week_round = (cup_round < cup->rounds->len - 1) ?
	g_array_index(cup->fixtures, Fixture, cup->fixtures->len - 1).week_round_number : -1;
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

    new.clid = clid;
    new.round = cup_round;
    new.replay_number = replay_number;
    new.week_number = week_number;
    new.week_round_number = week_round_number;
    new.teams[0] = (Team*)home_team;
    new.teams[1] = (Team*)away_team;
    for(i=0;i<3;i++)
	new.result[0][i] = new.result[1][i] = 0;

    new.home_advantage = home_advantage;
    new.second_leg = second_leg;
    new.decisive = decisive;
    new.attendance = -1;

    g_array_append_val(fixtures, new);
}

/** Return the round of the week which doesn't contain
    any matches yet except for matches of the cup with the given
    id.
    @param week_number The week we examine.
    @param clid The id of the cup we search a free round for.
    @return A round number. If the clid belongs to a league this is always 1. */
gint
fixture_get_free_round(gint week_number, gint clid)
{
    gint i, j;
    gint max_round = 1;

    if(clid < ID_CUP_START)
	return 1;

    for(i=0;i<cps->len;i++)
	if(cp(i).id != clid)
	    for(j=0;j<cp(i).fixtures->len;j++)
		if(g_array_index(cp(i).fixtures, Fixture, j).week_number == week_number &&
		   g_array_index(cp(i).fixtures, Fixture, j).week_round_number == max_round + 1)
		    max_round = MAX(max_round, 
				    g_array_index(cp(i).fixtures, Fixture, j).week_round_number);

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

    return (fix->result[0][0] + first_leg->result[1][0] ==
	    fix->result[1][0] + first_leg->result[0][0] &&
	    fix->result[0][0] == first_leg->result[0][0] &&
	    fix->result[0][1] == fix->result[1][1]);
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
	    g_array_index(cup_from_clid(fix->clid)->rounds, CupRound, fix->round).
	    round_robin_number_of_groups != 0);
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
    GArray *fixtures = (clid < ID_CUP_START) ?
	league_from_clid(clid)->fixtures : cup_from_clid(clid)->fixtures;


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
	   g_array_index(cup_from_clid(fix->clid)->fixtures, Fixture, i).teams[0] == fix->teams[1] &&
	   g_array_index(cup_from_clid(fix->clid)->fixtures, Fixture, i).teams[1] == fix->teams[0])
	    first_leg = &g_array_index(cup_from_clid(fix->clid)->fixtures, Fixture, i);

    if(first_leg == NULL)
	g_warning("fixture_get_first_leg: didn't find first leg match; cup %s round %d\n",
		  cup_from_clid(fix->clid)->name->str, fix->round);

    return first_leg;
}

/** Return a list of fixture pointers.
    @param clid The cup or league id.
    @param week_number The week number of the fixtures.
    @param week_round_number The week round of the fixtures. */
GPtrArray*
fixture_get_week_round_list(gint clid, gint week_number, gint week_round_number)
{
    gint i;
    GArray *fixtures = (clid < ID_CUP_START) ?
	league_from_clid(clid)->fixtures :
	cup_from_clid(clid)->fixtures;
    GPtrArray *fixtures_array = g_ptr_array_new();

    for(i=0;i<fixtures->len;i++)
	if(g_array_index(fixtures, Fixture, i).week_number == week_number &&
	   g_array_index(fixtures, Fixture, i).week_round_number == week_round_number)
	    g_ptr_array_add(fixtures_array, &g_array_index(fixtures, Fixture, i));

    return fixtures_array;
}

/** Print the result of the fixture into a buffer. */
void
fixture_result_to_buf(const Fixture *fix, gchar *buf)
{
    gchar local_buf[SMALL];

    if(fix->attendance < 0)
	strcpy(buf, "-- : --");
    else
    {
	sprintf(local_buf, "%d - %d", math_sum_int_array(fix->result[0], 3),
		math_sum_int_array(fix->result[1], 3));
	if(fix->result[0][2] + fix->result[1][2] != 0)
	    strcat(local_buf, " p.");
	else if(fix->result[0][1] + fix->result[1][1] != 0)
	    strcat(local_buf, " e.t.");

	if(fix->second_leg)
	    sprintf(buf, "%s (%d - %d)", local_buf,
		    fixture_get_first_leg(fix)->result[1][0],
		    fixture_get_first_leg(fix)->result[0][0]);
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

    if(week_round_number > 1)
    {
	for(i=0;i<cps->len;i++)
	    for(j=0;j<cp(i).fixtures->len;j++)
		if(g_array_index(cp(i).fixtures, Fixture, j).week_number == week_number &&
		   g_array_index(cp(i).fixtures, Fixture, j).week_round_number == week_round_number)
		    sum++;
    }
    else
    {
	for(i=0;i<ligs->len;i++)
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
    GArray *fixtures = (clid < ID_CUP_START) ?
	league_from_clid(clid)->fixtures :
	cup_from_clid(clid)->fixtures;

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
    GArray *fixtures = (clid < ID_CUP_START) ?
	league_from_clid(clid)->fixtures :
	cup_from_clid(clid)->fixtures;

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
fixture_get(gint type, gint clid, gint week_number, gint week_round_number, const Team *tm)
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
		league_cup_get_next_clid(clid):
		league_cup_get_previous_clid(clid);
	    fix = fixture_get(SHOW_CURRENT, new_clid, week, week_round, NULL);
    }

    if(fix == NULL)
	g_warning("fixture_get: no fixture found for type %d clid %d week %d round %d\n",
		  type, clid, week_number, week_round_number);

    return fix;
}
