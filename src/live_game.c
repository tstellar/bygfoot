#include "fixture.h"
#include "free.h"
#include "game_gui.h"
#include "live_game.h"
#include "maths.h"
#include "player.h"
#include "team.h"
#include "treeview.h"
#include "variables.h"

/** Constants determining the type of a LiveGameEvent. 
    @see live_game_create_event_type() */

/** Base probability that the ball gets from defending area
    to midfield area. */
#define CONSTANT_LIVE_GAME_AREA_DEF_MID 0.5
/** Influence of attack/defend values on the base probability; the lower
    the smaller the influence. */
#define CONSTANT_LIVE_GAME_AREA_DEF_MID_TEAM_EXPONENT 0.5

/** Base probability that the ball gets from midfield area
    to attack area or back to defend area. */
#define CONSTANT_LIVE_GAME_AREA_MID_ATT 0.5
#define CONSTANT_LIVE_GAME_AREA_MID_DEF 0.3
/** Influence of attack/defend values on the base probability; the lower
    the smaller the influence. */
#define CONSTANT_LIVE_GAME_AREA_MID_TEAM_EXPONENT 1

/** Base probability that the ball gets from attacking area
    to midfield area. */
#define CONSTANT_LIVE_GAME_AREA_ATT_MID 0.3
/** Influence of attack/defend values on the base probability; the lower
    the smaller the influence. */
#define CONSTANT_LIVE_GAME_AREA_ATT_MID_TEAM_EXPONENT 0.5

/** Base probability of a general event (no foul, no injury, no goal etc.) */
#define CONSTANT_LIVE_GAME_EVENT_GENERAL 0.5

/** Base probability for possession change. */
#define CONSTANT_LIVE_GAME_POSSESSION_CHANGES 0.2
/** Influence of the team values on the possession change. */
#define CONSTANT_LIVE_GAME_POSSESSION_TEAM_EXPONENT 2

/** Base prob for a scoring chance if a team is attacking. */
#define CONSTANT_LIVE_GAME_SCORING_CHANCE 0.2
/** Team values influence on the scoring chance. */
#define CONSTANT_LIVE_GAME_SCORING_CHANCE_TEAM_EXPONENT 1

/** Probability that it's the player in possession who
    has the scoring chance. */
#define CONSTANT_LIVE_GAME_PLAYER_IN_POSS_SHOOTS 0.5

/** Base probability to score with a scoring chance. */
#define CONSTANT_LIVE_GAME_SCORE_BASE_PROB 0.15
/** Base probability to score with a free kick. */
#define CONSTANT_LIVE_GAME_SCORE_FREE_KICK 0.35
/** Base probability to score with a penalty. */
#define CONSTANT_LIVE_GAME_SCORE_PENALTY 0.8
/** Influence of the attacker/goalie skills on the probability to score. */
#define CONSTANT_LIVE_GAME_SCORE_DUEL_EXPONENT 0.3
/** Influence of the team attacking/defending values on the probability to score. */
#define CONSTANT_LIVE_GAME_SCORE_TEAM_EXPONENT 0.7

/** The smaller this number, the smaller the probability of
    many stopping minutes after 90 or 45 minutes. */
#define CONSTANT_LIVE_GAME_BREAK_BASE 0.6
/** The bigger this number, the faster the probability of
    yet another additional minute after the 45th minute decays. */
#define CONSTANT_LIVE_GAME_45_BREAK_EXPONENT_FACTOR 1.3
/** The bigger this number, the faster the probability of
    yet another additional minute after the 90th minute decays. */
#define CONSTANT_LIVE_GAME_90_BREAK_EXPONENT_FACTOR 0.7
/** The probability that the team that shot on the goal stays
    in possession after a post or cross-bar hit. */
#define CONSTANT_LIVE_GAME_POSSESSION_AFTER_POST 0.3

/** Probability that there is a passing event after a special event.
    @see live_game_event_general() */
#define CONSTANT_LIVE_GAME_GENERAL_EVENT_SECOND_PLAYER 0.5

/** Constants for stadium events. */
#define CONSTANT_LIVE_GAME_STADIUM_EVENT_EXPONENT (0.1 * CONSTANT_LIVE_GAME_EVENT_GENERAL)
#define CONSTANT_LIVE_GAME_STADIUM_EVENT_FIRE 0.2
#define CONSTANT_LIVE_GAME_STADIUM_EVENT_RIOTS 0.5
#define CONSTANT_LIVE_GAME_STADIUM_EVENT_BREAKDOWN 1.0

/** Foul probabilities. */
#define CONSTANT_LIVE_GAME_FOUL (20.0 / 90.0 * CONSTANT_LIVE_GAME_EVENT_GENERAL)
#define CONSTANT_LIVE_GAME_FOUL_RED_INJURY 0.05
#define CONSTANT_LIVE_GAME_FOUL_RED 0.08
#define CONSTANT_LIVE_GAME_FOUL_YELLOW 0.23

/** Injury probabilities. */
#define CONSTANT_LIVE_GAME_INJURY (3.0 / 90 * CONSTANT_LIVE_GAME_EVENT_GENERAL)
#define CONSTANT_LIVE_GAME_INJURY_GOALIE_FACTOR 0.2
#define CONSTANT_LIVE_GAME_INJURY_IS_TEMP 0.7

/** Probability that a scoring chance is an own goal. */
#define CONSTANT_LIVE_GAME_SCORING_CHANCE_IS_OWN_GOAL 0.01

/** Probability of a free kick after a foul. */
#define CONSTANT_LIVE_GAME_FREE_KICK_PROB 0.15

/** Probability of a penalty after a foul. */
#define CONSTANT_LIVE_GAME_PENALTY_PROB 0.05

/** Probability that a scoring chance is a header. */
#define CONSTANT_LIVE_GAME_SCORING_CHANCE_IS_HEADER 0.35


/** The live game we calculate. */
LiveGame *match;
/** Whether the events are actually shown or not. */
gboolean show;

/** Convenience abbrev. */
#define units match->units
#define uni(i) g_array_index(units, LiveGameUnit, i)
#define last_unit uni(units->len - 1)
#define tm match->fix->teams
#define tm0 match->fix->teams[0]
#define tm1 match->fix->teams[1]

/** Calculate the result of a fixture using
    the live game variable.
    @param fix The fixture we calculate.
    @see live_game_create_unit(), live_game_evaluate_unit(),
    treeview_live_game_show_game_unit() */
void
live_game_calculate_fixture(Fixture *fix)
{
    live_game_reset(fix);

    do
    {
	live_game_create_unit();
	
	live_game_evaluate_unit(&last_unit);
	
	if(show)
	    game_gui_live_game_show_unit(&last_unit);
	
    }
    while(last_unit.event.type != LIVE_GAME_EVENT_END_MATCH);

    if(last_unit.event.type == LIVE_GAME_EVENT_END_MATCH)
	live_game_create_stats();
}

/** Create a game unit for the live game.
    @see #LiveGameUnit, #LiveGame, live_game_fill_new_unit() */
void
live_game_create_unit(void)
{
    LiveGameUnit new;

    if(debug)
	printf("live_game_create_unit\n");
    if(units->len == 0)
    {
	live_game_create_start_unit();
	return;
    }

    if(uni(units->len - 1).event.type == LIVE_GAME_EVENT_END_MATCH)
    {
	g_warning("live_game_create_unit: called after end of match.\n");
	return;
    }

    new.minute = live_game_get_minute();
    new.time = live_game_get_time(&last_unit);
    new.event.commentary = g_string_new("dummy commentary");
    new.event.values[LIVE_GAME_EVENT_VALUE_PLAYER] =
	new.event.values[LIVE_GAME_EVENT_VALUE_PLAYER2] = -1;    
    new.area = last_unit.area;

    if(query_live_game_event_is_break(new.minute, new.time))
    {
	new.event.type = live_game_get_break();
	new.possession = last_unit.possession;
	live_game_generate_commentary(&new);
	g_array_append_val(units, new);
	return;
    }
    else if(last_unit.time == LIVE_GAME_UNIT_TIME_PENALTIES)
	new.event.type = LIVE_GAME_EVENT_PENALTY;
    else
	live_game_fill_new_unit(&new);

    g_array_append_val(units, new);
}

/** Fill in a new unit depending on the team values and the constants from above.
    @param new The unit to fill in. */
void
live_game_fill_new_unit(LiveGameUnit *new)
{
    LiveGameUnit *old = &last_unit;
    gfloat rndom = math_rnd(0, 1);
    gfloat stadium_event = 
	1 - powf((gfloat)tm0->stadium.safety / 100,
		 CONSTANT_LIVE_GAME_STADIUM_EVENT_EXPONENT);
    gfloat possession_change, scoring_chance = 0;

    if(debug)
	printf("live_game_fill_new_unit\n");
    possession_change = CONSTANT_LIVE_GAME_EVENT_GENERAL *
	CONSTANT_LIVE_GAME_POSSESSION_CHANGES /
	live_game_pit_teams(old, CONSTANT_LIVE_GAME_POSSESSION_TEAM_EXPONENT);	

    new->possession = old->possession;

    if(old->event.type == LIVE_GAME_EVENT_GENERAL)
	new->area = live_game_get_area(new);

    if(new->area == LIVE_GAME_UNIT_AREA_ATTACK)
	scoring_chance = CONSTANT_LIVE_GAME_SCORING_CHANCE *
	    live_game_pit_teams(new, CONSTANT_LIVE_GAME_SCORING_CHANCE_TEAM_EXPONENT);

    if(rndom < CONSTANT_LIVE_GAME_FOUL)
	new->event.type = LIVE_GAME_EVENT_FOUL;
    else if(rndom < CONSTANT_LIVE_GAME_FOUL +
	    CONSTANT_LIVE_GAME_INJURY)
	new->event.type = LIVE_GAME_EVENT_INJURY;
    else if(rndom < CONSTANT_LIVE_GAME_FOUL +
	    CONSTANT_LIVE_GAME_INJURY +
	    stadium_event && !match->stadium_event)
	new->event.type = LIVE_GAME_EVENT_STADIUM;
    else if(rndom < CONSTANT_LIVE_GAME_FOUL +
	    CONSTANT_LIVE_GAME_INJURY +
	    stadium_event + possession_change)
    {
	new->event.type = LIVE_GAME_EVENT_LOST_POSSESSION;
	new->possession = !old->possession;
	if(new->area == LIVE_GAME_UNIT_AREA_ATTACK)
	    new->area = LIVE_GAME_UNIT_AREA_DEFEND;
	else if(new->area == LIVE_GAME_UNIT_AREA_DEFEND)
	    new->area = LIVE_GAME_UNIT_AREA_ATTACK;
    }
    else if(rndom < CONSTANT_LIVE_GAME_FOUL +
	    CONSTANT_LIVE_GAME_INJURY +
	    stadium_event + possession_change +
	    scoring_chance)
	new->event.type = LIVE_GAME_EVENT_SCORING_CHANCE;
    else
	new->event.type = LIVE_GAME_EVENT_GENERAL;
}

/** Create the first unit of a match. */
void
live_game_create_start_unit(void)
{
    LiveGameUnit new;

    if(debug)
	printf("live_game_create_start_unit\n");
    new.event.values[LIVE_GAME_EVENT_VALUE_PLAYER] =
	new.event.values[LIVE_GAME_EVENT_VALUE_PLAYER2] = -1;

    /*d*/
    new.event.commentary = g_string_new("Match's started.");

    new.minute = 1;
    new.time = LIVE_GAME_UNIT_TIME_FIRST_HALF;
    new.possession = math_rndi(0, 1);
    new.area = LIVE_GAME_UNIT_AREA_MIDFIELD;
    match->started_game = new.possession;

    new.event.type = LIVE_GAME_EVENT_START_MATCH;
    new.event.values[LIVE_GAME_EVENT_VALUE_TEAM] = new.possession;
	    
    g_array_append_val(units, new);
}

/** Evaluate a live game unit. This means we find out what happens
    after the unit, depending on its type.
    @param unit The unit we evaluate.
    @see The live_game_event* functions. */
void
live_game_evaluate_unit(LiveGameUnit *unit)
{
    gint type = unit->event.type;

    if(debug)
	printf("live_game_evaluate_unit type %d\n", type);
    if(type == LIVE_GAME_EVENT_FOUL)
	live_game_event_foul(TRUE);
    else if(type == LIVE_GAME_EVENT_LOST_POSSESSION)
	live_game_event_lost_possession();
    else if(type == LIVE_GAME_EVENT_INJURY)
	live_game_event_injury(-1, -1, FALSE);
    else if(type == LIVE_GAME_EVENT_STADIUM)
	live_game_event_stadium();
    else if(type == LIVE_GAME_EVENT_SCORING_CHANCE)
	live_game_event_scoring_chance();
    else if(type == LIVE_GAME_EVENT_PENALTY)
	live_game_event_penalty();
    else if(type == LIVE_GAME_EVENT_GENERAL)
	live_game_event_general(FALSE);
    else if(type == LIVE_GAME_EVENT_START_MATCH ||
	    type == LIVE_GAME_EVENT_HALF_TIME ||
	    type == LIVE_GAME_EVENT_EXTRA_TIME ||
	    type == LIVE_GAME_EVENT_PENALTIES)
    {
	live_game_generate_commentary(&last_unit);
	if(show)
	    game_gui_live_game_show_unit(&last_unit);

	if(type != LIVE_GAME_EVENT_PENALTIES)
	    live_game_event_general(TRUE);
    }
    else if(type != LIVE_GAME_EVENT_END_MATCH)
	g_warning("live_game_evaluate_unit: unknown event type %d\n",
		  type);
}

/** Calculate a foul event.
    @param general Whether to create a general event after
    showing this one. @see live_game_event_general() */
void
live_game_event_foul(gboolean general)
{
    gfloat rndom = math_rnd(0, 1);
    gint type;

    if(debug)
	printf("live_game_event_foul\n");
    if(uni(units->len - 2).event.type == LIVE_GAME_EVENT_GENERAL)
	last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER] =
	    uni(units->len - 2).event.values[LIVE_GAME_EVENT_VALUE_PLAYER];
    else
	last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER] =
	    game_get_player(tm[last_unit.possession],
			    last_unit.area, 0, -1, FALSE);

    last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER2] =
	game_get_player(tm[!last_unit.possession],
			last_unit.area, 0, -1, FALSE);

    if(rndom < CONSTANT_LIVE_GAME_FOUL_RED_INJURY)
	type = LIVE_GAME_EVENT_FOUL_RED_INJURY;
    else if(rndom < CONSTANT_LIVE_GAME_FOUL_RED)
	type = LIVE_GAME_EVENT_FOUL_RED;
    else if(rndom < CONSTANT_LIVE_GAME_FOUL_YELLOW)
	type = LIVE_GAME_EVENT_FOUL_YELLOW;
    else
	type = LIVE_GAME_EVENT_FOUL;

    last_unit.event.type = type;

    live_game_generate_commentary(&last_unit);
    if(show)
	game_gui_live_game_show_unit(&last_unit);

    if(type == LIVE_GAME_EVENT_FOUL_RED ||
       type == LIVE_GAME_EVENT_FOUL_RED_INJURY ||
       (type == LIVE_GAME_EVENT_FOUL_YELLOW &&
	query_live_game_second_yellow(!last_unit.possession,
				      last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER2])))
    {
	live_game_event_send_off(!last_unit.possession,
				 last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER2]);
	if(type == LIVE_GAME_EVENT_FOUL_RED_INJURY)
	    live_game_event_injury(last_unit.possession,
				   last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER], TRUE);
    }

    if(last_unit.area == LIVE_GAME_UNIT_AREA_ATTACK)
    {
	rndom = math_rnd(0, 1);
	if(rndom < CONSTANT_LIVE_GAME_PENALTY_PROB)
	    live_game_event_penalty();
	else if(rndom < CONSTANT_LIVE_GAME_FREE_KICK_PROB)
	    live_game_event_free_kick();
	else
	    live_game_event_general(TRUE);
    }
    else
	live_game_event_general(TRUE);
}

/** Calculate a lost possession event. */
void
live_game_event_lost_possession(void)
{
    if(debug)
	printf("live_game_event_lost_possession\n");
    last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER] =
	game_get_player(tm[last_unit.possession], 
			last_unit.area, 0, -1, TRUE);
	
    if(uni(units->len - 2).event.type == LIVE_GAME_EVENT_GENERAL)
	last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER2] =
	    uni(units->len - 2).event.values[LIVE_GAME_EVENT_VALUE_PLAYER];
    else
	last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER2] =
	    game_get_player(tm[!last_unit.possession], 
			    uni(units->len - 2).area, 0, -1, FALSE);

    if(debug)
	printf("## pls %d %d\n", 
	       last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER],
	       last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER2]);

    live_game_generate_commentary(&last_unit);
    if(show)
	game_gui_live_game_show_unit(&last_unit);

    live_game_event_general(TRUE);
}

/** Calculate an injury event. 
    @param team The team the player is from.
    @param player The player that's injured, or -1 if we have to 
    choose a random one.
    @param create_new Whether to put the event into a new unit instead of
    the last one. */
void
live_game_event_injury(gint team, gint player, gboolean create_new)
{
    LiveGameUnit new;

    if(debug)
	printf("live_game_event_injury\n");
    if(create_new)
    {
	new = last_unit;
	new.event.commentary = g_string_new("injury");
	g_array_append_val(units, new);
	last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER] =
	    player;
	last_unit.event.values[LIVE_GAME_EVENT_VALUE_TEAM] =
	    team;
    }
    else
	live_game_event_injury_get_player();
    
    last_unit.minute = -1;
    last_unit.event.type = LIVE_GAME_EVENT_INJURY;

    if(math_rnd(0, 1) < CONSTANT_LIVE_GAME_INJURY_IS_TEMP)
	last_unit.event.type = LIVE_GAME_EVENT_TEMP_INJURY;

    live_game_generate_commentary(&last_unit);
    if(show)
	game_gui_live_game_show_unit(&last_unit);

    /*d*/
    live_game_event_general(TRUE);
}

/** Calculate a stadium event. */
void
live_game_event_stadium(void)
{
    gfloat rndom = math_rnd(0, 1);

    if(debug)
	printf("live_game_event_stadium\n");
    if(rndom < CONSTANT_LIVE_GAME_STADIUM_EVENT_FIRE)
	last_unit.event.type = LIVE_GAME_EVENT_STADIUM_FIRE;
    else if(rndom < CONSTANT_LIVE_GAME_STADIUM_EVENT_RIOTS)
	last_unit.event.type = LIVE_GAME_EVENT_STADIUM_RIOTS;
    else if(rndom < CONSTANT_LIVE_GAME_STADIUM_EVENT_BREAKDOWN)
	last_unit.event.type = LIVE_GAME_EVENT_STADIUM_BREAKDOWN;

    live_game_generate_commentary(&last_unit);
    if(show)
	game_gui_live_game_show_unit(&last_unit);

    live_game_event_general(TRUE);
}

/** Calculate a scoring chance event. */
void
live_game_event_scoring_chance(void)
{
    if(debug)
	printf("live_game_event_scoring_chance\n");
    if(math_rnd(0, 1) < CONSTANT_LIVE_GAME_SCORING_CHANCE_IS_OWN_GOAL)
    {
	last_unit.event.type = LIVE_GAME_EVENT_OWN_GOAL;
	last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER] =
	    game_get_player(tm[!last_unit.possession], last_unit.area, 0, -1, FALSE);
    }
    else
    {
	if(uni(units->len - 2).event.values[LIVE_GAME_EVENT_VALUE_PLAYER] != -1 &&
	   math_rnd(0, 1) < CONSTANT_LIVE_GAME_PLAYER_IN_POSS_SHOOTS)
	    last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER] =
		uni(units->len - 2).event.values[LIVE_GAME_EVENT_VALUE_PLAYER];
	else
	{
	    if(uni(units->len - 2).event.values[LIVE_GAME_EVENT_VALUE_PLAYER] != -1)
	    {
		last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER] =
		    game_get_player(tm[last_unit.possession], last_unit.area, 0, 
				    uni(units->len - 2).event.values[LIVE_GAME_EVENT_VALUE_PLAYER],
				    TRUE);

		last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER2] =
		    uni(units->len - 2).event.values[LIVE_GAME_EVENT_VALUE_PLAYER];
	    }
	    else
	    {
		last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER] =
		    game_get_player(tm[last_unit.possession], last_unit.area, 0, -1, TRUE);

		last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER2] =
		    game_get_player(tm[last_unit.possession], last_unit.area, 0, 
				    last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER], TRUE);
	    }
	}
    }

    live_game_generate_commentary(&last_unit);
    if(show)
	game_gui_live_game_show_unit(&last_unit);

    live_game_event_duel();
}

/** Calculate a penalty event. */
void
live_game_event_penalty(void)
{
    LiveGameUnit new;

    if(debug)
	printf("live_game_event_penalty\n");
    new = last_unit;
    new.minute = -1;
    new.event.type = LIVE_GAME_EVENT_PENALTY;
    new.event.commentary = g_string_new("penalty");

    g_array_append_val(units, new);

    if(last_unit.time == LIVE_GAME_UNIT_TIME_PENALTIES)
    {
	if(uni(units->len - 2).event.type == LIVE_GAME_EVENT_PENALTIES)
	{
	    last_unit.event.values[LIVE_GAME_EVENT_VALUE_TEAM] =
		math_rndi(0, 1);
	    last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER] =
		game_get_player(tm[last_unit.event.values[LIVE_GAME_EVENT_VALUE_TEAM]],
				GAME_PLAYER_TYPE_PENALTY, 0, -1, FALSE);
	}
	else if(uni(units->len - 4).event.type == LIVE_GAME_EVENT_PENALTIES)
	{
	    last_unit.event.values[LIVE_GAME_EVENT_VALUE_TEAM] =
		!uni(units->len - 3).event.values[LIVE_GAME_EVENT_VALUE_TEAM];
	    last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER] =
		game_get_player(tm[last_unit.event.values[LIVE_GAME_EVENT_VALUE_TEAM]],
				GAME_PLAYER_TYPE_PENALTY, 0, -1, FALSE);
	}
	else
	{
	    last_unit.event.values[LIVE_GAME_EVENT_VALUE_TEAM] =
		!uni(units->len - 2).event.values[LIVE_GAME_EVENT_VALUE_TEAM];
	    last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER] =
		game_get_player(tm[last_unit.event.values[LIVE_GAME_EVENT_VALUE_TEAM]],
				GAME_PLAYER_TYPE_PENALTY,
				-uni(units->len - 4).event.values[LIVE_GAME_EVENT_VALUE_PLAYER], -1, FALSE);
	}
    }
    else
    {
	last_unit.event.values[LIVE_GAME_EVENT_VALUE_TEAM] =
	    last_unit.possession;

	if(my_team == tm[last_unit.possession] &&
	   options[OPT_PENALTY_SHOOTER] != -1)
	    last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER] =
		options[OPT_PENALTY_SHOOTER];
	else
	    last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER] =
		game_get_player(tm[last_unit.possession], GAME_PLAYER_TYPE_PENALTY, 0, -1, FALSE);
    }

    live_game_generate_commentary(&last_unit);
    if(show)
	game_gui_live_game_show_unit(&last_unit);

    live_game_event_duel();
}

/** Calculate a general event. 
    @param create_new Whether we create a new unit for the event. */
void
live_game_event_general(gboolean create_new)
{
    LiveGameUnit new;

    if(debug)
	printf("live_game_event_general\n");
    if(create_new)
    {
	new.minute = live_game_get_minute();
	new.time = last_unit.time;

	if(last_unit.event.type == LIVE_GAME_EVENT_GENERAL ||
	   last_unit.event.type == LIVE_GAME_EVENT_START_MATCH ||
	   last_unit.event.type == LIVE_GAME_EVENT_LOST_POSSESSION ||
	   last_unit.event.type == LIVE_GAME_EVENT_FOUL ||
	   last_unit.event.type == LIVE_GAME_EVENT_FOUL_YELLOW ||
	   last_unit.event.type == LIVE_GAME_EVENT_SEND_OFF ||
	   last_unit.event.type == LIVE_GAME_EVENT_INJURY ||
	   last_unit.event.type == LIVE_GAME_EVENT_TEMP_INJURY ||
	   last_unit.event.type == LIVE_GAME_EVENT_STADIUM ||
	   last_unit.event.type == LIVE_GAME_EVENT_STADIUM_BREAKDOWN ||
	   last_unit.event.type == LIVE_GAME_EVENT_STADIUM_FIRE ||
	   last_unit.event.type == LIVE_GAME_EVENT_STADIUM_RIOTS ||
	   ((last_unit.event.type == LIVE_GAME_EVENT_POST ||
	     last_unit.event.type == LIVE_GAME_EVENT_CROSS_BAR) &&
	    math_rnd(0, 1) < CONSTANT_LIVE_GAME_POSSESSION_AFTER_POST))
	{
	    if(debug)
		printf("##### last type: %d\n", last_unit.event.type);
	    new.possession = last_unit.possession;
	    new.area = (last_unit.event.type == LIVE_GAME_EVENT_GENERAL) ?
		live_game_get_area(&last_unit) : last_unit.area;
	}
	else if(last_unit.event.type == LIVE_GAME_EVENT_GOAL ||
		last_unit.event.type == LIVE_GAME_EVENT_OWN_GOAL ||
		last_unit.event.type == LIVE_GAME_EVENT_MISSED ||
		last_unit.event.type == LIVE_GAME_EVENT_SAVE ||
		last_unit.event.type == LIVE_GAME_EVENT_POST ||
		last_unit.event.type == LIVE_GAME_EVENT_CROSS_BAR)
	{
	    new.possession = !last_unit.possession;
	    if(last_unit.event.type == LIVE_GAME_EVENT_GOAL ||
	       last_unit.event.type == LIVE_GAME_EVENT_OWN_GOAL)
		new.area = LIVE_GAME_UNIT_AREA_MIDFIELD;
	    else
		new.area = LIVE_GAME_UNIT_AREA_DEFEND;
	}
	else if(last_unit.event.type == LIVE_GAME_EVENT_HALF_TIME)
	{
	    new.possession = !match->started_game;
	    new.time = LIVE_GAME_UNIT_TIME_SECOND_HALF;
	    new.area = LIVE_GAME_UNIT_AREA_MIDFIELD;
	}
	else if(last_unit.event.type == LIVE_GAME_EVENT_EXTRA_TIME)
	{
	    new.possession = math_rndi(0, 1);
	    new.time = LIVE_GAME_UNIT_TIME_EXTRA_TIME;
	    new.area = LIVE_GAME_UNIT_AREA_MIDFIELD;
	}
	else
	    g_warning("live_game_event_general: unknown event type: %d\n",
		      last_unit.event.type);
	
	new.event.type = LIVE_GAME_EVENT_GENERAL;
	new.event.commentary = g_string_new("general");	

	g_array_append_val(units, new);
    }

    live_game_event_general_get_players();

    if(debug)
	printf("+++ general %d %d\n",
	       last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER],
	       last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER2]);

    live_game_generate_commentary(&last_unit);
}

/** Fill in the players values in a general unit. */
void
live_game_event_general_get_players(void)
{
    gint *pl1 = &last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER];
    gint *pl2 = &last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER2];
    gint old_pl1 = 
	uni(units->len - 2).event.values[LIVE_GAME_EVENT_VALUE_PLAYER],
	old_pl2 = 
	uni(units->len - 2).event.values[LIVE_GAME_EVENT_VALUE_PLAYER2];
    gint type = uni(units->len - 2).event.type;
	
    if(debug)
	printf("live_game_event_general_get_players\n");
    *pl1 = *pl2 = -1;

    if(type == LIVE_GAME_EVENT_LOST_POSSESSION)
    {
	*pl1 = old_pl2;
	*pl2 = game_get_player(tm[last_unit.possession], 
			       last_unit.area, 0, *pl1,
			       TRUE);
    }
    else if(type != LIVE_GAME_EVENT_GENERAL)
    {
	*pl1 = game_get_player(tm[last_unit.possession],
			       last_unit.area, 0, -1, TRUE);
	if(math_rnd(0, 1) < CONSTANT_LIVE_GAME_GENERAL_EVENT_SECOND_PLAYER)
	    *pl2 = game_get_player(tm[last_unit.possession],
				   last_unit.area, 0, *pl1, TRUE);
    }
    else
    {
	*pl2 = old_pl1;
	*pl1 = game_get_player(tm[last_unit.possession],
			       last_unit.area, 0, *pl2, TRUE);
    }
}

/** Calculate a free kick event. */
void
live_game_event_free_kick(void)
{
    LiveGameUnit new = last_unit;

    if(debug)
	printf("live_game_event_free_kick\n");
    new.event.values[LIVE_GAME_EVENT_VALUE_PLAYER] =
	new.event.values[LIVE_GAME_EVENT_VALUE_PLAYER2] = -1;

    new.minute = -1;

    new.event.type = LIVE_GAME_EVENT_FREE_KICK;
    new.event.values[LIVE_GAME_EVENT_VALUE_TEAM] =
	new.possession;
    new.event.commentary = g_string_new("freekick");
    if(tm[new.possession] == my_team &&
       options[OPT_PENALTY_SHOOTER] != -1)
	new.event.values[LIVE_GAME_EVENT_VALUE_PLAYER] =
	    options[OPT_PENALTY_SHOOTER];
    else
	new.event.values[LIVE_GAME_EVENT_VALUE_PLAYER] =
	    game_get_player(tm[new.possession], new.area, 0, -1, TRUE);

    g_array_append_val(units, new);

    live_game_generate_commentary(&last_unit);
    if(show)
	game_gui_live_game_show_unit(&last_unit);

    live_game_event_duel();
}

/** Calculate a send-off event. */
void
live_game_event_send_off(gint team, gint player)
{
    LiveGameUnit new = last_unit;

    if(debug)
	printf("live_game_event_send_off\n");
    new.event.values[LIVE_GAME_EVENT_VALUE_PLAYER] =
	new.event.values[LIVE_GAME_EVENT_VALUE_PLAYER2] = -1;

    new.minute = -1;

    new.event.type = LIVE_GAME_EVENT_SEND_OFF;
    new.event.values[LIVE_GAME_EVENT_VALUE_PLAYER] = player;
    new.event.values[LIVE_GAME_EVENT_VALUE_TEAM] = team;
    new.event.commentary = g_string_new("send off");

    g_array_append_val(units, new);

    live_game_generate_commentary(&last_unit);
    if(show)
	game_gui_live_game_show_unit(&last_unit);
}


/** Calculate whether a player who tries to score succeeds. */
void
live_game_event_duel(void)
{
    gfloat rndom = math_rnd(0, 1);
    gfloat scoring_prob;
    gfloat duel_factor;
    LiveGameUnit new = last_unit;
    Player *attacker, *goalie;
    gint res_idx1, res_idx2;

    if(debug)
	printf("live_game_event_duel\n");
    new.event.values[LIVE_GAME_EVENT_VALUE_PLAYER] =
	new.event.values[LIVE_GAME_EVENT_VALUE_PLAYER2] = -1;

    new.minute = -1;

    new.event.values[LIVE_GAME_EVENT_VALUE_PLAYER] =
	last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER];

    new.event.commentary = g_string_new("duel");
	
    attacker = player_of(tm[new.possession],
			 new.event.values[LIVE_GAME_EVENT_VALUE_PLAYER]);
    goalie = player_of(tm[!new.possession], 0);
    duel_factor = (((gfloat)attacker->cskill * powf((gfloat)attacker->fitness / 100, 
						    CONSTANT_GAME_PLAYER_FITNESS_EXPONENT)) /
		   ((gfloat)goalie->cskill * powf((gfloat)goalie->fitness / 100, 
						  CONSTANT_GAME_PLAYER_FITNESS_EXPONENT)));
    res_idx1 = new.possession;
    if(new.time == LIVE_GAME_UNIT_TIME_PENALTIES)
	res_idx2 = 2;
    else if(new.time == LIVE_GAME_UNIT_TIME_EXTRA_TIME)
	res_idx2 = 1;
    else
	res_idx2 = 0;

    if(last_unit.event.type == LIVE_GAME_EVENT_PENALTY)
	scoring_prob = CONSTANT_LIVE_GAME_SCORE_PENALTY * duel_factor;
    else if(last_unit.event.type == LIVE_GAME_EVENT_FREE_KICK)
	scoring_prob = CONSTANT_LIVE_GAME_SCORE_FREE_KICK * duel_factor;
    else
	scoring_prob = CONSTANT_LIVE_GAME_SCORE_BASE_PROB * 
	    powf(duel_factor, CONSTANT_LIVE_GAME_SCORE_DUEL_EXPONENT) *
	    powf(match->team_values[new.possession][GAME_TEAM_VALUE_ATTACK] /
		 match->team_values[new.possession][GAME_TEAM_VALUE_DEFEND],
		 CONSTANT_LIVE_GAME_SCORE_TEAM_EXPONENT);
    
    if(rndom < scoring_prob)
    {
	new.event.type = LIVE_GAME_EVENT_GOAL;
	match->fix->result[res_idx1][res_idx2]++;
    }
    else
	new.event.type = math_gauss_disti(LIVE_GAME_EVENT_POST, LIVE_GAME_EVENT_CROSS_BAR);

    g_array_append_val(units, new);

    live_game_generate_commentary(&last_unit);
    if(show)
	game_gui_live_game_show_unit(&last_unit);

    if(last_unit.time != LIVE_GAME_UNIT_TIME_PENALTIES)
	live_game_event_general(TRUE);
}

/** Find out whether the specified player already has a yellow card
    in this game.
    @param team The team index, 0 or 1.
    @param player The player index.
    @return TRUE or FALSE. */
gboolean
query_live_game_second_yellow(gint team, gint player)
{
    gint i;

    for(i=0;i<units->len - 1;i++)
	if(uni(i).event.type == LIVE_GAME_EVENT_FOUL_YELLOW &&
	   uni(i).possession != team &&
	   uni(i).event.values[LIVE_GAME_EVENT_VALUE_PLAYER2] == player)
	    return TRUE;

    return FALSE;
}
/** Find out whether there should be a half-time break 
    or extra time break or so. 
    @param minute The minute of the #LiveGameUnit we want to find
    the break event for.
    @param time The #LiveGameUnitTime of the #LiveGameUnit we want to find
    the break event for.
    @return TRUE if we have a break, FALSE otherwise. */
gboolean
query_live_game_event_is_break(gint minute, gint time)
{
    gfloat rndom;

    if(time == LIVE_GAME_UNIT_TIME_EXTRA_TIME && minute == 120)
	return TRUE;

    if(time == LIVE_GAME_UNIT_TIME_PENALTIES)
	return query_live_game_penalties_over();

    rndom = math_rnd(0, 1);

    if(time == LIVE_GAME_UNIT_TIME_FIRST_HALF)
	return (minute >= 45 && rndom > 
		powf(CONSTANT_LIVE_GAME_BREAK_BASE,
		     (gfloat)(minute - 44) * CONSTANT_LIVE_GAME_45_BREAK_EXPONENT_FACTOR));
    else
	return (minute >= 90 && rndom > 
		powf(CONSTANT_LIVE_GAME_BREAK_BASE,
		     (gfloat)(minute - 89) * CONSTANT_LIVE_GAME_90_BREAK_EXPONENT_FACTOR));
}

/** Find out whether the final result of the penalties is
    already reached. 
    @return TRUE if the penalties are over,
    FALSE otherwise. */
gboolean
query_live_game_penalties_over(void)
{
    gint i;
    gint pen_attempted[2] = {0, 0};

    for(i=units->len - 1; i > 0; i--)
	if(uni(i).time == LIVE_GAME_UNIT_TIME_PENALTIES)
	{
	    if(uni(i).event.type == LIVE_GAME_EVENT_PENALTY)
		pen_attempted[uni(i).event.values[LIVE_GAME_EVENT_VALUE_TEAM]]++;
	}
	else
	    break;

    if(pen_attempted[0] + pen_attempted[1] >= 10)
	return (match->fix->result[0][2] != match->fix->result[1][2]);

    return
	(match->fix->result[0][2] - match->fix->result[1][2] > 5 - pen_attempted[1] ||
	 match->fix->result[1][2] - match->fix->result[0][2] > 5 - pen_attempted[0]);
}

/** Return a #LiveGameUnitTime depending on the time
    of the last unit. 
    @return A new #LiveGameUnitTime. */
gint
live_game_get_break(void)
{
    gint type;
    
    if(last_unit.time == LIVE_GAME_UNIT_TIME_FIRST_HALF)
	type = LIVE_GAME_EVENT_HALF_TIME;
    else if(last_unit.time == LIVE_GAME_UNIT_TIME_SECOND_HALF)
    {
	if(query_fixture_is_draw(match->fix))
	    type = LIVE_GAME_EVENT_EXTRA_TIME;
	else
	    type = LIVE_GAME_EVENT_END_MATCH;
    }
    else if(last_unit.time == LIVE_GAME_UNIT_TIME_EXTRA_TIME)
    {
	if(query_fixture_is_draw(match->fix))
	    type = LIVE_GAME_EVENT_PENALTIES;
	else
	    type = LIVE_GAME_EVENT_END_MATCH;
    }
    else
	type = LIVE_GAME_EVENT_END_MATCH;

    return type;
}

/** Get the time for the unit depending of time and
    event of the last one. 
    @param unit The unit before the one we create. 
    @return A #LiveGameUnitTime */
gint
live_game_get_time(const LiveGameUnit *unit)
{
    gint time;

    if(unit->event.type == LIVE_GAME_EVENT_HALF_TIME)
	time = LIVE_GAME_UNIT_TIME_SECOND_HALF;
    else if(unit->event.type == LIVE_GAME_EVENT_EXTRA_TIME)
	time = LIVE_GAME_UNIT_TIME_EXTRA_TIME;
    else if(unit->event.type == LIVE_GAME_EVENT_PENALTIES)
	time = LIVE_GAME_UNIT_TIME_PENALTIES;
    else
	time = unit->time;

    return time;
}

/** Return the minute for the next game unit. This depends on
    #CONSTANT_LIVE_GAME_UNITS_PER_MINUTE.
    @return A new minute for a LiveGameUnit. */
gint
live_game_get_minute(void)
{
    gint i;
    
    if(last_unit.event.type == LIVE_GAME_EVENT_HALF_TIME)
	return 46;
    else if(last_unit.event.type == LIVE_GAME_EVENT_EXTRA_TIME)
	return 91;
    else if(last_unit.event.type == LIVE_GAME_EVENT_PENALTIES ||
	    last_unit.time == LIVE_GAME_UNIT_TIME_PENALTIES)
	return 120;

    for(i=units->len - 1; i>=0; i--)
	if(uni(i).minute != -1)
	    return uni(i).minute + 1;

    return -1;
}

/** Return the minute of the unit (ie. look up the last unit
    with a 'normal' minute value if minute = -1).
    @param unit The unit we examine.
    @return A minute between 1 and 120. */
gint
live_game_unit_get_minute(const LiveGameUnit *unit)
{
    gint i, j;

    for(i=units->len -1; i >= 0; i--)
	if(&uni(i) == unit)
	    break;

    if(i == -1)
	g_warning("live_game_unit_get_minute: reached end of units array.\n");
    else
	for(j=i;j>=0;j--)
	    if(uni(j).minute != -1)
		return uni(j).minute;

    return -1;
}

/** Free the live game variable before we begin a new live game.
    @param fix The fixture we'll show. */
void
live_game_reset(Fixture *fix)
{
    match = query_my_team_involved(fix) ? &live_game : &live_game_temp;
    show = (query_my_team_involved(fix) && options[OPT_BOOL_SHOW_LIVE]);

    free_live_game(match);
    units = g_array_new(FALSE, FALSE, sizeof(LiveGameUnit));

    match->fix = fix;
    match->subs_left[0] = match->subs_left[1] = 3;
    match->stadium_event = FALSE;
    
    if(fix->home_advantage)
	match->home_advantage = 
	    math_rnd(CONSTANT_GAME_HOME_ADVANTAGE_LOWER,
		     CONSTANT_GAME_HOME_ADVANTAGE_UPPER);

    game_get_values(match->fix, match->team_values,
		    match->home_advantage);
}

/** Generate commentary for the live game event in the unit. 
    @param unit The unit we comment. */
void
live_game_generate_commentary(LiveGameUnit *unit)
{
    GString *commentary = unit->event.commentary;

    switch(unit->event.type)
    {
	case LIVE_GAME_EVENT_GENERAL:
	    if(unit->event.values[LIVE_GAME_EVENT_VALUE_PLAYER] != -1 &&
	       unit->event.values[LIVE_GAME_EVENT_VALUE_PLAYER2] != -1)
		g_string_printf(commentary, "general, %s passes to %s",
				player_of_id(tm[unit->possession], 
					     unit->event.values[LIVE_GAME_EVENT_VALUE_PLAYER2])->name->str,
				player_of_id(tm[unit->possession], 
					     unit->event.values[LIVE_GAME_EVENT_VALUE_PLAYER])->name->str);
	    else if(unit->event.values[LIVE_GAME_EVENT_VALUE_PLAYER] != -1)
		g_string_printf(commentary, "%s has the ball.",
				player_of_id(tm[unit->possession], 
					     unit->event.values[LIVE_GAME_EVENT_VALUE_PLAYER])->name->str);
	    else
		g_string_printf(commentary, "match's getting boring");
	    break;
	case LIVE_GAME_EVENT_START_MATCH:
	    g_string_printf(commentary, "Match started.");
	    break;
	case LIVE_GAME_EVENT_HALF_TIME:
	    g_string_printf(commentary, "Half time.");
	    break;
	case LIVE_GAME_EVENT_EXTRA_TIME:
	    g_string_printf(commentary, "Extra time.");
	    break;
	case LIVE_GAME_EVENT_END_MATCH:
	    g_string_printf(commentary, "Match is over!");
	    break;
	case LIVE_GAME_EVENT_LOST_POSSESSION:
	    g_string_printf(commentary, "%s loses ball to %s.",
			    player_of_id(tm[!unit->possession], 
					 unit->event.values[LIVE_GAME_EVENT_VALUE_PLAYER2])->name->str,
			    player_of_id(tm[unit->possession], 
					 unit->event.values[LIVE_GAME_EVENT_VALUE_PLAYER])->name->str);
	    break;
	case LIVE_GAME_EVENT_SCORING_CHANCE:
	    g_string_printf(commentary, "scoring chance by %s",
			    player_of_id(tm[unit->possession], 
					 unit->event.values[LIVE_GAME_EVENT_VALUE_PLAYER])->name->str);
	    break;
	case LIVE_GAME_EVENT_PENALTY:
	    g_string_printf(commentary, "%s shoots penalty",
			    player_of_id(tm[unit->possession], 
					 unit->event.values[LIVE_GAME_EVENT_VALUE_PLAYER])->name->str);
	    break;
	case LIVE_GAME_EVENT_FREE_KICK:
	    g_string_printf(commentary, "%s shoots free kick",
			    player_of_id(tm[unit->possession], 
					 unit->event.values[LIVE_GAME_EVENT_VALUE_PLAYER])->name->str);
	    break;
	case LIVE_GAME_EVENT_GOAL:
	    g_string_printf(commentary, "Goal!");
	    break;
	case LIVE_GAME_EVENT_OWN_GOAL:
	    g_string_printf(commentary, "Oh no! %s scored an own goal!",
			    player_of_id(tm[!unit->possession], 
					 unit->event.values[LIVE_GAME_EVENT_VALUE_PLAYER])->name->str);
	    break;
	case LIVE_GAME_EVENT_MISSED:
	    g_string_printf(commentary, "Missed!");
	    break;
	case LIVE_GAME_EVENT_SAVE:
	    g_string_printf(commentary, "Save!");
	    break;
	case LIVE_GAME_EVENT_POST:
	    g_string_printf(commentary, "Post!");
	    break;
	case LIVE_GAME_EVENT_CROSS_BAR:
	    g_string_printf(commentary, "Cross bar!");
	    break;
	case LIVE_GAME_EVENT_FOUL:
	    g_string_printf(commentary, "%s fouls %s.",
			    player_of_id(tm[!unit->possession], 
					 unit->event.values[LIVE_GAME_EVENT_VALUE_PLAYER2])->name->str,
			    player_of_id(tm[unit->possession], 
					 unit->event.values[LIVE_GAME_EVENT_VALUE_PLAYER])->name->str);
	    break;
	case LIVE_GAME_EVENT_FOUL_RED:
	    g_string_printf(commentary, "%s fouls %s; he gets a red card!",
			    player_of_id(tm[!unit->possession], 
					 unit->event.values[LIVE_GAME_EVENT_VALUE_PLAYER2])->name->str,
			    player_of_id(tm[unit->possession], 
					 unit->event.values[LIVE_GAME_EVENT_VALUE_PLAYER])->name->str);
	    break;
	case LIVE_GAME_EVENT_FOUL_RED_INJURY:
	    g_string_printf(commentary, "%s fouls %s; he gets a red card and %s is injured!",
			    player_of_id(tm[!unit->possession], 
					 unit->event.values[LIVE_GAME_EVENT_VALUE_PLAYER2])->name->str,
			    player_of_id(tm[unit->possession], 
					 unit->event.values[LIVE_GAME_EVENT_VALUE_PLAYER])->name->str,
			    player_of_id(tm[unit->possession], 
					 unit->event.values[LIVE_GAME_EVENT_VALUE_PLAYER])->name->str);
	    break;
	case LIVE_GAME_EVENT_FOUL_YELLOW:
	    g_string_printf(commentary, "%s fouls %s; he gets a yellow card.",
			    player_of_id(tm[!unit->possession], 
					 unit->event.values[LIVE_GAME_EVENT_VALUE_PLAYER2])->name->str,
			    player_of_id(tm[unit->possession], 
					 unit->event.values[LIVE_GAME_EVENT_VALUE_PLAYER])->name->str);
	    break;
	case LIVE_GAME_EVENT_SEND_OFF:
	    g_string_printf(commentary, "%s is sent off.",
			    player_of_id(tm[!unit->possession], 
					 unit->event.values[LIVE_GAME_EVENT_VALUE_PLAYER])->name->str);
	    break;
	case LIVE_GAME_EVENT_INJURY:
	    g_string_printf(commentary, "%s is injured.",
			    player_of_id(tm[unit->possession], 
					 unit->event.values[LIVE_GAME_EVENT_VALUE_PLAYER])->name->str);
	    break;
	case LIVE_GAME_EVENT_TEMP_INJURY:
	    g_string_printf(commentary, "%s is injured but he can continue to play.",
			    player_of_id(tm[unit->possession], 
					 unit->event.values[LIVE_GAME_EVENT_VALUE_PLAYER])->name->str);
	    break;
	case LIVE_GAME_EVENT_PENALTIES:
	    g_string_printf(commentary, "Penalty shoot-out!");
	    break;
	case LIVE_GAME_EVENT_STADIUM_BREAKDOWN:
	    g_string_printf(commentary, "There's a breakdown in the stadium");
	    break;
	case LIVE_GAME_EVENT_STADIUM_FIRE:
	    g_string_printf(commentary, "There's a fire in the stadium.");
	    break;
	case LIVE_GAME_EVENT_STADIUM_RIOTS:
	    g_string_printf(commentary, "There are riots in the stadium.");
	    break;
    }
}

/** Assemble some stats like ball possession or shots
    on goal. @see #LiveGameStats */
void
live_game_create_stats(void)
{
    gint i, possession[2] = {0, 0};
    LiveGameStats *stat = &match->stats;

    for(i=0;i<LIVE_GAME_STAT_VALUE_END;i++)
	stat->values[0][i] =
	    stat->values[1][i] = 0;

    stat->values[0][LIVE_GAME_STAT_VALUE_GOALS] = 
	math_sum_int_array(match->fix->result[0], 2);
    stat->values[1][LIVE_GAME_STAT_VALUE_GOALS] = 
	math_sum_int_array(match->fix->result[1], 2);

    for(i=0;i<units->len;i++)
    {
	if(uni(i).minute != -1)
	    possession[uni(i).possession]++;
	if(uni(i).event.type == LIVE_GAME_EVENT_SCORING_CHANCE ||
	   uni(i).event.type == LIVE_GAME_EVENT_FREE_KICK)
	    stat->values[uni(i).possession][LIVE_GAME_STAT_VALUE_SHOTS]++;
	else if(uni(i).event.type == LIVE_GAME_EVENT_PENALTY && 
		uni(i).time != LIVE_GAME_UNIT_TIME_PENALTIES)
	    stat->values[uni(i).possession][LIVE_GAME_STAT_VALUE_PENALTIES]++;
	else if(uni(i).event.type == LIVE_GAME_EVENT_INJURY ||
		uni(i).event.type == LIVE_GAME_EVENT_TEMP_INJURY)
	    stat->values[uni(i).possession][LIVE_GAME_STAT_VALUE_INJURIES]++;
	else if(uni(i).event.type == LIVE_GAME_EVENT_FOUL ||
		uni(i).event.type == LIVE_GAME_EVENT_FOUL_YELLOW ||
		uni(i).event.type == LIVE_GAME_EVENT_FOUL_RED ||
		uni(i).event.type == LIVE_GAME_EVENT_FOUL_RED_INJURY)
	{
	    stat->values[!uni(i).possession][LIVE_GAME_STAT_VALUE_FOULS]++;
	    if(uni(i).event.type == LIVE_GAME_EVENT_FOUL_YELLOW)
		stat->values[!uni(i).possession][LIVE_GAME_STAT_VALUE_CARDS]++;
	}
	else if(uni(i).event.type == LIVE_GAME_EVENT_SEND_OFF)
	    stat->values[!uni(i).possession][LIVE_GAME_STAT_VALUE_REDS]++;
    }

    stat->values[0][LIVE_GAME_STAT_VALUE_POSSESSION] = 
	(gint)rint((gfloat)possession[0] / (gfloat)(possession[0] + possession[1]) * 100);
    stat->values[1][LIVE_GAME_STAT_VALUE_POSSESSION] = 
	100 - stat->values[0][LIVE_GAME_STAT_VALUE_POSSESSION];

    printf("goals\t %d \t %d\n", stat->values[0][LIVE_GAME_STAT_VALUE_GOALS],
	   stat->values[1][LIVE_GAME_STAT_VALUE_GOALS]);
    printf("shots\t %d \t %d\n", stat->values[0][LIVE_GAME_STAT_VALUE_SHOTS],
	   stat->values[1][LIVE_GAME_STAT_VALUE_SHOTS]);
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

/** Calculate which area the ball is going to be in in
    the next unit.
    @param unit The previous unit.
    @return An area, defend, midfield or attack. */
gint
live_game_get_area(const LiveGameUnit *unit)
{
    /*d*/
    gint i;
    gint new_area = unit->area;
    gfloat rndom = math_rnd(0, 1);
    gfloat probs[4] =
	{CONSTANT_LIVE_GAME_AREA_DEF_MID *
	 live_game_pit_teams(unit, CONSTANT_LIVE_GAME_AREA_DEF_MID_TEAM_EXPONENT),
	 CONSTANT_LIVE_GAME_AREA_MID_ATT *
	 live_game_pit_teams(unit, CONSTANT_LIVE_GAME_AREA_MID_TEAM_EXPONENT),
	 CONSTANT_LIVE_GAME_AREA_MID_DEF /
	 live_game_pit_teams(unit, CONSTANT_LIVE_GAME_AREA_MID_TEAM_EXPONENT),
	 CONSTANT_LIVE_GAME_AREA_ATT_MID /
	 live_game_pit_teams(unit, CONSTANT_LIVE_GAME_AREA_ATT_MID_TEAM_EXPONENT)};

    if(unit->area == LIVE_GAME_UNIT_AREA_DEFEND && rndom < probs[0])
	new_area = LIVE_GAME_UNIT_AREA_MIDFIELD;
    else if(unit->area == LIVE_GAME_UNIT_AREA_MIDFIELD)
    {
	if(rndom < probs[1])
	    new_area = LIVE_GAME_UNIT_AREA_ATTACK;
	else if(rndom < probs[1] + probs[2])
	    new_area = LIVE_GAME_UNIT_AREA_DEFEND;
    }
    else
	if(rndom < probs[3])
	    new_area = LIVE_GAME_UNIT_AREA_MIDFIELD;

    if(debug)
	printf("**** new area %d\n", new_area);
    return new_area;
}

/** Return the team values factor weighted with the given exponent
    and depending on the pitch area.
    @param unit The unit we calculate the value for.
    @param exponent The weighting exponent. */
gfloat
live_game_pit_teams(const LiveGameUnit *unit, gfloat exponent)
{
    gfloat factor;

    if(unit->area == LIVE_GAME_UNIT_AREA_DEFEND)
	factor = powf(match->team_values[unit->possession][GAME_TEAM_VALUE_DEFEND] /
		      match->team_values[!unit->possession][GAME_TEAM_VALUE_ATTACK], exponent);
    else if(unit->area == LIVE_GAME_UNIT_AREA_MIDFIELD)
	factor = powf(match->team_values[unit->possession][GAME_TEAM_VALUE_MIDFIELD] /
		      match->team_values[!unit->possession][GAME_TEAM_VALUE_MIDFIELD], exponent);
    else
	factor = powf(match->team_values[unit->possession][GAME_TEAM_VALUE_ATTACK] /
		      match->team_values[!unit->possession][GAME_TEAM_VALUE_DEFEND], exponent);

    return factor;
}

/** Find a random player (influenced by fitness) who gets
    injured. */
void
live_game_event_injury_get_player(void)
{
    gint i, j;
    gfloat probs[22];
    gfloat rndom;
    
    for(j=0;j<2;j++)
    {
	probs[j * 11] = CONSTANT_LIVE_GAME_INJURY_GOALIE_FACTOR * 
	    (gfloat)(100 - player_of(tm[j], 0)->fitness) * 
	    (player_of(tm[j], 0)->cskill != 0);
	for(i=1;i<11;i++)
	    probs[i + j * 11] = probs[i + j * 11 - 1] + 
		(gfloat)(100 - player_of(tm[j], i)->fitness) * 
		(player_of(tm[j], i)->cskill != 0);
    }
	
    rndom = math_rnd(0, probs[21]);

    if(rndom < probs[0])
    {
	last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER] = 
	    player_of(tm[0], 0)->id;
	last_unit.event.values[LIVE_GAME_EVENT_VALUE_TEAM] = 0;
    }
    else
	for(i=1;i<22;i++)
	    if(probs[i - 1] <= rndom && rndom < probs[i])
	    {
		last_unit.event.values[LIVE_GAME_EVENT_VALUE_PLAYER] = 
		    player_of(tm[(i > 10)], i % 11)->id;
		last_unit.event.values[LIVE_GAME_EVENT_VALUE_TEAM] = (i > 10);
	    }
}
