#include "fixture.h"
#include "game.h"
#include "game_gui.h"
#include "maths.h"
#include "misc.h"
#include "option.h"
#include "player.h"
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

	printf("%s attack %.1f midf %.1f defend %.1f style %d struct %d\n",
	       tm[i]->name->str,
	       team_values[i][GAME_TEAM_VALUE_ATTACK],
	       team_values[i][GAME_TEAM_VALUE_MIDFIELD],
	       team_values[i][GAME_TEAM_VALUE_DEFEND],
	       tm[i]->style, tm[i]->structure);
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

    probs[0] = (skills) ? (gfloat)g_array_index(players, Player, 1).cskill * 
	powf((gfloat)g_array_index(players, Player, 1).fitness, 
	     const_float("float_player_fitness_exponent")) *
	weights[g_array_index(players, Player, 1).pos - 1] :
	weights[g_array_index(players, Player, 1).pos - 1] *
	(g_array_index(players, Player, 1).cskill != 0);
    for(i=1;i<10;i++)
	probs[i] = probs[i - 1] + 
	    ((skills) ? (gfloat)g_array_index(players, Player, i + 1).cskill * 
	     powf((gfloat)g_array_index(players, Player, i + 1).fitness, 
		  const_float("float_player_fitness_exponent")) *
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

    /*d*/
    fix->attendance = 1000;

    for(i=0;i<2;i++)
	for(j=0;j<11;j++)
	    if(player_of(fix->teams[i], j)->cskill > 0)
		game_player_increase(fix->clid, player_of(fix->teams[i], j),
				     GAME_PLAYER_INCREASE_GAMES);
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

/** Increase the number of shots in the player struct.
    @param fix The game being played.
    @param team The team index.
    @param player_id The player id. */
void
game_player_increase(gint clid, Player *pl, gint type)
{
    gint i;
    PlayerGamesGoals new;

    for(i=0;i<pl->games_goals->len;i++)
	if(g_array_index(pl->games_goals, PlayerGamesGoals, i).clid == clid)
	{
	    if(type == GAME_PLAYER_INCREASE_SHOTS)
		g_array_index(pl->games_goals, PlayerGamesGoals, i).shots++;
	    else if(type == GAME_PLAYER_INCREASE_GOALS)
		g_array_index(pl->games_goals, PlayerGamesGoals, i).goals++;
	    else if(type == GAME_PLAYER_INCREASE_GAMES)
		g_array_index(pl->games_goals, PlayerGamesGoals, i).games++;
		
	    return;
	}

    /* Entry not found, we create a new one. */    
    new.clid = clid;
    new.shots = new.goals = new.games = 0;

    g_array_append_val(pl->games_goals, new);

    game_player_increase(clid, pl, type);
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
	    pl->recovery = duration[i - 1];
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
	 player_substitution_good_structure(tm->structure,
					    player_of(tm, player_number)->cpos,
					    ((Player*)g_ptr_array_index(substitutes, 0))->pos));

    substitute = ((Player*)g_ptr_array_index(substitutes, 0))->id;
    player_swap(tm, player_number,
		tm, player_id_index(tm, ((Player*)g_ptr_array_index(substitutes, 0))->id));

    g_ptr_array_free(substitutes, TRUE);

    if(adapt_structure)
    {
	team_change_structure(tm, team_find_appropriate_structure(tm));
	team_rearrange(tm);
    }

    if(team_is_user(tm) == current_user)
    {
	game_gui_write_av_skills();
	
	selected_row[0] = -1;	
	treeview_show_user_player_list(&usr(current_user), 1);
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
	    return i;

    return -1;
}

/** Send off a player. Choose number of weeks banned randomly.
    @param clid League or cup id of the match.
    @param pl The player.
    @param red Whether it's a red or a yellow card.
    @param second_yellow Whether this is only a yellow. */
void
game_player_card(gint clid, Player *pl, gboolean red, gboolean second_yellow)
{
    gint i;
    PlayerCard new;

    if(red)
	pl->cskill = 0;

    for(i=0;i<pl->cards->len;i++)
	if(g_array_index(pl->cards, PlayerCard, i).clid == clid)
	{
	    if(red)
	    {
		if(second_yellow)
		{
		    g_array_index(pl->cards, PlayerCard, i).yellow = 0;
		    g_array_index(pl->cards, PlayerCard, i).red = 1;
		}
		else
		    g_array_index(pl->cards, PlayerCard, i).red = 
			game_player_get_ban_duration();
	    }
	    else
		g_array_index(pl->cards, PlayerCard, i).yellow++;
	    return;
	}

    new.clid = clid;
    new.red = 0;
    new.yellow = 0;

    g_array_append_val(pl->cards, new);

    game_player_card(clid, pl, red, second_yellow);
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

    if(team_is_user(tm) == current_user)
    {
	game_gui_write_av_skills();
	
	selected_row[0] = -1;	
	treeview_show_user_player_list(&usr(current_user), 1);
    }
}

/** Decrease the players' fitness during a live game.
    @param fix The match being played. */
void
game_decrease_fitness(const Fixture *fix)
{
    gint i, j;

    for(i=0;i<2;i++)
	for(j=0;j<11;j++)
	    if(player_of(fix->teams[i], j)->cskill > 0)
		player_decrease_fitness(player_of(fix->teams[i], j));
}
