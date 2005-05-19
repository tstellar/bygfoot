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

    for(i=0;i<2;i++)
    {
	for(j=0;j<GAME_TEAM_VALUE_END;j++)
	    team_values[i][j] = 0;
	
	style_factor = (gfloat)tm[i]->style * const_float("float_game_style_factor");

	team_values[i][GAME_TEAM_VALUE_GOALIE] = 
	    game_get_player_contribution(player_of_idx_team(tm[i], 0), FALSE) * 
	    (1 + home_advantage * (i == 0)) *
	    (1 + const_float("float_player_boost_skill_effect") * tm[i]->boost);

	for(j=1;j<11;j++)
	{
	    team_values[i][GAME_TEAM_VALUE_ATTACK] +=
		game_get_player_contribution(player_of_idx_team(tm[i], j), GAME_TEAM_VALUE_ATTACK);
	    team_values[i][GAME_TEAM_VALUE_MIDFIELD] +=
		game_get_player_contribution(player_of_idx_team(tm[i], j), GAME_TEAM_VALUE_MIDFIELD);
	    team_values[i][GAME_TEAM_VALUE_DEFEND] +=
		game_get_player_contribution(player_of_idx_team(tm[i], j), GAME_TEAM_VALUE_DEFEND);
	}

	for(j=GAME_TEAM_VALUE_DEFEND;j<GAME_TEAM_VALUE_DEFEND + 3;j++)
	    team_values[i][j] *= 
		((1 + home_advantage * (i == 0)) *
		 (1 + const_float("float_player_boost_skill_effect") * tm[i]->boost));

	team_values[i][GAME_TEAM_VALUE_DEFEND] *= (1 - style_factor);
	team_values[i][GAME_TEAM_VALUE_ATTACK] *= (1 + style_factor);
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
		player = player_of_idx_team(tm, 1)->id;
	    else
		for(i=1;i<10;i++)
		    if(rndom < probs[i] && rndom > probs[i - 1])
			player = player_of_idx_team(tm, i + 1)->id;
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
	    printf("%d %20s health %d cskill %.2f\n", i, player_of_idx_team(tm, i)->name->str,
		   player_of_idx_team(tm, i)->health, player_of_idx_team(tm, i)->cskill);
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
	if(player_of_idx_team(tm, i)->cskill != 0)
	    g_ptr_array_add(players, player_of_idx_team(tm, i));

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
	  query_cup_is_national(fix->clid))) ?
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

	if(debug < 50)
	{
	    fix->teams[0]->stadium.safety -= 
		math_rnd(const_float("float_game_stadium_safety_deterioration_lower"),
			 const_float("float_game_stadium_safety_deterioration_upper"));
	    fix->teams[0]->stadium.safety = CLAMP(fix->teams[0]->stadium.safety, 0, 1);
	}
    }

    for(i=0;i<2;i++)
    {
	for(j=0;j<fix->teams[i]->players->len;j++)
	{
	    if(j < 11)
	    {
		if(g_array_index(fix->teams[i]->players, Player, j).cskill > 0)
		    player_games_goals_set(&g_array_index(fix->teams[i]->players, Player, j), fix->clid,
					   PLAYER_VALUE_GAMES, 1, TRUE);
		
		g_array_index(fix->teams[i]->players, Player, j).participation = TRUE;
	    }
	}

	if(user_idx[i] != -1)
	{
	    if(i == 1 || !fix->home_advantage)
	    {
		usr(user_idx[i]).money_out[1][MON_OUT_JOURNEY] -= 
		    (gint)(finance_wage_unit(fix->teams[i]) * journey_factor);
		usr(user_idx[i]).money -= (gint)(finance_wage_unit(fix->teams[i]) * journey_factor);
	    }

	    if(!fix->home_advantage)
	    {
		usr(user_idx[i]).money_in[1][MON_IN_TICKET] += (gint)rint((gfloat)ticket_income / 2);
		usr(user_idx[i]).money += (gint)rint((gfloat)ticket_income / 2);
	    }

	    usr(user_idx[i]).counters[COUNT_USER_SHOW_RES] = 1;
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
    gint max_att = MIN((gint)rint((gfloat)league_cup_average_capacity(tm[0]->clid) *
				  const_float("float_game_stadium_attendance_average_exceed_factor") *
				  math_rnd(0.9, 1.1)),
		       tm[0]->stadium.capacity);

    if(fix->clid < ID_CUP_START && 
       team_get_league_rank(tm[1]) <
       (gint)rint((gfloat)league_from_clid(fix->clid)->teams->len *
		  const_float("float_game_stadium_attendance_rank_percentage")))
	factor *= const_float("float_game_stadium_attendance_rank_factor");

    if(fix->clid >= ID_CUP_START)
    {
	if(cup_from_clid(fix->clid)->rounds->len - fix->round <=
	   const_int("int_game_stadium_attendance_cup_rounds_full_house"))
	    factor = 1;
	else if(query_cup_is_national(fix->clid))
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
		player_of_idx_team(teams[i], j)->id;
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
	    if(!query_integer_is_in_array(player_of_idx_team(teams[i], j)->id,
					  usr(stat2).live_game.team_state[i].player_ids,
					  0, 11))
		subs[i]++;
	
	if(subs[i] > usr(stat2).live_game.subs_left[i])
	{
	    game_reset_players(i);
	    return_value = FALSE;
	}
    }

    return return_value;
}

/** Undo the player swaps the user made during a live game pause. 
    @param idx The team index in the current live game fixture. */
void
game_reset_players(gint idx)
{
    gint i;
    Team *tm = usr(stat2).live_game.fix->teams[idx];
    GArray *players = g_array_new(FALSE, FALSE, sizeof(Player));

    for(i=0;i<11;i++)
	g_array_append_val(players, 
			   *player_of_id_team(tm, usr(stat2).live_game.team_state[idx].player_ids[i]));
    
    for(i=0;i<tm->players->len;i++)
	if(!query_integer_is_in_array(player_of_idx_team(tm, i)->id,
				      usr(stat2).live_game.team_state[idx].player_ids,
				      0, 11))
	    g_array_append_val(players, *player_of_idx_team(tm, i));

    g_array_free(tm->players, TRUE);

    tm->players = players;

    team_change_structure(tm, team_find_appropriate_structure(tm));
    team_rearrange(tm);

    treeview_show_user_player_list();
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
	current_players[i] = player_of_idx_team(tm, i)->id;
	if(!query_integer_is_in_array(player_of_idx_team(tm, i)->id,
				      usr(stat2).live_game.team_state[team_number].player_ids,
				      0, 11))
	{
	    subs_in[cnt] = player_of_idx_team(tm, i)->id;
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
			 const_int("int_player_injury_duration_dev_ligament")),
	50};

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

    if(pl->health == PLAYER_INJURY_CAREER_STOP && team_is_user(pl->team) != -1)
	user_event_add(&usr(team_is_user(pl->team)), EVENT_TYPE_PLAYER_CAREER_STOP, pl->id, -1,
		       NULL, NULL);
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
	if(g_array_index(tm->players, Player, i).cskill > 0)
	    g_ptr_array_add(substitutes, &g_array_index(tm->players, Player, i));

    g_ptr_array_sort_with_data(substitutes, (GCompareDataFunc)player_compare_substitute_func,
			       GINT_TO_POINTER(player_of_idx_team(tm, player_number)->cpos));
    adapt_structure = 
	(math_get_place(team_find_appropriate_structure(tm), 1) + 
	 math_get_place(team_find_appropriate_structure(tm), 2) + 
	 math_get_place(team_find_appropriate_structure(tm), 3) != 10 ||
	 (player_of_idx_team(tm, player_number)->cpos != ((Player*)g_ptr_array_index(substitutes, 0))->pos &&
	  player_substitution_good_structure(tm->structure,
					     player_of_idx_team(tm, player_number)->cpos,
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
	treeview_show_user_player_list();
    }

    return substitute;
}

/** Find out whether we substitute a player to balance
    a team after a red card.
    @param clid The cup/league id of the fixture.
    @param tm The team.
    @return A player index or -1 if we don't substitute.
*/
gint
game_find_to_substitute(gint clid, const Team *tm)
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
	if(player_card_get(player_of_idx_team(tm, i), clid, PLAYER_VALUE_CARD_RED) <= 0)
	    g_ptr_array_add(players, player_of_idx_team(tm, i));

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
    @param clid The id of the fixture.
    @param tm The team.
    @param player The player index.
    @param to_substitute The return location for the index of
    the player to substitute.
    @param substitute The return location for the player who comes into the game. */
void
game_substitute_player_send_off(gint clid, Team *tm, gint player_number, 
				gint *to_substitute, gint *substitute)
{
    gint i;
    gint position = -1;
    GPtrArray *substitutes = NULL;
    gint current_structure = team_find_appropriate_structure(tm);
    gint num_forw = current_structure % 10,
	num_mid = math_get_place(current_structure, 2),
	num_def = math_get_place(current_structure, 3);

    *to_substitute = game_find_to_substitute(clid, tm);

    if(*to_substitute == -1)
	return;

    substitutes = g_ptr_array_new();
    for(i=11;i<tm->players->len;i++)
	g_ptr_array_add(substitutes, player_of_idx_team(tm, i));

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
	treeview_show_user_player_list();
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
	    if(player_of_idx_team(fix->teams[i], j)->cskill > 0)
		player_decrease_fitness(player_of_idx_team(fix->teams[i], j));
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
       unit->event.type == LIVE_GAME_EVENT_FREE_KICK ||
       unit->event.type == LIVE_GAME_EVENT_HEADER)
	stats->values[unit->possession][LIVE_GAME_STAT_VALUE_SHOTS]++;
    else if(unit->event.type == LIVE_GAME_EVENT_PENALTY)
	stats->values[unit->possession][LIVE_GAME_STAT_VALUE_PENALTIES]++;
    else if(unit->event.type == LIVE_GAME_EVENT_INJURY)
    {
	stats->values[unit->event.team][LIVE_GAME_STAT_VALUE_INJURIES]++;
	game_update_stats_player(live_game, live_game_unit);
    }
    else if(unit->event.type == LIVE_GAME_EVENT_FOUL ||
	    unit->event.type == LIVE_GAME_EVENT_FOUL_YELLOW ||
	    unit->event.type == LIVE_GAME_EVENT_FOUL_RED ||
	    unit->event.type == LIVE_GAME_EVENT_FOUL_RED_INJURY)
    {
	stats->values[unit->event.team][LIVE_GAME_STAT_VALUE_FOULS]++;
	if(unit->event.type == LIVE_GAME_EVENT_FOUL_YELLOW)
	{
	    stats->values[unit->event.team][LIVE_GAME_STAT_VALUE_CARDS]++;
	    game_update_stats_player(live_game, live_game_unit);
	}
    }
    else if(unit->event.type == LIVE_GAME_EVENT_SEND_OFF)
    {
	stats->values[unit->event.team][LIVE_GAME_STAT_VALUE_REDS]++;
	game_update_stats_player(live_game, live_game_unit);
    }
    else if(unit->event.type == LIVE_GAME_EVENT_GOAL ||
	    unit->event.type == LIVE_GAME_EVENT_OWN_GOAL)
    {
	if(live_game_unit_before(unit, -1)->event.type != LIVE_GAME_EVENT_PENALTY &&
	   unit->event.type != LIVE_GAME_EVENT_OWN_GOAL)
	    stats->values[unit->event.team][LIVE_GAME_STAT_VALUE_GOALS_REGULAR]++;
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
    gint team = unit->event.team,
	player = unit->event.player,
	player2 = unit->event.player2;
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
				player_of_id_team(tm[team], player)->name->str))
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
    
	sprintf(buf, "%s %d%s", player_of_id_team(tm[team], player)->name->str,
		minute, buf2);
	new = g_string_new(buf);
	g_ptr_array_add(stats->players[array_index][LIVE_GAME_STAT_ARRAY_SCORERS], new);
    }
    else
    {
	strcpy(buf, "");
	if(unit->event.type == LIVE_GAME_EVENT_INJURY)
	{
	    sprintf(buf, "%s", player_of_id_team(tm[team], player)->name->str);
	    players = stats->players[team][LIVE_GAME_STAT_ARRAY_INJURED];
	}
	else if(unit->event.type == LIVE_GAME_EVENT_FOUL_YELLOW)
	{
	    sprintf(buf, "%s", player_of_id_team(tm[team], player2)->name->str);
	    players = stats->players[team][LIVE_GAME_STAT_ARRAY_YELLOWS];
	}
	else if(unit->event.type == LIVE_GAME_EVENT_SEND_OFF)
	{
	    sprintf(buf, "%s", player_of_id_team(tm[team], player)->name->str);
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
    GPtrArray *teams = NULL;
    Cup *cup = NULL;

    if(query_fixture_has_tables(fix))
	table_update(fix);
    
    for(i=0;i<2;i++)
    {
	if(team_is_user(fix->teams[i]) == -1)
	    team_update_cpu_team(fix->teams[i],
				 (fixture_user_team_involved(fix) != -1));
	else
	    team_update_post_match(fix->teams[i], fix->clid);
    }

    if(fix->clid < ID_CUP_START)
	return;

    cup = cup_from_clid(fix->clid);

    if(fix->round == cup->rounds->len - 1 &&
       fix == &g_array_index(cup->fixtures, Fixture, cup->fixtures->len - 1))
    {
	teams = cup_get_teams_sorted(cup);
	
	if(team_is_user((Team*)g_ptr_array_index(teams, 0)) != -1)
	    user_history_add(&usr(team_is_user((Team*)g_ptr_array_index(teams, 0))),
			     USER_HISTORY_WIN_FINAL, ((Team*)g_ptr_array_index(teams, 0))->id,
			     fix->clid, fix->round,((Team*)g_ptr_array_index(teams, 1))->name->str);	    
	else if(team_is_user((Team*)g_ptr_array_index(teams, 1)) != -1)
	    user_history_add(&usr(team_is_user((Team*)g_ptr_array_index(teams, 1))),
			     USER_HISTORY_LOSE_FINAL, ((Team*)g_ptr_array_index(teams, 1))->id,
			     fix->clid, fix->round,((Team*)g_ptr_array_index(teams, 0))->name->str);
	g_ptr_array_free(teams, TRUE);
    }
    else if(fixture_user_team_involved(fix) != -1)
	user_history_add(&usr(fixture_user_team_involved(fix)), USER_HISTORY_REACH_CUP_ROUND,
			 usr(fixture_user_team_involved(fix)).team_id,
			 fix->clid, fix->round, "");
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
    
    stadium->safety *= (1 - reduce);
    stadium->capacity = (gint)rint((gfloat)stadium->capacity *
				   (1 - reduce *
				    const_float("float_game_stadium_capacity_reduce_factor")));
}

/** Return the maximum possible values for defence, midfield and attack. 
    Used in the opponent preview. */
void
game_get_max_values(gfloat max_values[3])
{
    gint i, j;
    Player pl;

    pl.skill = pl.cskill = const_float("float_player_max_skill");
    pl.fitness = 1;

    for(i=0;i<3;i++)
    {
	max_values[i] = 0;
	for(j=1;j<11;j++)
	{
	    pl.cpos = player_get_position_from_structure(442, j);
	    max_values[i] += game_get_player_contribution(&pl, i);
	}
    }
}

/** Find out whether there's a default penalty shooter
    in the team.
    @return A player id or -1. */
gint
game_get_default_penalty_shooter(const Team *tm)
{
    gint return_value = -1;

    if(team_is_user(tm) != -1 &&
       option_int("int_opt_user_penalty_shooter",
		  &usr(team_is_user(tm)).options) != -1 &&
       player_of_id_team(tm, 
			 option_int("int_opt_user_penalty_shooter",
				    &usr(team_is_user(tm)).options)) != NULL &&
       player_id_index(tm,
		       option_int("int_opt_user_penalty_shooter",
				  &usr(team_is_user(tm)).options)) < 11 &&
       player_of_id_team(tm, 
			 option_int("int_opt_user_penalty_shooter",
				    &usr(team_is_user(tm)).options))->cskill != 0)
	return_value = option_int("int_opt_user_penalty_shooter",
				  &usr(team_is_user(tm)).options);

    return return_value;
}
