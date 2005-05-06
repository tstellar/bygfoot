#include "cup.h"
#include "fixture.h"
#include "free.h"
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
    Cup new;

    new.name = g_string_new("");
    new.short_name = g_string_new("");
    new.symbol = g_string_new("");
    new.sid = g_string_new("");

    new.id = (new_id) ? cup_id_new : -1;
    new.group = -1;
    new.add_week = 0;
    new.last_week = -1;
    new.week_gap = 1;
    new.yellow_red = 1000;
    new.skill_diff = 0;
    new.overall_teams = -1;

    new.choose_teams = g_array_new(FALSE, FALSE, sizeof(CupChooseTeam));
    new.rounds = g_array_new(FALSE, FALSE, sizeof(CupRound));
    new.teams = g_array_new(FALSE, FALSE, sizeof(Team));
    new.team_names = g_ptr_array_new();
    new.fixtures = g_array_new(FALSE, FALSE, sizeof(Fixture));
    new.bye = NULL;

    new.properties = g_ptr_array_new();

    new.next_fixture_update_week = -1;
    new.next_fixture_update_week_round = -1;
    
    return new;
}

/** Return a CupChooseTeam with default values. */
CupChooseTeam
cup_choose_team_new(void)
{
    CupChooseTeam new;

    new.sid = g_string_new("");
    new.number_of_teams = -1;
    new.start_idx = new.end_idx = -1;
    new.randomly = FALSE;
    new.generate = FALSE;

    return new;
}

/** Return a CupRound with default values. */
CupRound
cup_round_new(void)
{
    CupRound new;

    new.home_away = TRUE;
    new.replay = 0;
    new.neutral = FALSE;
    new.round_robin_number_of_groups = 0;
    new.round_robin_number_of_advance = -1;
    new.round_robin_number_of_best_advance = 0;
    new.tables = g_array_new(FALSE, FALSE, sizeof(Table));

    return new;
}

/** Reset some arrays at the beginning of a new season. */
void
cup_reset(Cup *cup)
{
    gint i, j;
    
    if(cup->teams->len > 0)
	free_teams_array(&cup->teams, TRUE);

    g_ptr_array_free(cup->team_names, TRUE);
    cup->team_names = g_ptr_array_new();

    g_array_free(cup->fixtures, TRUE);
    cup->fixtures = g_array_new(FALSE, FALSE, sizeof(Fixture));

    free_g_ptr_array(&cup->bye);

    for(i=0;i<cup->rounds->len;i++)
    {
	for(j=0;j<g_array_index(cup->rounds, CupRound, i).tables->len;j++)
	    free_table(&g_array_index(g_array_index(cup->rounds, CupRound, i).tables, Table, j));
	
	g_array_free(g_array_index(cup->rounds, CupRound, i).tables, TRUE);
	g_array_index(cup->rounds, CupRound, i).tables = g_array_new(FALSE, FALSE, sizeof(Table));
    }
}

/** Find out whether a choose_team definition refers to a 
    league in the country. If so, no teams get loaded for that
    choose_team. */
gboolean
query_cup_choose_team_is_league(const gchar *sid)
{
    gint i;

    for(i=0;i<ligs->len;i++)
	if(strcmp(lig(i).sid->str, sid) == 0)
	    return TRUE;

    return FALSE;
}

/** Write the cup or league of the chooseteam into the appropriate pointer. */
void
cup_get_choose_team_league_cup(const CupChooseTeam *ct, 
			       League **league, Cup **cup)
{
    gint i, idx;
    gchar trash[SMALL];

    *league = NULL;
    *cup = NULL;

    sscanf(ct->sid->str, "%[^0-9]%d", trash, &idx);

    if(g_str_has_prefix(ct->sid->str, "league"))
    {
	*league = &lig(idx - 1);
	*cup = NULL;
    }
    else if(g_str_has_prefix(ct->sid->str, "cup") && strlen(ct->sid->str) < 6)
    {
	*cup = &cp(idx - 1);
	*league = NULL;
    }
    else
    {
	for(i=0;i<ligs->len;i++)
	    if(strcmp(lig(i).sid->str, ct->sid->str) == 0)
	    {
		*league = &lig(i);
		*cup = NULL;
		break;
	    }

	for(i=0;i<acps->len;i++)
	{
	    if(strcmp(acp(i)->sid->str, ct->sid->str) == 0)
	    {
		*cup = &cp(i);
		*league = NULL;
		break;
	    }
	}
    }
}

/** Return the pointers to the teams participating in the 
    cup. If necessary, teams are generated and stored in the teams
    array of the cup. */
GPtrArray*
cup_get_team_pointers(Cup *cup)
{
    gint i;
    GPtrArray *teams = g_ptr_array_new();

    if(debug > 60)
	printf("cup_get_team_pointers %s \n", cup->name->str);

    for(i=0;i<cup->choose_teams->len;i++)
	if(g_array_index(cup->choose_teams, CupChooseTeam, i).generate)
	    cup_load_choose_team_generate(cup,
					  &g_array_index(cup->choose_teams, CupChooseTeam, i));
	else
	    cup_load_choose_team(cup, teams, 
				 &g_array_index(cup->choose_teams, CupChooseTeam, i));

    for(i=0;i<cup->teams->len;i++)
    {
	team_generate_players_stadium(&g_array_index(cup->teams, Team, i));
	if(teams->len < cup->overall_teams)
	    g_ptr_array_add(teams, &g_array_index(cup->teams, Team, i));
    }

    return teams;
}

/** Get the pointers to the teams (already generated, in one of the leagues or cups)
    specified in the chooseteam. Add them to the 'teams' pointer array. */
void
cup_load_choose_team(Cup *cup, GPtrArray *teams, const CupChooseTeam *ct)
{
    gint i, j, start, end;
    gint number_of_teams = 0;
    GPtrArray *cup_teams_sorted = NULL;
    League *league = NULL;
    Cup *cup_temp = NULL;

    if(debug > 60)
	printf("cup_load_choose_team: %s, %s \n", cup->name->str,
	       ct->sid->str);

    cup_get_choose_team_league_cup(ct, &league, &cup_temp);

    if(cup_temp == NULL)
    {	    
	if(ct->number_of_teams == -1)
	    for(j=0;j<league->teams->len;j++)
		g_ptr_array_add(teams, &g_array_index(league->teams, Team, j));
	else
	{
	    start = ct->start_idx - 1;
	    end = ct->end_idx - start;

	    gint order[end];
	    for(j=0;j<end;j++)
		order[j] = j + start;

	    if(ct->randomly)
		math_generate_permutation(order, start, start + end);

	    for(j = 0; j < end; j++)
	    {
		if(!query_team_is_in_international_cups(
		       team_of_id(g_array_index(league->table.elements, TableElement, order[j]).team_id), cup->group))
		{
		    g_ptr_array_add(teams, 
				    team_of_id(g_array_index(league->table.elements, TableElement, order[j]).team_id));
		    g_ptr_array_add(cup->team_names, 
				    team_of_id(g_array_index(league->table.elements, TableElement, order[j]).team_id)->name);
		    number_of_teams++;

		    if(number_of_teams == ct->number_of_teams)
			break;
		}
	    }
	}

	if(ct->number_of_teams != -1 &&
	   number_of_teams != ct->number_of_teams)
	{
	    g_warning("cup_load_choose_team (+): not enough teams (that don't participate in international cups yet) found in chooseteam %s for cup %s (%d specified, %d found) cup group %d.\n ",
		      ct->sid->str, cup->name->str, ct->number_of_teams, number_of_teams, cup->group);
	    main_exit_program(EXIT_CHOOSE_TEAM_ERROR, NULL);
	}
    }
    else
    {
	if(season == 1 && cup->add_week == 0)
	{
	    if(lig(0).teams->len < ct->number_of_teams)
	    {
		g_warning("cup_load_choose_team: not enough teams in league 0 for chooseteam %s (%d; required: %d) in cup %s\n",
			  ct->sid->str, lig(0).teams->len, ct->number_of_teams, cup->name->str);
		main_exit_program(EXIT_CHOOSE_TEAM_ERROR, NULL);
	    }

	    gint permutation[lig(0).teams->len];
	    math_generate_permutation(permutation, 0, lig(0).teams->len - 1);

	    for(i = ct->start_idx - 1; i <= ct->end_idx - 1; i++)
	    {
		if(!query_team_is_in_international_cups(
		       &g_array_index(lig(0).teams,
				      Team, permutation[i - ct->start_idx + 1]), cup->group))
		{
		    g_ptr_array_add(teams, &g_array_index(lig(0).teams,
							  Team, permutation[i - ct->start_idx + 1]));
		    g_ptr_array_add(cup->team_names,
				    g_array_index(lig(0).teams,
						  Team, permutation[i - ct->start_idx + 1]).name);
		    number_of_teams++;
		}
		
		if(number_of_teams == ct->number_of_teams)
		    break;
	    }
	    
	    if(number_of_teams != ct->number_of_teams)
	    {
		g_warning("cup_load_choose_team: not enough teams found in league 0 for chooseteam %s (%d; required: %d) in cup %s (group %d)\n",
			  ct->sid->str, number_of_teams, ct->number_of_teams, cup->name->str, cup->group);
		main_exit_program(EXIT_CHOOSE_TEAM_ERROR, NULL);
	    }
	}
	else
	{
	    cup_teams_sorted = cup_get_teams_sorted(cup_temp);

	    if(ct->number_of_teams == -1)
	    {
		start = 0;
		end = cup_teams_sorted->len;
	    }
	    else
	    {
		start = ct->start_idx - 1;
		end = ct->end_idx;
	    }

	    for(j = start; j < end; j++)
	    {
		if(!query_team_is_in_international_cups(
		       (Team*)g_ptr_array_index(cup_teams_sorted, j), cup->group))
		{
		    g_ptr_array_add(teams, g_ptr_array_index(cup_teams_sorted, j));
		    g_ptr_array_add(cup->team_names, ((Team*)g_ptr_array_index(cup_teams_sorted, j))->name);
		    number_of_teams++;

		    if(number_of_teams == ct->number_of_teams)
			break;
		}
	    }
	
	    g_ptr_array_free(cup_teams_sorted, TRUE);

	    if(ct->number_of_teams != -1 &&
	       number_of_teams != ct->number_of_teams)
	    {
		g_warning("cup_load_choose_team(*): not enough teams (that don't participate in international cups yet) found in chooseteam %s for cup %s (%d specified, %d found) cup group %d.\n ",
			  ct->sid->str, cup->name->str, ct->number_of_teams, number_of_teams, cup->group);
		main_exit_program(EXIT_CHOOSE_TEAM_ERROR, NULL);
	    }
	}
    }
}

/** Load the teams specified in the chooseteam from a non-country league. */
void
cup_load_choose_team_generate(Cup *cup, const CupChooseTeam *ct)
{
    gint j, k;
    gint number_of_teams, end_idx = -1;
    GArray *teams_local = NULL;
    GArray *leagues = NULL;
    GPtrArray *sids = NULL;

    if(debug > 60)
	printf("cup_load_choose_team_generate: %s, %s \n", cup->name->str,
	       ct->sid->str);

    leagues = g_array_new(FALSE, FALSE, sizeof(League));
    teams_local = g_array_new(FALSE, FALSE, sizeof(Team));
    sids = misc_separate_strings(ct->sid->str);

    for(j=0;j<sids->len;j++)
    {
	if(!query_cup_choose_team_is_league(((GString*)g_ptr_array_index(sids, j))->str))
	{
	    xml_league_read(((GString*)g_ptr_array_index(sids, j))->str, leagues);
		    
	    for(k=0; k < g_array_index(leagues, League, leagues->len - 1).teams->len; k++)
		g_array_append_val(teams_local, g_array_index(
				       g_array_index(leagues, League, leagues->len - 1).teams, Team,  k));

	    free_g_array(&g_array_index(leagues, League, leagues->len - 1).teams);
	    free_league(&g_array_index(leagues, League, leagues->len - 1));
	}
    }

    g_array_free(leagues, TRUE);
    
    gint permutation[teams_local->len];
    for(j=0;j<teams_local->len;j++)
	permutation[j] = j;

    if(ct->randomly && teams_local->len > 0)
    {
	if(ct->start_idx == -1)
	    math_generate_permutation(permutation, 0, teams_local->len - 1);
	else
	    math_generate_permutation(permutation, ct->start_idx - 1, ct->end_idx - 1);
    }

    number_of_teams = 0;
    if(ct->start_idx == -1)
	end_idx = teams_local->len;
    else
	end_idx = ct->end_idx -
	    ct->start_idx + 1;

    if(teams_local->len < end_idx)
    {
	g_warning("cup_load_choose_team_generate: not enough teams (%d) in chooseteam %s in cup %s (%d are specified) \n",
		  teams_local->len, ct->sid->str, cup->name->str, end_idx);
	
	main_exit_program(EXIT_CHOOSE_TEAM_ERROR, NULL);
    }

    for(j = 0; j < end_idx; j++)
    {
	if(!query_team_is_in_international_cups(&g_array_index(teams_local, Team, permutation[j]), cup->group))
	{
	    g_array_append_val(cup->teams, g_array_index(teams_local, Team, permutation[j]));
	    g_array_index(cup->teams, Team, cup->teams->len - 1).clid = cup->id;
	    g_ptr_array_add(cup->team_names, 
				   g_array_index(cup->teams, Team, cup->teams->len - 1).name);

	    number_of_teams++;
	}

	if(number_of_teams == ct->number_of_teams)
	    break;
    }
    
    if(((ct->number_of_teams != -1 && number_of_teams != ct->number_of_teams) ||
	(ct->number_of_teams == -1 && number_of_teams != teams_local->len)) &&
       teams_local->len > 0)
    {
	g_warning("cup_load_choose_team_generate: not enough teams (that don't participate in international cups yet) found in chooseteam %s for cup %s (%d specified, %d found).\n ", ct->sid->str, cup->name->str, ct->number_of_teams, number_of_teams);
	main_exit_program(EXIT_CHOOSE_TEAM_ERROR, NULL);
    }

    for(j=teams_local->len - 1; j>=0;j--)
	if(query_team_is_in_international_cups(&g_array_index(teams_local, Team, j), cup->group))
	    g_array_remove_index(teams_local, j);
	
    free_g_string_array(&sids);
    free_teams_array(&teams_local, FALSE);
}

/** Return a pointer array of teams ordered corresponding to
    their success in the cup. A bit tricky because we have to
    fetch the team pointers corresponding to their name because
    the team pointers in the fixtures are partially invalid because
    of promotion relegation. */
GPtrArray*
cup_get_teams_sorted(const Cup *cup)
{
    gint i, j;
    GPtrArray *teams = g_ptr_array_new();
    GArray *team_ids = g_array_new(FALSE, FALSE, sizeof(gint));
    
    for(i=0;i<cup->fixtures->len;i++)
	for(j=0;j<2;j++)
	    if(!query_misc_integer_is_in_g_array(
		   g_array_index(cup->fixtures, Fixture, i).team_ids[j], team_ids))
	    {
		g_array_append_val(team_ids, g_array_index(cup->fixtures, Fixture, i).team_ids[j]);
		g_ptr_array_add(teams, team_of_id(g_array_index(cup->fixtures, Fixture, i).team_ids[j]));
	    }

    g_ptr_array_sort_with_data(teams, cup_compare_success, (gpointer)cup);

    g_array_free(team_ids, TRUE);

    return teams;
}

/** Compare two teams in a pointer array taking into account
    their success in a cup. */
gint
cup_compare_success(gconstpointer a, gconstpointer b, gpointer data)
{
    const Cup *cup = (const Cup*)data;
    const CupRound *cupround = NULL;
    const GArray *fixtures = cup->fixtures;
    const Team *team1 = *(const Team**)a;
    const Team *team2 = *(const Team**)b;
    const Fixture *last_fix = &g_array_index(fixtures, Fixture, fixtures->len - 1);
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
	else if(round_reached1 != last_fix->round)
	    return_value = 0;
	else
	{
	    if(GPOINTER_TO_INT(fixture_winner_of(last_fix, TRUE)) ==  team1->id)
		return_value = -1;
	    else
		return_value = 1;
	}
    }

    return return_value;
}

/** Compare two teams in cup tables. */
gint
cup_compare_success_tables(const Team *tm1, const Team *tm2, const Cup *cup, gint round)
{
    gint i, j;
    gint return_value = 0;
    const CupRound *cupround = &g_array_index(cup->rounds, CupRound, round);
    const TableElement *elem1 = NULL, *elem2 = NULL;

    if(team_get_cup_rank(tm1, cupround) > team_get_cup_rank(tm2, cupround))
	return_value = 1;
    else if(team_get_cup_rank(tm1, cupround) < team_get_cup_rank(tm2, cupround))
	return_value = -1;
    else
    {
	for(i=0;i<cupround->tables->len;i++)
	    for(j=0;j<g_array_index(cupround->tables, Table, i).elements->len;j++)
		if(g_array_index(g_array_index(cupround->tables, Table, i).elements, TableElement, j).team_id == tm1->id)
		    elem1 = &g_array_index(g_array_index(cupround->tables, Table, i).elements, TableElement, j);
		else if(g_array_index(g_array_index(cupround->tables, Table, i).elements, TableElement, j).team_id == tm2->id)
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
    gint round = -1;
    gint i;

    for(i=0;i<fixtures->len;i++)
	if(g_array_index(fixtures, Fixture, i).team_ids[0] == tm->id ||
	   g_array_index(fixtures, Fixture, i).team_ids[1] == tm->id)
	    round = MAX(round, g_array_index(fixtures, Fixture, i).round);

    return round;
}

/** Print an error and exit the program if there's a problem loading
    the CupChooseTeam.
    @param *cup The cup the choose_team is from.
    @param choose_team The choose_team that caused the error.
    @param user Whether the function is called from cup_load_choose_team_user()
    or cup_load_choose_teams(). */
void
cup_choose_team_abort(const Cup *cup, const CupChooseTeam *choose_team, gboolean user)
{
    if(user)
	g_warning("cup_load_choose_team_user: there was an error loading choose_team %s from cup %s. maybe there aren't enough teams in the specified league or cup.\n", choose_team->sid->str, cup->sid->str);
    else
	g_warning("cup_load_choose_teams: there was an error loading choose_team %s from cup %s. maybe there aren't enough teams in the specified league or cup.\n", choose_team->sid->str, cup->sid->str);
    
    main_exit_program(EXIT_CHOOSE_TEAM_ERROR, NULL);
}

/** Calculate the week number the first matchday of the given
    cup round takes place.
    @param cup The cup we examine.
    @param cup_round The index of the cup round in the cup.rounds array.
    @return A week number. */
gint
cup_get_first_week_of_cup_round(const Cup *cup, gint cup_round)
{
    gint week_number;

    if(cup_round == cup->rounds->len - 1)
	week_number = cup->last_week - 
	    (cup_get_matchdays_in_cup_round(cup, cup_round) - 1) * cup->week_gap;
    else
	week_number = cup_get_first_week_of_cup_round(cup, cup_round + 1) -
	    cup_get_matchdays_in_cup_round(cup, cup_round) * cup->week_gap;

    if(week_number <= 0)
    {
	g_warning("cup_get_first_week_of_cup_round: first week of cup %s cup round %d is not positive (%d).\nPlease lower the week gap or set a later last week.\n", cup->name->str, cup_round, week_number);
	main_exit_program(EXIT_FIRST_WEEK_ERROR, NULL);
    }

    return week_number;
}

/** Calculate the last week of a cup if we only know the first week. */
gint
cup_get_last_week_from_first(const Cup *cup, gint first_week)
{
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
cup_get_matchdays_in_cup_round(const Cup *cup, gint cup_round)
{
    gint number_of_teams = -1;
    gint number_of_matchdays = -1;
    
    if(g_array_index(cup->rounds, CupRound, cup_round).
       round_robin_number_of_groups > 0)
    {
	number_of_teams = cup_round_get_number_of_teams(cup, cup_round);
	number_of_matchdays = 
	    2 * ((number_of_teams / g_array_index(cup->rounds, CupRound, cup_round).
		  round_robin_number_of_groups) - 1);
    }
    else if(g_array_index(cup->rounds, CupRound, cup_round).home_away)
	number_of_matchdays = 2;
    else
	number_of_matchdays = 1;

    return number_of_matchdays;
}

/** Return the number of teams playing in the given cup round.
    @param cup The cup we examine.
    @param cup_round The index of the cup round.
    @return The number teams. */
gint
cup_round_get_number_of_teams(const Cup *cup, gint cup_round)
{
    gint number_of_teams = -1;

    if(cup_round == 0)
	number_of_teams = cup->overall_teams;
    else if(g_array_index(cup->rounds, CupRound, cup_round - 1).round_robin_number_of_groups > 0)
    {
	number_of_teams = 
	    g_array_index(cup->rounds, CupRound, cup_round - 1).round_robin_number_of_groups *
	    g_array_index(cup->rounds, CupRound, cup_round - 1).round_robin_number_of_advance +
	    g_array_index(cup->rounds, CupRound, cup_round - 1).round_robin_number_of_best_advance;
    }
    else
	number_of_teams = cup_round_get_number_of_teams(cup, cup_round - 1) / 2;

    return number_of_teams;
}

/** Return the cup pointer belonging to the id.
    @param clid The id we look for.
    @return The cup pointer or NULL if failed. */
Cup*
cup_from_clid(gint clid)
{
    gint i;

    for(i=0;i<cps->len;i++)
	if(cp(i).id == clid)
	    return &cp(i);

    g_warning("cup_from_clid: didn't find cup with id %d\n", clid);

    return NULL;
}

/** Find the cup with the given sid. */
Cup*
cup_from_sid(const gchar *sid)
{
    gint i;

    for(i=0;i<cps->len;i++)
	if(strcmp(cp(i).sid->str, sid) == 0)
	    return &cp(i);

    g_warning("cup_from_sid: didn't find cup with sid %s \n", sid);

    return NULL;
}

/** Return the name of a cup round, e.g. 'round robin'
    or 'final' or so.
    @param fix A fixture belonging to the round.
    @param buf The buffer we write the name into*/
void
cup_round_name(const Fixture *fix, gchar *buf)
{
    const Cup *cup = cup_from_clid(fix->clid);
    const CupRound *cup_round = 
	&g_array_index(cup->rounds, CupRound, fix->round);

    cup_get_round_name(cup, fix->round, buf);

    if(cup_round->home_away && cup_round->round_robin_number_of_groups == 0)
    {
	if(fix->second_leg)
	    strcat(buf, " -- Second leg");
	else
	    strcat(buf, " -- First leg");
    }
    else if(fix->replay_number > 0)
	strcat(buf, " -- Replay match");
}

/** Return the cup round given by the number. */
void
cup_get_round_name(const Cup *cup, gint round, gchar *buf)
{
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
	    sprintf(buf, "Last %d", (gint)rint(powf(2, cup->rounds->len - round)));
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
    GPtrArray *teams_sorted = cup_get_teams_sorted(cup);
    Team *tm = (Team*)g_ptr_array_index(teams_sorted, 0);
    
    g_ptr_array_free(teams_sorted, TRUE);

    return tm;
}

/** Find out whether a cup with add_week 1000 should begin now. */
gboolean
query_cup_begins(const Cup *cup)
{
    gint i;
    League *league = NULL;
    Cup *cup_temp = NULL;
    gboolean proceed = FALSE;

    for(i=0;i<cup->choose_teams->len;i++)
    {
	cup_get_choose_team_league_cup(
	    &g_array_index(cup->choose_teams,
			   CupChooseTeam, i), &league, &cup_temp);

	if((cup_temp == NULL && 
	    g_array_index(league->fixtures, Fixture,
			  league->fixtures->len - 1).week_number == week &&
	    g_array_index(league->fixtures, Fixture,
			  league->fixtures->len - 1).week_round_number == week_round) ||
	   (league == NULL && 
	    g_array_index(cup_temp->fixtures, Fixture,
			  cup_temp->fixtures->len - 1).week_number == week &&
	    g_array_index(cup_temp->fixtures, Fixture,
			  cup_temp->fixtures->len - 1).week_round_number == week_round))
	    proceed = TRUE;
    }

    if(!proceed)
	return FALSE;

    for(i=0;i<cup->choose_teams->len;i++)
    {
	cup_get_choose_team_league_cup(
	    &g_array_index(cup->choose_teams,
			   CupChooseTeam, i), &league, &cup_temp);
	if((cup_temp == NULL &&
	    g_array_index(league->fixtures, Fixture,
			  league->fixtures->len - 1).attendance == -1) ||
	   (league == NULL &&
	    g_array_index(cup_temp->fixtures, Fixture,
			  cup_temp->fixtures->len - 1).attendance == -1))
	    return FALSE;
    }

    return TRUE;    
}
