/*
   live_game.c

   Bygfoot Football Manager -- a small and simple GTK2-based
   football management game.

   http://bygfoot.sourceforge.net

   Copyright (C) 2005  Gyözö Both (gyboth@bygfoot.com)

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "callbacks.h"
#include "fixture.h"
#include "free.h"
#include "game.h"
#include "game_gui.h"
#include "league.h"
#include "lg_commentary.h"
#include "live_game.h"
#include "main.h"
#include "maths.h"
#include "misc.h"
#include "misc_callback_func.h"
#include "option.h"
#include "player.h"
#include "strategy.h"
#include "support.h"
#include "team.h"
#include "treeview.h"
#include "user.h"
#include "variables.h"
#include "window.h"

/** The live game we calculate. */
#define match ((LiveGame*)statp)

/** Whether the events are actually shown or not. */
gboolean show;

/** Convenience abbrevs. */
#define unis match->units
#define uni(i) g_array_index(unis, LiveGameUnit, i)
#define last_unit uni(unis->len - 1)
#define tms match->fix->teams
#define tm0 match->fix->teams[0]
#define tm1 match->fix->teams[1]

/** Calculate the result of a fixture using
    the live game variable.
    @param fix The fixture we calculate.
*/
void
live_game_calculate_fixture(Fixture *fix)
{
    if(stat0 != STATUS_LIVE_GAME_PAUSE && 
       stat0 != STATUS_LIVE_GAME_CHANGE)
	live_game_initialize(fix);
    else
	stat0 = STATUS_SHOW_LIVE_GAME;

    game_get_values(match->fix, match->team_values,
		    match->home_advantage);

    if((debug > 80 && stat2 != -1) ||
       debug > 130)
	g_print("\n\nlive_game_calculate_fixture\n%04d %s %s %04d\n\n",
	       tm0->id, tm0->name, tm1->name, tm1->id);

    do
    {
	live_game_create_unit();
	live_game_evaluate_unit(&last_unit);
    }
    while(last_unit.event.type != LIVE_GAME_EVENT_END_MATCH &&
	  stat0 != STATUS_LIVE_GAME_PAUSE &&
	  stat0 != STATUS_LIVE_GAME_CHANGE);

    if(last_unit.event.type == LIVE_GAME_EVENT_END_MATCH)
    {
	if(stat2 != -1 || stat5 < -1000)
	    lg_commentary_free_tokens();
	game_post_match(fix);
    }
    else if(stat0 == STATUS_LIVE_GAME_CHANGE)
	live_game_resume();
}

/** Initialize a few things at the beginning of a live game. */
void
live_game_initialize(Fixture *fix)
{
    stat2 = fixture_user_team_involved(fix);

    statp= (stat2 != -1) ? 
	&usr(stat2).live_game : &live_game_temp;
    show = (stat2 != -1 && 
	    option_int("int_opt_user_show_live_game", 
		       &usr(stat2).options));

    live_game_reset(match, fix, TRUE);

    if(show)
    {
	cur_user = stat2;
	on_button_back_to_main_clicked(NULL, NULL);

	if(window.live == NULL)
	    window.live = window_create(WINDOW_LIVE);
	else
	    gtk_window_set_title(
		GTK_WINDOW(window.live),
		league_cup_get_name_string(((LiveGame*)statp)->fix->clid));
	window_live_set_up();
	game_gui_live_game_show_opponent();
    }

    game_initialize(fix);
    match->attendance = fix->attendance;

    if(stat2 != -1 || stat5 < -1000)
	lg_commentary_initialize(fix);
}

/** Create a game unit for the live game.
    @see #LiveGameUnit, #LiveGame, live_game_fill_new_unit() */
void
live_game_create_unit(void)
{
    LiveGameUnit new;

    if((debug > 100 && stat2 != -1) ||
       debug > 130)
	g_print("\t\tlive_game_create_unit\n");

    if(unis->len == 0)
    {
	live_game_create_start_unit();
	return;
    }

    if(uni(unis->len - 1).event.type == LIVE_GAME_EVENT_END_MATCH)
    {
	g_warning("live_game_create_unit: called after end of match.\n");
	return;
    }

    new.minute = live_game_get_minute();
    new.time = live_game_get_time(&last_unit);
    new.event.commentary = NULL;
    new.event.team =
	new.event.player =
	new.event.player2 = -1;
    new.area = last_unit.area;
    new.result[0] = last_unit.result[0];
    new.result[1] = last_unit.result[1];

    if(last_unit.event.type == LIVE_GAME_EVENT_HALF_TIME ||
       last_unit.event.type == LIVE_GAME_EVENT_EXTRA_TIME)
    {
	live_game_event_general(TRUE);
	return;
    }
    else if(query_live_game_event_is_break(new.minute, new.time))
    {
	new.event.type = live_game_get_break();
	new.possession = last_unit.possession;
	g_array_append_val(unis, new);
	return;
    }
    else if(new.time == LIVE_GAME_UNIT_TIME_PENALTIES)
	new.event.type = LIVE_GAME_EVENT_PENALTY;
    else
	live_game_fill_new_unit(&new);

    g_array_append_val(unis, new);
}

/** Fill in a new unit depending on the team values and the constants from above.
    @param new The unit to fill in. */
void
live_game_fill_new_unit(LiveGameUnit *new)
{
    LiveGameUnit *old = &last_unit;
    gfloat rndom = math_rnd(0, 1);
    gfloat stadium_event = 
	1 - powf(tm0->stadium.safety,
		 const_float("float_live_game_stadium_event_exponent"));
    gfloat possession_change, scoring_chance = 0, 
	injury_event_prob, foul_event_prob;

    if((debug > 100 && stat2 != -1) ||
       debug > 130)
	g_print("\t\tlive_game_fill_new_unit\n");

    possession_change = const_float("float_live_game_event_general") *
	const_float("float_live_game_possession_changes") /
	live_game_pit_teams(old, const_float("float_live_game_possession_team_exponent"));	

    injury_event_prob = const_float("float_live_game_injury") * 
	(1 + (const_float("float_player_boost_injury_effect") *
	      (tm0->boost != 0 || tm1->boost != 0)));

    foul_event_prob = const_float("float_live_game_foul") *
	(1 + (tm0->boost + tm1->boost) * const_float("float_team_boost_foul_factor"));

    new->possession = old->possession;

    if(old->event.type == LIVE_GAME_EVENT_GENERAL)
	new->area = live_game_get_area(new);

    if(new->area == LIVE_GAME_UNIT_AREA_ATTACK)
	scoring_chance = const_float("float_live_game_scoring_chance") *
	    live_game_pit_teams(new, const_float("float_live_game_scoring_chance_team_exponent"));

    if(rndom < foul_event_prob)
	new->event.type = LIVE_GAME_EVENT_FOUL;
    else if(rndom < foul_event_prob +
	    injury_event_prob)
	new->event.type = LIVE_GAME_EVENT_INJURY;
    else if(rndom < foul_event_prob +
	    injury_event_prob +
	    stadium_event && match->stadium_event == -1)
	new->event.type = LIVE_GAME_EVENT_STADIUM;
    else if(rndom < foul_event_prob +
	    injury_event_prob +
	    stadium_event + possession_change)
    {
	new->event.type = LIVE_GAME_EVENT_LOST_POSSESSION;
	new->possession = !old->possession;
	if(new->area == LIVE_GAME_UNIT_AREA_ATTACK)
	    new->area = LIVE_GAME_UNIT_AREA_DEFEND;
	else if(new->area == LIVE_GAME_UNIT_AREA_DEFEND)
	    new->area = LIVE_GAME_UNIT_AREA_ATTACK;

    }
    else if(rndom < foul_event_prob +
	    injury_event_prob +
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

    if((debug > 100 && stat2 != -1) ||
       debug > 130)
	g_print("\t\tlive_game_create_start_unit\n");
    new.event.player =
	new.event.player2 = -1;

    new.minute = 0;
    new.event.commentary = NULL;
    new.time = LIVE_GAME_UNIT_TIME_FIRST_HALF;
    new.possession = math_rndi(0, 1);
    new.area = LIVE_GAME_UNIT_AREA_MIDFIELD;
    match->started_game = new.possession;
    new.result[0] = new.result[1] = 0;

    new.event.type = LIVE_GAME_EVENT_START_MATCH;
    new.event.team = new.possession;
	    
    g_array_append_val(unis, new);
}

/** Evaluate a live game unit. This means we find out what happens
    after the unit, depending on its type.
    @param unit The unit we evaluate.
    @see The live_game_event* functions. */
void
live_game_evaluate_unit(LiveGameUnit *unit)
{
    gint type = unit->event.type;

    if((debug > 100 && stat2 != -1) ||
       debug > 130)
	g_print("\t\tlive_game_evaluate_unit\n");
    if(type == LIVE_GAME_EVENT_FOUL)
	live_game_event_foul();
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
    else if(type == LIVE_GAME_EVENT_START_MATCH)
	live_game_finish_unit();
    else if(type == LIVE_GAME_EVENT_HALF_TIME ||
	    type == LIVE_GAME_EVENT_EXTRA_TIME ||
	    type == LIVE_GAME_EVENT_PENALTIES ||
	    type == LIVE_GAME_EVENT_END_MATCH)
    {
	live_game_finish_unit();
	if(type != LIVE_GAME_EVENT_END_MATCH && show && 
	   option_int("int_opt_user_pause_break", &usr(stat2).options))
	    misc_callback_pause_live_game();
    }
    else if(type != LIVE_GAME_EVENT_END_MATCH)
	g_warning("live_game_evaluate_unit: unknown event type %d\n",
		  type);
}

/** Calculate a foul event. */
void
live_game_event_foul(void)
{
    gfloat rndom = math_rnd(0, 1);
    gint type, fouled_player, foul_player, foul_team;

    if((debug > 100 && stat2 != -1) ||
       debug > 130)
	g_print("\t\tlive_game_event_foul\n");
    if(math_rnd(0, 1) > const_float("float_live_game_foul_by_possession") *
       game_get_foul_possession_factor(
	   tms[last_unit.possession]->boost, tms[!last_unit.possession]->boost))
    {	
	foul_team = last_unit.event.team = !last_unit.possession;
	if(uni(unis->len - 2).event.type == LIVE_GAME_EVENT_GENERAL)
	    fouled_player = last_unit.event.player =
		uni(unis->len - 2).event.player;
	else
	    fouled_player = last_unit.event.player =
		game_get_player(tms[last_unit.possession],
				last_unit.area, 0, -1, FALSE);

	foul_player = last_unit.event.player2 =
	    game_get_player(tms[!last_unit.possession],
			    last_unit.area, 0, -1, FALSE);
    }
    else
    {
	foul_team = last_unit.event.team = last_unit.possession;
	fouled_player = last_unit.event.player = 
	    game_get_player(tms[!last_unit.possession],
			    last_unit.area, 0, -1, FALSE);
	foul_player = last_unit.event.player2 =
	    game_get_player(tms[last_unit.possession],
			    last_unit.area, 0, -1, FALSE);
    }

    if(rndom < const_float("float_live_game_foul_red_injury"))
	type = LIVE_GAME_EVENT_FOUL_RED_INJURY;
    else if(rndom < const_float("float_live_game_foul_red"))
	type = LIVE_GAME_EVENT_FOUL_RED;
    else if(rndom < const_float("float_live_game_foul_yellow"))
    {
	type = LIVE_GAME_EVENT_FOUL_YELLOW;
	player_card_set(player_of_id_team(tms[foul_team], foul_player),
			match->fix->clid, PLAYER_VALUE_CARD_YELLOW, 1, TRUE);
	player_of_id_team(tms[foul_team], foul_player)->career[PLAYER_VALUE_CARD_YELLOW]++;
    }
    else
	type = LIVE_GAME_EVENT_FOUL;

    last_unit.event.type = type;

    live_game_finish_unit();

    if(type == LIVE_GAME_EVENT_FOUL_RED ||
       type == LIVE_GAME_EVENT_FOUL_RED_INJURY ||
       (type == LIVE_GAME_EVENT_FOUL_YELLOW &&
	query_live_game_second_yellow(foul_team, foul_player)))
    {
	live_game_event_send_off(foul_team, foul_player, 
				 query_live_game_second_yellow(foul_team, foul_player));
	if(type == LIVE_GAME_EVENT_FOUL_RED_INJURY)
	    live_game_event_injury(!foul_team, fouled_player, TRUE);
    }

    if(last_unit.area == LIVE_GAME_UNIT_AREA_ATTACK && foul_team !=
       last_unit.possession)
    {
	rndom = math_rnd(0, 1);
	if(rndom < const_float("float_live_game_penalty_prob"))
	    live_game_event_penalty();
	else if(rndom < const_float("float_live_game_free_kick_prob"))
	    live_game_event_free_kick();
	else
	    last_unit.possession = !foul_team;
    }
    else
	last_unit.possession = !foul_team;
}

/** Calculate a lost possession event. */
void
live_game_event_lost_possession(void)
{
    if((debug > 100 && stat2 != -1) ||
       debug > 130)
	g_print("\t\tlive_game_event_lost_possession\n");
    last_unit.event.player =
	game_get_player(tms[last_unit.possession], 
			last_unit.area, 0, -1, TRUE);
	
    if(uni(unis->len - 2).event.type == LIVE_GAME_EVENT_GENERAL)
	last_unit.event.player2 =
	    uni(unis->len - 2).event.player;
    else
	last_unit.event.player2 =
	    game_get_player(tms[!last_unit.possession], 
			    uni(unis->len - 2).area, 0, -1, FALSE);

    live_game_finish_unit();

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
    gint old_structure = -1, sub_in = -1;
    gint usr_idx = -1;

    if((debug > 100 && stat2 != -1) ||
       debug > 130)
	g_print("\t\tlive_game_event_injury\n");

    if(create_new)
    {
	new = last_unit;
	new.event.commentary = NULL;
	g_array_append_val(unis, new);
	last_unit.event.player =
	    player;
	last_unit.event.team =
	    team;
    }
    else
	live_game_injury_get_player();

    usr_idx = team_is_user(tms[last_unit.event.team]);
    
    last_unit.minute = -1;
    last_unit.event.type = LIVE_GAME_EVENT_INJURY;

    if(math_rnd(0, 1) < const_float("float_live_game_injury_is_temp"))
    {
	last_unit.event.type = LIVE_GAME_EVENT_TEMP_INJURY;

	if(debug < 50 ||
	   usr_idx == -1)
	    player_of_id_team(tms[last_unit.event.team],
			      last_unit.event.player)->fitness =
		MAX(0, player_of_id_team(tms[last_unit.event.team],
					 last_unit.event.player)->fitness -
		    math_rnd(
			const_float("float_live_game_temp_injury_fitness_decrease_lower"),
			const_float("float_live_game_temp_injury_fitness_decrease_upper")));
    }
    
    live_game_finish_unit();

    if(debug >= 50 &&
       usr_idx != -1)
	return;

    if(last_unit.event.type == LIVE_GAME_EVENT_INJURY)
    {
	game_player_injury(player_of_id_team(tms[last_unit.event.team],
					     last_unit.event.player));

	if(match->subs_left[last_unit.event.team] > 0)
	{
	    if(show && 
	       usr_idx != -1 && 
	       ((option_int("int_opt_user_pause_injury",
			    &usr(usr_idx).options) &&
		 !option_int("int_opt_user_auto_sub",
			     &usr(usr_idx).options)) ||
		tms[last_unit.event.team]->players->len == 11))
		misc_callback_pause_live_game();
	    else if(tms[last_unit.event.team]->players->len > 11)
	    {
		sub_in = game_substitute_player(tms[last_unit.event.team],
						player_id_index(tms[last_unit.event.team],
								last_unit.event.player));
		if(sub_in != -1)
		{
		    old_structure = tms[last_unit.event.team]->structure;
		    live_game_event_substitution(
			last_unit.event.team, sub_in,
			last_unit.event.player);
		    
		if(old_structure != tms[last_unit.event.team]->structure)
		    live_game_event_team_change(last_unit.event.team,
						LIVE_GAME_EVENT_STRUCTURE_CHANGE);
		}
	    }
	}
    }

    game_get_values(match->fix, match->team_values,
		    match->home_advantage);
}

/** Calculate a stadium event. */
void
live_game_event_stadium(void)
{
    gint i;
    gfloat rndom = math_rnd(0, 1);
    gfloat probs[3] =
	{const_float("float_live_game_stadium_event_breakdown"),
	 const_float("float_live_game_stadium_event_riots"),
	 const_float("float_live_game_stadium_event_fire")};

    for(i=1;i<3;i++)
	probs[i] += probs[i - 1];

    if((debug > 100 && stat2 != -1) ||
       debug > 130)
	g_print("\t\tlive_game_event_stadium\n");
    if(rndom <= probs[0])
	last_unit.event.type = LIVE_GAME_EVENT_STADIUM_BREAKDOWN;
    else if(rndom <= probs[1])
	last_unit.event.type = LIVE_GAME_EVENT_STADIUM_RIOTS;
    else if(rndom <= probs[2])
	last_unit.event.type = LIVE_GAME_EVENT_STADIUM_FIRE;

    live_game_finish_unit();

    if(team_is_user(tm0) != -1 && debug < 50)
	game_stadium_event(&tm0->stadium, last_unit.event.type);

    match->stadium_event = last_unit.event.type;

    live_game_event_general(TRUE);
}

/** Calculate a scoring chance event. */
void
live_game_event_scoring_chance(void)
{
    gint res_idx = -1;

    if(last_unit.time == LIVE_GAME_UNIT_TIME_EXTRA_TIME)
	res_idx = 1;
    else
	res_idx = 0;

    if((debug > 100 && stat2 != -1) ||
       debug > 130)
	g_print("\t\tlive_game_event_scoring_chance\n");

    if(math_rnd(0, 1) < const_float("float_live_game_scoring_chance_is_own_goal"))
    {
	last_unit.event.type = LIVE_GAME_EVENT_OWN_GOAL;
	last_unit.event.player =
	    game_get_player(tms[!last_unit.possession], GAME_PLAYER_TYPE_DEFEND, 0, -1, FALSE);
	last_unit.event.team = !last_unit.possession;
	match->fix->result[last_unit.possession][res_idx]++;
	last_unit.result[last_unit.possession]++;

	player_streak_add_to_prob(player_of_id_team(tms[last_unit.event.team],
						    last_unit.event.player),
				  const_float("float_player_streak_add_own_goal"));
    }
    else
    {
	last_unit.event.team = last_unit.possession;

	if(uni(unis->len - 2).event.player != -1 &&
	   math_rnd(0, 1) < const_float("float_live_game_player_in_poss_shoots") &&
	   query_player_id_in_team(uni(unis->len - 2).event.player,
				   tms[last_unit.possession]))
	    last_unit.event.player =
		uni(unis->len - 2).event.player;
	else
	{
	    if(uni(unis->len - 2).event.player != -1 &&
	       query_player_id_in_team(uni(unis->len - 2).event.player,
				       tms[last_unit.possession]))
	    {
		last_unit.event.player =
		    game_get_player(tms[last_unit.possession], last_unit.area, 0, 
				    uni(unis->len - 2).event.player,
				    TRUE);

		last_unit.event.player2 =
		    uni(unis->len - 2).event.player;
	    }
	    else
	    {
		last_unit.event.player =
		    game_get_player(
			tms[last_unit.possession], last_unit.area, 0, -1, TRUE);

		last_unit.event.player2 =
		    game_get_player(tms[last_unit.possession], last_unit.area, 0, 
				    last_unit.event.player, TRUE);
	    }
	}

	if(math_rnd(0, 1) < const_float("float_live_game_scoring_chance_is_header"))
	    last_unit.event.type = LIVE_GAME_EVENT_HEADER;
    }

    live_game_finish_unit();

    if(last_unit.event.type != LIVE_GAME_EVENT_OWN_GOAL)
	live_game_event_duel();
    else
	live_game_event_general(TRUE);
}

/** Calculate a penalty event. */
void
live_game_event_penalty(void)
{
    LiveGameUnit new;

    if((debug > 100 && stat2 != -1) ||
       debug > 130)
	g_print("\t\tlive_game_event_penalty\n");

    if(last_unit.time != LIVE_GAME_UNIT_TIME_PENALTIES)
    {
	new = last_unit;
	new.event.commentary = NULL;
	new.minute = -1;
	new.event.type = LIVE_GAME_EVENT_PENALTY;

	g_array_append_val(unis, new);
    }

    last_unit.event.player2 =
	last_unit.event.team = -1;

    if(last_unit.time == LIVE_GAME_UNIT_TIME_PENALTIES)
    {
	if(live_game_penalties_taken() == 1)
	{
	    last_unit.possession = math_rndi(0, 1);
	    last_unit.event.player =
		game_get_player(tms[last_unit.possession],
				GAME_PLAYER_TYPE_PENALTY, -1, -1, FALSE);
	}
	else if(live_game_penalties_taken() == 2)
	{
	    last_unit.possession = !uni(unis->len - 3).possession;
	    last_unit.event.player =
		game_get_player(tms[last_unit.possession],
				GAME_PLAYER_TYPE_PENALTY, -1, -1, FALSE);
	}
	else
	{
	    last_unit.possession = !uni(unis->len - 3).possession;
	    last_unit.event.player =
		game_get_player(tms[last_unit.possession],
				GAME_PLAYER_TYPE_PENALTY,
				uni(unis->len - 4).event.player, -1, FALSE);
	}
    }
    else
    {
	last_unit.event.team =
	    last_unit.possession;

	last_unit.event.player = 
	    game_get_default_penalty_shooter(tms[last_unit.possession]);
	if(last_unit.event.player == -1)
	    last_unit.event.player =
		game_get_player(tms[last_unit.possession], GAME_PLAYER_TYPE_PENALTY, -1, -1, FALSE);
    }

    live_game_finish_unit();

    live_game_event_duel();
}

/** Calculate a general event. 
    @param create_new Whether we create a new unit for the event. */
void
live_game_event_general(gboolean create_new)
{
    LiveGameUnit new;

    if((debug > 100 && stat2 != -1) ||
       debug > 130)
	g_print("\t\tlive_game_event_general\n");

    if(create_new && stat0 == STATUS_LIVE_GAME_PAUSE)
	return;

    if(create_new)
    {
	new.minute = live_game_get_minute();
	new.time = last_unit.time;
	new.event.commentary = NULL;
	new.event.type = LIVE_GAME_EVENT_GENERAL;
	new.result[0] = last_unit.result[0];
	new.result[1] = last_unit.result[1];
	new.event.team = -1;

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
	   (last_unit.event.type >= LIVE_GAME_EVENT_STRUCTURE_CHANGE &&
	    last_unit.event.type <= LIVE_GAME_EVENT_BOOST_CHANGE_ON) ||
	   ((last_unit.event.type == LIVE_GAME_EVENT_POST ||
	     last_unit.event.type == LIVE_GAME_EVENT_CROSS_BAR) &&
	    math_rnd(0, 1) < const_float("float_live_game_possession_after_post")))
	{
	    new.possession = last_unit.possession;
	    new.area = (last_unit.event.type == LIVE_GAME_EVENT_GENERAL) ?
		live_game_get_area(&last_unit) : last_unit.area;
	}
	else if(last_unit.event.type == LIVE_GAME_EVENT_GOAL ||
		last_unit.event.type == LIVE_GAME_EVENT_OWN_GOAL ||
		last_unit.event.type == LIVE_GAME_EVENT_MISS ||
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
	    main_exit_program(EXIT_INT_NOT_FOUND, 
			      "live_game_event_general: unknown event type: %d\n",
			      last_unit.event.type);
	
	g_array_append_val(unis, new);
    }

    live_game_event_general_get_players();

    live_game_finish_unit();

    /** First, check whether CPU strategy changes are made. */
    if(team_is_user(tm0) == -1)
	strategy_live_game_check(match, 0);

    if(team_is_user(tm1) == -1)
	strategy_live_game_check(match, 1);

    if(last_unit.event.type >= LIVE_GAME_EVENT_STRUCTURE_CHANGE &&
       last_unit.event.type <= LIVE_GAME_EVENT_BOOST_CHANGE_ON)
	live_game_event_general(TRUE);
}

/** Fill in the players values in a general unit. */
void
live_game_event_general_get_players(void)
{
    gint *pl1 = &last_unit.event.player;
    gint *pl2 = &last_unit.event.player2;
    gint old_pl1 = 
	uni(unis->len - 2).event.player;
    gint type = uni(unis->len - 2).event.type;
	
    if((debug > 100 && stat2 != -1) ||
       debug > 130)
	g_print("\t\tlive_game_event_general_get_players\n");
    *pl1 = *pl2 = -1;

    if(type == LIVE_GAME_EVENT_LOST_POSSESSION)
    {
	*pl2 = old_pl1;
	*pl1 = game_get_player(tms[last_unit.possession], 
			       last_unit.area, 0, *pl2,
			       TRUE);
    }
    else if(type != LIVE_GAME_EVENT_GENERAL)
    {
	*pl1 = game_get_player(tms[last_unit.possession],
			       last_unit.area, 0, -1, TRUE);
	if(math_rnd(0, 1) < const_float("float_live_game_general_event_second_player"))
	    *pl2 = game_get_player(tms[last_unit.possession],
				   last_unit.area, 0, *pl1, TRUE);
    }
    else
    {
	*pl2 = query_player_id_in_team(old_pl1, tms[last_unit.possession]) ? old_pl1 : -1;
	*pl1 = game_get_player(tms[last_unit.possession],
			       last_unit.area, 0, *pl2, TRUE);
    }
}

/** Calculate a free kick event. */
void
live_game_event_free_kick(void)
{
    LiveGameUnit new = last_unit;

    if((debug > 100 && stat2 != -1) ||
       debug > 130)
	g_print("\t\tlive_game_event_free_kick\n");
    new.event.player =
	new.event.player2 = -1;

    new.minute = -1;

    new.event.type = LIVE_GAME_EVENT_FREE_KICK;
    new.event.team =
	new.possession;
    new.event.commentary = NULL;

    new.event.player =
	game_get_default_penalty_shooter(tms[new.possession]);

    if(new.event.player == -1)
	new.event.player =
	    game_get_player(tms[new.possession], new.area, 0, -1, TRUE);

    g_array_append_val(unis, new);

    live_game_finish_unit();

    live_game_event_duel();
}

/** Calculate a send-off event. */
void
live_game_event_send_off(gint team, gint player, gboolean second_yellow)
{
    LiveGameUnit new = last_unit;
    gint substitute = -1, to_substitute = -1;
    gint usr_idx = team_is_user(tms[team]);

    if((debug > 100 && stat2 != -1) ||
       debug > 130)
	g_print("\t\tlive_game_event_send_off\n");
    new.event.player =
	new.event.player2 = -1;

    new.minute = -1;

    new.event.type = LIVE_GAME_EVENT_SEND_OFF;
    new.event.player = player;
    new.event.team = team;
    new.event.commentary = NULL;

    g_array_append_val(unis, new);

    live_game_finish_unit();

    if(debug >= 50 && usr_idx != -1)
	return;

    player_streak_add_to_prob(
	player_of_id_team(tms[team], player),
	const_float("float_player_streak_add_sendoff"));
    if(player_of_id_team(tms[team], player)->streak == PLAYER_STREAK_HOT)
    {
	player_of_id_team(tms[team], player)->streak = PLAYER_STREAK_NONE;
	player_streak_reset_count(player_of_id_team(tms[team], player));
    }

    player_of_id_team(tms[team], player)->cskill = 0;

    if(second_yellow)
	player_card_set(player_of_id_team(tms[team], player), match->fix->clid, PLAYER_VALUE_CARD_RED, 2, FALSE);
    else
	player_card_set(player_of_id_team(tms[team], player), match->fix->clid, PLAYER_VALUE_CARD_RED, 
			game_player_get_ban_duration(), FALSE);

    player_of_id_team(tms[team], player)->career[PLAYER_VALUE_CARD_RED]++;
    
    if(usr_idx != -1)
    {
	tms[team]->structure = team_find_appropriate_structure(tms[team]);
	team_rearrange(tms[team]);
	live_game_event_team_change(team, LIVE_GAME_EVENT_STRUCTURE_CHANGE);
    }

    if(match->subs_left[team] > 0)
    {
	if(show && usr_idx != -1 &&
	   ((option_int("int_opt_user_pause_red",
			&usr(usr_idx).options) &&
	     !option_int("int_opt_user_auto_sub",
			 &usr(usr_idx).options)) ||
	    tms[team]->players->len == 1))
	    misc_callback_pause_live_game();
	else if(tms[team]->players->len > 11)
	{
	    game_substitute_player_send_off(match->fix->clid,
					    tms[team], player_id_index(tms[team], player),
					    &to_substitute, &substitute);

	    if(to_substitute != -1)
		live_game_event_substitution(team, substitute, to_substitute);
	    else
	    {
		tms[team]->structure = team_find_appropriate_structure(tms[team]);
		team_rearrange(tms[team]);
	    }
	    live_game_event_team_change(team, LIVE_GAME_EVENT_STRUCTURE_CHANGE);
	}
    }
    else if(usr_idx == -1)
    {
	tms[team]->structure = team_find_appropriate_structure(tms[team]);
	team_rearrange(tms[team]);
	live_game_event_team_change(team, LIVE_GAME_EVENT_STRUCTURE_CHANGE);
    }

    game_get_values(match->fix, match->team_values,
		    match->home_advantage);
}

/** Show a substitution event.
    @param team_number The team that substitutes.
    @param sub_in The id of the player who moves into the team.
    @param sub_out The id of the player who gets replaced. */
void
live_game_event_substitution(gint team_number, gint sub_in, gint sub_out)
{
    LiveGameUnit new = last_unit;
    
    new.minute = -1;
    new.time = live_game_get_time(&last_unit);

    new.event.type = LIVE_GAME_EVENT_SUBSTITUTION;
    new.event.team = team_number;
    new.event.player = sub_in;
    new.event.player2 = sub_out;
    new.event.commentary = NULL;

    if(player_of_id_team(tms[team_number], sub_in)->cskill > 0)
    {
	match->subs_left[team_number]--;

	player_streak_add_to_prob(
	    player_of_id_team(tms[team_number], sub_in),
	    const_float("float_player_streak_add_sub_out"));
	
	player_streak_add_to_prob(
	    player_of_id_team(tms[team_number], sub_in),
	    const_float("float_player_streak_add_sub_in"));	

	player_games_goals_set(player_of_id_team(tms[team_number], sub_in),
			       match->fix->clid, PLAYER_VALUE_GAMES, 1);
	player_of_id_team(tms[team_number], sub_in)->career[PLAYER_VALUE_GAMES]++;
	player_of_id_team(tms[team_number], sub_in)->participation = TRUE;

	if(show)
	    game_gui_live_game_show_opponent();
    }

    g_array_append_val(unis, new);

    live_game_finish_unit();
}

/** Show a team change event, e.g. structure change. 
    @param team_number The index of the team.
    @param event_type The event type. */
void
live_game_event_team_change(gint team_number, gint event_type)
{
    LiveGameUnit new = last_unit;
    
    new.minute = -1;
    new.time = live_game_get_time(&last_unit);

    new.event.team = team_number;
    new.event.player =
	new.event.player2 = -1;    
    new.event.type = event_type;
    new.event.commentary = NULL;

    g_array_append_val(unis, new);
    
    live_game_finish_unit();

    if(show)
	game_gui_live_game_show_opponent();
}

/** Calculate whether a player who tries to score succeeds. */
void
live_game_event_duel(void)
{
    gfloat rndom = math_rnd(0, 1);
    gfloat scoring_prob;
    gfloat duel_factor;
    LiveGameUnit new = last_unit;
    Player *attacker, *goalie, *assistant;
    gint res_idx1, res_idx2;

    if((debug > 100 && stat2 != -1) ||
       debug > 130)
	g_print("\t\tlive_game_event_duel\n");

    new.minute = -1;
    new.event.team = new.possession;
    new.event.commentary = NULL;
	
    attacker = player_of_id_team(tms[new.possession],
				 new.event.player);
    goalie = player_of_idx_team(tms[!new.possession], 0);

    assistant = (new.event.player2 != -1) ? 
	player_of_id_team(tms[new.possession], new.event.player2) : NULL;

    new.event.player2 = goalie->id;

    duel_factor = player_get_game_skill(attacker, FALSE, TRUE) /
	player_get_game_skill(goalie, FALSE, TRUE);

    res_idx1 = new.possession;
    if(new.time == LIVE_GAME_UNIT_TIME_PENALTIES)
	res_idx2 = 2;
    else if(new.time == LIVE_GAME_UNIT_TIME_EXTRA_TIME)
	res_idx2 = 1;
    else
	res_idx2 = 0;

    if(last_unit.event.type == LIVE_GAME_EVENT_PENALTY)
	scoring_prob = const_float("float_live_game_score_penalty") * duel_factor;
    else if(last_unit.event.type == LIVE_GAME_EVENT_FREE_KICK)
	scoring_prob = const_float("float_live_game_score_free_kick") * duel_factor;
    else
	scoring_prob = const_float("float_live_game_score_base_prob") * 
	    powf(duel_factor, const_float("float_live_game_score_duel_exponent")) *
	    powf(match->team_values[new.possession][GAME_TEAM_VALUE_ATTACK] /
		 match->team_values[!new.possession][GAME_TEAM_VALUE_DEFEND],
		 const_float("float_live_game_score_team_exponent"));

    if(new.time != LIVE_GAME_UNIT_TIME_PENALTIES)
    {
	player_games_goals_set(attacker, match->fix->clid, PLAYER_VALUE_SHOTS, 1);
	attacker->career[PLAYER_VALUE_SHOTS]++;
    }

    if(rndom < scoring_prob)
    {
	new.event.type = LIVE_GAME_EVENT_GOAL;
	match->fix->result[res_idx1][res_idx2]++;
	new.result[res_idx1]++;

	if(new.time != LIVE_GAME_UNIT_TIME_PENALTIES)
	{
	    player_games_goals_set(attacker, match->fix->clid, PLAYER_VALUE_GOALS, 1);
	    player_games_goals_set(goalie, match->fix->clid, PLAYER_VALUE_GOALS, 1);
	    attacker->career[PLAYER_VALUE_GOALS]++;
	    goalie->career[PLAYER_VALUE_GOALS]++;

	    player_streak_add_to_prob(attacker,
				      const_float("float_player_streak_add_goal"));
	    player_streak_add_to_prob(goalie,
				      const_float("float_player_streak_add_goalie_goal"));
	    if(assistant != NULL)
		player_streak_add_to_prob(
		    assistant, const_float("float_player_streak_add_assist"));
	}
    }
    else
	new.event.type = math_gauss_disti(LIVE_GAME_EVENT_POST, LIVE_GAME_EVENT_CROSS_BAR);

    if(new.time != LIVE_GAME_UNIT_TIME_PENALTIES &&
       (new.event.type == LIVE_GAME_EVENT_SAVE ||
	new.event.type == LIVE_GAME_EVENT_GOAL))
    {
	player_games_goals_set(goalie, match->fix->clid, PLAYER_VALUE_SHOTS, 1);
	goalie->career[PLAYER_VALUE_SHOTS]++;

	if(new.event.type == LIVE_GAME_EVENT_SAVE)
	    player_streak_add_to_prob(goalie,
				      const_float("float_player_streak_add_goalie_save"));
    }
    
    g_array_append_val(unis, new);

    live_game_finish_unit();

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

    for(i=0;i<unis->len - 1;i++)
	if(uni(i).event.type == LIVE_GAME_EVENT_FOUL_YELLOW &&
	   uni(i).possession != team &&
	   uni(i).event.player2 == player)
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

    if(time == LIVE_GAME_UNIT_TIME_EXTRA_TIME)
	return (minute >= 120);

    if(time == LIVE_GAME_UNIT_TIME_PENALTIES)
	return query_live_game_penalties_over();

    rndom = math_rnd(0, 1);

    if(time == LIVE_GAME_UNIT_TIME_FIRST_HALF)
	return (minute >= 45 && rndom > 
		powf(const_float("float_live_game_break_base"),
		     (gfloat)(minute - 44) * 
		     const_float("float_live_game_45_break_exponent_factor")));
    else
	return (minute >= 90 && rndom > 
		powf(const_float("float_live_game_break_base"),
		     (gfloat)(minute - 89) * 
		     const_float("float_live_game_90_break_exponent_factor")));
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

    for(i=unis->len - 1; i > 0; i--)
	if(uni(i).time == LIVE_GAME_UNIT_TIME_PENALTIES)
	{
	    if(uni(i).event.type == LIVE_GAME_EVENT_PENALTY)
		pen_attempted[uni(i).possession]++;
	}
	else
	    break;

    if(pen_attempted[0] + pen_attempted[1] >= 10)
	return (match->fix->result[0][2] != match->fix->result[1][2] &&
		pen_attempted[0] == pen_attempted[1]);

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

/** Return the minute for the next game unit.
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

    for(i=unis->len - 1; i>=0; i--)
	if(uni(i).minute != -1)
	    return uni(i).minute + 1;

    return -1;
}

/** Return the minutes remaining in the match. */
gint
live_game_get_minutes_remaining(const LiveGameUnit *unit)
{
    gint current_min = live_game_unit_get_minute(unit);
    gint return_value = -1;
    
    if(unit->time == LIVE_GAME_UNIT_TIME_EXTRA_TIME)
	return_value = 120 - current_min;
    else if(unit->time == LIVE_GAME_UNIT_TIME_SECOND_HALF)
	return_value = 90 - current_min;
    else if(unit->time == LIVE_GAME_UNIT_TIME_FIRST_HALF)
	return_value = 45 - current_min;	    

    return return_value;
}

/** Return the minute of the unit (ie. look up the last unit
    with a 'normal' minute value if minute = -1).
    @param unit The unit we examine.
    @return A minute between 1 and 120. */
gint
live_game_unit_get_minute(const LiveGameUnit *unit)
{
    gint i, j;

    for(i=unis->len - 1; i >= 0; i--)
	if(&uni(i) == unit)
	    break;

    if(i == -1)
	main_exit_program(EXIT_INT_NOT_FOUND,
			  "live_game_unit_get_minute: reached end of units array.");
    else
	for(j=i;j>=0;j--)
	    if(uni(j).minute != -1)
		return uni(j).minute;

    return -1;
}

/** Return the unit before or after the specified one.
    @param unit The unit specified.
    @param gap How many units to skip. */
LiveGameUnit*
live_game_unit_before(const LiveGameUnit* unit, gint gap)
{
    gint i;

    if(gap > 0)
    {
	for(i=unis->len - 1;i>=0;i--)
	    if(&uni(i) == unit)
	    {
		if(i - gap > 0)
		    return &uni(i - gap);
		else
		    g_warning("live_game_unit_before: no unit found for gap %d\n", gap);
	    }
    }
    else
    {
	for(i=unis->len - 1;i>=0;i--)
	    if(&uni(i) == unit)
	    {
		if(i + gap < unis->len - 1)
		    return &uni(i + gap);
		else
		    g_warning("live_game_unit_before: no unit found for gap %d\n", gap);
	    }
    }

    main_exit_program(EXIT_POINTER_NOT_FOUND, NULL);

    return NULL;
}

/** Calculate which area the ball is going to be in in
    the next unit.
    @param unit The previous unit.
    @return An area, defend, midfield or attack. */
gint
live_game_get_area(const LiveGameUnit *unit)
{
    gint new_area = unit->area;
    gfloat rndom = math_rnd(0, 1);
    gfloat probs[4] =
	{const_float("float_live_game_area_def_mid") *
	 live_game_pit_teams(unit, const_float("float_live_game_area_def_mid_team_exponent")),
	 const_float("float_live_game_area_mid_att") *
	 live_game_pit_teams(unit, const_float("float_live_game_area_mid_team_exponent")),
	 const_float("float_live_game_area_mid_def") /
	 live_game_pit_teams(unit, const_float("float_live_game_area_mid_team_exponent")),
	 const_float("float_live_game_area_att_mid") /
	 live_game_pit_teams(unit, const_float("float_live_game_area_att_mid_team_exponent"))};

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

/** Some polishing of the latest unit. Write commentary etc. */
void
live_game_finish_unit(void)
{
    LiveGameUnit *unit = &last_unit;

    if((debug > 100 && stat2 != -1) ||
       debug > 130)
	g_print("\t\tlive_game_finish_unit\n");

    if((debug > 100 && stat2 != -1) ||
       debug > 130)
	g_print("OOOO1 idx %d min %d type %d poss %d team %d pl %d %d\n", unis->len - 1,
	       unit->minute,
	       unit->event.type, unit->possession, unit->event.team,
	       unit->event.player,
	       unit->event.player2);

    if(unit->minute != -1 && unit->time != LIVE_GAME_UNIT_TIME_PENALTIES)
    {
	if(stat2 != -1 && usr(stat2).tm->boost == 1)
	    game_boost_cost();

	game_decrease_fitness(match->fix);
	game_get_values(match->fix, match->team_values,
			match->home_advantage);
	if(stat2 != -1 &&
	   stat2 == cur_user && show &&
	   unit->minute % opt_int("int_opt_live_game_player_list_refresh") == 0)
	    treeview_show_user_player_list();
    }

    if(stat2 != -1 || stat5 < -1000)
    {
	if(unit->time != LIVE_GAME_UNIT_TIME_PENALTIES)
	{
	    game_update_stats(match, unit);
	    if(show)
		treeview_show_game_stats(GTK_TREE_VIEW(lookup_widget(window.live, "treeview_stats")),
					 match);
	}

	lg_commentary_generate(match, unit, NULL, -1);

	if(-stat5 - 1000 == unit->event.type)
	{
	    g_print("type %d com **%s**", unit->event.type, unit->event.commentary);
	    if(g_strrstr(unit->event.commentary, "[") ||
	       g_strrstr(unit->event.commentary, "]") ||
	       g_strrstr(unit->event.commentary, "<") ||
	       g_strrstr(unit->event.commentary, ">") ||
	       g_strrstr(unit->event.commentary, "=") ||
	       g_strrstr(unit->event.commentary, " G ") ||
	       g_strrstr(unit->event.commentary, " L ") ||
	       g_strrstr(unit->event.commentary, " GE ") ||
	       g_strrstr(unit->event.commentary, " LE "))
		g_print(" ERROR?\n");
	    else
		g_print("\n");
	}

	unit->event.verbosity = live_game_event_get_verbosity(unit->event.type);
    }

    if(show)
	game_gui_live_game_show_unit(unit);

    if((debug > 100 && stat2 != -1) ||
       debug > 130)
	g_print("OOOO idx %d type %d poss %d team %d pl %d %d\n", unis->len - 1,
	       unit->event.type, unit->possession, unit->event.team,
	       unit->event.player,
	       unit->event.player2);
}

/** Find a random player (influenced by fitness) who gets
    injured. */
void
live_game_injury_get_player(void)
{
    gint i, j;
    gfloat probs[22];
    gfloat rndom, fitness_factor;
    gfloat goalie_factor = 
	const_float("float_live_game_injury_goalie_factor");
    gfloat boost_factor = 
	const_float("float_player_boost_injury_effect");
    
    for(j=0;j<2;j++)
    {
	fitness_factor = (player_of_idx_team(tms[j], 0)->fitness < 0.025) ?
	    40 : 1 / player_of_idx_team(tms[j], 0)->fitness;
	probs[j * 11] = goalie_factor * fitness_factor * 
	    (player_of_idx_team(tms[j], 0)->cskill != 0) * (1 + tms[j]->boost * boost_factor);
	if(j == 1)
	    probs[11] += probs[10];
	
	for(i=1;i<11;i++)
	{
	    fitness_factor = (player_of_idx_team(tms[j], i)->fitness < 0.025) ?
		40 : 1 / ((gfloat)player_of_idx_team(tms[j], i)->fitness);
	    probs[i + j * 11] = probs[i + j * 11 - 1] + (1 - goalie_factor) * fitness_factor *
		(player_of_idx_team(tms[j], i)->cskill != 0) * (1 + tms[j]->boost * boost_factor);
	}
    }
    
    rndom = math_rnd(0, probs[21]);

    if(rndom < probs[0])
    {
	last_unit.event.player = 
	    player_of_idx_team(tm0, 0)->id;
	last_unit.event.team = 0;
    }
    else
	for(i=1;i<22;i++)
	    if(probs[i - 1] <= rndom && rndom < probs[i])
	    {
		last_unit.event.player = 
		    player_of_idx_team(tms[(i > 10)], i % 11)->id;
		last_unit.event.team = (i > 10);
	    }
}

/** Resume a live game. Show team changes. */
void
live_game_resume(void)
{
    gint i, j;
    gint subs_in[3],
	subs_out[3];

    statp = &usr(stat2).live_game;

    for(i=0;i<2;i++)
    {
	game_get_subs(i, subs_in, subs_out);
	for(j=0;j<3;j++)
	{
	    if(subs_in[j] != -1)
		live_game_event_substitution(i, subs_in[j], subs_out[j]);
	}

	if(tms[i]->structure != usr(stat2).live_game.team_state[i].structure)
	    live_game_event_team_change(i, LIVE_GAME_EVENT_STRUCTURE_CHANGE);

	if(tms[i]->style != usr(stat2).live_game.team_state[i].style)
	    live_game_event_team_change(i, LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_DEFEND +
					tms[i]->style + 2);

	if(tms[i]->boost != usr(stat2).live_game.team_state[i].boost)
	    live_game_event_team_change(i, LIVE_GAME_EVENT_BOOST_CHANGE_ANTI +
					tms[i]->boost + 1);
    }

    live_game_calculate_fixture(usr(stat2).live_game.fix);
}


/** Reset the live game variable before we begin a new live game.
    @param live_game The pointer to the live game.
    @param fix The fixture we'll show.
    @param free Whether or not to free the variable before resetting. */
void
live_game_reset(LiveGame *live_game, Fixture *fix, gboolean free_variable)
{
    gint i;

    if(free_variable)
	free_live_game(live_game);

    live_game->units = g_array_new(FALSE, FALSE, sizeof(LiveGameUnit));
    live_game->action_ids[0] = g_array_new(FALSE, FALSE, sizeof(gint));
    live_game->action_ids[1] = g_array_new(FALSE, FALSE, sizeof(gint));
    
    for(i=0;i<LIVE_GAME_STAT_ARRAY_END;i++)
    {
	live_game->stats.players[0][i] = g_ptr_array_new();
	live_game->stats.players[1][i] = g_ptr_array_new();
    }

    for(i=0;i<LIVE_GAME_STAT_VALUE_END;i++)
	live_game->stats.values[0][i] =
	    live_game->stats.values[1][i] = 0;

    live_game->fix = fix;
    if(fix != NULL)
    {
	live_game->fix_id = fix->id;
	live_game->team_names[0] = g_strdup(fix->teams[0]->name);
	live_game->team_names[1] = g_strdup(fix->teams[1]->name);
    }
    else
    {
	live_game->fix_id = -1;
	live_game->attendance = -1;
	live_game->team_names[0] = 
	    live_game->team_names[1] = NULL;
    }

    live_game->subs_left[0] = live_game->subs_left[1] = 3;
    live_game->stadium_event = -1;
    
    if(fix != NULL && fix->home_advantage)
	live_game->home_advantage = 
	    math_rnd(const_float("float_game_home_advantage_lower"),
		     const_float("float_game_home_advantage_upper"));
    else
	live_game->home_advantage = 0;
}

/** Determine verbosity level depending on the event type. */
gint
live_game_event_get_verbosity(gint event_type)
{
    gint return_value = -1;

    if(event_type == LIVE_GAME_EVENT_START_MATCH ||
       event_type == LIVE_GAME_EVENT_HALF_TIME ||
       event_type == LIVE_GAME_EVENT_EXTRA_TIME ||
       event_type == LIVE_GAME_EVENT_END_MATCH ||
       event_type == LIVE_GAME_EVENT_PENALTIES ||
       event_type == LIVE_GAME_EVENT_GOAL ||
       event_type == LIVE_GAME_EVENT_OWN_GOAL)
	return_value = 0;
    else if(event_type == LIVE_GAME_EVENT_PENALTY ||
	    event_type == LIVE_GAME_EVENT_SCORING_CHANCE ||
	    event_type == LIVE_GAME_EVENT_HEADER ||
	    event_type == LIVE_GAME_EVENT_FREE_KICK ||
	    event_type == LIVE_GAME_EVENT_POST ||
	    event_type == LIVE_GAME_EVENT_MISS ||
	    event_type == LIVE_GAME_EVENT_CROSS_BAR ||
	    event_type == LIVE_GAME_EVENT_SAVE)
	return_value = 1;
    else if(event_type == LIVE_GAME_EVENT_SEND_OFF ||
	    event_type == LIVE_GAME_EVENT_INJURY)
	return_value = 2;
    else if(event_type == LIVE_GAME_EVENT_FOUL_RED ||
	    event_type == LIVE_GAME_EVENT_FOUL_YELLOW ||
	    event_type == LIVE_GAME_EVENT_FOUL_RED_INJURY)
	return_value = 3;
    else if(event_type == LIVE_GAME_EVENT_FOUL ||
	    event_type == LIVE_GAME_EVENT_TEMP_INJURY ||
	    event_type == LIVE_GAME_EVENT_STADIUM_RIOTS ||
	    event_type == LIVE_GAME_EVENT_STADIUM_BREAKDOWN ||
	    event_type == LIVE_GAME_EVENT_STADIUM_FIRE ||
	    event_type == LIVE_GAME_EVENT_LOST_POSSESSION)
	return_value = 4;
    else if(event_type == LIVE_GAME_EVENT_SUBSTITUTION ||
	    event_type == LIVE_GAME_EVENT_STRUCTURE_CHANGE ||
	    event_type == LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_DEFEND ||
	    event_type == LIVE_GAME_EVENT_STYLE_CHANGE_DEFEND ||
	    event_type == LIVE_GAME_EVENT_STYLE_CHANGE_BALANCED ||
	    event_type == LIVE_GAME_EVENT_STYLE_CHANGE_ATTACK ||
	    event_type == LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_ATTACK ||
	    event_type == LIVE_GAME_EVENT_BOOST_CHANGE_ANTI ||
	    event_type == LIVE_GAME_EVENT_BOOST_CHANGE_OFF ||
	    event_type == LIVE_GAME_EVENT_BOOST_CHANGE_ON)
	return_value = 5;
    else if(event_type == LIVE_GAME_EVENT_GENERAL)
	return_value = 6;
    else
	main_exit_program(EXIT_INT_NOT_FOUND, 
			  "live_game_event_get_verbosity: unknown event type %d \n",
			  event_type);

    return return_value;
}

/** Write a result like '2:3 e.t.' into the buffer.
    @param swap Whether to swap the scores. */
void
live_game_unit_result_to_buf(const LiveGameUnit *unit, gchar *buf, gboolean swap)
{
    gint idx = (swap) ? 1 : 0;

    if(unit->time == LIVE_GAME_UNIT_TIME_PENALTIES)
	/* A result after penalties. */
	sprintf(buf, _("%d : %d p."), unit->result[idx],
		unit->result[!idx]);
    else if(unit->time == LIVE_GAME_UNIT_TIME_EXTRA_TIME)
	/* A result after extra time. */
	sprintf(buf, _("%d : %d e.t."), unit->result[idx],
		unit->result[!idx]);
    else
	sprintf(buf, "%d : %d", unit->result[idx],
		unit->result[!idx]);
}

/** Count the number of penalties taken during penalty shoot-out. */
gint
live_game_penalties_taken(void)
{
    gint i, penalties = 0;

    for(i=unis->len - 1;i>=0;i--)
    {
	if(uni(i).event.type == LIVE_GAME_EVENT_PENALTIES)
	    break;
	else if(uni(i).time == LIVE_GAME_UNIT_TIME_PENALTIES &&
		uni(i).event.type == LIVE_GAME_EVENT_PENALTY)
	    penalties++;
    }

    return penalties;
}
