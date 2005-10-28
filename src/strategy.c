/*
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

#include "fixture.h"
#include "league.h"
#include "live_game.h"
#include "main.h"
#include "misc.h"
#include "option.h"
#include "player.h"
#include "strategy.h"
#include "team.h"

GPtrArray *token_strat[2];

/** Return the sid of a random strategy from the 
    strategies array (also dependent on the priorities
    of the strategies). */
gchar*
strategy_get_random(void)
{
    gint i, rndom = 
	math_rndi(1, g_array_index(strategies, Strategy, strategies->len - 1).priority);
    
    if(rndom <= g_array_index(strategies, Strategy, 0).priority)
	return g_strdup(g_array_index(strategies, Strategy, 0).sid);
    
    for(i=1;i<strategies->len;i++)
	if(rndom <= g_array_index(strategies, Strategy, i).priority)
	    return g_strdup(g_array_index(strategies, Strategy, i).sid);

    main_exit_program(EXIT_STRATEGY_ERROR, "team_strategy_get_random: no strategy found.");

    return NULL;
}

/** Compare function for sorting the players given a specific
    strategy. */
gint
strategy_compare_players(gconstpointer a,
			 gconstpointer b,
			 gpointer user_data)
{
    const Player *pl1 = *(const Player**)a;
    const Player *pl2 = *(const Player**)b;
    const StrategyPrematch *strat = (StrategyPrematch*)user_data;
    gint return_value = 0;

    if(pl1->pos != pl2->pos)
	return_value = misc_int_compare(pl2->pos, pl1->pos);
    else if(pl1->cskill == 0 && pl2->cskill > 0)
	return_value = 1;
    else if(pl2->cskill == 0 && pl1->cskill > 0)
	return_value = -1;
    else if(strat->min_fitness != 0 &&
	    pl1->fitness < strat->min_fitness &&
	    pl2->fitness >= strat->min_fitness)
	return_value = 1;
    else if(strat->min_fitness != 0 &&
	    pl1->fitness >= strat->min_fitness &&
	    pl2->fitness < strat->min_fitness)
	return_value = -1;
    else
    {
	gfloat skill1 = player_get_game_skill(pl1, TRUE, TRUE),
	    skill2 = player_get_game_skill(pl2, TRUE, TRUE);
	
	if(strat->lineup == STRAT_LINEUP_BEST)
	    return_value = misc_float_compare(skill1, skill2);
	else if(strat->lineup == STRAT_LINEUP_WEAKEST)
	    return_value = misc_float_compare(skill2, skill1);
	else if(strat->lineup == STRAT_LINEUP_FITTEST)
	{
	    return_value = misc_float_compare(pl1->fitness, pl2->fitness);
	    if(pl1->fitness == pl2->fitness)
		return_value = misc_float_compare(skill1, skill2);
	}
	else
	    g_warning("strategy_compare_players: unknown lineup type %d\n", strat->lineup);
    }

    return return_value;
}

/** Check whether a lineup described in the prematch using the given
    formation can be made with the players. */
gboolean
query_strategy_formation_possible(const GPtrArray *players, const StrategyPrematch *prematch,
				  gint formation)
{
    gint i, pos[3] = {0, 0, 0};

    for(i=0;i<players->len;i++)
	if(((Player*)g_ptr_array_index(players, i))->pos > 0 &&
	   ((Player*)g_ptr_array_index(players, i))->cskill > 0 &&
	   ((Player*)g_ptr_array_index(players, i))->fitness >=
	   prematch->min_fitness)
	    pos[((Player*)g_ptr_array_index(players, i))->pos - 1]++;

    if(pos[2] >= math_get_place(formation, 1) &&
       pos[1] >= math_get_place(formation, 2) &&
       pos[0] >= math_get_place(formation, 3))
	return TRUE;

    return FALSE;
}

/** Make the necessary substitutions to satisfy the given prematch
    and formation requirements. */
void
strategy_update_lineup(Team *tm, const GPtrArray *players,
		       const StrategyPrematch *prematch, gint formation)
{
    gint i;
    GArray *ids = g_array_new(FALSE, FALSE, sizeof(gint));
    GArray *new_players = g_array_new(FALSE, FALSE, sizeof(Player));
    gint form[3] = {math_get_place(formation, 3),
		    math_get_place(formation, 2),
		    math_get_place(formation, 1)},
	pos[3] = {0, 0, 0};
	
    /* Repair goalie if necessary. */
    if(((Player*)g_ptr_array_index(players, 0))->cskill == 0)
	strategy_repair_player((Player*)g_ptr_array_index(players, 0));
    
    g_array_append_val(ids, ((Player*)g_ptr_array_index(players, 0))->id);

    for(i=0;i<players->len;i++)
	if(((Player*)g_ptr_array_index(players, i))->pos > 0 &&
	   pos[((Player*)g_ptr_array_index(players, i))->pos - 1] <
	   form[((Player*)g_ptr_array_index(players, i))->pos - 1])
	{
	    g_array_append_val(ids, ((Player*)g_ptr_array_index(players, i))->id);
	    pos[((Player*)g_ptr_array_index(players, i))->pos - 1]++;
	}

    for(i=0;i<ids->len;i++)
	g_array_append_val(new_players, 
			   *(player_of_id_team(tm, g_array_index(ids, gint, i))));

    for(i=0;i<tm->players->len;i++)
	if(!query_misc_integer_is_in_g_array(
	       g_array_index(tm->players, Player, i).id, ids))
	    g_array_append_val(new_players, g_array_index(tm->players, Player, i));
    
    g_array_free(tm->players, TRUE);
    tm->players = new_players;    
    g_array_free(ids, TRUE);
    
    tm->structure = formation;
    team_rearrange(tm);
}

/** Delete red cards, cure injuries etc. Used to
    make sure a CPU team doesn't break. */
void
strategy_repair_player(Player *pl)
{
    gint i;

    for(i=0;i<pl->cards->len;i++)
	g_array_index(pl->cards, PlayerCard, i).red = 0;
	
    pl->health =
	pl->recovery = 0;
    pl->cskill = 
	player_get_cskill(pl, pl->pos, FALSE);
	
    pl->fitness = 
	math_rnd(const_float("float_player_fitness_lower"),
		 const_float("float_player_fitness_upper"));    
}

/** 'Repair' exactly as many players as are required to be able
    to make a lineup of healthy players with the primary formation
    of the given prematch. */
void
strategy_repair_players(GPtrArray *players, 
			const StrategyPrematch *prematch)
{
    gint i, j;
    gint form[3] = 
	{math_get_place(g_array_index(prematch->formations, gint, 0), 3),
	 math_get_place(g_array_index(prematch->formations, gint, 0), 2),
	 math_get_place(g_array_index(prematch->formations, gint, 0), 1)};
    gint pos[3] = {0, 0, 0};
    
    for(i=0;i<players->len;i++)
	if(((Player*)g_ptr_array_index(players, i))->pos > 0 &&
	   ((Player*)g_ptr_array_index(players, i))->cskill > 0)
	    pos[((Player*)g_ptr_array_index(players, i))->pos - 1]++;

    for(i=0;i<3;i++)
    {
	while(pos[i] < form[i])
	    for(j=0;j<players->len;j++)
		if(((Player*)g_ptr_array_index(players, j))->pos == i + 1 &&
		   ((Player*)g_ptr_array_index(players, j))->cskill == 0)
		{
		    strategy_repair_player((Player*)g_ptr_array_index(players, j));
		    pos[i]++;		
		}
    }

    g_ptr_array_sort_with_data(players, (GCompareDataFunc)strategy_compare_players,
			       (gpointer)prematch);
}

/** Make team changes according to the rules in the prematch. */
void
strategy_apply_prematch(Team *tm, const StrategyPrematch *prematch)
{
    gint i;
    GPtrArray *players = player_get_pointers_from_array(tm->players);

    tm->style = prematch->style;
    tm->boost = prematch->boost;    

    g_ptr_array_sort_with_data(players, (GCompareDataFunc)strategy_compare_players,
			       (gpointer)prematch);

    for(i=0;i<prematch->formations->len;i++)
	if(query_strategy_formation_possible(
	       players, prematch, 
	       g_array_index(prematch->formations, gint, i)))
	{
	    strategy_update_lineup(
		tm, players,prematch,
		g_array_index(prematch->formations, gint, i));
	    break;
	}

    /* We have to repair players to be able to satisfy
       a formation. */
    if(i == prematch->formations->len)
    {
	strategy_repair_players(players, prematch);
	strategy_update_lineup(tm, players, prematch,
			       g_array_index(prematch->formations, gint, 0));
    }

    g_ptr_array_free(players, TRUE);
}

/** Make necessary subs etc. for a CPU team. */
void
strategy_update_team_pre_match(Team *tm)
{
    gint i;
    const GArray *prematches =
	strategy_from_sid(tm->strategy_sid)->prematch;

    strategy_set_tokens(tm, NULL);

    for(i=prematches->len - 1; i >= 0; i--)
	if(misc_parse_condition(g_array_index(prematches, StrategyPrematch, i).condition,
				token_strat))
	{
	   strategy_apply_prematch(tm, &g_array_index(prematches, StrategyPrematch, i));
	   break;
	}

    strategy_free_tokens();

    if(i == -1)
	main_exit_program(EXIT_STRATEGY_ERROR,
			  "strategy_update_team_pre_match: none of the prematch conditions of strategy %s for team %s are fulfilled. remember that a strategy should contain an unconditional prematch.", 
			  tm->strategy_sid, tm->name);
}

/** Get the strategy going with the sid. */
Strategy*
strategy_from_sid(const gchar *sid)
{
    gint i;

    for(i=0;i<strategies->len;i++)
	if(strcmp(g_array_index(strategies, Strategy, i).sid, sid) == 0)
	    return &g_array_index(strategies, Strategy, i);

    main_exit_program(EXIT_STRATEGY_ERROR, 
		      "strategy_from_sid: strategy '%s' not found.", sid);

    return NULL;
}

/** Add tokens that will be evaluated when checking
    strategy conditions. */
void
strategy_set_tokens(const Team *tm, const Fixture *fixture)
{
    const Fixture *fix = (fixture == NULL) ? 
	team_get_fixture(tm, FALSE) : fixture;
    const Team *opp = (fix == NULL) ? NULL :
	fix->teams[fix->teams[0] == tm];

    token_strat[0] = g_ptr_array_new();
    token_strat[1] = g_ptr_array_new();

    if(opp == NULL)
	return;

    misc_token_add(token_strat,
		   option_int("string_token_homeadv", &tokens),
		   misc_int_to_char(((fix->teams[0] == tm) ? 1 : -1) *
				    fix->home_advantage));

    misc_token_add(token_strat,
		   option_int("string_token_cup", &tokens),
		   misc_int_to_char(fix->clid >= ID_CUP_START));

    misc_token_add(token_strat,
		   option_int("string_token_avskilldiff", &tokens),
		   misc_int_to_char((gint)rint(team_get_average_skill(tm, FALSE) -
					       team_get_average_skill(opp, FALSE))));
    if(tm->clid < ID_CUP_START &&
       opp->clid < ID_CUP_START)
	misc_token_add(token_strat,
		       option_int("string_token_team_layerdiff", &tokens),
		       misc_int_to_char(league_from_clid(tm->clid)->layer -
					league_from_clid(opp->clid)->layer));

    misc_token_add(token_strat,
		   option_int("string_token_goals_to_win", &tokens),
		   misc_int_to_char(fixture_get_goals_to_win(fix, tm)));
}

/** Free the token arrays. */
void
strategy_free_tokens(void)
{
    gint i;

    for(i=0;i<token_strat[0]->len;i++)
    {
	g_free(g_ptr_array_index(token_strat[0], i));
	g_free(g_ptr_array_index(token_strat[1], i));
    }

    g_ptr_array_free(token_strat[0], TRUE);
    g_ptr_array_free(token_strat[1], TRUE);
}

/** Fill the necessary tokens during a live game. */
void
strategy_live_game_set_tokens(const LiveGame *match, gint team_idx)
{
    gint tmp_int, 
	current_min = live_game_unit_get_minute(
	    &g_array_index(match->units, LiveGameUnit, match->units->len - 1));
    const Team *tm = match->fix->teams[team_idx];

    strategy_set_tokens(tm, match->fix);

    misc_token_add(token_strat,
		   option_int("string_token_subs_left", &tokens),
		   misc_int_to_char(match->subs_left[team_idx]));
    misc_token_add(token_strat,
		   option_int("string_token_num_def", &tokens),
		   misc_int_to_char(math_get_place(tm->structure, 3)));
    misc_token_add(token_strat,
		   option_int("string_token_num_mid", &tokens),
		   misc_int_to_char(math_get_place(tm->structure, 2)));
    misc_token_add(token_strat,
		   option_int("string_token_num_att", &tokens),
		   misc_int_to_char(math_get_place(tm->structure, 1)));
    misc_token_add(token_strat,
		   option_int("string_token_form", &tokens),
		   misc_int_to_char(tm->structure));    
    misc_token_add(token_strat,
		   option_int("string_token_time", &tokens), 
		   misc_int_to_char(
		       g_array_index(match->units, LiveGameUnit, match->units->len - 1).time));
    misc_token_add(token_strat,
		   option_int("string_token_minute", &tokens), 
		   misc_int_to_char(current_min));

    tmp_int = live_game_get_minutes_remaining(
	&g_array_index(match->units, LiveGameUnit, match->units->len - 1));
    
    if(tmp_int > 0)
	misc_token_add(token_strat, 
		       option_int("string_token_minute_remaining", &tokens), 
		       misc_int_to_char(tmp_int));

    if(query_fixture_is_draw(match->fix))
	tmp_int = 120 - current_min;
    else
	tmp_int = 90 - current_min;

    if(tmp_int > 0)
	misc_token_add(token_strat,
		       option_int("string_token_minute_total", &tokens),
		       misc_int_to_char(tmp_int));
}

/** Compare function for sorting the players when
    looking for substitutes. */
gint
strategy_compare_players_sub(gconstpointer a,
			     gconstpointer b,
			     gpointer user_data)
{
    gfloat skill1, skill2;
    const Player *pl1 = *(const Player**)a;
    const Player *pl2 = *(const Player**)b;
    gint position = GPOINTER_TO_INT(user_data) % 10;
    gint property = (GPOINTER_TO_INT(user_data) - position) / 10;
    gint return_value = 0;

    if(pl1->pos != pl2->pos &&
       (pl1->pos == position || pl2->pos == position))
	return_value = (pl1->pos == position) ? -1 : 1;
    else
	switch(property)
	{
	    default:
		g_warning("strategy_compare_players_sub: unknown property %d\n",
			  property);
		return_value = 0;
		break;
	    case STRAT_LINEUP_FITTEST:
		return_value = misc_float_compare(pl1->fitness,
						  pl2->fitness);
		break;
	    case STRAT_LINEUP_UNFITTEST:
		return_value = misc_float_compare(pl2->fitness,
						  pl1->fitness);
		break;
	    case STRAT_LINEUP_BEST:
		skill1 = player_get_game_skill(pl1, TRUE, TRUE);
		skill2 = player_get_game_skill(pl2, TRUE, TRUE);	
		return_value = misc_float_compare(skill1, skill2);
		break;
	    case STRAT_LINEUP_WEAKEST:
		skill1 = player_get_game_skill(pl1, TRUE, TRUE);
		skill2 = player_get_game_skill(pl2, TRUE, TRUE);	
		return_value = misc_float_compare(skill2, skill1);
		break;
	}

    return return_value;
}

/** Compare two player positions, taking into account the number
    of players playing the position in the team. */
gint
strategy_compare_positions(gconstpointer a,
			   gconstpointer b,
			   gpointer user_data)
{
    gint i, pos[4] = {0, 0, 0, 0};
    gint pos1 = *(gint*)a,
	pos2 = *(gint*)b;
    const Team *tm = (const Team*)user_data;
    
    for(i=0;i<11;i++)
	if(player_of_idx_team(tm, i)->cskill > 0 &&
	   player_is_banned(player_of_idx_team(tm, i)) <= 0)
	    pos[player_of_idx_team(tm, i)->pos]++;

    return misc_int_compare(pos[pos1], pos[pos2]);
}

/** Find an appropriate player to send out or in.
    @param tm The team we work with.
    @param position The position of the player we seek.
    @param property According to which property to sort players.
    @param sub_in Whether we look for a player to send in or out. */
gint
strategy_get_sub(const Team *tm, gint position, 
		 gint property, gboolean sub_in)
{
    gint i, start = (sub_in) ? 11 : 0,
	stop = (sub_in) ? tm->players->len : 11;
    GPtrArray *players = g_ptr_array_new();
    gint return_value = -1;
    GArray *positions = g_array_new(FALSE, FALSE, sizeof(gint));

    if(position < 90)
	g_array_append_val(positions, position);
    else
    {
	while(position >= 90)
	{
	    i = math_get_place(position, 1);
	    g_array_append_val(positions, i);
	    position = (position - position % 10) / 10;
	}
	
	g_array_sort_with_data(positions, 
			       (GCompareDataFunc)strategy_compare_positions,
			       (gpointer)tm);
    }

    for(i=start;i<stop;i++)
	if(player_of_idx_team(tm, i)->cskill > 0 &&
	   query_misc_integer_is_in_g_array(
	       player_of_idx_team(tm, i)->pos, positions) &&
	   (i > 10 || player_is_banned(player_of_idx_team(tm, i)) <= 0))
	    g_ptr_array_add(players, (gpointer)player_of_idx_team(tm, i));

    if(players->len == 0)
    {
	g_ptr_array_free(players, TRUE);
	return -1;
    }

    g_ptr_array_sort_with_data(players, 
			       (GCompareDataFunc)strategy_compare_players_sub,
			       GINT_TO_POINTER(property * 10 + g_array_index(positions, gint, 0)));

    return_value = ((Player*)g_ptr_array_index(players, 0))->id;

    g_ptr_array_free(players, TRUE);
    
    return return_value;
}

/** Apply the strategy actions specified to the given team. */
void
strategy_live_game_apply_action(LiveGame *match, gint team_idx,
				const StrategyMatchAction *action)
{
    gint sub_in_id = -1, sub_out_id = -1;
    Team *tm = match->fix->teams[team_idx];
    gint old_form = tm->structure;

    g_array_append_val(match->action_ids[team_idx], action->id);

    if(action->style != -100 && tm->style != action->style)
    {
	tm->style = action->style;
	live_game_event_team_change(team_idx, 
				    LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_DEFEND +
				    tm->style + 2);
    }

    if(action->boost != -100 && tm->boost != action->boost)
    {
	tm->boost = action->boost;
	live_game_event_team_change(team_idx,
				    LIVE_GAME_EVENT_BOOST_CHANGE_ANTI +
				    tm->boost + 1);
    }

    if(action->sub_in_pos != -1 && match->subs_left[team_idx] > 0 &&
       misc_parse_condition(action->sub_condition, token_strat))
    {
	sub_in_id = strategy_get_sub(tm, action->sub_in_pos, 
				     action->sub_in_prop, TRUE);
	sub_out_id = strategy_get_sub(tm, action->sub_out_pos, 
				      action->sub_in_prop, FALSE);

	if(sub_in_id > 0 && sub_out_id > 0)
	{
	    player_swap(tm, player_id_index(tm, sub_out_id),
			tm, player_id_index(tm, sub_in_id));

	    team_change_structure(tm, team_find_appropriate_structure(tm));
	    team_rearrange(tm);
	    
	    live_game_event_substitution(team_idx, sub_in_id, sub_out_id);

	    if(tm->structure != old_form)
		live_game_event_team_change(team_idx,
					    LIVE_GAME_EVENT_STRUCTURE_CHANGE);
	}
    }
}

/** Take match actions specified in the team's strategy
    if necessary. */
void
strategy_live_game_check(LiveGame *match, gint team_idx)
{
    gint i;
    Team *tm = match->fix->teams[team_idx];
    const Strategy *strat = strategy_from_sid(tm->strategy_sid);

    strategy_live_game_set_tokens(match, team_idx);

    for(i=strat->match_action->len - 1; i >= 0; i--)
    {
	if((match->subs_left[team_idx] > 0 ||
	    g_array_index(strat->match_action, 
			  StrategyMatchAction, i).sub_in_pos == -1) &&
	   !query_misc_integer_is_in_g_array(
	       g_array_index(strat->match_action, StrategyMatchAction, i).id,
	       match->action_ids[team_idx]) &&
	   misc_parse_condition(
	       g_array_index(strat->match_action, StrategyMatchAction, i).condition,
	       token_strat))
	{
	    strategy_live_game_apply_action(
		match, team_idx, 
		&g_array_index(strat->match_action, StrategyMatchAction, i));
	    break;
	}
    }

    strategy_free_tokens();
}
