#include "free.h"
#include "league.h"
#include "maths.h"
#include "player.h"
#include "team.h"
#include "variables.h"

/** How much a player's skill can deviate from 
    the average skill in his team. */
#define CONSTANT_PLAYER_AVERAGE_SKILL_VARIANCE 0.1
/** Lower limit for player ages. */
#define CONSTANT_PLAYER_AGE_LOWER (18 * 52)
/** Upper limit for player ages. */
#define CONSTANT_PLAYER_AGE_UPPER (36 * 52)
/** Lower limit for player peak ages. */
#define CONSTANT_PLAYER_PEAK_AGE_LOWER (30 * 52)
/** Upper limit for player peak ages. */
#define CONSTANT_PLAYER_PEAK_AGE_UPPER (33 * 52)
/** By how many weeks the peak age of goalies is
    greater. */
#define CONSTANT_PLAYER_PEAK_AGE_GOALIE_ADDITION (2 * 52)
/** Limits for initial fitness. */
#define CONSTANT_PLAYER_FITNESS_LOWER 85
/** Limits for initial fitness. */
#define CONSTANT_PLAYER_FITNESS_UPPER 100

/** The bounds determining the player positions in a newly created
    team for players 13 to CONSTANT_TEAM_MAX_PLAYERS.
    Player 11 is always the second goalie. */
#define CONSTANT_PLAYER_POS_BOUND1 15
#define CONSTANT_PLAYER_POS_BOUND2 17

/** Bounds for the contract time at player generation. */
#define CONSTANT_PLAYER_CONTRACT_LOWER 52
#define CONSTANT_PLAYER_CONTRACT_UPPER 4 * 52

/** Bounds for the last skill update at player generation. */
#define CONSTANT_PLAYER_LSU_LOWER 2
#define CONSTANT_PLAYER_LSU_UPPER 10

/** These determine the value calculation of players.
    Value is a function of skill and talent involving
    a power.
    @see player_assign_value()*/
#define CONSTANT_PLAYER_VALUE_SKILL_WEIGHT 0.65
#define CONSTANT_PLAYER_VALUE_POWER 3.5

/** These determine the wage calculation of players.
    Wage depends on the value and a random factor near 1. 
    @see player_assign_wage() */
#define CONSTANT_PLAYER_WAGE_VALUE_FACTOR 0.01
#define CONSTANT_PLAYER_WAGE_RANDOM_DEV 0.15

/** This determines the accuracy of the scout's 
    talent estimate. The smaller the better. */
#define CONSTANT_PLAYER_ETAL_SCOUT_FACTOR 7

/** Create and return a new player.
    @param tm The team the player will belong to.
    @param average_skill The average skill of the team. 
    The player's skill can deviate from this value by #CONSTANT_PLAYER_AVERAGE_SKILL_VARIANCE %
    @return A newly created player. */
Player
player_new(const Team *tm, gint average_skill)
{
    gfloat skill_factor = 
	math_rnd(1 - CONSTANT_PLAYER_AVERAGE_SKILL_VARIANCE,
		 1 + CONSTANT_PLAYER_AVERAGE_SKILL_VARIANCE);
    Player new;

    new.name = 
	g_string_new(((GString*)g_ptr_array_index(player_names, math_rndi(0, player_names->len - 1)))->str);
    new.clid = tm->clid;
    new.team_id = tm->id;
    new.id = player_new_id(tm->players);
    new.pos = player_get_position_from_structure(tm->structure, tm->players->len);
    new.cpos = new.pos;
    new.skill = CLAMP((gint)rint((gfloat)average_skill * skill_factor), 0, CONSTANT_PLAYER_MAX_SKILL);
    new.cskill = new.skill;
    new.age = math_gauss_disti(CONSTANT_PLAYER_AGE_LOWER,
			       CONSTANT_PLAYER_AGE_UPPER);
    new.peak_age =
	math_rndi(CONSTANT_PLAYER_PEAK_AGE_LOWER +
		  (new.pos == PLAYER_POS_GOALIE) * CONSTANT_PLAYER_PEAK_AGE_GOALIE_ADDITION,
		  CONSTANT_PLAYER_PEAK_AGE_UPPER +
		  (new.pos == PLAYER_POS_GOALIE) * CONSTANT_PLAYER_PEAK_AGE_GOALIE_ADDITION);
    new.talent = player_new_talent(new.skill);
    new.etal = player_estimate_talent(&new);
    new.fitness = math_rndi(CONSTANT_PLAYER_FITNESS_LOWER, CONSTANT_PLAYER_FITNESS_UPPER);
    new.health = new.recovery = new.goals = new.games = 0;
    new.value = player_assign_value(&new);
    new.wage = player_assign_wage(&new);
    new.contract = math_rndi(CONSTANT_PLAYER_CONTRACT_LOWER, CONSTANT_PLAYER_CONTRACT_UPPER);
    new.lsu = math_rndi(CONSTANT_PLAYER_LSU_LOWER, CONSTANT_PLAYER_LSU_UPPER);
    new.cards = g_array_new(FALSE, FALSE, sizeof(PlayerCard));
    /* todo: make player history struct. */
    new.history = NULL;
    
    return new;
}

/** Return a player id that's not yet 'occupied'.
    @param players The player array the new player will belong to. 
    @return A new id that none of the other players has. */
gint
player_new_id(const GArray *players)
{
    gint i, j;

    for(i=0;i<CONSTANT_TEAM_MAX_PLAYERS;i++)
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
	    (player_number > 10 && player_number < CONSTANT_PLAYER_POS_BOUND1))
	position = PLAYER_POS_DEFENDER;
    else if(player_number < bound[1] ||
	    (player_number > 10 && player_number < CONSTANT_PLAYER_POS_BOUND2))
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
    gint talent = math_gauss_disti(2 * skill - CONSTANT_PLAYER_MAX_SKILL,
				   CONSTANT_PLAYER_MAX_SKILL);
    if(talent < skill)
	talent = 2 * skill - talent;

    return talent;
}

/** Estimate a player's talent. The quality of the estimate
    depends on the quality of the scout.
    @param pl The player of which we'd like to estimate the talent.
    @return A talent estimate. */
gint
player_estimate_talent(const Player *pl)
{
    gint i;

    /* the maximal deviance in both directions */
    gint deviance_bound[2] =
	{pl->talent - pl->skill, CONSTANT_PLAYER_MAX_SKILL - pl->talent};

    /* the scout's maximal deviance */
    gfloat scout_deviance = (scout % 10) * CONSTANT_PLAYER_ETAL_SCOUT_FACTOR;

    /* adjust deviance_bounds with regard to the scout's
       deviance */
    for(i=0;i<2;i++)
	deviance_bound[i] = MIN(deviance_bound[i], scout_deviance);

    return math_rndi(pl->talent - deviance_bound[0],
		     pl->talent + deviance_bound[1]);
}

/** Assign a (transfer) value to a player. The value depends on skill,
    talent and age.
    @param pl The player we examine. 
    @return The value of the player. */
gint
player_assign_value(const Player *pl)
{
    gint value;

    value = (gint)powf((CONSTANT_PLAYER_VALUE_SKILL_WEIGHT * (gfloat)pl->skill
			+ (1 - CONSTANT_PLAYER_VALUE_SKILL_WEIGHT) * (gfloat)pl->talent * 0.7),
		       CONSTANT_PLAYER_VALUE_POWER);

    if(pl->age <= CONSTANT_PLAYER_AGE_LOWER + 2 * 52)
	value = (gint)((gfloat)value * 1.05);
    else if(pl->age <= CONSTANT_PLAYER_AGE_LOWER + 4 * 52)
	value = (gint)((gfloat)value * 1.1);
    else if(pl->age >= CONSTANT_PLAYER_AGE_UPPER - 4 * 52)
	value = (gint)((gfloat)value * 0.95);
    else if(pl->age >= CONSTANT_PLAYER_AGE_UPPER - 2 * 52)
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

    wage = rint(((gfloat)pl->value * CONSTANT_PLAYER_WAGE_VALUE_FACTOR) *
		math_rnd(1 - CONSTANT_PLAYER_WAGE_RANDOM_DEV,
			 1 + CONSTANT_PLAYER_WAGE_RANDOM_DEV) );

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
    dest->history = NULL;

    for(i=0;i<source->cards->len;i++)
	g_array_append_val(dest->cards, 
			   g_array_index(source->cards, PlayerCard, i));
}

/** Copy a player into a team in a way that allows us to
    free the player afterwards.
    @param pl The player we copy.
    @param tm The team we copy the player to.
    @see player_copy() */
void
player_append_to_array(const Player *pl, Team *tm)
{
    Player new_player = player_new(tm, CONSTANT_PLAYER_MAX_SKILL);

    player_copy(pl, &new_player);

    g_array_append_val(tm->players, new_player);
}
