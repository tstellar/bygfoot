#include "cup.h"
#include "fixture.h"
#include "free.h"
#include "league.h"
#include "maths.h"
#include "player.h"
#include "team.h"
#include "variables.h"

/**
   Constants determining the probabilities for
   the playing styles of CPU teams.
   @see team_assign_playing_style().
*/
#define CONSTANT_TEAM_PLAYING_STYLE_PROB1 0.1
#define CONSTANT_TEAM_PLAYING_STYLE_PROB2 0.25
#define CONSTANT_TEAM_PLAYING_STYLE_PROB3 0.75
#define CONSTANT_TEAM_PLAYING_STYLE_PROB4 0.9

/**
   Constants determining the probabilities for
   the playing structures of CPU teams.
   @see team_assign_playing_structure().
*/
#define CONSTANT_TEAM_PLAYING_STRUCTURE_PROB1 0.15
#define CONSTANT_TEAM_PLAYING_STRUCTURE_PROB2 0.5
#define CONSTANT_TEAM_PLAYING_STRUCTURE_PROB3 0.7
#define CONSTANT_TEAM_PLAYING_STRUCTURE_PROB4 0.85

/** Kinda hard to explain. 
    @see team_generate_players()
    @see player_generate() */
#define CONSTANT_TEAM_SKILL_VARIANCE 0.2//0.075

/**
   Generate a team with default values, e.g. 
   random playing structure and an empty string
   as name.
   @return A new team.
*/
Team
team_new(void)
{
    Team new;

    new.name = g_string_new("");
    new.symbol = g_string_new("");
    
    new.clid = new.id = -1;
    new.structure = team_assign_playing_structure();
    new.style = team_assign_playing_style();

    new.stadium = team_stadium_new();

    new.players = g_array_new(FALSE, FALSE, sizeof(Player));

    return new;
}

/**
   Return a random playing style.
   @see The #TeamPlayingStyle enumeration.
   @see The #CONSTANT_TEAM_PLAYING_STYLE_PROB1 define.
*/
gint
team_assign_playing_style(void)
{
  gfloat rndom;

  rndom = math_rnd(0,1);

  /* all out defend */
  if(rndom < CONSTANT_TEAM_PLAYING_STYLE_PROB1)
    return -2;
  /* defend */
  else if(rndom < CONSTANT_TEAM_PLAYING_STYLE_PROB2)
    return -1;
  /* balanced */
  else if(rndom < CONSTANT_TEAM_PLAYING_STYLE_PROB3)
    return 0;
  /* attack */
  else if(rndom < CONSTANT_TEAM_PLAYING_STYLE_PROB4)
    return 1;

  /* all out attack */
  return 2;
}

/**
   Return a random playing structure.
   @see The #CONSTANT_TEAM_PLAYING_STRUCTURE_PROB1 define.
*/
gint
team_assign_playing_structure(void)
{
  gfloat rndom = math_rnd(0,1);
    
  if(rndom < CONSTANT_TEAM_PLAYING_STRUCTURE_PROB1)
    return 532;
  else if(rndom < CONSTANT_TEAM_PLAYING_STRUCTURE_PROB2)
    return 442;
  else if(rndom < CONSTANT_TEAM_PLAYING_STRUCTURE_PROB3)
    return 352;
  else if(rndom < CONSTANT_TEAM_PLAYING_STRUCTURE_PROB4)
    return 433;

  return 343;
}

/** Return a stadium struct with default values. */
Stadium
team_stadium_new(void)
{
    Stadium new;

    new.capacity = -1;
    new.safety = math_gauss_disti(80, 100);
    new.average_attendance = 
	new.possible_attendance = 
	new.games = 0;

    return new;
}

/* Fill the players array of the team.
   @param tm The team that gets filled. */
void
team_generate_players(Team *tm)
{
    gint i;    
    gfloat skill_factor = math_rnd(1 - CONSTANT_TEAM_SKILL_VARIANCE,
				   1 + CONSTANT_TEAM_SKILL_VARIANCE);
    Player new;
    gint average_skill;

    if(tm->clid < ID_CUP_START)
	average_skill = 
	    (gint)rint((gfloat)team_return_league_cup_value_int(tm, LEAGUE_CUP_VALUE_AVERAGE_SKILL) *
		       skill_factor);
    else
	average_skill = 
	    (gint)rint((gfloat)lig(0).average_skill +
		       (gfloat)team_return_league_cup_value_int(tm, LEAGUE_CUP_VALUE_SKILL_DIFF)) *
	    skill_factor;

    average_skill = CLAMP(average_skill, 0, CONSTANT_PLAYER_MAX_SKILL);
    
    for(i=0;i<CONSTANT_TEAM_MAX_PLAYERS;i++)
    {
	new = player_new(tm, average_skill);
	g_array_append_val(tm->players, new);
    }
}

/** Return a certain value from the league or cup struct
    the team belongs to.
    @param tm The team we examine.
    @param value_type This tells us which value to return. 
    @see #LeagueCupValue */
gint
team_return_league_cup_value_int(const Team *tm, gint value_type)
{
    gint idx = league_cup_get_index_from_clid(tm->clid);

    if(tm->clid >= ID_CUP_START)
	switch(value_type)
	{
	    case LEAGUE_CUP_VALUE_ID:
		return cp(idx).id;
	    case LEAGUE_CUP_VALUE_LAST_WEEK:
		return cp(idx).last_week;
	    case LEAGUE_CUP_VALUE_WEEK_GAP:
		return cp(idx).week_gap;
	    case LEAGUE_CUP_VALUE_YELLOW_RED:
		return cp(idx).yellow_red;
	    case LEAGUE_CUP_VALUE_AVERAGE_CAPACITY:
		return cp(idx).average_capacity;
	    case LEAGUE_CUP_VALUE_CUP_TYPE:
		return cp(idx).type;
	    case LEAGUE_CUP_VALUE_SKILL_DIFF:
		return cp(idx).skill_diff;
	    default:
		g_warning("team_return_league_cup_value_int: unknown value_type for cups: %d\n", 
			  value_type);
		return -1;
	}

    switch(value_type)
    {
	case LEAGUE_CUP_VALUE_ID:
	    return lig(idx).id;
	case LEAGUE_CUP_VALUE_FIRST_WEEK:
	    return lig(idx).first_week;
	case LEAGUE_CUP_VALUE_WEEK_GAP:
	    return lig(idx).week_gap;
	case LEAGUE_CUP_VALUE_YELLOW_RED:
	    return lig(idx).yellow_red;
	case LEAGUE_CUP_VALUE_AVERAGE_SKILL:
	    return lig(idx).average_skill;
	case LEAGUE_CUP_VALUE_AVERAGE_CAPACITY:
	    return lig(idx).average_capacity;
	default:
	    g_warning("team_return_league_cup_value_int: unknown value_type for leagues: %d\n",
		      value_type);
	    return -1;
    }

    return -1;
}

/** Print name or short name or such of the team's league or cup
    into a string.
    @param tm The pointer to the team.
    @param value_type Determines which value we want; @see #LeagueCupValue
    @param buf The buffer we print the string into. */
void
team_get_league_cup_string(const Team *tm, gint value_type, gchar *buf)
{
    gint idx = league_cup_get_index_from_clid(tm->clid);

    if(tm->clid >= ID_CUP_START)
	switch(value_type)
	{
	    default:
		sprintf(buf, "%s", lig(idx).name->str);
		break;
	    case LEAGUE_CUP_VALUE_SHORT_NAME:
		sprintf(buf, "%s", lig(idx).short_name->str);
		break;
	    case LEAGUE_CUP_VALUE_SID:
		sprintf(buf, "%s", lig(idx).sid->str);
		break;
	    case LEAGUE_CUP_VALUE_SYMBOL:
		sprintf(buf, "%s", lig(idx).symbol->str);
		break;
	}
    else
	switch(value_type)
	{
	    default:
		sprintf(buf, "%s", cp(idx).name->str);
		break;
	    case LEAGUE_CUP_VALUE_SHORT_NAME:
		sprintf(buf, "%s", cp(idx).short_name->str);
		break;
	    case LEAGUE_CUP_VALUE_SID:
		sprintf(buf, "%s", cp(idx).sid->str);
		break;
	    case LEAGUE_CUP_VALUE_SYMBOL:
		sprintf(buf, "%s", cp(idx).symbol->str);
		break;
	}
}

/** Copy a team to another team. The destination team
    has to be a fully allocated team (because it gets 
    freed before we copy).
    @param source The team we copy.
    @param dest The team we overwrite. */
void
team_copy(const Team *source, Team *dest)
{
    gint i;
    Player new_player;

    free_team(dest);
    
    *dest = *source;

    dest->name = g_string_new(source->name->str);

    dest->players = g_array_new(FALSE, FALSE, sizeof(Player));
    
    for(i=0;i<source->players->len;i++)
    {
	new_player = player_new(dest, CONSTANT_PLAYER_MAX_SKILL);
	free_player(&new_player);
	player_copy(&g_array_index(source->players, Player, i),
		    &new_player);
	g_array_append_val(dest->players, new_player);
    }
}

/** Copy a team to an array in a way that allows us to
    free the team afterwards.
    @param tm The team we copy.
    @param teams_array The array of teams we copy the team to.
    @see team_copy() */
void
team_append_to_array(const Team *tm, GArray *teams_array)
{
    Team new_team = team_new();

    team_copy(tm, &new_team);

    g_array_append_val(teams_array, new_team);
}

/** Copy a team to an array and assign new ids.
    @param tm The team we copy.
    @param teams_array The array of teams we copy the team to.
    @param clid The cup/league id we assign.
    @param id The team id we assign.
 */
void
team_append_to_array_with_ids(const Team *tm, GArray *teams_array, gint clid, gint id)
{
    team_append_to_array(tm, teams_array);

    g_array_index(teams_array, Team, teams_array->len - 1).clid = clid;
    g_array_index(teams_array, Team, teams_array->len - 1).id = id;
}

/** Check whether the team is already part of an
    international cup. We'd like to avoid having Real Madrid
    both in the Champions' League and in the CWC.
    @param tm The team we check (by comparing names).
    @return TRUE if the team's already participating in a cup,
    FALSE otherwise. */
gboolean
query_is_in_international_cups(const Team *tm)
{
    gint i, j;

    for(i=0;i<cps->len;i++)
    {
	for(j=0;j<cp(i).teams->len;j++)
	    if(cp(i).type == CUP_TYPE_INTERNATIONAL &&
	       strcmp(tm->name->str, g_array_index(cp(i).teams, Team, j).name->str) == 0)
		return TRUE;

	for(j=0;j<cp(i).user_teams->len;j++)
	    if(tm == g_ptr_array_index(cp(i).user_teams, j))
		return TRUE;
    }

    return FALSE;
}

/** Check whether a team participates in a cup.
    @param tm The team.
    @param cup The cup.
    @return TRUE or FALSE. */
gboolean
query_is_in_cup(const Team *tm, const Cup *cup)
{
    gint i;

    if(tm->clid >= ID_CUP_START)
	return (tm->clid == cup->id);

    if(cup->type == CUP_TYPE_INTERNATIONAL)
    {
	for(i=0;i<cup->user_teams->len;i++)
	    if(tm == g_ptr_array_index(cup->user_teams, i))
	       return TRUE;

	return FALSE;
    }

    for(i=0;i<cup->fixtures->len;i++)
	if(tm == g_array_index(cup->fixtures, Fixture, i).teams[0] ||
	   tm == g_array_index(cup->fixtures, Fixture, i).teams[1])
	    return TRUE;

    return FALSE;
}

/** Return a GPtrArray containing the pointers
    to the teams from the teams array.
    @param teams The teams array we use.
    @return A GPtrArray containing pointers to the teams. */
GPtrArray*
team_get_pointers_from_array(const GArray *teams)
{
    gint i;
    GPtrArray *team_pointers = g_ptr_array_new();

    for(i=0;i<teams->len;i++)
	g_ptr_array_add(team_pointers, (gpointer)&g_array_index(teams, Team, i));

    return team_pointers;
}

/** Return a pointer array containing the teams from
    the leagues that are specified in the choose_teams array.
    @param choose_teams The choose_team array.
    @return A pointer array containing team pointers. */
GPtrArray*
team_get_pointers_from_choose_teams(const GArray *choose_teams)
{
    gint i, j, k;
    CupChooseTeam *ct = NULL;
    GPtrArray *teams = g_ptr_array_new();

    for(i=0;i<choose_teams->len;i++)
    {
	ct = &g_array_index(choose_teams, CupChooseTeam, i);
	for(j=0;j<ligs->len;j++)
	    if(strcmp(ct->sid->str, lig(j).sid->str) == 0)
	    {
		if(ct->number_of_teams == -1)
		    for(k=0;k<lig(j).teams->len;k++)
			g_ptr_array_add(teams, &g_array_index(lig(j).teams, Team, k));
		else
		{
		    gint order[ct->end_idx - ct->start_idx + 1];
		
		    for(k=0;k<ct->end_idx - ct->start_idx + 1;k++)
			order[k] = k;

		    if(ct->randomly)
			math_generate_permutation(order, 0, ct->end_idx - ct->start_idx);

		    for(k=0;k<ct->number_of_teams;k++)
			g_ptr_array_add(teams, &g_array_index(lig(j).teams, Team, order[k]));
		}
	    }
    }

    return teams;
}

/** Return the pointer to the team belonging to
    the two ids.
    @param clid The league/cup id of the team.
    @param id The id of the team.
    @return The pointer to the team. */
Team*
team_get_pointer_from_ids(gint clid, gint id)
{    
    if(clid < ID_CUP_START)
	return &g_array_index(lig(clid % 1000).teams, Team, id);
    else
	return &g_array_index(cp(clid % 1000).teams, Team, id);
}

/** Return the players of the team in a pointer array.
    @param tm The team we examine.
    @return The players of the team in an array. */
GPtrArray*
team_get_player_pointers(const Team *tm)
{
    gint i;
    GPtrArray *players = g_ptr_array_new();

    for(i=0;i<tm->players->len;i++)
	g_ptr_array_add(players, (gpointer)&g_array_index(tm->players, Player, i));

    return players;
}

/** Return a pointer to the next fixture the team plays in.
    @param tm The team we examine.
    @return The pointer to the fixture or NULL if none is found. */
Fixture*
team_get_next_fixture(const Team *tm)
{
    gint i, j;
    Fixture *fix = NULL;

    for(i=0;i<ligs->len;i++)
    {
	if(lig(i).id == tm->clid)
	{
	    for(j=0;j<lig(i).fixtures->len;j++)
		if((g_array_index(lig(i).fixtures, Fixture, j).teams[0] == tm ||
		    g_array_index(lig(i).fixtures, Fixture, j).teams[1] == tm) &&
		   (fix == NULL || query_fixture_is_earlier(fix, &g_array_index(lig(i).fixtures, Fixture, j))))
		    fix = &g_array_index(lig(i).fixtures, Fixture, j);
	}
    }

    for(i=0;i<cps->len;i++)
    {
	if(cp(i).type == CUP_TYPE_NATIONAL ||
	   query_is_in_cup(tm, &cp(i)))
	{
	    for(j=0;j<cp(i).fixtures->len;j++)
		if((g_array_index(cp(i).fixtures, Fixture, j).teams[0] == tm ||
		    g_array_index(cp(i).fixtures, Fixture, j).teams[1] == tm) &&
		   (fix == NULL || query_fixture_is_earlier(fix, &g_array_index(cp(i).fixtures, Fixture, j))))
		    fix = &g_array_index(cp(i).fixtures, Fixture, j);
	}
    }

    return fix;
}

/** Calculate the average cskill of the first 11 players.
    @param tm The team we examine. 
    @return The average skill. */
gfloat
team_average_cskill(const Team *tm)
{
    gint i;
    gfloat sum = 0;

    for(i=0;i<MIN(11, tm->players->len);i++)
	sum += ((gfloat)player_of(tm, i)->cskill * powf((gfloat)player_of(tm, i)->fitness / 100, 0.25));

    return sum / (gfloat)(i - 1);
}
