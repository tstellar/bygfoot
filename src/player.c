#include "cup.h"
#include "free.h"
#include "game_gui.h"
#include "league.h"
#include "maths.h"
#include "misc.h"
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

/** Get a player's index in the players array from
    his id.
    @param tm The team.
    @param player_id The player's id. 
    @return The array index or -1. */
gint
player_id_index(const Team *tm, gint player_id)
{
    gint i;

    for(i=0;i<tm->players->len;i++)
	if(g_array_index(tm->players, Player, i).id == player_id)
	    return i;
    
    g_warning("player_id_index: didn't find player with id %d of team %s\n", player_id, tm->name->str);
    
    return -1;
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

/** Return the number of all games or goals the player's
    participated in / scored in all cups and leagues.
    @param pl The player we examine. 
    @param goals Whether we sum up the goals.
    @return The number of goals. */
gint
player_all_games_goals(const Player *pl, gint type)
{
    gint i, sum = 0;

    for(i=0;i<pl->games_goals->len;i++)
	if(type == PLAYER_LIST_ATTRIBUTE_GOALS)
	    sum += g_array_index(pl->games_goals, PlayerGamesGoals, i).goals;
	else if(type == PLAYER_LIST_ATTRIBUTE_GAMES)
	    sum += g_array_index(pl->games_goals, PlayerGamesGoals, i).games;
	else if(type == PLAYER_LIST_ATTRIBUTE_SHOTS)
	    sum += g_array_index(pl->games_goals, PlayerGamesGoals, i).shots;

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

/** Compare two players in a pointer array.
    @param pl1 Pointer to the pointer to the first player.
    @param pl2 Pointer to the pointer to the second player.
    @param data Coded integer that tells us which attribute to compare. 
    @return 1, 0 or -1 (see the C qsort() function). */
gint
player_compare_func(gconstpointer a, gconstpointer b, gpointer data)
{
    gint type = GPOINTER_TO_INT(data) % 100;
    const Player *pl1 = (GPOINTER_TO_INT(data) < 100) ? 
	*(const Player**)a : (const Player*)a;
    const Player *pl2 = (GPOINTER_TO_INT(data) < 100) ? 
	*(const Player**)b : (const Player*)b;
    gint return_value = 0;

    if(type == PLAYER_COMPARE_ATTRIBUTE_GAME_SKILL)
	return_value = 
	    misc_float_compare(player_get_game_skill(pl1, FALSE),
			       player_get_game_skill(pl2, FALSE));
    else if(type == PLAYER_COMPARE_ATTRIBUTE_POS)
    {
	if(MIN(player_id_index(pl1->team, pl1->id), player_id_index(pl2->team, pl2->id)) < 11 &&
	   MAX(player_id_index(pl1->team, pl1->id), player_id_index(pl2->team, pl2->id)) >= 11)
	    return_value = 
		(player_id_index(pl1->team, pl1->id) > player_id_index(pl2->team, pl2->id)) ?
		1 : -1;
	else if(pl1->cskill == 0)
	    return_value = (pl2->cskill == 0) ? 0 : 1;
	else if(pl2->cskill == 0)
	    return_value = (pl1->cskill == 0) ? 0 : -1;
	else if(pl2->pos != pl1->pos)
	    return_value = misc_int_compare(pl2->pos, pl1->pos);
	else
	    return_value = 
		misc_float_compare(player_get_game_skill(pl1, TRUE),
				   player_get_game_skill(pl2, TRUE));
    }

    return return_value;
}


/** Compare two players in a pointer array.
    @param pl1 Pointer to the pointer to the first player.
    @param pl2 Pointer to the pointer to the second player.
    @param data Coded integer that tells us which position we
    would like to substitute.
    @return 1, 0 or -1 (see the C qsort() function). */
gint
player_compare_substitute_func(gconstpointer a, gconstpointer b, gpointer data)
{
    const Player *pl1 = *(const Player**)a;
    const Player *pl2 = *(const Player**)b;
    gint position = GPOINTER_TO_INT(data);
    gint skill_for_pos1 = (gint)rint((gfloat)player_get_cskill(pl1, position) * 
				     powf((gfloat)pl1->fitness / 10000, 
					  const_float("float_player_fitness_impact_on_skill"))),
	skill_for_pos2 = (gint)rint((gfloat)player_get_cskill(pl2, position) * 
				     powf((gfloat)pl2->fitness / 10000, 
					  const_float("float_player_fitness_impact_on_skill")));
    gint game_skill1 = (gint)rint(player_get_game_skill(pl1, FALSE)),
	game_skill2 = (gint)rint(player_get_game_skill(pl2, FALSE));
    gboolean good_structure1 =
	player_substitution_good_structure(pl1->team->structure, position, pl1->pos),
	good_structure2 =
	player_substitution_good_structure(pl2->team->structure, position, pl2->pos);
    gint return_value = 0;

    if(pl1->pos == position && pl2->pos == position)
	return_value = 
	    misc_int_compare(game_skill1, game_skill2);
    else if(pl1->pos == position)
	return_value = -1;
    else if(pl2->pos == position)
	return_value = 1;
    else if(position != PLAYER_POS_GOALIE)
    {
	if(good_structure1 && good_structure2)
	    return_value =
		misc_int_compare(game_skill1, game_skill2);
	else if(good_structure1)
	    return_value =
		misc_int_compare(game_skill1, skill_for_pos2);
	else if(good_structure2)
	    return_value =
		misc_int_compare(skill_for_pos1, game_skill2);
	else
	    return_value = 
		misc_int_compare(skill_for_pos1, skill_for_pos2);
    }
    else
	return_value = 
	    misc_int_compare(skill_for_pos1, skill_for_pos2);

/*     printf("%s %d %s %d   %d\n", pl1->name->str, pl1->pos, pl2->name->str,  */
/* 	   pl2->pos, return_value); */
/*     printf("\t gaski %.1f %.1f skipos %.1f %.1f struc %d %d\n", */
/* 	   (gfloat)game_skill1 / 10000, (gfloat)game_skill2 / 10000, */
/* 	   (gfloat)skill_for_pos1 / 10000, (gfloat)skill_for_pos2 / 10000, */
/* 	   good_structure1, good_structure2); */
	   	   
    return return_value;
}

/** Find out whether substituting a player into a team
    and adapting the team structure results in a normal
    structure.
    @param old_structure The team's structure before substituting.
    @param old_pos The position of the player that gets replaced.
    @param player_pos The position of the player that comes into the game.
    @return TRUE or FALSE. */
gboolean
player_substitution_good_structure(gint old_structure, gint old_pos, gint player_pos)
{
    gint accepted_structures[5] = {532, 442, 352, 433, 343};
    gint new_structure = 
	old_structure - (gint)rint(powf(10, PLAYER_POS_FORWARD - old_pos)) +
	(gint)rint(powf(10, PLAYER_POS_FORWARD - player_pos));

    return query_integer_is_in_array(new_structure, accepted_structures, 0, 5);
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

    if(stat0 == STATUS_LIVE_GAME_PAUSE)
    {
	if((player_number1 < 11 && player_is_banned(player_of(tm1, player_number1))) || 
	   (player_number2 < 11 && player_is_banned(player_of(tm1, player_number2))))
	{
	    game_gui_show_warning("You can't replace a banned player.");
	    return;
	}
    }

    player_move(tm1, player_number1, tm2, player_number2);
    if(player_number2 < 11)
	player_of(tm2, player_number2)->cpos = 
	    player_get_position_from_structure(tm2->structure, player_number2);
    else
	player_of(tm2, player_number2)->cpos = player_of(tm2, player_number2)->pos;

    player_of(tm2, player_number2)->cskill =
	player_get_cskill(player_of(tm2, player_number2), player_of(tm2, player_number2)->cpos);

    player_move(tm2, player_number2 + move,
		tm1, player_number1);    
    if(player_number1 < 11)
	player_of(tm1, player_number1)->cpos = 
	    player_get_position_from_structure(tm1->structure, player_number1);
    else
	player_of(tm1, player_number1)->cpos = player_of(tm1, player_number1)->pos;

    player_of(tm1, player_number1)->cskill =
	player_get_cskill(player_of(tm1, player_number1), player_of(tm1, player_number1)->cpos);
}

/** Return the player's cskill depending on
    whether he's on his normal position or not.
    @param pl The player we examine.
    @param position The position we's like to put the player.
    @return A new cskill. */
gint
player_get_cskill(const Player *pl, gint position)
{
    gfloat cskill_factor;

    if(pl->health != PLAYER_INJURY_NONE ||
       player_is_banned(pl) > 0)
	return 0;

    if(pl->pos != position)
    {
	if(position == PLAYER_POS_GOALIE ||
	   pl->pos == PLAYER_POS_GOALIE)
	    cskill_factor = 0.5;
	else if(abs(position - pl->pos) == 2)
	    cskill_factor = 0.65;
	else
	    cskill_factor = 0.75;

	return MIN((gint)rint((gfloat)pl->talent * cskill_factor), pl->skill);
    }
    else
	return pl->skill;
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

/** Return the player's skill contribution to his team.
    @param pl The player.
    @param skill Whether to take his skill or current skill into account.
    @return A float value representing the player's contribution. */
gfloat
player_get_game_skill(const Player *pl, gboolean skill)
{
    if(skill)
	return (gfloat)pl->skill * powf((gfloat)pl->fitness / 10000,
					const_float("float_player_fitness_impact_on_skill"));

    return (gfloat)pl->cskill * powf((gfloat)pl->fitness / 10000,
				     const_float("float_player_fitness_impact_on_skill"));
}

/** Decrease a player's fitness during a match.
    @param pl The player.  */
void
player_decrease_fitness(Player *pl)
{
    gint i;
    gint age_limits[7] =
	{const_int("int_player_fitness_decrease_peak_age_diff1"),
	 const_int("int_player_fitness_decrease_peak_age_diff2"),
	 const_int("int_player_fitness_decrease_peak_age_diff3"),
	 const_int("int_player_fitness_decrease_peak_age_diff4"),
	 const_int("int_player_fitness_decrease_peak_age_diff5"),
	 const_int("int_player_fitness_decrease_peak_age_diff6"),
	 const_int("int_player_fitness_decrease_peak_age_diff7")};
    gint reduce[8] =
	{const_int("int_player_fitness_decrease_minus1"),
	 const_int("int_player_fitness_decrease_minus2"),
	 const_int("int_player_fitness_decrease_minus3"),
	 const_int("int_player_fitness_decrease_minus4"),
	 const_int("int_player_fitness_decrease_minus5"),
	 const_int("int_player_fitness_decrease_minus6"),
	 const_int("int_player_fitness_decrease_minus7"),
	 const_int("int_player_fitness_decrease_minus_else")};

    gint diff = pl->peak_age - pl->age;

    for(i=0;i<7;i++)
	if(diff > age_limits[i])
	    break;

    pl->fitness = MAX(0, pl->fitness - reduce[i]);
}
