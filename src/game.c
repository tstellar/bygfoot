#include "fixture.h"
#include "game.h"
#include "maths.h"
#include "player.h"
#include "team.h"
#include "variables.h"

/** Influence in % of the game style towards more attack. 
    @see game_get_values() */
#define CONSTANT_GAME_STYLE_FACTOR 0.075

/** How the cskill of field players get weighted for the team values in
    a match. Rows are player position, columns value type. 
    @see game_get_player_contribution() */
gfloat player_weights[3][3] =
{{1, 0.4, 0.25},
 {0.625, 1, 0.625},
 {0.2, 0.6, 1.3}};

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
    printf("\nhome %.2f\n", home_advantage);
    for(i=0;i<2;i++)
    {
	for(j=0;j<GAME_TEAM_VALUE_END;j++)
	    team_values[i][j] = 0;
	
	style_factor = (gfloat)tm[i]->style * CONSTANT_GAME_STYLE_FACTOR;

	team_values[i][GAME_TEAM_VALUE_GOALIE] = 
	    game_get_player_contribution(player_of(tm[i], 0), FALSE) * 
	    (1 + home_advantage * (i == 0)) *
	    (1 + CONSTANT_PLAYER_BOOST_SKILL_EFFECT * (tm[i] == my_team && options[OPT_BOOL_BOOST] == 1));

	for(j=1;j<11;j++)
	{
	    team_values[i][GAME_TEAM_VALUE_ATTACK] +=
		game_get_player_contribution(player_of(tm[i], j), GAME_TEAM_VALUE_ATTACK);
	    team_values[i][GAME_TEAM_VALUE_MIDFIELD] +=
		game_get_player_contribution(player_of(tm[i], j), GAME_TEAM_VALUE_MIDFIELD);
	    team_values[i][GAME_TEAM_VALUE_DEFEND] +=
		game_get_player_contribution(player_of(tm[i], j), GAME_TEAM_VALUE_DEFEND);
	}

	team_values[i][GAME_TEAM_VALUE_ATTACK] *= 
	    ((1 + style_factor) * (1 + home_advantage * (i == 0)) *
	     (1 + CONSTANT_PLAYER_BOOST_SKILL_EFFECT * (tm[i] == my_team && options[OPT_BOOL_BOOST] == 1)));
	team_values[i][GAME_TEAM_VALUE_DEFEND] *= 
	    ((1 - style_factor) * (1 + home_advantage * (i == 0)) *
	     (1 + CONSTANT_PLAYER_BOOST_SKILL_EFFECT * (tm[i] == my_team && options[OPT_BOOL_BOOST] == 1)));
	
	/*d*/
	printf("%s attack %.1f midf %.1f defend %.1f\n", 
	       tm[i]->name->str, 
	       team_values[i][GAME_TEAM_VALUE_ATTACK],
	       team_values[i][GAME_TEAM_VALUE_MIDFIELD],
	       team_values[i][GAME_TEAM_VALUE_DEFEND]);
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
    return (gfloat)(pl->cskill * powf((gfloat)pl->fitness / 100, 
				      CONSTANT_GAME_PLAYER_FITNESS_EXPONENT) *
		    player_weights[pl->cpos - 1][type - GAME_TEAM_VALUE_DEFEND]);
}

/** Return a random attacking or defending player
    or the player that has to shoot the number_of_penalty'th penalty.
    @param tm The team we examine.
    @param player_type Whether we concentrate on attacking or defending players
    or would like to have a penalty shooting player.
    @param number_of_penalty Which penalty has to be taken (1st, 2nd etc.)
    @param not_this_one A player to exclude.
    @param skills Whether to weight with skills, too.
    @return A player index. */
gint
game_get_player(const Team *tm, gint player_type, 
		gint number_of_penalty, gint not_this_one,
		gboolean skills)
{
    gint i, player = not_this_one;
    gfloat weights[3];
    gfloat probs[10];
    gfloat rndom;

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
    else if(player_type == GAME_PLAYER_TYPE_INJURY)
	weights[0] = -1;
    else if(player_type == GAME_PLAYER_TYPE_PENALTY)
	/*d*/
	return player_of(tm, 10)->id;

    game_get_player_probs(tm->players, probs, weights, skills);

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

    if(weights[0] == -1)
	for(i=0;i<10;i++)
	    probs[i] = 1;
    else
    {
	probs[0] = (skills) ? (gfloat)g_array_index(players, Player, 1).cskill * 
	    powf((gfloat)g_array_index(players, Player, 1).fitness, 
		 CONSTANT_GAME_PLAYER_FITNESS_EXPONENT) *
	    weights[g_array_index(players, Player, 1).pos - 1] :
	    weights[g_array_index(players, Player, 1).pos - 1];
	for(i=1;i<10;i++)
	    probs[i] = probs[i - 1] + 
		((skills) ? (gfloat)g_array_index(players, Player, i + 1).cskill * 
		 powf((gfloat)g_array_index(players, Player, i + 1).fitness, 
		      CONSTANT_GAME_PLAYER_FITNESS_EXPONENT) *
		 weights[g_array_index(players, Player, i + 1).pos - 1] :
		 weights[g_array_index(players, Player, i + 1).pos - 1]);
    }
}
