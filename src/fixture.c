#include "cup.h"
#include "fixture.h"
#include "free.h"
#include "league.h"
#include "main.h"
#include "maths.h"
#include "misc.h"
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
	fixture_write_cup_round_robin(cup, 0);
    else
    {	
	if(cup->type == CUP_TYPE_INTERNATIONAL)
	    teams = cup_get_team_pointers(cup);
	else
	    teams = team_get_pointers_from_choose_teams(cup->choose_teams);

	fixture_write_knockout_round(teams, cup, 0);
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
/** Write round robins for the given cup and cup round.
    @param cup The cup the fixtures are for.
    @param cup_round The cup round. */
void
fixture_write_cup_round_robin(Cup *cup, gint cup_round)
{
    gint i, j;
    gint number_of_groups =
	g_array_index(cup->rounds, CupRound, cup_round).round_robin_number_of_groups;
    GPtrArray *teams = NULL;
    GPtrArray *teams_group[number_of_groups];
    Table new_table;
    TableElement new_table_element;

    if(cup_round == 0)
	teams = misc_randomise_g_pointer_array(cup_get_team_pointers(cup));
    /*todo*/
/*     else */
/* 	teams = cup_get_winners_of_round(cup, cup_round - 1); */

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
	new_table.elements = g_array_new(FALSE, FALSE, sizeof(TableElement));
    
	teams_group[i] = g_ptr_array_new();

	for(j=0;j<teams->len / number_of_groups;j++)
	{
	    g_ptr_array_add(teams_group[i], g_ptr_array_index(teams, j + i * number_of_groups));
	    new_table_element = 
		league_table_element_new((Team*)g_ptr_array_index(teams, j + i * number_of_groups));
	    g_array_append_val(new_table.elements, new_table_element);
	}

	g_array_append_val(cup->tables, new_table);

	fixture_write_round_robin((gpointer)cup, cup_round, teams_group[i]);
    }

    cup->next_fixture_update_week = 
	g_array_index(cup->fixtures, Fixture, cup->fixtures->len - 1).week_number;
    cup->next_fixture_update_week_round = 
	g_array_index(cup->fixtures, Fixture, cup->fixtures->len - 1).week_round_number;
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
    gboolean home_advantage, second_leg, decisive;
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

    second_leg = decisive = FALSE;
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
					   clid, home_advantage, second_leg, decisive);

    /* second half of fixtures */
    for(i = 0; i < len - 1; i++)
	for(j = 0; j < len / 2; j++)
	    fixture_write(fixtures, g_array_index(fixtures, Fixture, first_fixture + i * (len / 2) + j).teams[1],
			  g_array_index(fixtures, Fixture, first_fixture + i * (len / 2) + j).teams[0],
			  first_week + (len - 1 + i) * week_gap, 
			  fixture_get_free_round(first_week + (len - 1 + i) * week_gap, clid),
			  clid, cup_round, home_advantage, second_leg, decisive);
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
    @param home_advantage Whether there's home advantage.
    @param second_leg Whether this is the second leg of a round.
    @param decisive Whether the match has to have a winner. */
void
fixture_write_round_robin_matchday(GArray *fixtures, gint cup_round, GPtrArray *teams,
				   gint special, gint week_number, gint week_round_number,
				   gint clid, gboolean home_advantage, gboolean second_leg,
				   gboolean decisive)
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
		      clid, cup_round, home_advantage, second_leg, decisive);
}

/** Write fixtures for a knockout round, e.g. home/away games.
    @param teams The teams participating.
    @param cup The cup the fixtures belong to.
    @param cup_round The index of the cup round. */
void
fixture_write_knockout_round(GPtrArray *teams, Cup *cup, gint cup_round)
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
		      fixture_get_free_round(first_week, cup->id), cup->id, cup_round,
		      !round->neutral, FALSE, (!round->home_away && round->replay == 0));/*todo: adjust
											   round->replay */
    if(round->home_away)
	for(i=0; i<(len - bye_len) / 2; i++)
	    fixture_write(cup->fixtures, (Team*)g_ptr_array_index(teams, i + (len - bye_len) / 2),
			  (Team*)g_ptr_array_index(teams, i), first_week + cup->week_gap,
			  fixture_get_free_round(first_week + cup->week_gap, cup->id), cup->id, 
			  cup_round, !round->neutral, TRUE, TRUE);

    cup->next_fixture_update_week = 
	g_array_index(cup->fixtures, Fixture, cup->fixtures->len - 1).week_number;
    cup->next_fixture_update_week_round = 
	g_array_index(cup->fixtures, Fixture, cup->fixtures->len - 1).week_round_number;
}

/** Write a fixture and append it to a fixture array.
    @param fixtures The fixture array.
    @param home_team The pointer of the home team.
    @param away_team The pointer of the away team.
    @param week_number The week it takes place.
    @param week_round_number The week_round it takes place.
    @param clid The id of the league or cup.
    @param cup_round The index of the cup round or -1 if it's a league.
    @param home_advantage Whether there's home advantage.
    @param second_leg Whether this is the second leg of a round.
    @param decisive Whether the match has to have a winner. */
void
fixture_write(GArray *fixtures, Team *home_team, Team *away_team, gint week_number,
	      gint week_round_number, gint clid, gint cup_round, gboolean home_advantage,
	      gboolean second_leg, gboolean decisive)
{
    gint i;
    Fixture new;

    new.clid = clid;
    new.round = cup_round;
    new.week_number = week_number;
    new.week_round_number = week_round_number;
    new.teams[0] = (Team*)home_team;
    new.teams[1] = (Team*)away_team;
    for(i=0;i<3;i++)
	new.result[0][i] = new.result[1][i] = 0;

    new.home_advantage = home_advantage;
    new.second_leg = second_leg;
    new.decisive = decisive;
    new.attendance = 0;

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

/** Find out whether the current result of the fixture
    is a draw. This is a bit tricky because of second-leg games.
    In general (as a definition), fixtures with 'decisive' FALSE
    are never drawn.
    @param fix The pointer to the fixture.
    @return TRUE or FALSE. */
gboolean
query_fixture_is_draw(const Fixture *fix)
{
    return FALSE;
}

/** Check whether the user's team participates. 
    @param fix The fixture we check.
    @return TRUE or FALSE. */
gboolean
query_my_team_involved(const Fixture *fix)
{
    return (fix->teams[0] == my_team || fix->teams[1] == my_team);
}
