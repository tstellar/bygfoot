#include "cup.h"
#include "free.h"
#include "game_gui.h"
#include "league.h"
#include "maths.h"
#include "misc.h"
#include "option.h"
#include "player.h"
#include "team.h"
#include "user.h"

/** Create and return a new player.
    @param tm The team the player will belong to.
    @param average_skill The average skill of the team. 
    The player's skill can deviate from this value by #CONSTANT_PLAYER_AVERAGE_SKILL_VARIANCE %
    @return A newly created player. */
Player
player_new(Team *tm, gfloat average_skill)
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
    new.age = math_gauss_dist(const_float("float_player_age_lower"),
			      const_float("float_player_age_upper"));
    new.peak_age =
	math_rnd(const_float("float_player_peak_age_lower") +
		 (new.pos == PLAYER_POS_GOALIE) * 
		 const_float("float_player_peak_age_goalie_addition"),
		 const_float("float_player_peak_age_upper") +
		 (new.pos == PLAYER_POS_GOALIE) * 
		 const_float("float_player_peak_age_goalie_addition"));

    new.skill = CLAMP(average_skill * skill_factor, 0, 
		      const_float("float_player_max_skill"));
    new.talent = player_new_talent(new.skill);
    player_estimate_talent(&new);

    if(new.peak_age - new.age > const_float("float_player_peak_age_diff_younger1") ||
       new.peak_age - new.age < const_float("float_player_peak_age_diff_older1"))
	new.skill = new.skill * (1 - const_float("float_player_skill_reduction1"));
    else if(new.peak_age - new.age > const_float("float_player_peak_age_diff_younger2") ||
	    new.peak_age - new.age < const_float("float_player_peak_age_diff_peak_older"))
	new.skill = new.skill * (1 - const_float("float_player_skill_reduction2"));

    new.cskill = new.skill;

    new.fitness = math_rnd(const_float("float_player_fitness_lower"),
			   const_float("float_player_fitness_upper"));
    new.health = new.recovery = 0;
    new.value = player_assign_value(&new);
    new.wage = player_assign_wage(&new);
    new.contract = math_rnd(const_float("float_player_contract_lower"),
			    const_float("float_player_contract_upper"));
    new.lsu = math_rnd(const_float("float_player_lsu_lower"),
		       const_float("float_player_lsu_upper"));
    new.cards = g_array_new(FALSE, FALSE, sizeof(PlayerCard));
    new.games_goals = g_array_new(FALSE, FALSE, sizeof(PlayerGamesGoals));

    new.team = tm;
    new.participation = FALSE;
    new.offers = 0;
    
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
	    return i;
    }

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
gfloat
player_new_talent(gfloat skill)
{
    gfloat talent = math_gauss_dist(2 * skill - const_float("float_player_max_skill"),
				    const_float("float_player_max_skill"));
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
    gfloat scout_deviance[QUALITY_END];

    /* the maximal deviance in both directions */
    gfloat deviance_bound[2] =
	{pl->talent - pl->skill, const_float("float_player_max_skill") - pl->talent};

    for(i=0;i<QUALITY_END;i++)
    {
	scout_deviance[i] = (i + 1) * const_float("float_player_max_skill") *
	    (const_float("float_player_etal_scout_factor") / 100);
	/* adjust deviance_bounds with regard to the scout's
	   deviance */
	for(j=0;j<2;j++)
	    deviance_bound[j] = MIN(deviance_bound[j], scout_deviance[i]);
	
	pl->etal[i] = math_rnd(pl->talent - deviance_bound[0],
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
    gfloat diff = pl->peak_age - pl->age;

    value = (gint)powf((const_float("float_player_value_skill_weight") * pl->skill +
			(1 - const_float("float_player_value_skill_weight")) * pl->talent * 0.7),
		       const_float("float_player_value_power"));
    
    if(diff > const_float("float_player_peak_age_diff_older1"))
	value = (gint)rint((gfloat)value * (1 - const_float("float_player_value_scale1")));
    else if(diff > const_float("float_player_peak_age_diff_peak_older"))
	value = (gint)rint((gfloat)value * (1 - const_float("float_player_value_scale2")));
    else if(diff < const_float("float_player_peak_age_diff_younger1"))
	value = (gint)rint((gfloat)value * (1 + const_float("float_player_value_scale1")));
    else if(diff < const_float("float_player_peak_age_diff_younger2"))
	value = (gint)rint((gfloat)value * (1 + const_float("float_player_value_scale2")));

    value = math_round_integer(value, 2);

    return value;
}

/** Assign a wage to a player. The wage depends mainly on
    the value.
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
	if(player_of(tm, i)->id == player_id)
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
	if(type == PLAYER_VALUE_GOALS)
	    sum += g_array_index(pl->games_goals, PlayerGamesGoals, i).goals;
	else if(type == PLAYER_VALUE_GAMES)
	    sum += g_array_index(pl->games_goals, PlayerGamesGoals, i).games;
	else if(type == PLAYER_VALUE_SHOTS)
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

/** Compare two players in a (pointer) array.
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
	    return_value = 0;
/* 	    return_value =  */
/* 		misc_float_compare(player_get_game_skill(pl1, TRUE), */
/* 				   player_get_game_skill(pl2, TRUE)); */
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
    gfloat skill_for_pos1 = player_get_cskill(pl1, position) * 
	powf(pl1->fitness, const_float("float_player_fitness_exponent")),
	skill_for_pos2 = player_get_cskill(pl2, position) * 
	powf(pl2->fitness, const_float("float_player_fitness_exponent"));
    gfloat game_skill1 = player_get_game_skill(pl1, FALSE),
	game_skill2 = player_get_game_skill(pl2, FALSE);
    gboolean good_structure1 =
	player_substitution_good_structure(pl1->team->structure, position, pl1->pos),
	good_structure2 =
	player_substitution_good_structure(pl2->team->structure, position, pl2->pos);
    gint return_value = 0;

    if(pl1->pos == position && pl2->pos == position)
	return_value = 
	    misc_float_compare(game_skill1, game_skill2);
    else if(pl1->pos == position)
	return_value = -1;
    else if(pl2->pos == position)
	return_value = 1;
    else if(position != PLAYER_POS_GOALIE)
    {
	if(good_structure1 && good_structure2)
	    return_value =
		misc_float_compare(game_skill1, game_skill2);
	else if(good_structure1)
	    return_value =
		misc_float_compare(game_skill1, skill_for_pos2);
	else if(good_structure2)
	    return_value =
		misc_float_compare(skill_for_pos1, game_skill2);
	else
	    return_value = 
		misc_float_compare(skill_for_pos1, skill_for_pos2);
    }
    else
	return_value = 
	    misc_float_compare(skill_for_pos1, skill_for_pos2);

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

/** Copy a player into a team.
    @param pl The player.
    @param tm The dest team.
    @param insert_at The new index of the player in the players array. */
void
player_copy(Player *pl, Team *tm, gint insert_at)
{
    Player new = *pl;

    new.team = tm;
    new.id = player_new_id(tm->players);
    
    g_array_insert_val(tm->players, insert_at, new);

    if(insert_at < 11)
	player_of(tm, insert_at)->cpos = 
	    player_get_position_from_structure(tm->structure, insert_at);
    else
	player_of(tm, insert_at)->cpos = player_of(tm, insert_at)->pos;

    player_of(tm, insert_at)->cskill =
	player_get_cskill(player_of(tm, insert_at), 
			  player_of(tm, insert_at)->cpos);
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
gfloat
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

	return MIN(pl->talent * cskill_factor, pl->skill);
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
    Fixture *fix = team_get_fixture(pl->team, FALSE);
    gint yellow_red = -1, yellow, red;

    if(fix == NULL)
	return 0;

    if(fix->clid < ID_CUP_START)
	yellow_red = league_from_clid(fix->clid)->yellow_red;
    else
	yellow_red = cup_from_clid(fix->clid)->yellow_red;
    
    yellow = player_card_get(pl, fix->clid, PLAYER_VALUE_CARD_YELLOW);
    red = player_card_get(pl, fix->clid, PLAYER_VALUE_CARD_RED);

    if(red > 0)
	return red;

    if(yellow == yellow_red - 1)
	return -1;

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
	return pl->skill * powf(pl->fitness, const_float("float_player_fitness_exponent"));

    return pl->cskill * powf(pl->fitness, const_float("float_player_fitness_exponent"));
}

/** Decrease a player's fitness during a match.
    @param pl The player.  */
void
player_decrease_fitness(Player *pl)
{
    gint i;
    gint age_limits[7] = {const_float("float_player_peak_age_diff_younger1"),
			  const_float("float_player_peak_age_diff_younger2"),
			  const_float("float_player_peak_age_diff_younger3"),
			  const_float("float_player_peak_age_diff_peak_younger"),
			  const_float("float_player_peak_age_diff_peak_older"),
			  const_float("float_player_peak_age_diff_older1"),
			  const_float("float_player_peak_age_diff_older2")};
    gfloat reduce[8] =
	{const_float("float_player_fitness_decrease_younger1"),
	 const_float("float_player_fitness_decrease_younger2"),
	 const_float("float_player_fitness_decrease_younger3"),
	 const_float("float_player_fitness_decrease_peak_younger"),
	 const_float("float_player_fitness_decrease_peak_older"),
	 const_float("float_player_fitness_decrease_older1"),
	 const_float("float_player_fitness_decrease_older2"),
	 const_float("float_player_fitness_decrease_else")};

    gfloat diff = pl->peak_age - pl->age;
    gfloat reduction;

    for(i=0;i<7;i++)
	if(diff > age_limits[i])
	    break;

    reduction = reduce[i] * (1 + (gfloat)pl->team->boost * const_float("float_player_boost_fitness_effect")) *
	(1 - (pl->cpos == 0) * const_float("float_player_fitness_decrease_factor_goalie"));
    
    pl->fitness = MAX(0, pl->fitness - reduction);
}


/** Increase of player fitness after a match.
    Depends on the decrease values and the
    percentage values that determine how much of the
    decrease a player gains.
    @param pl The player we edit. */
void
player_update_fitness(Player *pl)
{
    gint i;
    gint age_limits[7] = {const_float("float_player_peak_age_diff_younger1"),
			  const_float("float_player_peak_age_diff_younger2"),
			  const_float("float_player_peak_age_diff_younger3"),
			  const_float("float_player_peak_age_diff_peak_younger"),
			  const_float("float_player_peak_age_diff_peak_older"),
			  const_float("float_player_peak_age_diff_older1"),
			  const_float("float_player_peak_age_diff_older2")};
    gfloat increase_base[8] =
	{const_float("float_player_fitness_increase_younger1"),
	 const_float("float_player_fitness_increase_younger2"),
	 const_float("float_player_fitness_increase_younger3"),
	 const_float("float_player_fitness_increase_peak_younger"),
	 const_float("float_player_fitness_increase_peak_older"),
	 const_float("float_player_fitness_increase_older1"),
	 const_float("float_player_fitness_increase_older2"),
	 const_float("float_player_fitness_increase_else")};

    gfloat diff = pl->peak_age - pl->age;
    gfloat increase = -1;

    if(pl->participation)
	return;

    for(i=0;i<7;i++)
	if(diff > age_limits[i])
	    break;

    increase = math_rnd(increase_base[i] - const_float("float_player_fitness_increase_variance"),
			increase_base[i] + const_float("float_player_fitness_increase_variance"));

    pl->fitness = MIN(pl->fitness + increase, 1);
}

/** Return the number of yellow cards of a player
    or the number of weeks banned.
    @param pl The player.
    @param clid The cup or league id.
    @param card_type Whether red or yellow cards. */
gint
player_card_get(const Player *pl, gint clid, gint card_type)
{
    gint i;
    gint return_value = 0;
    
    for(i=0;i<pl->cards->len;i++)
	if(g_array_index(pl->cards, PlayerCard, i).clid == clid)
	{
	    if(card_type == PLAYER_VALUE_CARD_YELLOW)
		return_value = g_array_index(pl->cards, PlayerCard, i).yellow;
	    else
		return_value = g_array_index(pl->cards, PlayerCard, i).red;

	    break;
	}

    return return_value;
}

/** Change a card value for the player.
    @param pl The player.
    @param clid The cup or league id.
    @param card_type Whether red or yellow card.
    @param value The new value.
    @param diff Whether we add the value to the old one or
    replace the old value by the new one. */
void
player_card_set(Player *pl, gint clid, gint card_type, gint value, gboolean diff)
{
    gint i, *card_value = NULL;
    PlayerCard new;

    for(i=0;i<pl->cards->len;i++)
	if(g_array_index(pl->cards, PlayerCard, i).clid == clid)
	{
	    if(card_type == PLAYER_VALUE_CARD_YELLOW)
		card_value = &g_array_index(pl->cards, PlayerCard, i).yellow;
	    else if(card_type == PLAYER_VALUE_CARD_RED)
		card_value = &g_array_index(pl->cards, PlayerCard, i).red;

	    if(diff)
		*card_value += value;
	    else
		*card_value = value;

	    if(*card_value < 0)
	    {
		g_warning("player_card_set: negative card value; setting to 0\n");
		*card_value = 0;
	    }
	    
	    return;
	}

    new.clid = clid;
    new.yellow = new.red = 0;

    g_array_append_val(pl->cards, new);

    player_card_set(pl, clid, card_type, value, diff);
}

/** Return the number of games or goals.
    @param pl The player.
    @param clid The cup or league id.
    @param card_type Whether games or goals cards. */
gint
player_games_goals_get(const Player *pl, gint clid, gint type)
{
    gint i, return_value = 0;

    for(i=0;i<pl->games_goals->len;i++)
	if(g_array_index(pl->games_goals, PlayerGamesGoals, i).clid == clid)
	{
	    if(type == PLAYER_VALUE_GAMES)
		return_value = g_array_index(pl->games_goals, PlayerGamesGoals, i).games;
	    else if(type == PLAYER_VALUE_GOALS)
		return_value = g_array_index(pl->games_goals, PlayerGamesGoals, i).goals;
	    else if(type == PLAYER_VALUE_SHOTS)
		return_value = g_array_index(pl->games_goals, PlayerGamesGoals, i).shots;
	    else
		g_warning("player_games_goals_get: unknown type %d.\n", type);
	}

    return return_value;
}

/** Change a card value for the player.
    @param pl The player.
    @param clid The cup or league id.
    @param type Whether games or goals or shots.
    @param value The new value.
    @param diff Whether we add the value to the old one or
    replace the old value by the new one. */
void
player_games_goals_set(Player *pl, gint clid, gint type, gint value, gboolean diff)
{
    gint i, *games_goals_value = NULL;
    PlayerGamesGoals new;

    for(i=0;i<pl->games_goals->len;i++)
	if(g_array_index(pl->games_goals, PlayerGamesGoals, i).clid == clid)
	{
	    if(type == PLAYER_VALUE_GAMES)
		games_goals_value = &g_array_index(pl->games_goals, PlayerGamesGoals, i).games;
	    else if(type == PLAYER_VALUE_GOALS)
		games_goals_value = &g_array_index(pl->games_goals, PlayerGamesGoals, i).goals;
	    else if(type == PLAYER_VALUE_SHOTS)
		games_goals_value = &g_array_index(pl->games_goals, PlayerGamesGoals, i).shots;

	    if(diff)
		*games_goals_value += value;
	    else
		*games_goals_value = value;

	    if(*games_goals_value < 0)
	    {
		g_warning("player_games_goals_set: negative value; setting to 0\n");
		*games_goals_value = 0;
	    }
	    
	    return;
	}

    new.clid = clid;
    new.games = new.goals = new.shots = 0;

    g_array_append_val(pl->games_goals, new);

    player_games_goals_set(pl, clid, type, value, diff);
}

/** Update skill and lsu of a user player.
    @param pl The player we update. */
void
player_update_skill(Player *pl)
{
    gint i;
    gint age_limits[7] =
	{const_float("float_player_peak_age_diff_younger1"),
	 const_float("float_player_peak_age_diff_younger2"),
	 const_float("float_player_peak_age_diff_younger3"),
	 const_float("float_player_peak_age_diff_peak_younger"),
	 const_float("float_player_peak_age_diff_peak_older"),
	 const_float("float_player_peak_age_diff_older1"),
	 const_float("float_player_peak_age_diff_older2")};
    gfloat factor_limits[3][2] =
	{{const_float("float_player_skill_devel_younger1_lower"),
	  const_float("float_player_skill_devel_younger1_upper")},
	 {const_float("float_player_skill_devel_younger2_lower"),
	  const_float("float_player_skill_devel_younger2_upper")},
	 {const_float("float_player_skill_devel_younger3_lower"),
	  const_float("float_player_skill_devel_younger3_upper")}};
    gint increase_decrease[4][2] =
	{{const_float("float_player_skill_devel_peak_lower"),
	  const_float("float_player_skill_devel_peak_upper")},
	 {const_float("float_player_skill_devel_older1_lower"),
	  const_float("float_player_skill_devel_older1_upper")},
	 {const_float("float_player_skill_devel_older2_lower"),
	  const_float("float_player_skill_devel_older2_upper")},
	 {const_float("float_player_skill_devel_else_lower"),
	  const_float("float_player_skill_devel_else_upper")}};
    gfloat diff;

    if(pl->age > pl->peak_age)
	pl->lsu += (pl->health == 0) ? 1 : const_float("float_player_lsu_injured_old");
    else if(pl->health == 0 &&
	    ((gfloat)player_games_goals_get(pl, pl->team->clid, PLAYER_VALUE_GAMES) / (gfloat)week >=
	     const_float("float_player_lsu_games_percentage") || 
	     math_rnd(0, 1) <= const_float("float_player_lsu_increase_prob")))
	pl->lsu++;

    if(pl->lsu < const_float("float_player_lsu_update_limit") ||
       math_rnd(0, 1) < powf(const_float("float_player_lsu_update_base_prob"),
			     pl->lsu - const_float("float_player_lsu_update_limit")))
	return;
	
    pl->lsu = 0;

    diff = pl->peak_age - pl->age;

    for(i=0;i<7;i++)
	if(diff > age_limits[i])
	    break;

    if(i < 3)
	pl->skill += (pl->talent - pl->skill) * math_rnd(factor_limits[i][0], factor_limits[i][1]);
    else if(i < 5)
	pl->skill += math_rnd(increase_decrease[0][0], increase_decrease[0][1]);
    else if(i < 7)
	pl->skill += math_rnd(increase_decrease[i - 4][0], increase_decrease[i - 4][1]);
    else
	pl->skill += math_rnd(increase_decrease[3][0], increase_decrease[3][1]);

    pl->skill = CLAMP(pl->skill, 0, pl->talent);
    pl->cskill = player_get_cskill(pl, pl->cpos);
    pl->value = player_assign_value(pl);

    for(i=0;i<QUALITY_END;i++)
	if(pl->skill > pl->etal[i])
	{
	    player_estimate_talent(pl);
	    break;
	}
}

/** Decrement the number of weeks until
    recovery depending on the user's physio.
    @param pl An injured player. */
void
player_update_injury(Player *pl)
{
    gint i, j;
    gfloat rndom;
    gint physio = user_from_team(pl->team)->physio;
    gfloat injury_decrease_probs[4][3] =
	{{const_float("float_player_injury_recovery_best0"),
	  const_float("float_player_injury_recovery_best1"),
	  const_float("float_player_injury_recovery_best2")},
	 {const_float("float_player_injury_recovery_good0"),
	  const_float("float_player_injury_recovery_good1"),
	  const_float("float_player_injury_recovery_good2")},
	 {const_float("float_player_injury_recovery_good2"),
	  const_float("float_player_injury_recovery_good1"),
	  const_float("float_player_injury_recovery_good0")},
	 {const_float("float_player_injury_recovery_best2"),
	  const_float("float_player_injury_recovery_best1"),
	  const_float("float_player_injury_recovery_best0")}};

    for(j=1;j<3;j++)
	injury_decrease_probs[physio][j] += injury_decrease_probs[physio][j - 1];

    rndom = math_rnd(0, 1);

    for(i=0;i<3;i++)
	if(rndom <= injury_decrease_probs[physio][i])
	{
	    pl->recovery = MAX(pl->recovery - i, 0);
	    break;
	}

    if(pl->recovery == 0)
    {
	pl->health = 0;
	pl->fitness = math_rnd(const_float("float_player_injury_recovery_fitness_lower"),
			       const_float("float_player_injury_recovery_fitness_upper"));
    }
}

/** Update players in user teams (age, skill, fitness etc.)
    @param tm The team of the player.
    @param idx The index in the players array. */
void
player_update_weekly(Team *tm, gint idx)
{
    Player *pl = player_of(tm, idx);
    
    pl->age += 0.0192;
    pl->contract -= 0.0192;

    if(pl->contract <= 0)
	player_remove_contract(tm, idx);

    player_update_skill(pl);
    if(pl->health > 0)
	player_update_injury(pl);    
}

/** Remove a player from a user team after the contract expired.
    @param tm The user team.
    @param idx The player index. */
void
player_remove_contract(Team *tm, gint idx)
{
    user_event_add(user_from_team(tm), EVENT_TYPE_PLAYER_LEFT, -1, -1, NULL,
		   player_of(tm, idx)->name->str);
    player_remove_from_team(tm, idx);
}

/** Remove a player from a team.
    @param idx The player index in the players array. */
void
player_remove_from_team(Team *tm, gint idx)
{
    free_player(player_of(tm, idx));
    g_array_remove_index(tm->players, idx);    
}

/** Make some player updates after a match
    for user players.
    @param pl The player we update.
    @param clid The fixture clid. */
void
player_update_post_match(Player *pl, gint clid)
{
    if(pl->health == 0)
	player_update_fitness(pl);
    		
    if(player_card_get(pl, clid, PLAYER_VALUE_CARD_RED) > 0)
	player_card_set(pl, clid, PLAYER_VALUE_CARD_RED, -1, TRUE);
}

/** Replace a player by a new one in a cpu team. 
    @param free_player Whether to free the memory occupied
    by the player. */
void
player_replace_by_new(Player *pl, gboolean free_player)
{
    Team *tm = pl->team;
    gint idx = player_id_index(tm, pl->id);
    Player new = player_new(tm, team_get_average_skill(tm, FALSE));
    
    new.pos = pl->pos;
    new.cpos = pl->cpos;
    new.id = pl->id;

    if(free_player)
	player_remove_from_team(tm, idx);
    else
	g_array_remove_index(tm->players, idx);
    g_array_insert_val(tm->players, idx, new);
}

/** Update players in user teams.
    @param tm The team of the player.
    @param idx The index in the players array. */
/*d maybe argument player pointer?*/
void
player_update_week_roundly(Team *tm, gint idx)
{
    Player *pl = player_of(tm, idx);

    pl->cskill = (pl->health > 0 || player_is_banned(pl) > 0) ?
	0 : player_get_cskill(pl, pl->cpos);
}

/** Return injury descriptions.  */
gchar*
player_injury_to_char(gint injury_type)
{      
    switch(injury_type)
    {
	default:
	    g_warning("player_injury_to_char: unknown type %d\n", injury_type);
	    return NULL;
	    break;
	case PLAYER_INJURY_NONE:
	    return _("OK");
	    break;
	case PLAYER_INJURY_CONCUSSION:
	    return _("Concussion");
	    break;
	case PLAYER_INJURY_PULLED_MUSCLE:
	    return _("Pulled muscle");
	    break;
	case PLAYER_INJURY_HAMSTRING:
	    return _("Hamstring");
	    break;
	case PLAYER_INJURY_GROIN:
	    return _("Groin injury");
	    break;
	case PLAYER_INJURY_FRAC_ANKLE:
	    return _("Fractured ankle");
	    break;
	case PLAYER_INJURY_RIB:
	    return _("Broken rib");
	    break;
	case PLAYER_INJURY_LEG:
	    return _("Broken leg");
	    break;
	case PLAYER_INJURY_BROK_ANKLE:
	    return _("Broken ankle");
	    break;
	case PLAYER_INJURY_ARM:
	    return _("Broken arm");
	    break;
	case PLAYER_INJURY_SHOULDER:
	    return _("Broken shoulder");
	    break;
	case PLAYER_INJURY_LIGAMENT:
	    return _("Torn crucial ligament");
	    break;
    }

    return NULL;
}
