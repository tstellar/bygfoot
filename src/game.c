#include "cup.h"
#include "finance.h"
#include "fixture.h"
#include "game.h"
#include "game_gui.h"
#include "league.h"
#include "live_game.h"
#include "maths.h"
#include "misc.h"
#include "option.h"
#include "player.h"
#include "table.h"
#include "team.h"
#include "treeview.h"
#include "user.h"
#include "variables.h"

/** Calculate attacking, defending and goalie values for the two teams
    of a fixture.
    @param fix The fixture we calculate.
    @param team_value The place to store the values.
    @param home_advantage The home advantage factor.
    @see #GameTeamValue
*/
void
game_get_values(const Fixture *fix, gfloat team_values[][GAME_TEAM_VALUE_END],
		gfloat home_advantage)
{
    gint i, j;
    Team *tm[2] = {fix->teams[0], fix->teams[1]};
    gfloat style_factor;

    /*d*/
/*     printf("\nhome %.2f\n", home_advantage); */
    for(i=0;i<2;i++)
    {
	for(j=0;j<GAME_TEAM_VALUE_END;j++)
	    team_values[i][j] = 0;
	
	style_factor = (gfloat)tm[i]->style * const_float("float_game_style_factor");

	team_values[i][GAME_TEAM_VALUE_GOALIE] = 
	    game_get_player_contribution(player_of(tm[i], 0), FALSE) * 
	    (1 + home_advantage * (i == 0)) *
	    (1 + const_float("float_player_boost_skill_effect") * tm[i]->boost);

	for(j=1;j<11;j++)
	{
	    team_values[i][GAME_TEAM_VALUE_ATTACK] +=
		game_get_player_contribution(player_of(tm[i], j), GAME_TEAM_VALUE_ATTACK);
	    team_values[i][GAME_TEAM_VALUE_MIDFIELD] +=
		game_get_player_contribution(player_of(tm[i], j), GAME_TEAM_VALUE_MIDFIELD);
	    team_values[i][GAME_TEAM_VALUE_DEFEND] +=
		game_get_player_contribution(player_of(tm[i], j), GAME_TEAM_VALUE_DEFEND);
	}

	for(j=GAME_TEAM_VALUE_DEFEND;j<GAME_TEAM_VALUE_DEFEND + 3;j++)
	    team_values[i][j] *= 
		((1 + style_factor) * (1 + home_advantage * (i == 0)) *
		 (1 + const_float("float_player_boost_skill_effect") * tm[i]->boost));

/* 	if(fixture_user_team_involved(fix) != -1) */
/* 	{ */
/* 	    printf("week %d %d\n", week, week_round); */
/* 	    printf("%s attack %.1f midf %.1f defend %.1f style %d struct %d\n", */
/* 		   tm[i]->name->str, */
/* 		   team_values[i][GAME_TEAM_VALUE_ATTACK], */
/* 		   team_values[i][GAME_TEAM_VALUE_MIDFIELD], */
/* 		   team_values[i][GAME_TEAM_VALUE_DEFEND], */
/* 		   tm[i]->style, tm[i]->structure); */
/* 	} */
    }
}

/** Return the contribution of a player to the attack, midfield or defend.
    @param pl The player.
    @param type Whether we have defend, midfield or attack value.
    @return The player's contribution depending on position and
    fitness. */
gfloat
game_get_player_contribution(const Player *pl, gint type)
{
/** How the cskill of field players get weighted for the team values in
    a match. Rows are player position, columns value type. 
    @see game_get_player_contribution() */
    gfloat player_weights[3][3] =
	{{const_float("float_player_team_weight_defender_defense"), 
	  const_float("float_player_team_weight_defender_midfield"),
	  const_float("float_player_team_weight_defender_attack")},
	 {const_float("float_player_team_weight_midfielder_defense"), 
	  const_float("float_player_team_weight_midfielder_midfield"), 
	  const_float("float_player_team_weight_midfielder_attack")},
	 {const_float("float_player_team_weight_forward_defense"), 
	  const_float("float_player_team_weight_forward_midfield"), 
	  const_float("float_player_team_weight_forward_attack")}};

    return player_get_game_skill(pl, FALSE) *
	player_weights[pl->cpos - 1][type - GAME_TEAM_VALUE_DEFEND];
}

/** Return a random attacking or defending player
    or the player that has to shoot the number_of_penalty'th penalty.
    @param tm The team we examine.
    @param player_type Whether we concentrate on attacking or defending players
    or would like to have a penalty shooting player.
    @param last_penalty The player that shot the last penalty.
    @param not_this_one A player to exclude.
    @param skills Whether to weight with skills, too.
    @return A player index. */
gint
game_get_player(const Team *tm, gint player_type, 
		gint last_penalty, gint not_this_one,
		gboolean skills)
{
    gint i, player = not_this_one;
    gfloat weights[3];
    gfloat probs[10];
    gfloat rndom;

    /*todo move to constants file?*/
    if(player_type == GAME_PLAYER_TYPE_ATTACK)
    {
	weights[0] = 0.25;
	weights[1] = 0.5;
	weights[2] = 1;
    }
    else if(player_type == GAME_PLAYER_TYPE_MIDFIELD)
    {
	weights[0] = 0.5;
	weights[1] = 1;
	weights[2] = 0.5;
    }
    else if(player_type == GAME_PLAYER_TYPE_DEFEND)
    {
	weights[0] = 1;
	weights[1] = 0.5;
	weights[2] = 0.25;
    }
    else if(player_type == GAME_PLAYER_TYPE_PENALTY)
	return game_get_penalty_taker(tm, last_penalty);
    else
	g_warning("game_get_player: unknown player type %d\n", player_type);

    game_get_player_probs(tm->players, probs, weights, skills);

    if(probs[9] > 0)
    {
	while(player == not_this_one)
	{
	    rndom = math_rnd(0, probs[9]);
	    
	    if(rndom < probs[0])
		player = player_of(tm, 1)->id;
	    else
		for(i=1;i<10;i++)
		    if(rndom < probs[i] && rndom > probs[i - 1])
			player = player_of(tm, i + 1)->id;
	}
    }
    else
    {
	g_warning("game_get_player: All players injured or banned, apparently.\n");
	printf("player list:\n");
	for(i=0;i<tm->players->len;i++)
	{
	    if(i < 10)
		printf("prob %.3f  ", probs[i]);
	    printf("%d %20s health %d cskill %.2f\n", i, player_of(tm, i)->name->str,
		   player_of(tm, i)->health, player_of(tm, i)->cskill);
	}
	return -1;
    }

    return player;
}

/** Write the probabilities for field players being picked
    (e.g. a random defender, or a player who gets injured) into
    the float array. Depends on skill mostly.
    @param players Player array.
    @param probs Array with probabilities that gets filled.
    @param weights How to weight the players depending on their position. 
    @param skills Whether to weight with skills, too. */
void
game_get_player_probs(GArray *players, gfloat *probs, gfloat *weights, gboolean skills)
{
    gint i;

    probs[0] = (skills) ? player_get_game_skill(&g_array_index(players, Player, 1), FALSE) *
	weights[g_array_index(players, Player, 1).pos - 1] :
	weights[g_array_index(players, Player, 1).pos - 1] *
	(g_array_index(players, Player, 1).cskill != 0);
    for(i=1;i<10;i++)
	probs[i] = probs[i - 1] + 
	    ((skills) ? player_get_game_skill(&g_array_index(players, Player, i + 1), FALSE) *
	     weights[g_array_index(players, Player, i + 1).pos - 1] :
	     weights[g_array_index(players, Player, i + 1).pos - 1] *
	     (g_array_index(players, Player, i + 1).cskill != 0));
}

/** Return the player who's shooting the following penalty
    (at penalty shoot-out).
    @param tm The team that has the penalty.
    @param last_penalty The player that shot the last penalty.
    @return A player id. */
gint
game_get_penalty_taker(const Team *tm, gint last_penalty)
{
    gint i, return_value = -1;
    GPtrArray *players = g_ptr_array_new();

    for(i=0;i<11;i++)
	if(player_of(tm, i)->cskill != 0)
	    g_ptr_array_add(players, player_of(tm, i));

    g_ptr_array_sort_with_data(players, (GCompareDataFunc)player_compare_func, 
			       GINT_TO_POINTER(PLAYER_COMPARE_ATTRIBUTE_GAME_SKILL));

    if(last_penalty == -1 ||
       last_penalty == ((Player*)g_ptr_array_index(players, players->len - 1))->id)
	return_value = ((Player*)g_ptr_array_index(players, 0))->id;

    for(i=0;i<players->len - 1;i++)
	if(last_penalty == ((Player*)g_ptr_array_index(players, i))->id)
	{
	    return_value = ((Player*)g_ptr_array_index(players, i + 1))->id;
	    break;
	}

    g_ptr_array_free(players, TRUE);

    return return_value;
}

/** Do some things that have to be done at the beginning of a
    game, e.g. calculate attendance etc.
    @param fix The fixture we examine. */
void
game_initialize(Fixture *fix)
{
    gint i, j;
    gfloat journey_factor =
	(fix->clid < ID_CUP_START ||
	 (fix->clid >= ID_CUP_START && 
	  cup_from_clid(fix->clid)->type == CUP_TYPE_NATIONAL)) ?
	const_float("float_game_finance_journey_factor_national") :
	const_float("float_game_finance_journey_factor_international");    
    gint user_idx[2] = {team_is_user(fix->teams[0]), team_is_user(fix->teams[1])};
    gint ticket_income = 0;

    game_assign_attendance(fix);
    ticket_income = 
	fix->attendance * const_int("int_team_stadium_ticket_price");

    if(user_idx[0] != -1 && fix->home_advantage)
    {
	usr(user_idx[0]).money += ticket_income;
	usr(user_idx[0]).money_in[1][MON_IN_TICKET] += ticket_income;

	fix->teams[0]->stadium.safety -= 
	    math_rnd(const_float("float_game_stadium_safety_deterioration_lower"),
		     const_float("float_game_stadium_safety_deterioration_upper"));
	fix->teams[0]->stadium.safety = CLAMP(fix->teams[0]->stadium.safety, 0, 1);
    }

    for(i=0;i<2;i++)
    {
	for(j=0;j<fix->teams[i]->players->len;j++)
	{
	    if(j < 11)
	    {
		if(player_of(fix->teams[i], j)->cskill > 0)
		    player_games_goals_set(player_of(fix->teams[i], j), fix->clid,
					   PLAYER_VALUE_GAMES, 1, TRUE);
	    }
		
	    player_of(fix->teams[i], j)->participation = 
		(j < 11 && player_of(fix->teams[i], j)->cskill > 0);
	}

	if(user_idx[i] != -1)
	{
	    if(i == 1 || !fix->home_advantage)
		usr(user_idx[i]).money_out[1][MON_OUT_JOURNEY] -= 
		    (gint)(finance_wage_unit(fix->teams[i]) * journey_factor);
	    if(!fix->home_advantage)
	    {
		usr(user_idx[i]).money_in[1][MON_IN_TICKET] += (gint)rint((gfloat)ticket_income / 2);
		usr(user_idx[i]).money += (gint)rint((gfloat)ticket_income / 2);
	    }
	}
    }
}

/** Find out how many spectators there were,
    depending on safety of the stadium etc.
    @param fix The match we examine. */
void
game_assign_attendance(Fixture *fix)
{
    Team *tm[2] = {fix->teams[0], fix->teams[1]};
    gfloat factor = 
	math_rnd(const_float("float_game_stadium_attendance_percentage_lower"),
		 const_float("float_game_stadium_attendance_percentage_upper")) *
	powf(tm[0]->stadium.safety, 
	     const_float("float_game_stadium_attendance_safety_exponent"));
    gint max_att = MIN((gint)rint((gfloat)league_cup_average_capacity(fix->clid) *
				  const_float("float_game_stadium_attendance_average_exceed_factor")),
		       tm[0]->stadium.capacity);

    if(fix->clid < ID_CUP_START && 
       team_rank(tm[1], fix->clid) < 
       (gint)rint((gfloat)league_from_clid(fix->clid)->teams->len *
		  const_float("float_game_stadium_attendance_rank_percentage")))
	factor *= const_float("float_game_stadium_attendance_rank_factor");

    if(fix->clid >= ID_CUP_START)
    {
	if(cup_from_clid(fix->clid)->rounds->len - fix->round <=
	   const_int("int_game_stadium_attendance_cup_rounds_full_house"))
	    factor = 1;
	else if(cup_from_clid(fix->clid)->type == CUP_TYPE_NATIONAL)
	    factor *= const_float("float_game_stadium_attendance_cup_national_factor");
	else
	    factor *= const_float("float_game_stadium_attendance_cup_international_factor");
    }

    fix->attendance = MIN((gint)rint((gfloat)tm[0]->stadium.capacity * factor), max_att);

    tm[0]->stadium.average_attendance = 
	(gint)rint((gfloat)(tm[0]->stadium.average_attendance * tm[0]->stadium.games + fix->attendance) /
		   (gfloat)(tm[0]->stadium.games + 1));
    tm[0]->stadium.possible_attendance += tm[0]->stadium.capacity;
    tm[0]->stadium.games++;
}

/** Save the team states in the current live game
    when a pause occurs. */
void
game_save_team_states(void)
{
    gint i, j;
    const Team *teams[2] = {usr(stat2).live_game.fix->teams[0], 
			    usr(stat2).live_game.fix->teams[1]};

    for(i=0;i<2;i++)
    {
	usr(stat2).live_game.team_state[i].structure = 
	    teams[i]->structure;
	usr(stat2).live_game.team_state[i].style = 
	    teams[i]->style;
	usr(stat2).live_game.team_state[i].boost = 
	    teams[i]->boost;
	
	for(j=0;j<11;j++)
	    usr(stat2).live_game.team_state[i].player_ids[j] = 
		player_of(teams[i], j)->id;
    }
}

/** Check whether the number of substitutions
    during a live game pause isn't too high.
    @return TRUE if the subs were ok, FALSE else. */
gboolean
game_check_live_game_resume_state(void)
{
    gint i, j;
    gint subs[2] = {0, 0};
    const Team *teams[2] = {usr(stat2).live_game.fix->teams[0], 
			    usr(stat2).live_game.fix->teams[1]};
    gboolean return_value = TRUE;

    for(i=0;i<2;i++)
    {
	for(j=0;j<11;j++)
	    if(!query_integer_is_in_array(player_of(teams[i], j)->id,
					  usr(stat2).live_game.team_state[i].player_ids,
					  0, 11))
		subs[i]++;
	
	if(subs[i] > usr(stat2).live_game.subs_left[i])
	    return_value = FALSE;
    }

    return return_value;
}

/** Find pairs of substituted players after a live game pause. */
void
game_get_subs(gint team_number, gint *subs_in, gint *subs_out)
{
    gint i, cnt = 0;
    const Team *tm = usr(stat2).live_game.fix->teams[team_number];
    gint current_players[11];
    
    for(i=0;i<3;i++)
	subs_in[i] = subs_out[i] = -1;
    
    for(i=0;i<11;i++)
    {
	current_players[i] = player_of(tm, i)->id;
	if(!query_integer_is_in_array(player_of(tm, i)->id,
				      usr(stat2).live_game.team_state[team_number].player_ids,
				      0, 11))
	{
	    subs_in[cnt] = player_of(tm, i)->id;
	    cnt++;
	}
    }

    cnt = 0;
    for(i=0;i<11;i++)
	if(!query_integer_is_in_array(usr(stat2).live_game.team_state[team_number].player_ids[i],
				      current_players, 0, 11))
	{
	    subs_out[cnt] = usr(stat2).live_game.team_state[team_number].player_ids[i];
	    cnt++;
	}
}

/** Choose an injury for a player and adjust health values.
    @param pl The player that gets injured. */
void
game_player_injury(Player *pl)
{
    gint i;
    gfloat rndom;
    /* probabilities of different injuries */
    gfloat injury_probs[13]={0,
			     const_float("float_player_injury_concussion"),
			     const_float("float_player_injury_pulled_muscle"),
			     const_float("float_player_injury_hamstring"),
			     const_float("float_player_injury_groin"),
			     const_float("float_player_injury_frac_ankle"),
			     const_float("float_player_injury_rib"),
			     const_float("float_player_injury_leg"),
			     const_float("float_player_injury_brok_ankle"),
			     const_float("float_player_injury_arm"),
			     const_float("float_player_injury_shoulder"),
			     const_float("float_player_injury_ligament"),
			     const_float("float_player_injury_career_stop")};
    
    gint duration[12]={
	math_gauss_disti(const_int("int_player_injury_duration_concussion") - 
			 const_int("int_player_injury_duration_dev_concussion"),
			 const_int("int_player_injury_duration_concussion") + 
			 const_int("int_player_injury_duration_dev_concussion")),
	math_gauss_disti(const_int("int_player_injury_duration_pulled_muscle") -
			 const_int("int_player_injury_duration_dev_pulled_muscle"),
			 const_int("int_player_injury_duration_pulled_muscle") +
			 const_int("int_player_injury_duration_dev_pulled_muscle")),
	math_gauss_disti(const_int("int_player_injury_duration_hamstring") -
			 const_int("int_player_injury_duration_dev_hamstring"),
			 const_int("int_player_injury_duration_hamstring") +
			 const_int("int_player_injury_duration_dev_hamstring")),
	math_gauss_disti(const_int("int_player_injury_duration_groin") -
			 const_int("int_player_injury_duration_dev_groin"),
			 const_int("int_player_injury_duration_groin") +
			 const_int("int_player_injury_duration_dev_groin")),
	math_gauss_disti(const_int("int_player_injury_duration_frac_ankle") -
			 const_int("int_player_injury_duration_dev_frac_ankle"),
			 const_int("int_player_injury_duration_frac_ankle") +
			 const_int("int_player_injury_duration_dev_frac_ankle")),
	math_gauss_disti(const_int("int_player_injury_duration_rib") -
			 const_int("int_player_injury_duration_dev_rib"),
			 const_int("int_player_injury_duration_rib") +
			 const_int("int_player_injury_duration_dev_rib")),
	math_gauss_disti(const_int("int_player_injury_duration_leg") -
			 const_int("int_player_injury_duration_dev_leg"),
			 const_int("int_player_injury_duration_leg") +
			 const_int("int_player_injury_duration_dev_leg")),
	math_gauss_disti(const_int("int_player_injury_duration_brok_ankle") -
			 const_int("int_player_injury_duration_dev_brok_ankle"),
			 const_int("int_player_injury_duration_brok_ankle") +
			 const_int("int_player_injury_duration_dev_brok_ankle")),
	math_gauss_disti(const_int("int_player_injury_duration_arm") - 
			 const_int("int_player_injury_duration_dev_arm"),
			 const_int("int_player_injury_duration_arm") +
			 const_int("int_player_injury_duration_dev_arm")),
	math_gauss_disti(const_int("int_player_injury_duration_shoulder") -
			 const_int("int_player_injury_duration_dev_shoulder"),
			 const_int("int_player_injury_duration_shoulder") +
			 const_int("int_player_injury_duration_dev_shoulder")),
	math_gauss_disti(const_int("int_player_injury_duration_ligament") -
			 const_int("int_player_injury_duration_dev_ligament"),
			 const_int("int_player_injury_duration_ligament") +
			 const_int("int_player_injury_duration_dev_ligament"))};

    for(i=1;i<13;i++)
	injury_probs[i] += injury_probs[i - 1];

    rndom = math_rnd(0, 1);

    for(i=1;i<13;i++)
	if(rndom >= injury_probs[i - 1] && rndom < injury_probs[i])
	{	    
	    pl->health = i;
	    pl->recovery = duration[i - 1] + 1;
	    pl->cskill = pl->fitness = 0;	     
	}
}

/** Return a factor influencing who's fouled whom
    depending on the states of the team boosts.
    @param boost1 Boost of the team in possession.
    @param boost2 Boost of the team not in possession.
    @return A factor.
*/
gfloat
game_get_foul_possession_factor(gint boost1, gint boost2)
{
    if(boost1 == boost2)
	return 1;

    if(abs(boost1 - boost2) == 1)
	return 1 + const_float("float_team_boost_foul_by_possession_factor1") *
	    (1 - 2 * (boost1 < boost2));

    return 1 + const_float("float_team_boost_foul_by_possession_factor2") *
	(1 - 2 * (boost1 < boost2));
}

/** Substitute a player during a match.
    @param tm The team we work on.
    @param player_number The index of the player. */
gint
game_substitute_player(Team *tm, gint player_number)
{
    gint i;
    GPtrArray *substitutes = g_ptr_array_new();
    gboolean adapt_structure;
    gint substitute = -1;

    for(i=11;i<tm->players->len;i++)
	g_ptr_array_add(substitutes, player_of(tm, i));

    g_ptr_array_sort_with_data(substitutes, (GCompareDataFunc)player_compare_substitute_func,
			       GINT_TO_POINTER(player_of(tm, player_number)->cpos));
    adapt_structure = 
	(math_get_place(team_find_appropriate_structure(tm), 1) + 
	 math_get_place(team_find_appropriate_structure(tm), 2) + 
	 math_get_place(team_find_appropriate_structure(tm), 3) != 10 ||
	 (player_of(tm, player_number)->cpos != ((Player*)g_ptr_array_index(substitutes, 0))->pos &&
	  player_substitution_good_structure(tm->structure,
					     player_of(tm, player_number)->cpos,
					     ((Player*)g_ptr_array_index(substitutes, 0))->pos)));

    substitute = ((Player*)g_ptr_array_index(substitutes, 0))->id;

    player_swap(tm, player_number,
		tm, player_id_index(tm, ((Player*)g_ptr_array_index(substitutes, 0))->id));

    g_ptr_array_free(substitutes, TRUE);

    if(adapt_structure)
    {
	team_change_structure(tm, team_find_appropriate_structure(tm));
	team_rearrange(tm);
    }

    if(team_is_user(tm) == cur_user)
    {
	game_gui_write_av_skills();
	
	selected_row[0] = -1;	
	treeview_show_user_player_list(&current_user);
    }

    return substitute;
}

/** Find out whether we substitute a player to balance
    a team after a red card.
    @param tm The team.
    @return A player index or -1 if we don't substitute.
*/
gint
game_find_to_substitute(const Team *tm)
{
    gint i;
    gint position_to_substitute = -1;    
    GPtrArray *players = g_ptr_array_new();
    gint return_value = -1;
    gint current_structure = team_find_appropriate_structure(tm);
    gint num_forw = current_structure % 10,
	num_mid = math_get_place(current_structure, 2),
	num_def = math_get_place(current_structure, 3);

    for(i=0;i<11;i++)
	if(player_is_banned(player_of(tm, i)) <= 0)
	    g_ptr_array_add(players, player_of(tm, i));

    g_ptr_array_sort_with_data(players, (GCompareDataFunc)player_compare_func,
			       GINT_TO_POINTER(PLAYER_COMPARE_ATTRIBUTE_GAME_SKILL));

    if(num_forw > 1 || MAX(num_mid, num_def) <= 2)
	position_to_substitute = PLAYER_POS_FORWARD;    
    else if(ABS(num_def - num_mid) > 1 ||
	    (num_forw == 0 && MAX(num_mid, num_def) > 2))
	position_to_substitute = (num_def > num_mid) ? PLAYER_POS_DEFENDER : PLAYER_POS_MIDFIELDER;
    else
	return -1;

    for(i=players->len - 1; i >= 0; i--)
	if(((Player*)g_ptr_array_index(players, i))->pos == position_to_substitute)
	{
	    return_value = ((Player*)g_ptr_array_index(players, i))->id;
	    g_ptr_array_free(players, TRUE);
	    return return_value;
	}

    g_ptr_array_free(players, TRUE);
    return -1;
}

/** Find out how long a player is banned. */
gint
game_player_get_ban_duration(void)
{
    gint i;
    gfloat rndom;
    gfloat duration_probs[6] =
	{0,
	 const_float("float_live_game_ban_1"),
	 const_float("float_live_game_ban_2"),
	 const_float("float_live_game_ban_3"),
	 const_float("float_live_game_ban_4"),
	 const_float("float_live_game_ban_5")};

    for(i=1;i<6;i++)
	duration_probs[i] += duration_probs[i - 1];

    rndom = math_rnd(0, 1);
    
    for(i=1;i<6;i++)
	if(duration_probs[i - 1] < rndom && rndom < duration_probs[i])
	    return i + 1;

    return -1;
}

/** Find out whether we make a sub after a send-off.
    @param tm The team.
    @param player The player index.
    @param to_substitute The return location for the index of
    the player to substitute.
    @param substitute The return location for the player who comes into the game. */
void
game_substitute_player_send_off(Team *tm, gint player_number, 
				gint *to_substitute, gint *substitute)
{
    gint i;
    gint position = -1;
    GPtrArray *substitutes = NULL;
    gint current_structure = team_find_appropriate_structure(tm);
    gint num_forw = current_structure % 10,
	num_mid = math_get_place(current_structure, 2),
	num_def = math_get_place(current_structure, 3);

    *to_substitute = game_find_to_substitute(tm);

    if(*to_substitute == -1)
	return;

    substitutes = g_ptr_array_new();
    for(i=11;i<tm->players->len;i++)
	g_ptr_array_add(substitutes, player_of(tm, i));

    if(num_forw == 0 && MAX(num_def, num_mid) > 2)
	position = PLAYER_POS_FORWARD;
    else
	position = (num_def > num_mid) ? PLAYER_POS_MIDFIELDER : PLAYER_POS_DEFENDER;

    g_ptr_array_sort_with_data(substitutes, (GCompareDataFunc)player_compare_substitute_func,
			       GINT_TO_POINTER(position));
    
    *substitute = ((Player*)g_ptr_array_index(substitutes, 0))->id;
    player_swap(tm, player_id_index(tm, *to_substitute),
		tm, player_id_index(tm, *substitute));

    g_ptr_array_free(substitutes, TRUE);

    team_change_structure(tm, team_find_appropriate_structure(tm));
    team_rearrange(tm);

    if(team_is_user(tm) == cur_user)
    {
	game_gui_write_av_skills();
	
	selected_row[0] = -1;	
	treeview_show_user_player_list(&current_user);
    }
}

/** Decrease the players' fitness during a live game.
    @param fix The match being played. */
void
game_decrease_fitness(const Fixture *fix)
{
    gint i, j;

    for(i=0;i<2;i++)
    {
	for(j=0;j<11;j++)
	    if(player_of(fix->teams[i], j)->cskill > 0)
		player_decrease_fitness(player_of(fix->teams[i], j));
    }
}

/** Assemble some stats like ball possession or shots
    on goal.
    @param live_game The pointer to the match.
    @see #LiveGameStats */
void
game_create_stats(gpointer live_game)
{
    LiveGame *match = (LiveGame*)live_game;
    gint i, possession[2] = {0, 0}, reg_goals[2] = {0, 0};
    LiveGameStats *stat = &match->stats;

    game_create_stats_players(live_game);

    for(i=0;i<LIVE_GAME_STAT_VALUE_END;i++)
	stat->values[0][i] =
	    stat->values[1][i] = 0;
    
    for(i=0;i<2;i++)
	stat->values[i][LIVE_GAME_STAT_VALUE_GOALS_REGULAR] = 
	    math_sum_int_array(match->fix->result[i], 2);

    for(i=0;i<match->units->len;i++)
    {
	if(g_array_index(match->units, LiveGameUnit, i).event.type == 
	   LIVE_GAME_EVENT_PENALTIES)
	    break;

	if(g_array_index(match->units, LiveGameUnit, i).minute != -1)
	    possession[g_array_index(match->units, LiveGameUnit, i).possession]++;

	if(g_array_index(match->units, LiveGameUnit, i).event.type == LIVE_GAME_EVENT_SCORING_CHANCE ||
	   g_array_index(match->units, LiveGameUnit, i).event.type == LIVE_GAME_EVENT_FREE_KICK)
	    stat->values[g_array_index(match->units, LiveGameUnit, i).possession][LIVE_GAME_STAT_VALUE_SHOTS]++;
	else if(g_array_index(match->units, LiveGameUnit, i).event.type == LIVE_GAME_EVENT_PENALTY)
	    stat->values[g_array_index(match->units, LiveGameUnit, i).possession]
		[LIVE_GAME_STAT_VALUE_PENALTIES]++;
	else if(g_array_index(match->units, LiveGameUnit, i).event.type == LIVE_GAME_EVENT_INJURY)
	    stat->values[g_array_index(match->units, LiveGameUnit, i).event.values[LIVE_GAME_EVENT_VALUE_TEAM]]
		[LIVE_GAME_STAT_VALUE_INJURIES]++;
	else if(g_array_index(match->units, LiveGameUnit, i).event.type == LIVE_GAME_EVENT_FOUL ||
		g_array_index(match->units, LiveGameUnit, i).event.type == LIVE_GAME_EVENT_FOUL_YELLOW ||
		g_array_index(match->units, LiveGameUnit, i).event.type == LIVE_GAME_EVENT_FOUL_RED ||
		g_array_index(match->units, LiveGameUnit, i).event.type == LIVE_GAME_EVENT_FOUL_RED_INJURY)
	{
	    stat->values[g_array_index(match->units, LiveGameUnit, i).event.values[LIVE_GAME_EVENT_VALUE_TEAM]]
		[LIVE_GAME_STAT_VALUE_FOULS]++;
	    if(g_array_index(match->units, LiveGameUnit, i).event.type == LIVE_GAME_EVENT_FOUL_YELLOW)
		stat->values[g_array_index(match->units, LiveGameUnit, i).
			     event.values[LIVE_GAME_EVENT_VALUE_TEAM]][LIVE_GAME_STAT_VALUE_CARDS]++;
	}
	else if(g_array_index(match->units, LiveGameUnit, i).event.type == LIVE_GAME_EVENT_SEND_OFF)
	    stat->values[g_array_index(match->units, LiveGameUnit, i).event.values[LIVE_GAME_EVENT_VALUE_TEAM]]
		[LIVE_GAME_STAT_VALUE_REDS]++;
	else if(g_array_index(match->units, LiveGameUnit, i).event.type == LIVE_GAME_EVENT_GOAL &&
		g_array_index(match->units, LiveGameUnit, i - 1).event.type != LIVE_GAME_EVENT_PENALTY)
	    reg_goals[g_array_index(match->units, LiveGameUnit, i).
		      event.values[LIVE_GAME_EVENT_VALUE_TEAM]]++;
    }

    for(i=0;i<2;i++)
    {
	stat->values[i][LIVE_GAME_STAT_VALUE_POSSESSION] = 
	    (gint)rint((gfloat)possession[i] / (gfloat)(possession[0] + possession[1]) * 100);
	stat->values[i][LIVE_GAME_STAT_VALUE_SHOT_PERCENTAGE] = 
	    (stat->values[i][LIVE_GAME_STAT_VALUE_SHOTS] > 0) ?
	    (gint)rint(((gfloat)reg_goals[i] / (gfloat)stat->values[i][LIVE_GAME_STAT_VALUE_SHOTS])
		       * 100) : 0;
    }

/*d*/
    printf("goals reg\t %d \t %d\n", stat->values[0][LIVE_GAME_STAT_VALUE_GOALS_REGULAR],
	   stat->values[1][LIVE_GAME_STAT_VALUE_GOALS_REGULAR]);
    printf("shots\t %d \t %d\n", stat->values[0][LIVE_GAME_STAT_VALUE_SHOTS],
	   stat->values[1][LIVE_GAME_STAT_VALUE_SHOTS]);
    printf("shotperc\t %d%% \t %d%%\n", stat->values[0][LIVE_GAME_STAT_VALUE_SHOT_PERCENTAGE],
	   stat->values[1][LIVE_GAME_STAT_VALUE_SHOT_PERCENTAGE]);
    printf("poss\t %d%% \t %d%%\n", stat->values[0][LIVE_GAME_STAT_VALUE_POSSESSION],
	   stat->values[1][LIVE_GAME_STAT_VALUE_POSSESSION]);
    printf("pen.\t %d \t %d\n", stat->values[0][LIVE_GAME_STAT_VALUE_PENALTIES],
	   stat->values[1][LIVE_GAME_STAT_VALUE_PENALTIES]);
    printf("fouls\t %d \t %d\n", stat->values[0][LIVE_GAME_STAT_VALUE_FOULS],
	   stat->values[1][LIVE_GAME_STAT_VALUE_FOULS]);
    printf("cards\t %d \t %d\n", stat->values[0][LIVE_GAME_STAT_VALUE_CARDS],
	   stat->values[1][LIVE_GAME_STAT_VALUE_CARDS]);
    printf("reds\t %d \t %d\n", stat->values[0][LIVE_GAME_STAT_VALUE_REDS],
	   stat->values[1][LIVE_GAME_STAT_VALUE_REDS]);
    printf("inj.\t %d \t %d\n", stat->values[0][LIVE_GAME_STAT_VALUE_INJURIES],
	   stat->values[1][LIVE_GAME_STAT_VALUE_INJURIES]);
}

/** Create arrays containing the names of the
    goal scorers and sent off and injured players.
    @param live_game The live game we examine. */
void
game_create_stats_players(gpointer live_game)
{
    gint i, j;
    LiveGame *match = (LiveGame*)live_game;
    LiveGameStats *stats = &match->stats;
    gint limit = const_int("int_team_max_players");
    gint scorer_ids[2][limit];
    gint cnt[2] = {0, 0};
    gint team, player, player2, array_index;
    gint minute = 0;
    gchar buf[SMALL], buf2[SMALL];
    GString *new = NULL;
    gboolean own_goal;
    GPtrArray *players = NULL;

    for(i=0;i<limit;i++)
	scorer_ids[0][i] = scorer_ids[1][i] = -1;

    for(i=0;i<match->units->len;i++)
    {	
	if(g_array_index(match->units, LiveGameUnit, i).event.type == LIVE_GAME_EVENT_PENALTIES)
	    return;

	minute = MAX(minute, g_array_index(match->units, LiveGameUnit, i).minute);
	team = g_array_index(match->units, LiveGameUnit, i).event.values[LIVE_GAME_EVENT_VALUE_TEAM];
	player = g_array_index(match->units, LiveGameUnit, i).event.values[LIVE_GAME_EVENT_VALUE_PLAYER];
	player2 = g_array_index(match->units, LiveGameUnit, i).event.values[LIVE_GAME_EVENT_VALUE_PLAYER2];

	if(g_array_index(match->units, LiveGameUnit, i).event.type == LIVE_GAME_EVENT_GOAL ||
	   g_array_index(match->units, LiveGameUnit, i).event.type == LIVE_GAME_EVENT_OWN_GOAL)
	{
	    own_goal = (g_array_index(match->units, LiveGameUnit, i).event.type == LIVE_GAME_EVENT_OWN_GOAL);
	    array_index = (own_goal) ? !team : team;

	    if(g_array_index(match->units, LiveGameUnit, i - 1).event.type == LIVE_GAME_EVENT_PENALTY)
		strcpy(buf2, " (P)");
	    else if(g_array_index(match->units, LiveGameUnit, i - 1).event.type == LIVE_GAME_EVENT_FREE_KICK)
		strcpy(buf2, " (FK)");
	    else if(g_array_index(match->units, LiveGameUnit, i).event.type == LIVE_GAME_EVENT_OWN_GOAL)
		strcpy(buf2, " (OG)");
	    else 
		strcpy(buf2, "");

	    if(query_integer_is_in_array(player + (100 * own_goal), scorer_ids[array_index], 0, limit))
	    {
		for(j=0;j<stats->players[LIVE_GAME_STAT_ARRAY_SCORERS][array_index]->len;j++)
		{
		    if(g_str_has_prefix(((GString*)g_ptr_array_index(
					     stats->players[LIVE_GAME_STAT_ARRAY_SCORERS][array_index], j))->str,
					player_of_id(match->fix->teams[team], player)->name->str))
		    {
			sprintf(buf, "%s %d%s",
				((GString*)g_ptr_array_index(
				    stats->players[LIVE_GAME_STAT_ARRAY_SCORERS][array_index], j))->str,
				minute, buf2);
			g_string_printf(((GString*)g_ptr_array_index(
					     stats->players[LIVE_GAME_STAT_ARRAY_SCORERS][array_index], j)),
					"%s", buf);
			break;
		    }

		    if(j == stats->players[LIVE_GAME_STAT_ARRAY_SCORERS][array_index]->len)
			g_warning("game_create_stats_scorers: didn't find scorer %d (team %d)\n",
				  player, team);
		}
	    }
	    else
	    {
		sprintf(buf, "%s %d%s", player_of_id(match->fix->teams[team], player)->name->str,
			minute, buf2);
		new = g_string_new(buf);
		g_ptr_array_add(stats->players[LIVE_GAME_STAT_ARRAY_SCORERS][array_index], new);
		scorer_ids[array_index][cnt[array_index]] = player + (100 * own_goal);
		cnt[array_index]++;
	    }
	}
	else
	{
	    strcpy(buf, "");
	    if(g_array_index(match->units, LiveGameUnit, i).event.type == LIVE_GAME_EVENT_INJURY)
	    {
		sprintf(buf, "%s", player_of_id(match->fix->teams[team], player)->name->str);
		players = stats->players[LIVE_GAME_STAT_ARRAY_INJURED][team];
	    }
	    else if(g_array_index(match->units, LiveGameUnit, i).event.type == LIVE_GAME_EVENT_FOUL_YELLOW)
	    {
		sprintf(buf, "%s", player_of_id(match->fix->teams[team], player2)->name->str);
		players = stats->players[LIVE_GAME_STAT_ARRAY_YELLOWS][team];
	    }
	    else if(g_array_index(match->units, LiveGameUnit, i).event.type == LIVE_GAME_EVENT_SEND_OFF)
	    {
		sprintf(buf, "%s", player_of_id(match->fix->teams[team], player)->name->str);
		players = stats->players[LIVE_GAME_STAT_ARRAY_REDS][team];
	    }

	    if(strlen(buf) > 0)
	    {
		new = g_string_new(buf);
		g_ptr_array_add(players, new);
	    }
	}
    }
}

/** Update the live game stats using the live game unit.
    @param live_game_stats Pointer to the live game.
    @param live_game_unit The live game unit. */
void
game_update_stats(gpointer live_game, gconstpointer live_game_unit)
{
    gint i;
    LiveGameStats *stats = &((LiveGame*)live_game)->stats;
    const LiveGameUnit *unit = (const LiveGameUnit*)live_game_unit;
    
    if(unit->minute != -1)
	stats->values[unit->possession][LIVE_GAME_STAT_VALUE_POSSESSION]++;

    if(unit->event.type == LIVE_GAME_EVENT_SCORING_CHANCE ||
       unit->event.type == LIVE_GAME_EVENT_FREE_KICK)
	stats->values[unit->possession][LIVE_GAME_STAT_VALUE_SHOTS]++;
    else if(unit->event.type == LIVE_GAME_EVENT_PENALTY)
	stats->values[unit->possession][LIVE_GAME_STAT_VALUE_PENALTIES]++;
    else if(unit->event.type == LIVE_GAME_EVENT_INJURY)
    {
	stats->values[unit->event.values[LIVE_GAME_EVENT_VALUE_TEAM]][LIVE_GAME_STAT_VALUE_INJURIES]++;
	game_update_stats_player(live_game, live_game_unit);
    }
    else if(unit->event.type == LIVE_GAME_EVENT_FOUL ||
	    unit->event.type == LIVE_GAME_EVENT_FOUL_YELLOW ||
	    unit->event.type == LIVE_GAME_EVENT_FOUL_RED ||
	    unit->event.type == LIVE_GAME_EVENT_FOUL_RED_INJURY)
    {
	stats->values[unit->event.values[LIVE_GAME_EVENT_VALUE_TEAM]][LIVE_GAME_STAT_VALUE_FOULS]++;
	if(unit->event.type == LIVE_GAME_EVENT_FOUL_YELLOW)
	{
	    stats->values[unit->event.values[LIVE_GAME_EVENT_VALUE_TEAM]][LIVE_GAME_STAT_VALUE_CARDS]++;
	    game_update_stats_player(live_game, live_game_unit);
	}
    }
    else if(unit->event.type == LIVE_GAME_EVENT_SEND_OFF)
    {
	stats->values[unit->event.values[LIVE_GAME_EVENT_VALUE_TEAM]][LIVE_GAME_STAT_VALUE_REDS]++;
	game_update_stats_player(live_game, live_game_unit);
    }
    else if(unit->event.type == LIVE_GAME_EVENT_GOAL &&
	    live_game_unit_before(unit, -1)->event.type != LIVE_GAME_EVENT_PENALTY)
    {
	stats->values[unit->event.values[LIVE_GAME_EVENT_VALUE_TEAM]][LIVE_GAME_STAT_VALUE_GOALS_REGULAR]++;
	game_update_stats_player(live_game, live_game_unit);
    }
    
    for(i=0;i<2;i++)
	stats->values[i][LIVE_GAME_STAT_VALUE_SHOT_PERCENTAGE] =
	    (stats->values[i][LIVE_GAME_STAT_VALUE_SHOTS] > 0) ?
	    (gint)rint(((gfloat)stats->values[i][LIVE_GAME_STAT_VALUE_GOALS_REGULAR] /
			(gfloat)stats->values[i][LIVE_GAME_STAT_VALUE_SHOTS]) * 100) : 0;
}

/** Update the player array in the live game stats.
    @param live_game Pointer to the live game.
    @param team The team of the player.
    @param player The player id.
    @param type The type of the stat. */
void
game_update_stats_player(gpointer live_game, gconstpointer live_game_unit)
{
    gint i;
    gchar buf[SMALL], buf2[SMALL];    
    LiveGameStats *stats = &((LiveGame*)live_game)->stats;
    const LiveGameUnit *unit = (const LiveGameUnit*)live_game_unit;
    gint minute = live_game_unit_get_minute(unit), array_index = -1;
    gboolean own_goal;
    gint team = unit->event.values[LIVE_GAME_EVENT_VALUE_TEAM],
	player = unit->event.values[LIVE_GAME_EVENT_VALUE_PLAYER],
	player2 = unit->event.values[LIVE_GAME_EVENT_VALUE_PLAYER2];
    const Team *tm[2] = {((LiveGame*)live_game)->fix->teams[0], 
			 ((LiveGame*)live_game)->fix->teams[1]};
    GString *new = NULL;
    GPtrArray *players = NULL;
    
    if(unit->event.type == LIVE_GAME_EVENT_GOAL ||
       unit->event.type == LIVE_GAME_EVENT_OWN_GOAL)
    {
	own_goal = (unit->event.type == LIVE_GAME_EVENT_OWN_GOAL);
	array_index = (own_goal) ? !team : team;

	if(live_game_unit_before(unit, -1)->event.type == LIVE_GAME_EVENT_PENALTY)
	    strcpy(buf2, " (P)");
	else if(live_game_unit_before(unit, -1)->event.type == LIVE_GAME_EVENT_FREE_KICK)
	    strcpy(buf2, " (FK)");
	else if(unit->event.type == LIVE_GAME_EVENT_OWN_GOAL)
	    strcpy(buf2, " (OG)");
	else 
	    strcpy(buf2, "");

	for(i=0;i<stats->players[array_index][LIVE_GAME_STAT_ARRAY_SCORERS]->len;i++)
	{
	    if(g_str_has_prefix(((GString*)g_ptr_array_index(
				     stats->players[array_index][LIVE_GAME_STAT_ARRAY_SCORERS], i))->str,
				player_of_id(tm[team], player)->name->str))
	    {
		sprintf(buf, "%s %d%s",
			((GString*)g_ptr_array_index(
			    stats->players[array_index][LIVE_GAME_STAT_ARRAY_SCORERS], i))->str,
			minute, buf2);
		g_string_printf(((GString*)g_ptr_array_index(
				     stats->players[array_index][LIVE_GAME_STAT_ARRAY_SCORERS], i)),
				"%s", buf);
		return;
	    }
	}
    
	sprintf(buf, "%s %d%s", player_of_id(tm[team], player)->name->str,
		minute, buf2);
	new = g_string_new(buf);
	g_ptr_array_add(stats->players[array_index][LIVE_GAME_STAT_ARRAY_SCORERS], new);
    }
    else
    {
	strcpy(buf, "");
	if(unit->event.type == LIVE_GAME_EVENT_INJURY)
	{
	    sprintf(buf, "%s", player_of_id(tm[team], player)->name->str);
	    players = stats->players[team][LIVE_GAME_STAT_ARRAY_INJURED];
	}
	else if(unit->event.type == LIVE_GAME_EVENT_FOUL_YELLOW)
	{
	    sprintf(buf, "%s", player_of_id(tm[team], player2)->name->str);
	    players = stats->players[team][LIVE_GAME_STAT_ARRAY_YELLOWS];
	}
	else if(unit->event.type == LIVE_GAME_EVENT_SEND_OFF)
	{
	    sprintf(buf, "%s", player_of_id(tm[team], player)->name->str);
	    players = stats->players[team][LIVE_GAME_STAT_ARRAY_REDS];
	}

	if(strlen(buf) > 0)
	{
	    new = g_string_new(buf);
	    g_ptr_array_add(players, new);
	}
    }
}

/** Make some team updates after a match.
    @param fix The match we examine. */
void
game_post_match(Fixture *fix)
{
    gint i;

    if(query_fixture_has_tables(fix))
	table_update(fix);
    
    for(i=0;i<2;i++)
    {
	if(team_is_user(fix->teams[i]) == -1 &&
	   fix->teams[i]->clid == fix->clid)
	    team_update_cpu_team(fix->teams[i],
				 (fixture_user_team_involved(fix) != -1));
	else
	    team_update_post_match(fix->teams[i], fix->clid);
    }
}

/** Reduce stadium capacity and safety after a stadium event.
    @param user The user whose stadium we demolish.
    @param type The event type. */
void
game_stadium_event(Stadium *stadium, gint type)
{
    gfloat reduce;
    gfloat reduce_factor[3][2] =
	{{const_float("float_game_stadium_safety_reduce_breakdown_lower"),
	  const_float("float_game_stadium_safety_reduce_breakdown_upper")},
	 {const_float("float_game_stadium_safety_reduce_riots_lower"),
	  const_float("float_game_stadium_safety_reduce_riots_upper")},
	 {const_float("float_game_stadium_safety_reduce_fire_lower"),
	  const_float("float_game_stadium_safety_reduce_fire_upper")}};

    reduce = math_rnd(reduce_factor[type - LIVE_GAME_EVENT_STADIUM_BREAKDOWN][0],
		      reduce_factor[type - LIVE_GAME_EVENT_STADIUM_BREAKDOWN][1]);
    
    printf("event %d saf %.2f cap %d\n",
	   type - LIVE_GAME_EVENT_STADIUM_BREAKDOWN, stadium->safety, stadium->capacity);
    stadium->safety *= (1 - reduce);
    stadium->capacity = (gint)rint((gfloat)stadium->capacity *
				   (1 - reduce *
				    const_float("float_game_stadium_capacity_reduce_factor")));

    printf("2 event %d saf %.2f cap %d\n",
	   type - LIVE_GAME_EVENT_STADIUM_BREAKDOWN, stadium->safety, stadium->capacity);
}
