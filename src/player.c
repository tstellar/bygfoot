#include "cup.h"
#include "free.h"
#include "league.h"
#include "maths.h"
#include "option.h"
#include "player.h"
#include "team.h"
#include "variables.h"

/** Create and return a new player.
    @param tm The team the player will belong to.
    @param average_skill The average skill of the team. 
    The player's skill can deviate from this value by #CONSTANT_PLAYER_AVERAGE_SKILL_VARIANCE %
    @return A newly created player. */
Player
player_new(Team *tm, gint average_skill)
{
    gfloat skill_factor = 
	math_rnd(1 - const_float("float_player_average_skill_variance"),
		 1 + const_float("float_player_average_skill_variance"));
    Player new;

    new.name = 
	g_string_new(((GString*)g_ptr_array_index(player_names, math_rndi(0, player_names->len - 1)))->str);
    new.id = player_new_id(tm->players);
    new.pos = player_get_position_from_structure(tm->structure, tm->players->len);
    new.cpos = new.pos;
    new.skill = CLAMP((gint)rint((gfloat)average_skill * skill_factor), 0, 
		      const_int("int_player_max_skill"));
    new.cskill = new.skill;
    new.age = math_gauss_disti(const_int("int_player_age_lower"),
			       const_int("int_player_age_upper"));
    new.peak_age =
	math_rndi(const_int("int_player_peak_age_lower") +
		  (new.pos == PLAYER_POS_GOALIE) * 
		  const_int("int_player_peak_age_goalie_addition"),
		  const_int("int_player_peak_age_upper") +
		  (new.pos == PLAYER_POS_GOALIE) * 
		  const_int("int_player_peak_age_goalie_addition"));
    new.talent = player_new_talent(new.skill);
    player_estimate_talent(&new);
    new.fitness = math_rndi(const_int("int_player_fitness_lower"),
			    const_int("int_player_fitness_upper"));
    new.health = new.recovery = 0;
    new.games_goals = g_array_new(FALSE, FALSE, sizeof(PlayerGamesGoals));
    new.value = player_assign_value(&new);
    new.wage = player_assign_wage(&new);
    new.contract = math_rndi(const_int("int_player_contract_lower"),
			     const_int("int_player_contract_upper"));
    new.lsu = math_rndi(const_int("int_player_lsu_lower"),
			const_int("int_player_lsu_upper"));
    new.cards = g_array_new(FALSE, FALSE, sizeof(PlayerCard));

    new.team = tm;
    
    return new;
}

/** Return a player id that's not yet 'occupied'.
    @param players The player array the new player will belong to.
    @return A new id that none of the other players has. */
gint
player_new_id(const GArray *players)
{
    gint i, j;

    for(i=0;i<const_int("int_team_max_players");i++)
    {
	for(j=0;j<players->len;j++)
	    if(g_array_index(players, Player, j).id == i)
		break;

	if(j == players->len)
	    return j;
    }

    g_warning("player_new_id: didn't find id for a new player.\n");

    return -1;
}

/** Return the appropriate position for the player with the given number.
    The position depends on the team structure if the player number is < 11
    and on some constants otherwise.
    @param structure The team structure, something like 442 or 352.
    @param player_number The number of the player within the team.
    @return A new position for the player. */
gint
player_get_position_from_structure(gint structure, gint player_number)
{
    gint position = -1;
    gint bound[2] =
	{math_get_place(structure, 3) + 1,
	 math_get_place(structure, 3) +
	 math_get_place(structure, 2) + 1};
    
    if(player_number % 11 == 0)
	position = PLAYER_POS_GOALIE;
    else if(player_number < bound[0] ||
	    (player_number > 10 && player_number < const_int("int_player_pos_bound1")))
	position = PLAYER_POS_DEFENDER;
    else if(player_number < bound[1] ||
	    (player_number > 10 && player_number < const_int("int_player_pos_bound2")))
	position = PLAYER_POS_MIDFIELDER;
    else
	position = PLAYER_POS_FORWARD;

    return position;
}

/** Calculate the talent value of the player based on his skill.
    @param skill The player's skill.
    @return The new talent value. */
gint
player_new_talent(gint skill)
{
    gint talent = math_gauss_disti(2 * skill - const_int("int_player_max_skill"),
				   const_int("int_player_max_skill"));
    if(talent < skill)
	talent = 2 * skill - talent;

    return talent;
}

/** Estimate a player's talent.
    @param pl The player of which we'd like to estimate the talent. */
void
player_estimate_talent(Player *pl)
{
    gint i, j;
    gint scout_deviance[QUALITY_END];

    /* the maximal deviance in both directions */
    gint deviance_bound[2] =
	{pl->talent - pl->skill, const_int("int_player_max_skill") - pl->talent};

    for(i=0;i<QUALITY_END;i++)
    {
	scout_deviance[i] = (i + 1) * const_int("int_player_etal_scout_factor");
	/* adjust deviance_bounds with regard to the scout's
	   deviance */
	for(j=0;j<2;j++)
	    deviance_bound[j] = MIN(deviance_bound[j], scout_deviance[i]);
	
	pl->etal[i] = math_rndi(pl->talent - deviance_bound[0],
				pl->talent + deviance_bound[1]);
    }
}

/** Assign a (transfer) value to a player. The value depends on skill,
    talent and age.
    @param pl The player we examine. 
    @return The value of the player. */
gint
player_assign_value(const Player *pl)
{
    gint value;

    value = (gint)powf((const_float("float_player_value_skill_weight") *
			(gfloat)pl->skill +
			(1 - const_float("float_player_value_skill_weight")) *
			(gfloat)pl->talent * 0.7),
		       const_float("float_player_value_power"));

    if(pl->age <= const_int("int_player_age_lower") + 2 * 52)
	value = (gint)((gfloat)value * 1.05);
    else if(pl->age <= const_int("int_player_age_lower") + 4 * 52)
	value = (gint)((gfloat)value * 1.1);
    else if(pl->age >= const_int("int_player_age_upper") - 4 * 52)
	value = (gint)((gfloat)value * 0.95);
    else if(pl->age >= const_int("int_player_age_upper") - 2 * 52)
	value = (gint)((gfloat)value * 0.9);

    value = math_round_integer(value, 2);

    return value;
}

/** Assign a value to a player. The value depends on skill,
    talent and age.
    @param pl The player we examine. 
    @return The wage of the player. */
gint
player_assign_wage(const Player *pl)
{
    gfloat wage;

    wage = rint(((gfloat)pl->value * const_float("float_player_wage_value_factor")) *
		math_rnd(1 - const_float("float_player_wage_random_dev"),
			 1 + const_float("float_player_wage_random_dev") ));

    return math_round_integer((gint)wage, 1);
}

/** Copy a player to another player. The destination player
    has to be a fully allocated player (because he gets 
    freed before we copy).
    @param source The player we copy.
    @param dest The player we overwrite. */
void
player_copy(const Player *source, Player *dest)
{
    gint i;

    free_player(dest);

    *dest = *source;
    dest->name = g_string_new(source->name->str);
    dest->cards = g_array_new(FALSE, FALSE, sizeof(PlayerCard));
    dest->games_goals = g_array_new(FALSE, FALSE, sizeof(PlayerGamesGoals));

    for(i=0;i<source->cards->len;i++)
	g_array_append_val(dest->cards, 
			   g_array_index(source->cards, PlayerCard, i));

    for(i=0;i<source->games_goals->len;i++)
	g_array_append_val(dest->games_goals, 
			   g_array_index(source->games_goals, PlayerGamesGoals, i));
}

/** Copy a player into a team in a way that allows us to
    free the player afterwards.
    @param pl The player we copy.
    @param tm The team we copy the player to.
    @see player_copy() */
void
player_append_to_array(const Player *pl, Team *tm)
{
    Player new_player = player_new(tm, const_int("int_player_max_skill"));

    player_copy(pl, &new_player);

    g_array_append_val(tm->players, new_player);
}

/** Remove a player from a team. */
void
player_remove_from_team(Team *tm, gint player_number)
{
    free_player(&g_array_index(tm->players, Player, player_number));

    g_array_remove_index(tm->players, player_number);
}

/** Return the pointer to the player given by the ids.
    @param clid The cup/league id of the team.
    @param team_id The id of the team.
    @param id The id of the player.
    @return A pointer to the player or NULL if he wasn't to be found. */
Player*
player_of_ids(gint clid, gint team_id, gint id)
{
    gint i;
    Team *tm = team_get_pointer_from_ids(clid, team_id);
    Player *pl = NULL;
    
    if(tm != NULL)
    {
	for(i=0;i<tm->players->len;i++)
	    if(g_array_index(tm->players, Player, i).id == id)
	    {
		pl = &g_array_index(tm->players, Player, i);
		break;
	    }		
    }

    if(pl == NULL)
	g_warning("player_of_ids: didn't find player with ids %d and %d and %d\n",
		  clid, team_id, id);

    return pl;
}

/** Return the number of all games or goals the player's
    participated in / scored in all cups and leagues.
    @param pl The player we examine. 
    @param goals Whether we sum up the goals.
    @return The number of goals. */
gint
player_all_games_goals(const Player *pl, gboolean goals)
{
    gint i, sum = 0;

    for(i=0;i<pl->games_goals->len;i++)
	if(goals)
	    sum += g_array_index(pl->games_goals, PlayerGamesGoals, i).goals;
	else
	    sum += g_array_index(pl->games_goals, PlayerGamesGoals, i).games;

    return sum;
}

/** Return the sum of all the yellow cards in all
    leagues and cups for the player.
    @param pl The player we examine.
    @return The number of all cards.*/
gint
player_all_cards(const Player *pl)
{
    gint i, sum = 0;

    for(i=0;i<pl->cards->len;i++)
	sum += g_array_index(pl->cards, PlayerCard, i).yellow;

    return sum;
}

/** Return a pointer to the number'th player of the team.
    @param tm The team.
    @param number The player number.
    @return A pointer to the player or NULL. */
Player*
player_of(const Team *tm, gint number)
{
    if(tm->players->len <= number)
    {
	g_warning("player_of: Player list of team %s too short for number %d.\n",
		  tm->name->str, number);
	return NULL;
    }

    return &g_array_index(tm->players, Player, number);
}


/** Return a pointer to the player with specified id of the team.
    @param tm The team.
    @param id The player's id.
    @return A pointer to the player or NULL. */
Player*
player_of_id(const Team *tm, gint id)
{
    gint i;
    
    for(i=0;i<tm->players->len;i++)
	if(g_array_index(tm->players, Player, i).id == id)
	    return &g_array_index(tm->players, Player, i);
    
    g_warning("player_of_id: didn't find player with id %d of team %s\n", id, tm->name->str);
    
    return NULL;
}

/** Compare two players in a pointer array.
    @param pl1 Pointer to the pointer to the first player.
    @param pl2 Pointer to the pointer to the second player.
    @param data Coded integer that tells us which attribute to compare. */
gint
player_compare_func(gconstpointer a, gconstpointer b, gpointer data)
{
    const Player *pl1 = *(const Player**)a;
    const Player *pl2 = *(const Player**)b;
    gint type = GPOINTER_TO_INT(data);
    gint return_value = 0;

    if(type == PLAYER_COMPARE_ATTRIBUTE_GAME_SKILL)
    {
	if(pl1->cskill == pl2->cskill && pl1->fitness == pl2->fitness)
	    return_value = 0;
	else if((gfloat)pl1->cskill * 
		powf((gfloat)pl1->fitness / 100, 
		     const_float("float_player_fitness_exponent")) >
		(gfloat)pl2->cskill * 
		powf((gfloat)pl2->fitness / 100, 
		     const_float("float_player_fitness_exponent")))
	    return_value = -1;
	else
	    return_value = 1;
    }

    return return_value;
}

/** Move a player from one player array to another one.
    @param tm1 The source team.
    @param player_number The index of the player in the players array.
    @param tm2 The dest team.
    @param insert_at The new index of the player in the players array. */
void
player_move(Team *tm1, gint player_number, Team *tm2, gint insert_at)
{
    Player pl = *player_of(tm1, player_number);

    pl.team = tm2;

    g_array_remove_index(tm1->players, player_number);

    g_array_insert_val(tm2->players, insert_at, pl);
}

/** Swap two players. */
void
player_swap(Team *tm1, gint player_number1, Team *tm2, gint player_number2)
{
    gint move = (tm1 == tm2 && player_number1 < player_number2) ? 
	-1 : 1;

    player_move(tm1, player_number1, tm2, player_number2);
    if(player_number2 < 11)
	player_of(tm2, player_number2)->cpos = 
	    player_get_position_from_structure(tm2->structure, player_number2);
    else
	player_of(tm2, player_number2)->cpos = player_of(tm2, player_number2)->pos;
    player_of(tm2, player_number2)->cskill =
	player_get_cskill(player_of(tm2, player_number2));

    player_move(tm2, player_number2 + move,
		tm1, player_number1);    
    if(player_number1 < 11)
	player_of(tm1, player_number1)->cpos = 
	    player_get_position_from_structure(tm1->structure, player_number1);
    else
	player_of(tm1, player_number1)->cpos = player_of(tm1, player_number1)->pos;
    player_of(tm1, player_number1)->cskill =
	player_get_cskill(player_of(tm1, player_number1));
}

/** Return the player's cskill depending on
    whether he's on his normal position or not.
    @param pl The player we examine.
    @return A new cskill. */
gint
player_get_cskill(const Player *pl)
{
    gfloat cskill_factor;

    if(pl->health != PLAYER_INJURY_NONE ||
       player_is_banned(pl) > 0)
	cskill_factor = 0.0;
    else if(pl->pos == pl->cpos)
	cskill_factor = 1.0;
    else if(pl->cpos == PLAYER_POS_GOALIE ||
	    pl->pos == PLAYER_POS_GOALIE)
	cskill_factor = 0.5;
    else if(abs(pl->cpos - pl->pos) == 2)
	cskill_factor = 0.65;
    else
    	cskill_factor = 0.75;

    if(pl->cpos != pl->pos)
	return MIN(pl->talent * cskill_factor, pl->skill);
    else
	return pl->skill * (cskill_factor != 0);
}

/** Find out whether the player is banned in
    the next match or whether he will be if he
    gets a yellow card.
    @param pl The player we examine.
    @return Number of games banned, or 0 if not banned, or -1
    if yellow. */
gint
player_is_banned(const Player *pl)
{
    gint i;
    Fixture *fix = team_get_next_fixture(pl->team);
    gint yellow_red;

    if(fix == NULL)
	return 0;

    if(fix->clid < ID_CUP_START)
	yellow_red = league_from_clid(fix->clid)->yellow_red;
    else
	yellow_red = cup_from_clid(fix->clid)->yellow_red;
    
    for(i=0;i<pl->cards->len;i++)
	if(g_array_index(pl->cards, PlayerCard, 0).clid == fix->clid)
	{
	    if(g_array_index(pl->cards, PlayerCard, 0).red > 0)
		return g_array_index(pl->cards, PlayerCard, 0).red;
	    
	    if(g_array_index(pl->cards, PlayerCard, 0).yellow ==
	       yellow_red - 1)
		return -1;

	    return 0;
	}

    return 0;
}
