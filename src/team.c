#include "cup.h"
#include "fixture.h"
#include "free.h"
#include "league.h"
#include "maths.h"
#include "option.h"
#include "player.h"
#include "team.h"
#include "user.h"
#include "variables.h"

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
*/
gint
team_assign_playing_style(void)
{
  gfloat rndom;

  rndom = math_rnd(0,1);

  /* all out defend */
  if(rndom < const_float("float_team_playing_style_prob1"))
    return -2;
  /* defend */
  else if(rndom < const_float("float_team_playing_style_prob2"))
    return -1;
  /* balanced */
  else if(rndom < const_float("float_team_playing_style_prob3"))
    return 0;
  /* attack */
  else if(rndom < const_float("float_team_playing_style_prob4"))
    return 1;

  /* all out attack */
  return 2;
}

/**
   Return a random playing structure.
*/
gint
team_assign_playing_structure(void)
{
  gfloat rndom = math_rnd(0,1);
    
  if(rndom < const_float("float_team_playing_structure_prob1"))
    return 532;
  else if(rndom < const_float("float_team_playing_structure_prob2"))
    return 442;
  else if(rndom < const_float("float_team_playing_structure_prob3"))
    return 352;
  else if(rndom < const_float("float_team_playing_structure_prob4"))
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
    gfloat skill_factor = math_rnd(1 - const_float("float_team_skill_variance"),
				   1 + const_float("float_team_skill_variance"));
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

    average_skill = CLAMP(average_skill, 0, const_int("int_player_max_skill"));
    
    for(i=0;i<const_int("int_team_max_players");i++)
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

    if(tm->clid < ID_CUP_START)
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
	new_player = player_new(dest, const_int("int_player_max_skill"));
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

/** Check whether the team is a user-managed team.
    @param tm The team we examine.
    @return The user's index in the #users array or -1.*/
gint
team_is_user(const Team *tm)
{
    gint i;

    for(i=0;i<users->len;i++)
	if(usr(i).tm == tm)
	    return i;

    return -1;
}

/** Return the overall average skill or
    the cskill of the first 11 players.
    @param tm The team we examine.
    @param cskill Whether to take into account all players. */
gfloat
team_get_average_skill(const Team *tm, gboolean cskill)
{
    gint i, counter = 0;
    gfloat sum = 0;

    if(!cskill)
    {
	for(i=0;i<tm->players->len;i++)
	    if(player_of(tm, i)->cskill != 0)
	    {
		sum += player_of(tm, i)->skill;
		counter++;
	    }
    }
    else
	for(i=0;i<11;i++)
	{
	    sum += player_of(tm, i)->cskill * 
		powf((gfloat)player_of(tm, i)->fitness / 100, 
		     const_float("float_player_fitness_exponent"));
	    counter++;
	}

    return sum / (gfloat)counter;
}

/** Return the rank of the team.
    @param tm The team we examine. */
gint
team_rank(const Team *tm)
{
    gint i, j;
    GArray *elements = NULL;

    if(tm->clid < ID_CUP_START)
    {
	elements = league_from_clid(tm->clid)->table.elements;
	for(i=0;i<elements->len;i++)
	    if(g_array_index(elements, TableElement, i).team == tm)
		return i + 1;
    }
    else
    {
	for(i=0;i<cup_from_clid(tm->clid)->tables->len;i++)
	{
	    elements = g_array_index(cup_from_clid(tm->clid)->tables, Table, i).elements;
	    for(j=0;j<elements->len;j++)
		if(g_array_index(elements, TableElement, j).team == tm)
		    return j + 1;
	}
    }

    return -1;
}

/** Return the structure that fits the positions of
    the first 11 players.
    @param tm The team we examine.
    @return A new structure. */
gint
team_find_appropriate_structure(const Team *tm)
{
  gint i;
  gint structure = 0;

  for(i=1;i<11;i++)
    {
	if(player_of(tm, i)->pos == PLAYER_POS_DEFENDER)
	    structure += 100;
	else if(player_of(tm, i)->pos == PLAYER_POS_MIDFIELDER)
	    structure += 10;
	else
	    structure++;
    }
    
  return structure;
}

/** Change the structure of a team and the appropriate
    cpos and cskill values.
    @param tm The team.
    @param new_structure The new structure value, e.g. 442. */
void
team_change_structure(Team *tm, gint new_structure)
{
  gint i;

  tm->structure = new_structure;

  for(i=1;i<11;i++)
    {
	player_of(tm, i)->cpos =
	    player_get_position_from_structure(new_structure, i);

	player_of(tm, i)->cskill =
	    player_get_cskill(player_of(tm, i));
    }
}

/* Try to set each of the first 11 players on his
   favoured position and sort the substitutes by position.
   @param tm The team we rearrange. */
void
team_rearrange(Team *tm)
{
  gint i, j;

  /* reaarrange field players */
  for(i=0;i<11;i++)
    {
      if(player_of(tm, i)->pos !=
	 player_of(tm, i)->cpos)
      {
	  for(j=i+1;j<11;j++)
	      if(player_of(tm, j)->pos ==
		 player_of(tm, i)->cpos &&
		 player_of(tm, j)->cskill > 0)
	      {
		  player_swap(tm, i, tm, j);
		  break;
	      }
      }
    }
    
  /* sort substitutes */
  i = 11;
  while(i != tm->players->len)
    {
	for(j=i+1;j<tm->players->len;j++)
	    if(player_of(tm, i)->pos >
	       player_of(tm, j)->pos)
	    {
		player_swap(tm, i, tm, j);
		i = 10;
		break;
	    }
	i++;
    }
}
