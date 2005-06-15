#include "cup.h"
#include "fixture.h"
#include "game.h"
#include "game_gui.h"
#include "league.h"
#include "maths.h"
#include "misc.h"
#include "option.h"
#include "player.h"
#include "team.h"
#include "transfer.h"
#include "user.h"

/**
   Generate a team with default values, e.g. 
   random playing structure and an empty string
   as name.
   @return A new team.
*/
Team
team_new(gboolean new_id)
{
    Team new;

    new.name = g_string_new("");
    new.names_file = g_string_new("");
    new.symbol = g_string_new("");
    
    new.clid = -1;
    new.id = (new_id) ? team_id_new : -1;
    new.structure = team_assign_playing_structure();
    new.style = team_assign_playing_style();
    new.boost = 0;

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
    new.safety = 0;

    new.average_attendance = 
	new.possible_attendance = 
	new.games = 0;

    return new;
}

/* Fill the players array of the team and the stadium.
   @param tm The team that gets filled. */
void
team_generate_players_stadium(Team *tm)
{
    gint i;    
    gfloat skill_factor = math_rnd(1 - const_float("float_team_skill_variance"),
				   1 + const_float("float_team_skill_variance"));
    Player new;
    gfloat average_skill;
    gfloat wages = 0;

    if(tm->clid < ID_CUP_START)
	average_skill = 
	    const_float("float_player_max_skill") * skill_factor *
	    (((gfloat)league_from_clid(tm->clid)->average_skill) / 10000);
    else
	average_skill = 
	    skill_factor * team_get_average_skills(lig(0).teams) *
	    (1 + ((gfloat)cup_from_clid(tm->clid)->skill_diff / 10000));

    average_skill = CLAMP(average_skill, 0, const_float("float_player_max_skill"));    

    for(i=0;i<const_int("int_team_cpu_players");i++)
    {
	new = player_new(tm, average_skill, TRUE);
	wages += new.wage;
	g_array_append_val(tm->players, new);
    }

    tm->stadium.average_attendance = tm->stadium.possible_attendance =
	tm->stadium.games = 0;    
    tm->stadium.safety = 
	math_rnd(const_float("float_team_stadium_safety_lower"),
		 const_float("float_team_stadium_safety_upper"));

    tm->stadium.capacity = 
	math_round_integer((gint)rint((wages / (gfloat)const_int("int_team_stadium_ticket_price")) *
				      const_float("float_team_stadium_size_wage_factor")), 2);
}

/** Check whether the team is already part of an
    international cup. We'd like to avoid having Real Madrid
    both in the Champions' League and in the CWC.
    @param tm The team we check (by comparing names).
    @param group The cup group the team shouldn't be in.
    @return TRUE if the team's already participating in a cup,
    FALSE otherwise. */
gboolean
query_team_is_in_international_cups(const Team *tm, gint group)
{
    gint i, j;

    if(group == -1)
	return FALSE;

    for(i=0;i<cps->len;i++)
	if(cp(i).group == group)
	    for(j=0;j<cp(i).team_names->len;j++)
		if(strcmp(tm->name->str, ((GString*)g_ptr_array_index(cp(i).team_names, j))->str) == 0)
		{
		    if(debug > 90)
			printf("team %s group %d found in %s \n", tm->name->str,
			       group, cp(i).name->str);
		    return TRUE;
		}
    
    return FALSE;
}

/** Check whether a team participates in a cup.
    @param tm The team.
    @param cup The cup.
    @return TRUE or FALSE. */
gboolean
query_team_is_in_cup(const Team *tm, const Cup *cup)
{
    gint i;

    for(i=0;i<cup->team_names->len;i++)
	if(strcmp(tm->name->str, ((GString*)g_ptr_array_index(cup->team_names, i))->str) == 0)
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

/** Return the pointer to the team belonging to
    the id. */
Team*
team_of_id(gint id)
{    
    gint i, j;

    for(i=0;i<ligs->len;i++)
	for(j=0;j<lig(i).teams->len;j++)
	    if(g_array_index(lig(i).teams, Team, j).id == id)
		return &g_array_index(lig(i).teams, Team, j);

    for(i=0;i<cps->len;i++)
	for(j=0;j<cp(i).teams->len;j++)
	    if(((Team*)g_ptr_array_index(cp(i).teams, j))->id == id)
		return (Team*)g_ptr_array_index(cp(i).teams, j);

    g_warning("team_of_id: team with id %d not found.", id);

    return NULL;
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
	g_ptr_array_add(players, &g_array_index(tm->players, Player, i));

    return players;
}

/** Return a pointer to the next or last fixture the team participates in.
    @param tm The team we examine.
    @return The pointer to the fixture or NULL if none is found. */
Fixture*
team_get_fixture(const Team *tm, gboolean last_fixture)
{
    gint i, j;
    Fixture *fix = NULL;

    if(!last_fixture && 
       (stat0 == STATUS_LIVE_GAME_PAUSE ||
	stat0 == STATUS_SHOW_LIVE_GAME) &&
       (tm == ((LiveGame*)statp)->fix->teams[0] ||
	tm == ((LiveGame*)statp)->fix->teams[1]))
	return ((LiveGame*)statp)->fix;
    
    if(!last_fixture)
    {
	for(i=0;i<ligs->len;i++)
	{
	    if(lig(i).id == tm->clid)
	    {
		for(j=0;j<lig(i).fixtures->len;j++)
		    if(g_array_index(lig(i).fixtures, Fixture, j).attendance == -1 &&
		       query_fixture_team_involved((&g_array_index(lig(i).fixtures, Fixture, j)), tm->id))
		    {
			fix = &g_array_index(lig(i).fixtures, Fixture, j);
			break;
		    }

		break;
	    }
	}

	for(i=0;i<acps->len;i++)
	    if(fix == NULL ||
	       fix->week_number != week ||
	       fix->week_round_number != week_round)
	    {
		if(query_cup_is_national(acp(i)->id) ||
		   query_team_is_in_cup(tm, acp(i)))
		{
		    for(j=0;j<acp(i)->fixtures->len;j++)
			if(g_array_index(acp(i)->fixtures, Fixture, j).attendance == -1 &&
			   query_fixture_team_involved((&g_array_index(acp(i)->fixtures, Fixture, j)), tm->id) &&
			   (fix == NULL ||
			    query_fixture_is_earlier(&g_array_index(acp(i)->fixtures, Fixture, j), fix)))
			{
			    fix = &g_array_index(acp(i)->fixtures, Fixture, j);
			    break;
			}
		}
	    }
    }
    else
    {
	for(i=0;i<ligs->len;i++)
	{
	    if(lig(i).id == tm->clid)
	    {
		for(j=lig(i).fixtures->len - 1;j>=0;j--)
		    if(g_array_index(lig(i).fixtures, Fixture, j).attendance != -1 &&
		       query_fixture_team_involved((&g_array_index(lig(i).fixtures, Fixture, j)), tm->id))
		    {
			fix = &g_array_index(lig(i).fixtures, Fixture, j);
			break;
		    }
		
		break;
	    }
	}

	for(i=0;i<acps->len;i++)
	    if(fix == NULL ||
	       fix->week_number != week ||
	       fix->week_round_number != week_round - 1)
	    {
		if(query_cup_is_national(acp(i)->id) ||
		   query_team_is_in_cup(tm, acp(i)))
		{
		    for(j=acp(i)->fixtures->len - 1;j>=0;j--)
			if(g_array_index(acp(i)->fixtures, Fixture, j).attendance != -1 &&
			   query_fixture_team_involved((&g_array_index(acp(i)->fixtures, Fixture, j)), tm->id) &&
			   (fix == NULL ||
			    query_fixture_is_later(&g_array_index(acp(i)->fixtures, Fixture, j), fix)))
			{
			    fix = &g_array_index(acp(i)->fixtures, Fixture, j);
			    break;
			}
		}
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
	    if(player_of_idx_team(tm, i)->cskill != 0)
	    {
		sum += player_of_idx_team(tm, i)->skill;
		counter++;
	    }
    }
    else
	for(i=0;i<11;i++)
	{
	    sum += player_get_game_skill(player_of_idx_team(tm, i), FALSE);
	    counter++;
	}

    return (counter > 0) ? sum / (gfloat)counter : 0;
}

/** Return the rank of the team in the league tables. */
gint
team_get_league_rank(const Team *tm)
{
    gint i;
    GArray *elements = league_from_clid(tm->clid)->table.elements;

    for(i=0;i<elements->len;i++)
	if(g_array_index(elements, TableElement, i).team == tm)
	    return i + 1;
    
    g_warning("team_get_league_rank: no rank found for team %s in league %s. \n",
	      tm->name->str, league_cup_get_name_string(tm->clid));
    return -1;
}

/** Return the rank of the team in the round robin stage. */
gint
team_get_cup_rank(const Team *tm, const CupRound *cupround)
{
    gint i, j;

    for(i=0;i<cupround->tables->len;i++)
    {
	for(j=0;j<g_array_index(cupround->tables, Table, i).elements->len;j++)
	    if(g_array_index(g_array_index(cupround->tables, Table, i).elements, TableElement, j).team_id == tm->id)
		return j + 1;
    }

    g_warning("team_get_cup_rank: no rank found for team %s. \n ", tm->name->str);
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

  for(i=0;i<11;i++)
      if(player_of_idx_team(tm, i)->cskill > 0 && player_of_idx_team(tm, i)->pos != 0)
	  structure += (gint)rint(powf(10, PLAYER_POS_FORWARD - player_of_idx_team(tm, i)->pos));

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
	player_of_idx_team(tm, i)->cpos =
	    player_get_position_from_structure(new_structure, i);

	player_of_idx_team(tm, i)->cskill =
	    player_get_cskill(player_of_idx_team(tm, i), 
			      player_of_idx_team(tm, i)->cpos, TRUE);
    }
}

/* Try to set each of the first 11 players on his
   favoured position and sort the substitutes by position.
   @param tm The team we rearrange. */
void
team_rearrange(Team *tm)
{
    gint i;

    g_array_sort_with_data(tm->players, (GCompareDataFunc)player_compare_func,
			   GINT_TO_POINTER(100 + PLAYER_COMPARE_ATTRIBUTE_POS));

    for(i=0;i<tm->players->len;i++)
    {
	player_of_idx_team(tm, i)->cpos = (i < 11) ?
	    player_get_position_from_structure(tm->structure, i) : player_of_idx_team(tm, i)->pos;
	if(player_of_idx_team(tm, i)->cskill > 0)
	    player_of_idx_team(tm, i)->cskill = (i < 11) ?
		player_get_cskill(player_of_idx_team(tm, i), 
				  player_of_idx_team(tm, i)->cpos, TRUE) : player_of_idx_team(tm, i)->skill;
    }
}

/** Return the name of the current setting of a team attribute, e.g. style.
    @param  tm The team.
    @param attribute The attribute. */
gchar*
team_attribute_to_char(gint attribute, gint value)
{
    switch(attribute)
    {
	default:
	    g_warning("team_attribute_to_char: unknown attribute %d\n", attribute);
	    break;
	case TEAM_ATTRIBUTE_STYLE:
	    switch(value)
	    {
		case -2:
		    return _("ALL OUT DEFEND");
		case -1:
		    return _("DEFEND");
		case 0:
		    return _("BALANCED");
		case 1:
		    return _("ATTACK");
		case 2:
		    return _("ALL OUT ATTACK");
	    }
	    break;
	case TEAM_ATTRIBUTE_BOOST:
	    switch(value)
	    {
		case -1:
		    return _("ANTI");
		case 0:
		    return _("OFF");
		case 1:
		    return _("ON");
	    }
	    break;
    }

    return NULL;
}

/** Change a team attribute of the current user and print a message.
    @param attribute The attribute.
    @param new_value The new value. */
void
team_change_attribute_with_message(Team *tm, gint attribute, gint new_value)
{
    gchar buf[SMALL];

    switch(attribute)
    {
	default:
	    g_warning("team_attribute_to_char: unknown attribute %d\n", attribute);
	    break;
	case TEAM_ATTRIBUTE_STYLE:
	    current_user.tm->style = new_value;
	    sprintf(buf, "Team style changed to %s.", team_attribute_to_char(attribute, new_value));
	    break;
	case TEAM_ATTRIBUTE_BOOST:
	    current_user.tm->boost = new_value;
	    sprintf(buf, "Boost changed to %s.", team_attribute_to_char(attribute, new_value));
	    break;
    }

    game_gui_print_message(buf);
}

/** Make cpu players healthy etc.
    @param tm The cpu team.
    @param reset_fitness Whether to reset the fitness values of playes. */
void
team_update_cpu_corrections(Team *tm, gboolean reset_fitness)
{
    gint i, j;
    Player *pl;

    for(i=0;i<tm->players->len;i++)
    {
	pl = player_of_idx_team(tm, i);

	for(j=0;j<pl->cards->len;j++)
	    g_array_index(pl->cards, PlayerCard, j).red = 0;	

	if(pl->cskill == 0)
	{
	    pl->health = pl->recovery = 0;
	    pl->cskill = player_get_cskill(pl, pl->cpos, FALSE);

	    pl->fitness = math_rnd(const_float("float_player_fitness_lower"),
				   const_float("float_player_fitness_upper"));
	}
	
	if(pl->pos != pl->cpos)
	{
	    pl->pos = pl->cpos;
	    pl->cskill = player_get_cskill(pl, pl->cpos, FALSE);
	}

	if(reset_fitness)
	    pl->fitness = math_rnd(const_float("float_player_fitness_lower"),
				   const_float("float_player_fitness_upper"));
    }
    
    team_rearrange(tm);
    
    tm->structure = team_find_appropriate_structure(tm);

    if(!player_substitution_good_structure(tm->structure, PLAYER_POS_DEFENDER, PLAYER_POS_DEFENDER))
    {
	tm->structure = team_assign_playing_structure();
	for(i=0;i<tm->players->len;i++)
	{
	    pl = player_of_idx_team(tm, i);
	    pl->pos = pl->cpos = player_get_position_from_structure(tm->structure, i);
	}
    }
}

/** Make some random substitutions in the cpu team. */
void
team_update_cpu_subs(Team *tm)
{
    gint i;
    gint number_of_subs = math_rndi(const_int("int_team_subs_lower"),
				    const_int("int_team_subs_upper"));
    gint player_numbers[11];

    math_generate_permutation(player_numbers, 0, 10);

    for(i=0;i<number_of_subs;i++)
	game_substitute_player(tm, player_numbers[i]);

    if(player_get_game_skill(player_of_idx_team(tm, 0), 0) <
       player_get_game_skill(player_of_idx_team(tm, 10), 0) &&
       math_rnd(0, 1) < const_float("float_team_replace_worse_goalie"))
	game_substitute_player(tm, 0);
}

/** Change a cpu team's structure. */
void
team_update_cpu_structure(Team *tm)
{
    gint i;
    Player *pl = NULL;

    tm->structure = team_assign_playing_structure();

    for(i=0;i<tm->players->len;i++)
    {
	pl = &g_array_index(tm->players, Player, i);
	pl->pos = pl->cpos = player_get_position_from_structure(tm->structure, i);
    }
}

/** Replace some players by new ones in a team. */
void
team_update_cpu_new_players(Team *tm)
{
    gint i;
    gint number_of_new = math_rndi(const_int("int_team_new_players_lower"),
				   const_int("int_team_new_players_upper"));
    gint player_numbers[tm->players->len];

    math_generate_permutation(player_numbers, 0, tm->players->len - 1);

    for(i=0;i<number_of_new;i++)
    {
	if(!query_transfer_player_is_on_list(player_of_idx_team(tm, player_numbers[i])))
	    player_replace_by_new(player_of_idx_team(tm, player_numbers[i]), TRUE);
    }
}

/** Heal players, re-set fitnesses, make some random subs
    and replace some players with new ones.
    @param tm The team we examine.
    @param reset_fitness Whether to reset the fitness values of players. */
void
team_update_cpu_team(Team *tm, gboolean reset_fitness)
{
    team_update_cpu_corrections(tm, reset_fitness);
    team_update_cpu_subs(tm);

    if(math_rnd(0, 1) < const_float("float_team_new_player_probability"))
	team_update_cpu_new_players(tm);

    if(math_rnd(0, 1) < const_float("float_team_new_style"))
	tm->style = team_assign_playing_style();

    if(math_rnd(0, 1) < const_float("float_team_new_structure"))
	team_update_cpu_structure(tm);
}

/** Increase player ages etc.
    @param tm The user team we examine. */
void
team_update_user_team_weekly(Team *tm)
{
    gint i;

    for(i=tm->players->len - 1;i>=0;i--)
	player_update_weekly(tm, i);
}

/** Regenerate player fitness etc. after a match. 
    @param tm The user team we examine.
    @param clid The fixture clid. */
void
team_update_post_match(Team *tm, gint clid)
{
    gint i;

    for(i=0;i<tm->players->len;i++)
	player_update_post_match(player_of_idx_team(tm, i), clid);
}

/** Some updates each round.
    @param tm The user team we examine. */
void
team_update_user_team_week_roundly(Team *tm)
{
    gint i;

    for(i=0;i<tm->players->len;i++)
	player_update_week_roundly(tm, i);
}

/** Return a value from the league table element going with the team.
    @param type The type of the value. */
gint
team_get_table_value(const Team *tm, gint type)
{
    gint i;
    const GArray *elements = NULL;

    if(tm->clid >= ID_CUP_START)
    {
	g_warning("team_get_table_value: team is not a league team: %s \n", tm->name->str);
	return -1;
    }
    
    elements = league_from_clid(tm->clid)->table.elements;

    for(i=0;i<elements->len;i++)
	if(g_array_index(elements, TableElement, i).team_id == tm->id)
	    break;

    if(i == elements->len)
    {
	g_warning("team_get_table_value: table entry not found for team %s \n", tm->name->str);
	return -1;
    }

    return g_array_index(elements, TableElement, i).values[type];
}

/** Compare function for team arrays or pointer arrays. */
gint
team_compare_func(gconstpointer a, gconstpointer b, gpointer data)
{
    gint type = GPOINTER_TO_INT(data) % 100;
    const Team *tm1 = (GPOINTER_TO_INT(data) < 100) ? 
	*(const Team**)a : (const Team*)a;
    const Team *tm2 = (GPOINTER_TO_INT(data) < 100) ? 
	*(const Team**)b : (const Team*)b;
    gint return_value = 0;

    if(type == TEAM_COMPARE_LEAGUE_RANK)
    {
	if(tm1->clid == tm2->clid)
	    return_value = misc_int_compare(team_get_league_rank(tm2), team_get_league_rank(tm1));
	else
	    return_value = misc_int_compare(league_from_clid(tm2->clid)->layer,
					    league_from_clid(tm1->clid)->layer);
    }
    else if(type == TEAM_COMPARE_LEAGUE_LAYER)
	return_value = misc_int_compare(league_from_clid(tm2->clid)->layer,
					league_from_clid(tm1->clid)->layer);
    else if(type == TEAM_COMPARE_OFFENSIVE)
    {
	gint gf1 = team_get_table_value(tm1, TABLE_GF),
	    gf2 = team_get_table_value(tm2, TABLE_GF),
	    ga1 = team_get_table_value(tm1, TABLE_GA),
	    ga2 = team_get_table_value(tm2, TABLE_GA);

	if(gf1 > gf2)
	    return_value = -1;
	else if(gf1 < gf2)
	    return_value = 1;
	else if(ga1 < ga2)
	    return_value =  -1;
	else if(ga1 > ga2)
	    return_value = 1;
	else
	    return_value = 0;
    }
    else if(type == TEAM_COMPARE_DEFENSE)
    {
	gint gf1 = team_get_table_value(tm1, TABLE_GF),
	    gf2 = team_get_table_value(tm2, TABLE_GF),
	    ga1 = team_get_table_value(tm1, TABLE_GA),
	    ga2 = team_get_table_value(tm2, TABLE_GA);

	if(ga1 > ga2)
	    return_value = 1;
	else if(ga1 < ga2)
	    return_value = -1;
	else if(gf1 > gf2)
	    return_value =  -1;
	else if(gf1 < gf2)
	    return_value = 1;
	else
	    return_value = 0;
    }
    else if(type == TEAM_COMPARE_UNSORTED)
	return_value = 0;

    return return_value;
}

/** Return the teams from all leagues sorted by the
    specified function. 
    @param type The integer to pass to the compare function.
    @param cup Whether we return the international cup teams or league teams. */
GPtrArray*
team_get_sorted(GCompareDataFunc compare_function, gint type, gboolean cup)
{ 
    gint i, j;
    GPtrArray *teams = g_ptr_array_new();

    if(!cup)
    {
	for(i=0;i<ligs->len;i++)
	    for(j=0;j<lig(i).teams->len;j++)
		g_ptr_array_add(teams, &g_array_index(lig(i).teams, Team, j));
    }
    else
    {
	for(i=0;i<acps->len;i++)
	    for(j=0;j<acp(i)->teams->len;j++)
		g_ptr_array_add(teams, g_ptr_array_index(acp(i)->teams, j));
    }

    g_ptr_array_sort_with_data(teams, compare_function, GINT_TO_POINTER(type));

    return teams;
}

/** Find a new team for a user, depending on whether he's been
    fired or very successful. */
Team*
team_get_new(const Team *tm, gboolean fire)
{
    gint i;
    gint lower = 0, upper = 0;
    gint bound1 = (fire) ? const_int("int_team_new_bound_upper") :
	const_int("int_team_new_bound_lower"),
	bound2 = (fire) ? const_int("int_team_new_bound_lower") :
	const_int("int_team_new_bound_upper");
    gint idx = -1;
    GPtrArray *teams = team_get_sorted(team_compare_func, TEAM_COMPARE_LEAGUE_RANK, FALSE);
    Team *return_value;

    for(i=0;i<teams->len;i++)
	if((Team*)g_ptr_array_index(teams, i) != tm)
	    upper++;
	else
	{
	    idx = i;
	    break;
	}

    for(i=teams->len - 1; i >= 0; i--)
	if((Team*)g_ptr_array_index(teams, i) != tm)
	    lower++;
	else
	    break;
    
    bound1 = MIN(bound1, upper);
    bound2 = MIN(bound2, lower);

    return_value = (Team*)g_ptr_array_index(teams, math_rndi(i - bound1, i + bound2));
    while(return_value == tm)
	return_value = (Team*)g_ptr_array_index(teams, math_rndi(i - bound1, i + bound2));

    g_ptr_array_free(teams, TRUE);

    return return_value;
}

/** Return the index of the team in the teams array. */
gint
team_get_index(const Team *tm)
{
    gint i;
    gpointer *teams = league_cup_get_teams(tm->clid);

    if(tm->clid < ID_CUP_START)
    {
	for(i=0;i<((GArray*)teams)->len;i++)
	    if(&g_array_index((GArray*)teams, Team, i) == tm)
		return i;
    }
    else
    {
	for(i=0;i<((GPtrArray*)teams)->len;i++)
	    if((Team*)g_ptr_array_index((GPtrArray*)teams, i) == tm)
		return i;
    }

    g_warning("team_get_index: team %s not found.\n", tm->name->str);

    return -1;
}

/** Return the average of the average skills of the teams in the array. */
gfloat
team_get_average_skills(const GArray *teams)
{
    gint i;
    gfloat sum = 0;

    if(teams->len == 0)
	return 0;

    for(i=0;i<teams->len;i++)
	sum += team_get_average_skill(&g_array_index(teams, Team, i), FALSE);

    return sum / teams->len;
}

/** Find out whether a team plays at a given date. */
gboolean
query_team_plays(const Team *tm, gint week_number, gint week_round_number)
{
    gint i, j;

    for(i=0;i<ligs->len;i++)
	for(j=0;j<lig(i).fixtures->len;j++)
	    if(g_array_index(lig(i).fixtures, Fixture, j).week_number == week_number && 
	       g_array_index(lig(i).fixtures, Fixture, j).week_round_number == week_round_number &&
	       (g_array_index(lig(i).fixtures, Fixture, j).teams[0] == tm ||
		g_array_index(lig(i).fixtures, Fixture, j).teams[1] == tm))
		return TRUE;

    for(i=0;i<acps->len;i++)
	for(j=0;j<acp(i)->fixtures->len;j++)
	    if(g_array_index(acp(i)->fixtures, Fixture, j).week_number == week_number && 
	       g_array_index(acp(i)->fixtures, Fixture, j).week_round_number == week_round_number &&
	       (g_array_index(acp(i)->fixtures, Fixture, j).teams[0] == tm ||
		g_array_index(acp(i)->fixtures, Fixture, j).teams[1] == tm))
		return TRUE;

    return FALSE;
}

/** Find the league team with the specified name. */
Team*
team_get_from_name(const gchar *name)
{
    gint i, j;

    for(i=0;i<ligs->len;i++)
	for(j=0;j<lig(i).teams->len;j++)
	    if(strcmp(g_array_index(lig(i).teams, Team, j).name->str, name) == 0)
		return &g_array_index(lig(i).teams, Team, j);

    g_warning("team_get_from_name: team with name %s not found.\n", name);

    return NULL;
}

/** Show the results of the user team against the specified team. 
    @param buf The buffer to fill with the results.
    @param sort Whether to sort the results according to home/away. */
void
team_write_own_results(const Team *tm, gchar *buf, gboolean sort)
{
    gint i, res[2];
    gchar buf2[SMALL], buf3[SMALL], buf4[SMALL], neutral[SMALL];
    gint place;
    GPtrArray *matches = fixture_get_matches(current_user.tm, tm);        

    strcpy(buf4, "");
    strcpy(neutral, "");

    for(i=0;i<matches->len;i++)
    {
	res[0] = math_sum_int_array(((Fixture*)g_ptr_array_index(matches, i))->result[0], 2);
	res[1] = math_sum_int_array(((Fixture*)g_ptr_array_index(matches, i))->result[1], 2);
	
	if(res[((Fixture*)g_ptr_array_index(matches, i))->teams[0] != current_user.tm] >
	   res[((Fixture*)g_ptr_array_index(matches, i))->teams[0] == current_user.tm])
	    sprintf(buf2, _("W  %d : %d"), 
		    res[((Fixture*)g_ptr_array_index(matches, i))->teams[0] != current_user.tm],
		    res[((Fixture*)g_ptr_array_index(matches, i))->teams[0] == current_user.tm]);
	else if(res[((Fixture*)g_ptr_array_index(matches, i))->teams[0] != current_user.tm] <
		res[((Fixture*)g_ptr_array_index(matches, i))->teams[0] == current_user.tm])
	    sprintf(buf2, _("L  %d : %d"), 
		    res[((Fixture*)g_ptr_array_index(matches, i))->teams[0] != current_user.tm],
		    res[((Fixture*)g_ptr_array_index(matches, i))->teams[0] == current_user.tm]);
	else
	    sprintf(buf2, _("Dw %d : %d"),
		    res[((Fixture*)g_ptr_array_index(matches, i))->teams[0] != current_user.tm],
		    res[((Fixture*)g_ptr_array_index(matches, i))->teams[0] == current_user.tm]);
	
	if(((Fixture*)g_ptr_array_index(matches, i))->home_advantage)
	{
	    
	    if(((Fixture*)g_ptr_array_index(matches, i))->teams[0] == current_user.tm)
	    {
		sprintf(buf3, _("%s (H) "), buf2);
		place = 0;
	    }
	    else
	    {
		sprintf(buf3, _("<span background='%s' foreground='%s'>%s (A) </span> "), 
			const_app("string_treeview_live_game_commentary_away_bg"),
			const_app("string_treeview_live_game_commentary_away_fg"),
			buf2);
		place = 1;
	    }
	}
	else
	{
	    sprintf(buf3, _("%s (N) "), buf2);
	    place = -1;
	}

	if(!sort || place == 1)
	    strcat(buf4, buf3);
	else
	{
	    if(place == 0)
	    {
		strcpy(buf2, buf4);
		sprintf(buf4, "%s%s", buf3, buf2);
	    }
	    else if(place == -1)
		strcat(neutral, buf3);
	}
    }

    sprintf(buf, "%s%s", buf4, neutral);

    g_ptr_array_free(matches, TRUE);
}

/** Show a row of WDWWLL type results and the goals for and against.
    @param tm The team we find the results for.
    @param buf The buffer we print the results into. */
void
team_write_results(const Team *tm, gchar *result_buf, gchar *goals_buf)
{
    gint i;
    GPtrArray *latest_fixtures = fixture_get_latest(tm);
    gint res[2], goals[2] = {0, 0};
    gint end_idx = latest_fixtures->len - const_int("int_treeview_latest_results");

    strcpy(result_buf, "");
    end_idx = MAX(0, end_idx);
    for(i=latest_fixtures->len - 1;i>=end_idx;i--)
    {
	res[0] = math_sum_int_array(((Fixture*)g_ptr_array_index(latest_fixtures, i))->result[0], 3);
	res[1] = math_sum_int_array(((Fixture*)g_ptr_array_index(latest_fixtures, i))->result[1], 3);
	goals[0] += 
	    math_sum_int_array(((Fixture*)
				g_ptr_array_index(latest_fixtures, i))->
			       result[(((Fixture*)g_ptr_array_index(latest_fixtures, i))->teams[0] != tm)], 2);
	goals[1] += 
	    math_sum_int_array(((Fixture*)
				g_ptr_array_index(latest_fixtures, i))->
			       result[(((Fixture*)g_ptr_array_index(latest_fixtures, i))->teams[0] == tm)], 2);
	if(res[0] == res[1])
	    strcat(result_buf, _("Dw "));
	else if(res[(((Fixture*)g_ptr_array_index(latest_fixtures, i))->teams[0] == tm)] >
		res[(((Fixture*)g_ptr_array_index(latest_fixtures, i))->teams[0] != tm)])
	    strcat(result_buf, _("L  "));
	else
	    strcat(result_buf, _("W  "));
    }

    sprintf(goals_buf, "%d : %d", goals[0], goals[1]);
    g_ptr_array_free(latest_fixtures, TRUE);
}

/** Find out whether the team is in the given pointer array. */
gboolean
query_team_is_in_teams_array(const Team *tm, const GPtrArray *teams)
{
    gint i;

    for(i=0;i<teams->len;i++)
	if((Team*)g_ptr_array_index(teams, i) == tm)
	    return TRUE;

    return FALSE;
}
