#include "cup.h"
#include "fixture.h"
#include "free.h"
#include "game_gui.h"
#include "league.h"
#include "main.h"
#include "maths.h"
#include "misc.h"
#include "name.h"
#include "option.h"
#include "player.h"
#include "team.h"
#include "transfer.h"
#include "user.h"

/** Create and return a new player.
    @param tm The team the player will belong to.
    @param average_talent The average talent of the team. 
    @return A newly created player. */
Player
player_new(Team *tm, gfloat average_talent, gboolean new_id)
{
    gint i;
    gfloat skill_factor = 
	math_rnd(1 - const_float("float_player_average_talent_variance"),
		 1 + const_float("float_player_average_talent_variance"));
    Player new;

    new.name = (new_id) ? 
	name_get(tm->names_file) : NULL;

    new.id = (new_id) ? player_id_new : -1;
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

    new.peak_region = 
	math_gauss_dist(const_float("float_player_peak_region_lower"),
			const_float("float_player_peak_region_upper"));

    new.talent = CLAMP(average_talent * skill_factor, 0, 
		       const_float("float_player_max_skill"));

    new.skill = player_skill_from_talent(&new);
    new.cskill = new.skill;
    player_estimate_talent(&new);

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

    for(i=0;i<PLAYER_VALUE_END;i++)
	new.career[i] = 0;

    new.team = tm;
    new.participation = FALSE;
    new.offers = 0;
    
    new.streak = PLAYER_STREAK_NONE;
    new.streak_count = new.streak_prob = 0;

    return new;
}

/** Complete the player structure (some values have been read
    from a team def file). */
void
player_complete_def(Player *pl, gfloat average_talent)
{
    gint i;
    gfloat skill_factor = 
	math_rnd(1 - const_float("float_player_average_talent_variance"),
		 1 + const_float("float_player_average_talent_variance"));

    pl->peak_age = math_rnd(const_float("float_player_peak_age_lower") +
			    (pl->pos == PLAYER_POS_GOALIE) * 
			    const_float("float_player_peak_age_goalie_addition"),
			    const_float("float_player_peak_age_upper") +
			    (pl->pos == PLAYER_POS_GOALIE) * 
		 const_float("float_player_peak_age_goalie_addition"));

    if(opt_int("int_opt_load_defs") == 2)
    {
	pl->age = math_gauss_dist(const_float("float_player_age_lower"),
				  const_float("float_player_age_upper"));
	pl->peak_age =
	    math_rnd(const_float("float_player_peak_age_lower") +
		     (pl->pos == PLAYER_POS_GOALIE) * 
		     const_float("float_player_peak_age_goalie_addition"),
		     const_float("float_player_peak_age_upper") +
		     (pl->pos == PLAYER_POS_GOALIE) * 
		     const_float("float_player_peak_age_goalie_addition"));

	pl->peak_region = 
	    math_gauss_dist(const_float("float_player_peak_region_lower"),
			    const_float("float_player_peak_region_upper"));

	pl->talent = CLAMP(average_talent * skill_factor, 0, 
			   const_float("float_player_max_skill"));
	pl->skill = player_skill_from_talent(pl);
    }

    pl->cpos = pl->pos;
    player_estimate_talent(pl);

    pl->cskill = pl->skill;
    pl->fitness = math_rnd(const_float("float_player_fitness_lower"),
			   const_float("float_player_fitness_upper"));

    pl->health = pl->recovery = 0;
    pl->value = player_assign_value(pl);
    pl->wage = player_assign_wage(pl);
    pl->contract = math_rnd(const_float("float_player_contract_lower"),
			    const_float("float_player_contract_upper"));
    pl->lsu = math_rnd(const_float("float_player_lsu_lower"),
		       const_float("float_player_lsu_upper"));
    pl->cards = g_array_new(FALSE, FALSE, sizeof(PlayerCard));
    pl->games_goals = g_array_new(FALSE, FALSE, sizeof(PlayerGamesGoals));

    for(i=0;i<PLAYER_VALUE_END;i++)
	pl->career[i] = 0;

    pl->participation = FALSE;
    pl->offers = 0;
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
    
    if(player_number == 0 ||
       player_number == 11)
	position = PLAYER_POS_GOALIE;
    else if(player_number < bound[0] ||
	    (player_number > 10 &&
	     player_number < (11 + (const_int("int_team_cpu_players") - 11) *
			      const_float("float_player_pos_bound1"))))
	position = PLAYER_POS_DEFENDER;
    else if(player_number < bound[1] ||
	    (player_number > 10 &&
	     player_number < (11 + (const_int("int_team_cpu_players") - 11) *
			      const_float("float_player_pos_bound2"))))
	position = PLAYER_POS_MIDFIELDER;
    else
	position = PLAYER_POS_FORWARD;

    return position;
}

/** Calculate a player's skill depending on his age
    and talent. */
gfloat
player_skill_from_talent(const Player *pl)
{
    gfloat skill = pl->talent, cur_age = pl->peak_age;

    if(pl->age < pl->peak_age)
    {
	while(cur_age > pl->age)
	{
	    cur_age -= ((const_float("float_player_lsu_update_limit") + 2) * 0.0192);
	    if(pl->peak_age - cur_age > pl->peak_region)
		skill *= (1 - ((pl->peak_age - cur_age) * 
			       const_float("float_player_skill_update_younger_factor") +
			       const_float("float_player_skill_update_younger_add")));
	}
    }
    else
    {
	while(cur_age < pl->age)
	{
	    cur_age += ((const_float("float_player_lsu_update_limit") + 2) * 0.0192);
	    if(cur_age - pl->peak_age > pl->peak_region)
		skill *= (1 - ((cur_age - pl->peak_age) * 
			       const_float("float_player_skill_update_older_factor") +
			       const_float("float_player_skill_update_older_add")));
	}
    }

    if(skill > pl->talent)
	skill = pl->talent;
    
    return skill;
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
    gfloat value;

    value = powf((const_float("float_player_value_skill_weight") * pl->skill +
		  (1 - const_float("float_player_value_skill_weight")) * 
		  pl->talent * 0.7), const_float("float_player_value_power"));
    
    if(pl->peak_age > pl->age)
	value *= (1 + const_float("float_player_value_younger_factor") *
		  (pl->peak_age - pl->age));
    else
	value *= (1 - const_float("float_player_value_older_factor") *
		  (pl->age - pl->peak_age));

    return math_round_integer((gint)rint(value), 2);
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
    
    g_warning("player_id_index: didn't find player with id %d of team %s\n", player_id, tm->name);
    
    main_exit_program(EXIT_INT_NOT_FOUND, NULL);

    return -1;
}

/** Return a pointer to the number'th player of the team.
    @param tm The team.
    @param number The player number.
    @return A pointer to the player or NULL. */
Player*
player_of_idx_team(const Team *tm, gint number)
{
    if(tm->players->len <= number)
    {
	g_warning("player_of_idx_team: Player list of team %s too short for number %d.\n",
		  tm->name, number);

	main_exit_program(EXIT_POINTER_NOT_FOUND, NULL);

	return NULL;
    }

    return &g_array_index(tm->players, Player, number);
}


/** Return a pointer to the player with specified id of the team.
    @param tm The team.
    @param id The player's id.
    @return A pointer to the player or NULL. */
Player*
player_of_id_team(const Team *tm, gint id)
{
    gint i;
    
    for(i=0;i<tm->players->len;i++)
	if(g_array_index(tm->players, Player, i).id == id)
	    return &g_array_index(tm->players, Player, i);
    
    g_warning("player_of_id_team: didn't find player with id %d of team %s\n", id, tm->name);
    
    main_exit_program(EXIT_POINTER_NOT_FOUND, NULL);

    return NULL;
}

/** Find out whether the given player id is in the team. */
gboolean
query_player_id_in_team(gint player_id, const Team *tm)
{
    gint i;

    for(i=0;i<tm->players->len;i++)
	if(g_array_index(tm->players, Player, i).id == player_id)
	    return TRUE;

    return FALSE;
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
    gint data_int = ABS(GPOINTER_TO_INT(data));
    gint data_int2 = GPOINTER_TO_INT(data);
    gint type = data_int % 100;
    const Player *pl1 = (data_int < 100) ? 
	*(const Player**)a : (const Player*)a;
    const Player *pl2 = (data_int < 100) ? 
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
    }
    else if(type == PLAYER_COMPARE_ATTRIBUTE_LEAGUE_GOALS)
    {
	gint goals1 = player_games_goals_get(pl1, pl1->team->clid, PLAYER_VALUE_GOALS),
	    games1 = player_games_goals_get(pl1, pl1->team->clid, PLAYER_VALUE_GAMES),
	    shots1 = player_games_goals_get(pl1, pl1->team->clid, PLAYER_VALUE_SHOTS),
	    goals2 = player_games_goals_get(pl2, pl2->team->clid, PLAYER_VALUE_GOALS),
	    games2 = player_games_goals_get(pl2, pl2->team->clid, PLAYER_VALUE_GAMES),
	    shots2 = player_games_goals_get(pl2, pl2->team->clid, PLAYER_VALUE_SHOTS);

	if(goals1 != goals2)
	    return_value = misc_int_compare(goals1, goals2);
	else if(games1 != games2)
	    return_value = misc_int_compare(games2, games1);
	else
	    return_value = misc_int_compare(shots2, shots1);
    }
    
    if(data_int2 != 0)
	return_value *= (data_int / data_int2);

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
    gfloat skill_for_pos1 = player_get_cskill(pl1, position, FALSE) * 
	powf(pl1->fitness, const_float("float_player_fitness_exponent")),
	skill_for_pos2 = player_get_cskill(pl2, position, FALSE) * 
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

    return query_integer_is_in_array(new_structure, accepted_structures, 5);
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
    
    g_array_insert_val(tm->players, insert_at, new);

    if(insert_at < 11)
	player_of_idx_team(tm, insert_at)->cpos = 
	    player_get_position_from_structure(tm->structure, insert_at);
    else
	player_of_idx_team(tm, insert_at)->cpos = player_of_idx_team(tm, insert_at)->pos;

    player_of_idx_team(tm, insert_at)->cskill =
	player_get_cskill(player_of_idx_team(tm, insert_at), 
			  player_of_idx_team(tm, insert_at)->cpos, TRUE);
}

/** Move a player from one player array to another one.
    @param tm1 The source team.
    @param player_number The index of the player in the players array.
    @param tm2 The dest team.
    @param insert_at The new index of the player in the players array. */
void
player_move(Team *tm1, gint player_number, Team *tm2, gint insert_at)
{
    Player pl = *player_of_idx_team(tm1, player_number);

    pl.team = tm2;

    g_array_remove_index(tm1->players, player_number);

    g_array_insert_val(tm2->players, insert_at, pl);
}

/** Swap two players. */
void
player_swap(Team *tm1, gint player_number1, Team *tm2, gint player_number2)
{
    gint clid = -1;
    gint move = (tm1 == tm2 && player_number1 < player_number2) ? 
	-1 : 1;

    if(stat0 == STATUS_LIVE_GAME_PAUSE)
    {
	clid = usr(stat2).live_game.fix->clid;
	if((player_number1 < 11 && player_is_banned(player_of_idx_team(tm1, player_number1)) > 0 &&
	    player_of_idx_team(tm1, player_number1)->participation) ||
	   (player_number2 < 11 && player_is_banned(player_of_idx_team(tm2, player_number2)) > 0 &&
	    player_of_idx_team(tm2, player_number2)->participation))
	{
	    game_gui_show_warning(_("You can't replace a banned player."));
	    return;
	}
    }

    player_move(tm1, player_number1, tm2, player_number2);
    if(player_number2 < 11)
	player_of_idx_team(tm2, player_number2)->cpos = 
	    player_get_position_from_structure(tm2->structure, player_number2);
    else
	player_of_idx_team(tm2, player_number2)->cpos = player_of_idx_team(tm2, player_number2)->pos;

    player_of_idx_team(tm2, player_number2)->cskill =
	player_get_cskill(player_of_idx_team(tm2, player_number2), 
			  player_of_idx_team(tm2, player_number2)->cpos, TRUE);

    player_move(tm2, player_number2 + move,
		tm1, player_number1);    
    if(player_number1 < 11)
	player_of_idx_team(tm1, player_number1)->cpos = 
	    player_get_position_from_structure(tm1->structure, player_number1);
    else
	player_of_idx_team(tm1, player_number1)->cpos = player_of_idx_team(tm1, player_number1)->pos;

    player_of_idx_team(tm1, player_number1)->cskill =
	player_get_cskill(player_of_idx_team(tm1, player_number1), 
			  player_of_idx_team(tm1, player_number1)->cpos, TRUE);
}

/** Return the player's cskill depending on
    whether he's on his normal position or not.
    @param pl The player we examine.
    @param position The position we's like to put the player.
    @param check_health Whether to check for injury and ban (profiling reasons).
    @return A new cskill. */
gfloat
player_get_cskill(const Player *pl, gint position, gboolean check_health)
{
    gfloat cskill_factor;

    if(check_health &&
       (pl->health != PLAYER_INJURY_NONE ||
	player_is_banned(pl) > 0))
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
    gfloat goalie_factor = 
	1 - const_float("float_player_fitness_decrease_factor_goalie") *
	(pl->cpos == 0);
    gfloat boost_factor = 
	1 + (gfloat)pl->team->boost * 
	const_float("float_player_boost_fitness_effect");
    gfloat streak_factor = 1 + (gfloat)pl->streak * 
	const_float("float_player_streak_influence_fitness_decrease");

    if(pl->age < pl->peak_age - pl->peak_region)
	pl->fitness -= (((pl->peak_age - pl->peak_region - pl->age) *
			 const_float("float_player_fitness_decrease_younger_factor") +
			 const_float("float_player_fitness_decrease_add")) *
			goalie_factor * boost_factor * streak_factor);
    else if(pl->age > pl->peak_age + pl->peak_region)
	pl->fitness -= (((pl->age - pl->peak_age - pl->peak_region) *
			 const_float("float_player_fitness_decrease_older_factor") +
			 const_float("float_player_fitness_decrease_add")) *
			goalie_factor * boost_factor * streak_factor);
    else
	pl->fitness -= (const_float("float_player_fitness_decrease_add") *
			goalie_factor * boost_factor * streak_factor);

    pl->fitness = MAX(0, pl->fitness);
}


/** Increase of player fitness after a match.
    Depends on the decrease values and the
    percentage values that determine how much of the
    decrease a player gains.
    @param pl The player we edit. */
void
player_update_fitness(Player *pl)
{
    gfloat variance = 
	math_rnd(1 - const_float("float_player_fitness_increase_variance"),
		 1 + const_float("float_player_fitness_increase_variance"));
    gfloat streak_factor = 
	1 + (pl->streak * const_float("float_player_streak_influence_fitness_increase"));

    if(pl->participation)
    {
	pl->participation = FALSE;
	return;
    }

    if(pl->age < pl->peak_age - pl->peak_region)
	pl->fitness += (((pl->peak_age - pl->peak_region - pl->age) *
			 const_float("float_player_fitness_increase_younger_factor") +
			 const_float("float_player_fitness_increase_add")) *
			variance * streak_factor);
    else if(pl->age > pl->peak_age + pl->peak_region)
	pl->fitness += (((pl->age - pl->peak_age - pl->peak_region) *
			 const_float("float_player_fitness_increase_older_factor") +
			 const_float("float_player_fitness_increase_add")) *
			variance * streak_factor);
    else
	pl->fitness += (const_float("float_player_fitness_increase_add") * 
			variance * streak_factor);

    pl->fitness = MIN(pl->fitness, 1);
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
	    {
		g_warning("player_games_goals_get: unknown type %d.\n", type);
		main_exit_program(EXIT_INT_NOT_FOUND, NULL);
	    }
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
player_games_goals_set(Player *pl, gint clid, gint type, gint value)
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

	    *games_goals_value += value;

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

    player_games_goals_set(pl, clid, type, value);
}

/** Update skill and lsu of a user player.
    @param pl The player we update. */
void
player_update_skill(Player *pl)
{
    gint i;

    if(!query_player_is_youth(pl))
    {
	if(pl->age > pl->peak_age)
	    pl->lsu += (pl->health == 0) ? 1 : const_float("float_player_lsu_injured_old");
	else if(pl->health == 0 &&
		((gfloat)player_games_goals_get(pl, pl->team->clid, PLAYER_VALUE_GAMES) / (gfloat)week >=
		 const_float("float_player_lsu_games_percentage") || 
		 math_rnd(0, 1) <= const_float("float_player_lsu_increase_prob")))
	    pl->lsu++;
    }
    else
	pl->lsu += (pl->health != 0) ? 0 :
	    const_float("float_youth_academy_lsu_addition_best") -
	    (user_from_team(pl->team)->youth_academy.av_coach *
	     const_float("float_youth_academy_lsu_penalty"));

    if(pl->lsu < const_float("float_player_lsu_update_limit") ||
       math_rnd(0, 1) < powf(const_float("float_player_lsu_update_base_prob"),
			     pl->lsu - const_float("float_player_lsu_update_limit")))
	return;
	
    pl->lsu = 0;

    if(pl->age < pl->peak_age - pl->peak_region)
	pl->skill *= (1 + ((pl->peak_age - pl->age) * 
			   const_float("float_player_skill_update_younger_factor") +
			   const_float("float_player_skill_update_younger_add")));
    else if(pl->age > pl->peak_age + pl->peak_region)
	pl->skill *= (1 - ((pl->age - pl->peak_age) * 
			   const_float("float_player_skill_update_older_factor") +
			   const_float("float_player_skill_update_older_add")));
    else
	pl->skill *= math_rnd(1 - const_float("float_player_skill_update_peak"),
			      1 + const_float("float_player_skill_update_peak"));

    pl->skill = CLAMP(pl->skill, 0, pl->talent);
    pl->cskill = player_get_cskill(pl, pl->cpos, TRUE);
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

/** Weekly test whether a player goes on a hot/cold
    streak; if he's on a streak, decrease the streak counter. */
void
player_update_streak(Player *pl)
{
    gfloat streak_type, streak_prob, 
	streak_length, decrease_factor = 0;
    gfloat streak_prob_factor = 
	const_float("float_player_streak_prob_max") - 
	const_float("float_player_streak_prob_zero"),
	streak_prob_add = const_float("float_player_streak_prob_zero");

    /** Player streak is locked. */
    if(pl->streak_count < 0)
    {
	pl->streak_count++;
	
	if(pl->streak_count >= 0)
	    pl->streak_count = 0;

	return;
    }
    else if(pl->streak_count > 0)
    {
	if(pl->streak == PLAYER_STREAK_HOT)
	    decrease_factor = -1;
	else if(pl->streak == PLAYER_STREAK_COLD)
	    decrease_factor = 1;
	else
	    g_warning("player_update_streak: streak count is positive (%.1f) but player %s is not on a streak!\n", pl->streak_count, pl->name);

	pl->streak_count -= 
	    (pl->streak_prob * decrease_factor *
	     const_float("float_player_streak_count_decrease_factor") +
	     const_float("float_player_streak_count_decrease_add"));
    
	/** Streak is over. */
	if(pl->streak_count <= 0)
	{
	    pl->streak = PLAYER_STREAK_NONE;
	    player_streak_reset_count(pl);
	}
	
	return;
    }

    if(query_player_is_in_ya(pl))
	return;

    /** Now let's find out whether there's a new streak. */
    streak_type = math_rnd(-1, 1);
    streak_prob = math_rnd(0, 1);
    streak_length = math_rnd(const_float("float_player_streak_length_lower"),
			     const_float("float_player_streak_length_upper"));

    if(streak_type < pl->streak_prob &&
       ((pl->streak_prob > 0 &&  
	 streak_prob < streak_prob_factor * pl->streak_prob + streak_prob_add) ||
	(pl->streak_prob <= 0 && 
	 streak_prob < streak_prob_add * pl->streak_prob + streak_prob_add)))
    {
	pl->streak = PLAYER_STREAK_HOT;
	pl->streak_count = streak_length;
    }
    else if(streak_type > pl->streak_prob &&
	    ((pl->streak_prob > 0 &&  
	      streak_prob < -streak_prob_add * pl->streak_prob + streak_prob_add) ||
	     (pl->streak_prob <= 0 && 
	      streak_prob < -streak_prob_factor * pl->streak_prob + streak_prob_add)))
    {
	pl->streak = PLAYER_STREAK_COLD;
	pl->streak_count = streak_length;
    }
}

/** Update a player in a user team (age, skill etc.). */
void
player_update_weekly(Player *pl)
{
    pl->age += 0.0192;

    if(debug < 50)
	if(!sett_int("int_opt_disable_contracts") &&
	   pl->age > const_float("float_player_age_lower"))
	    pl->contract -= 0.0192;

    if(!sett_int("int_opt_disable_contracts") && debug < 50 &&
       pl->contract * 12 <= opt_user_int("int_opt_user_contract_limit") &&
       (pl->contract + 0.0192) * 12 > opt_user_int("int_opt_user_contract_limit"))
	user_event_add(user_from_team(pl->team), EVENT_TYPE_WARNING,
		       -1, -1, NULL, _("%s's contract expires in %.1f years."),
		       pl->name, pl->contract);

    if(pl->contract <= 0)
	player_remove_contract(pl);

    player_update_skill(pl);

    if(pl->health > 0)
	player_update_injury(pl);
    else
	player_update_streak(pl);
}

/** Remove a player from a user team after the contract expired.
    @param tm The user team.
    @param idx The player index. */
void
player_remove_contract(Player *pl)
{
    user_event_add(user_from_team(pl->team), EVENT_TYPE_PLAYER_LEFT, -1, -1, NULL,
		   pl->name);
    player_remove_from_team(pl->team, player_id_index(pl->team, pl->id));
}

/** Remove a player from a team.
    @param idx The player index in the players array. */
void
player_remove_from_team(Team *tm, gint idx)
{
    transfer_remove_player_ptr(player_of_idx_team(tm, idx));
    free_player(player_of_idx_team(tm, idx));
    g_array_remove_index(tm->players, idx);    
}

/** Make some player updates after a match
    for user players.
    @param pl The player we update.
    @param clid The fixture clid. */
void
player_update_post_match(Player *pl, const Fixture *fix)
{
    gint yellow_red = league_cup_get_yellow_red(fix->clid);
    gint winner = -1;

    if(player_card_get(pl, fix->clid, PLAYER_VALUE_CARD_RED) > 0)
	player_card_set(pl, fix->clid, PLAYER_VALUE_CARD_RED, -1, TRUE);

    if(player_card_get(pl, fix->clid, PLAYER_VALUE_CARD_YELLOW) >= yellow_red)
    {
	player_card_set(pl, fix->clid, PLAYER_VALUE_CARD_YELLOW, 0, FALSE);
	
	if(player_card_get(pl, fix->clid, PLAYER_VALUE_CARD_RED) == 0 && debug < 50)
	    player_card_set(pl, fix->clid, PLAYER_VALUE_CARD_RED, 1, FALSE);
    }

    if(pl->cpos == PLAYER_POS_GOALIE &&
       ((fix->result[0][0] == 0 && fix->teams[1] == pl->team) ||
	(fix->result[1][0] == 0 && fix->teams[0] == pl->team)))
	player_streak_add_to_prob(
	    pl, const_float("float_player_streak_add_goalie_clean"));

    /** No streak change if we had a draw. */
    if(!fix->decisive && fix->result[0][0] == fix->result[1][0])
       return;

    winner = GPOINTER_TO_INT(fixture_winner_of(fix, TRUE));

    if(winner == pl->team->id)
	player_streak_add_to_prob(
	    pl, const_float("float_player_streak_add_win"));
    else
	player_streak_add_to_prob(
	    pl, const_float("float_player_streak_add_loss"));
}

/** Replace a player by a new one in a cpu team. 
    @param free_player Whether to free the memory occupied
    by the player. */
void
player_replace_by_new(Player *pl, gboolean free_player)
{
    Team *tm = pl->team;
    gint idx = player_id_index(tm, pl->id);
    Player new = player_new(tm, team_get_average_skill(tm, FALSE), FALSE);
        
    new.name = name_get(pl->team->names_file);
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
    Player *pl = player_of_idx_team(tm, idx);

    if(pl->health == 0 && 
       (week_round == 1 ||
	(week_round > 1 && query_team_plays(tm, week, week_round))))
	player_update_fitness(pl);
    		
    pl->cskill = (pl->health > 0 || player_is_banned(pl) > 0) ?
	0 : player_get_cskill(pl, pl->cpos, TRUE);    
}

/** Return injury descriptions.  */
gchar*
player_injury_to_char(gint injury_type)
{      
    switch(injury_type)
    {
	default:
	    g_warning("player_injury_to_char: unknown type %d\n", injury_type);
	    return "";
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

/** Nullify some stuff at the beginning of the season and change the skills
    of cpu players.
    @param skill_change Skill change of the player in percentage. */
void
player_season_start(Player *pl, gfloat skill_change)
{
    gint i;

    if(pl->games_goals->len > 0)
    {
	g_array_free(pl->games_goals, TRUE);
	pl->games_goals = g_array_new(FALSE, FALSE, sizeof(PlayerGamesGoals));
    }

    if(pl->cards->len > 0)
    {
	g_array_free(pl->cards, TRUE);
	pl->cards = g_array_new(FALSE, FALSE, sizeof(PlayerGamesGoals));
    }    

    if(skill_change != 0)
    {
	pl->skill *= (1 + skill_change);

	pl->skill = CLAMP(pl->skill, 0, pl->talent);
	pl->cskill = player_get_cskill(pl, pl->cpos, TRUE);
	pl->value = player_assign_value(pl);

	for(i=0;i<QUALITY_END;i++)
	    if(pl->skill > pl->etal[i])
	    {
		player_estimate_talent(pl);
		break;
	    }
    }
}

/** Return the last name of a player. */
gchar*
player_get_last_name(const gchar *name)
{
    gint i;
    gchar buf[SMALL];
    gchar *rev_name = NULL;
    const gchar *temp;

    if(!g_utf8_validate(name, -1, NULL))
    {
	g_warning("player_get_last_name: invalid utf8-string: %s \n", name);
	return "";
    }

    temp = name;
    for(i=0;i<g_utf8_strlen(name, -1);i++)
    {
	temp = g_utf8_next_char(temp);
	if(g_unichar_isspace(g_utf8_get_char(temp)))
	   break;
    }
    
    rev_name = g_utf8_strreverse(name, -1);

    g_utf8_strncpy(buf, rev_name, g_utf8_strlen(name, -1) - i - 2);
    g_free(rev_name);

    rev_name = g_utf8_strreverse(buf, -1);

    return rev_name;
}

/** Create a pointer array with pointers to the players
    and return it. */
GPtrArray*
player_get_pointers_from_array(const GArray *players_array)
{
    gint i;
    GPtrArray *players = g_ptr_array_new();

    for(i=0;i<players_array->len;i++)
	g_ptr_array_add(players, &g_array_index(players_array, Player, i));

    return players;
}


/** Move a player from the team to the youth academy. */
void
player_move_to_ya(gint idx)
{
    Player *pl = player_of_idx_team(current_user.tm, idx);
    Player player = *pl;

    if(query_transfer_player_is_on_list(pl))
	transfer_remove_player_ptr(pl);

    player.cskill = player.skill;
    player.cpos = player.pos;

    g_array_remove_index(current_user.tm->players, idx);
    g_array_append_val(current_user.youth_academy.players, player);
}

/** Move a player from the youth academy to the team. */
void
player_move_from_ya(gint idx)
{
    Player *pl = &g_array_index(current_user.youth_academy.players, Player, idx);
    Player player = *pl;

    g_array_remove_index(current_user.youth_academy.players, idx);
    g_array_append_val(current_user.tm->players, player);
}

/** Wrapper for the streak probability addition operation
    so that it always stays between -1 and 1. */
void
player_streak_add_to_prob(Player *pl, gfloat add)
{
    /** No streaks for CPU players (yet). */
    if(team_is_user(pl->team) == -1)
	return;

    pl->streak_prob += add;
    pl->streak_prob = CLAMP(pl->streak_prob, -1, 1);
}

/** Find out whether a player is in the youth academy. */
gboolean
query_player_is_in_ya(const Player *pl)
{
    gint i;

    for(i=0;i<pl->team->players->len;i++)
	if(pl == &g_array_index(pl->team->players, Player, i))
	    return FALSE;

    return TRUE;
}
