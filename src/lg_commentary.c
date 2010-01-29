/*
   lg_commentary.c

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

#include <locale.h>

#include "cup.h"
#include "file.h"
#include "fixture.h"
#include "free.h"
#include "language.h"
#include "league.h"
#include "lg_commentary.h"
#include "live_game.h"
#include "main.h"
#include "maths.h"
#include "misc.h"
#include "option.h"
#include "player.h"
#include "team.h"
#include "variables.h"
#include "xml_lg_commentary.h"

/** The replacement strings for the live game commentary tokens. */
GPtrArray *token_rep[2];
/** Whether there was a commentary repetition rejection. */
gboolean repetition;

/** Generate commentary for the live game event in the unit.
    @param live_game The live game being calculated.
    @param unit The live game unit we generate the commentary for.
    @param event_type The event type we generate the commentary for (needed
    when commentary is being tested). */
void
lg_commentary_generate(const LiveGame *live_game, LiveGameUnit *unit, 
		       const gchar *event_name, gint ev_type)
{
#ifdef DEBUG
    printf("lg_commentary_generate\n");
#endif

    gint i, event_type = -1;
    gint commentary_idx = -1;
    gchar buf[SMALL];
    GArray *commentaries = NULL;

    if(unit != NULL)
	event_type = unit->event.type;
    else
	event_type = (event_name == NULL) ? ev_type : 
	    xml_lg_commentary_event_name_to_int(event_name);

    if(event_type == -1)
	return;

    if(live_game != NULL)
    {
	lg_commentary_set_basic_tokens(unit, live_game->fix);
	lg_commentary_set_team_tokens(unit, live_game->fix);
	lg_commentary_set_player_tokens(unit, live_game->fix);
	lg_commentary_set_stats_tokens(&live_game->stats, token_rep);
    }
    
    if(event_type == LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_DEFEND ||
       event_type == LIVE_GAME_EVENT_STYLE_CHANGE_DEFEND ||
       event_type == LIVE_GAME_EVENT_STYLE_CHANGE_BALANCED ||
       event_type == LIVE_GAME_EVENT_STYLE_CHANGE_ATTACK ||
       event_type == LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_ATTACK)
	commentaries = lg_commentary[LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_DEFEND];
    else if(event_type == LIVE_GAME_EVENT_BOOST_CHANGE_ANTI ||
	    event_type == LIVE_GAME_EVENT_BOOST_CHANGE_OFF ||
	    event_type == LIVE_GAME_EVENT_BOOST_CHANGE_ON)
	commentaries = lg_commentary[LIVE_GAME_EVENT_BOOST_CHANGE_ANTI];
    else
	commentaries = lg_commentary[event_type];

    commentary_idx = lg_commentary_select(commentaries, buf);

    if(commentary_idx == -1)
	debug_print_message("lg_commentary_generate: didn't find fitting commentary for unit type %d \n",
		  event_type);

    if(live_game != NULL)
    {
	unit->event.commentary = g_strdup(buf);
	unit->event.commentary_id = (commentary_idx == -1) ? 
	    -1 : g_array_index(commentaries, LGCommentary, commentary_idx).id;
	
	for(i=0;i<tokens.list->len;i++)
	    if(i != option_int("string_token_team_home", &tokens) &&
	       i != option_int("string_token_team_away", &tokens) &&
	       i != option_int("string_token_attendance", &tokens) &&
	       i != option_int("string_token_cup_round_name", &tokens) &&
	       i != option_int("string_token_league_cup_name", &tokens) &&
	       i != option_int("string_token_yellow_limit", &tokens) &&
	       i != option_int("string_token_team_layer0", &tokens) &&
	       i != option_int("string_token_team_layer1", &tokens))
		misc_token_remove(token_rep, i);
    }
    else
	g_print("%s: \"%s\"\n", event_name, buf);
}

/** Select a commentary from the array depending on the tokens
    available and write the text into the buffer. */
gint
lg_commentary_select(const GArray *commentaries, gchar *buf)
{
#ifdef DEBUG
    printf("lg_commentary_select\n");
#endif

    gint i, order[commentaries->len];

    lg_commentary_get_order(commentaries, order);

    repetition = FALSE;

    for(i=0;i<commentaries->len;i++)
	if(lg_commentary_check_commentary(&g_array_index(commentaries, LGCommentary, order[i]), buf))
	    break;

    if(i == commentaries->len)
    {
	repetition = TRUE;
	for(i=0;i<commentaries->len;i++)
	    if(lg_commentary_check_commentary(&g_array_index(commentaries, LGCommentary, order[i]), buf))
		break;
    }

    if(i == commentaries->len)
    {
	strcpy(buf, "FIXME!");
	return -1;
    }

    return order[i];
}

/** Check whether the commentary conditions are fulfilled and whether
    we can replace all tokens in the commentary. */
gboolean
lg_commentary_check_commentary(const LGCommentary *commentary, gchar *dest)
{
#ifdef DEBUG
    printf("lg_commentary_check_commentary\n");
#endif

    if(strlen(commentary->text) == 0 ||
       (commentary->condition != NULL &&
	!misc_parse_condition(commentary->condition, token_rep)) ||
       (repetition == FALSE && query_lg_commentary_is_repetition(commentary->id)))
	return FALSE;

    return misc_string_replace_all_tokens(token_rep, commentary->text, dest);
}

/** Check whether a commentary with given id has been used in the last
    few commentaries. Return TRUE if so and FALSE otherwise. */
gboolean
query_lg_commentary_is_repetition(gint id)
{
#ifdef DEBUG
    printf("query_lg_commentary_is_repetition\n");
#endif

    const GArray *units = ((LiveGame*)statp)->units;
    gint units_min = units->len - const_int("int_lg_commentary_check_backwards") - 1;
    gint i, min = MAX(units_min, 0);

    for(i=units->len - 2; i>= min; i--)
	if(g_array_index(units, LiveGameUnit, i).event.commentary_id == id)
	    return TRUE;

    return FALSE;
}

/** Write a random order of indices into the integer array
    (only depending on the priority values of the commentaries). 
    I don't like this implementation of ordering the commentaries
    according to their priority :-P can't think of a better one, though. */
void
lg_commentary_get_order(const GArray *commentaries, gint *order)
{
#ifdef DEBUG
    printf("lg_commentary_get_order\n");
#endif

    gint i, j, order_idx = 0;
    gint priority_sum = 0, bounds[commentaries->len + 1];

    bounds[0] = 0;

    for(i=0;i<commentaries->len;i++)
    {
	priority_sum += g_array_index(commentaries, LGCommentary, i).priority;
	bounds[i + 1] = priority_sum;
	order[i] = -1;
    }

    gint permutation[priority_sum];

    math_generate_permutation(permutation, 1, priority_sum);

    for(i=0;i<priority_sum;i++)
    {
	if(order_idx == commentaries->len)
	    break;

	for(j=0;j<commentaries->len;j++)
	    if(bounds[j] < permutation[i] && permutation[i] <= bounds[j + 1])
	    {
		if(!query_integer_is_in_array(j, order, commentaries->len))
		{
		    order[order_idx] = j;
		    order_idx++;
		}

		break;
	    }
    }
}

/** Fill the stats tokens. */
void
lg_commentary_set_stats_tokens(const LiveGameStats *stats, GPtrArray **token_arrays)
{
#ifdef DEBUG
    printf("lg_commentary_set_stats_tokens\n");
#endif

    misc_token_add(token_arrays, 
		   option_int("string_token_stat_shots0", &tokens),
		   misc_int_to_char(stats->values[0][LIVE_GAME_STAT_VALUE_SHOTS]));

    misc_token_add(token_arrays,
		   option_int("string_token_stat_shot_per0", &tokens),
		   misc_int_to_char(stats->values[0][LIVE_GAME_STAT_VALUE_SHOT_PERCENTAGE]));
    misc_token_add(token_arrays,
		   option_int("string_token_stat_poss0", &tokens),
		   misc_int_to_char((gint)rint(100 * 
					       ((gfloat)stats->values[0][LIVE_GAME_STAT_VALUE_POSSESSION] /
						((gfloat)stats->values[0][LIVE_GAME_STAT_VALUE_POSSESSION] + 
						 (gfloat)stats->values[1][LIVE_GAME_STAT_VALUE_POSSESSION])))));
    misc_token_add(token_arrays,
		   option_int("string_token_stat_pen0", &tokens),
		   misc_int_to_char(stats->values[0][LIVE_GAME_STAT_VALUE_PENALTIES]));
    misc_token_add(token_arrays,
		   option_int("string_token_stat_fouls0", &tokens),
		   misc_int_to_char(stats->values[0][LIVE_GAME_STAT_VALUE_FOULS]));
    misc_token_add(token_arrays,
		   option_int("string_token_stat_yellows0", &tokens),
		   misc_int_to_char(stats->values[0][LIVE_GAME_STAT_VALUE_CARDS]));
    misc_token_add(token_arrays,
		   option_int("string_token_stat_reds0", &tokens),
		   misc_int_to_char(stats->values[0][LIVE_GAME_STAT_VALUE_REDS]));
    misc_token_add(token_arrays,
		   option_int("string_token_stat_injs0", &tokens),
		   misc_int_to_char(stats->values[0][LIVE_GAME_STAT_VALUE_INJURIES]));

    misc_token_add(token_arrays,
		   option_int("string_token_stat_shots1", &tokens),
		   misc_int_to_char(stats->values[1][LIVE_GAME_STAT_VALUE_SHOTS]));
    misc_token_add(token_arrays,
		   option_int("string_token_stat_shot_per1", &tokens),
		   misc_int_to_char(stats->values[1][LIVE_GAME_STAT_VALUE_SHOT_PERCENTAGE]));
    misc_token_add(token_arrays,
		   option_int("string_token_stat_poss1", &tokens),
		   misc_int_to_char((gint)rint(100 * ((gfloat)stats->values[1][LIVE_GAME_STAT_VALUE_POSSESSION] /
						      ((gfloat)stats->values[0][LIVE_GAME_STAT_VALUE_POSSESSION] + 
						       (gfloat)stats->values[1][LIVE_GAME_STAT_VALUE_POSSESSION])))));
    misc_token_add(token_arrays,
		   option_int("string_token_stat_pen1", &tokens),
		   misc_int_to_char(stats->values[1][LIVE_GAME_STAT_VALUE_PENALTIES]));
    misc_token_add(token_arrays,
		   option_int("string_token_stat_fouls1", &tokens),
		   misc_int_to_char(stats->values[1][LIVE_GAME_STAT_VALUE_FOULS]));
    misc_token_add(token_arrays,
		   option_int("string_token_stat_yellows1", &tokens),
		   misc_int_to_char(stats->values[1][LIVE_GAME_STAT_VALUE_CARDS]));
    misc_token_add(token_arrays,
		   option_int("string_token_stat_reds1", &tokens),
		   misc_int_to_char(stats->values[1][LIVE_GAME_STAT_VALUE_REDS]));
    misc_token_add(token_arrays,
		   option_int("string_token_stat_injs1", &tokens),
		   misc_int_to_char(stats->values[1][LIVE_GAME_STAT_VALUE_INJURIES]));
}

/** Fill the tokens that contain general information. */
void
lg_commentary_set_basic_tokens(const LiveGameUnit *unit, const Fixture *fix)
{
#ifdef DEBUG
    printf("lg_commentary_set_basic_tokens\n");
#endif

    gchar buf[SMALL];
    gint tmp_int = 1, current_min = live_game_unit_get_minute(unit);
    gint avskill0 = (gint)rint(team_get_average_skill(fix->teams[0], TRUE)),
	avskill1 = (gint)rint(team_get_average_skill(fix->teams[1], TRUE));
    
    sprintf(buf, "%d : %d", unit->result[0], unit->result[1]);
    misc_token_add(token_rep, 
		   option_int("string_token_result", &tokens),
		   g_strdup(buf));

    misc_token_add(token_rep,
		   option_int("string_token_time", &tokens), 
		   misc_int_to_char(unit->time));
    misc_token_add(token_rep,
		   option_int("string_token_minute", &tokens), 
		   misc_int_to_char(current_min));

    tmp_int = live_game_get_minutes_remaining(unit);
    
    if(tmp_int > 0)
	misc_token_add(token_rep, 
		       option_int("string_token_minute_remaining", &tokens), 
		       misc_int_to_char(tmp_int));

    if(query_fixture_is_draw(fix))
	tmp_int = 120 - current_min;
    else
	tmp_int = 90 - current_min;

    if(tmp_int > 0)
	misc_token_add(token_rep,
		       option_int("string_token_minute_total", &tokens),
		       misc_int_to_char(tmp_int));

    misc_token_add(token_rep,
		   option_int("string_token_goals0", &tokens), 
		   misc_int_to_char(unit->result[0]));
    misc_token_add(token_rep,
		   option_int("string_token_goals1", &tokens), 
		   misc_int_to_char(unit->result[1]));
    misc_token_add(token_rep,
		   option_int("string_token_goal_diff", &tokens), 
		   misc_int_to_char(ABS(unit->result[0] - unit->result[1])));
    misc_token_add(token_rep,
		   option_int("string_token_extra", &tokens), 
		   lg_commentary_get_extra_data(unit, fix));

    misc_token_add(token_rep,
		   option_int("string_token_possession", &tokens), 
		   misc_int_to_char(unit->possession));
    misc_token_add(token_rep,
		   option_int("string_token_no_possession", &tokens), 
		   misc_int_to_char(!unit->possession));

    misc_token_add(token_rep,
		   option_int("string_token_team_avskill0", &tokens), 
		   misc_int_to_char(avskill0));
    misc_token_add(token_rep,
		   option_int("string_token_team_avskill1", &tokens), 
		   misc_int_to_char(avskill1));
    misc_token_add(token_rep,
		   option_int("string_token_team_avskilldiff", &tokens), 
		   misc_int_to_char(ABS(avskill0 - avskill1)));
}

/** Set the tokens containing team data. */
void
lg_commentary_set_team_tokens(const LiveGameUnit *unit, const Fixture *fix)
{
#ifdef DEBUG
    printf("lg_commentary_set_team_tokens\n");
#endif

    if(unit->result[0] != unit->result[1])
    {
	misc_token_add(token_rep,
		       option_int("string_token_team_losing", &tokens), 
		       g_strdup(fix->teams[(unit->result[0] > unit->result[1])]->name));
	misc_token_add(token_rep,
		       option_int("string_token_team_winning", &tokens), 
		       g_strdup(fix->teams[(unit->result[0] < unit->result[1])]->name));
	misc_token_add(token_rep,
		       option_int("string_token_team_losingn", &tokens), 
		       misc_int_to_char((unit->result[0] > unit->result[1])));
	misc_token_add(token_rep,
		       option_int("string_token_team_winningn", &tokens), 
		       misc_int_to_char((unit->result[0] < unit->result[1])));
    }

    if(unit->event.team != -1)
	misc_token_add(token_rep,
		       option_int("string_token_team", &tokens), 
		       misc_int_to_char(unit->event.team));
}

/** Set the player tokens. */
void
lg_commentary_set_player_tokens(const LiveGameUnit *unit, const Fixture *fix)
{
#ifdef DEBUG
    printf("lg_commentary_set_player_tokens\n");
#endif

    Player *pl1 = NULL,
	*pl2 = NULL;

    if(unit->event.type == LIVE_GAME_EVENT_GENERAL ||
       unit->event.type == LIVE_GAME_EVENT_SCORING_CHANCE ||
       unit->event.type == LIVE_GAME_EVENT_HEADER ||
       unit->event.type == LIVE_GAME_EVENT_PENALTY ||
       unit->event.type == LIVE_GAME_EVENT_FREE_KICK)
    {
	pl1 = player_of_id_team(fix->teams[unit->possession],
				unit->event.player);

	if(unit->event.player2 != -1)
	    pl2 = player_of_id_team(fix->teams[unit->possession],
				    unit->event.player2);
    }
    else if(unit->event.type == LIVE_GAME_EVENT_SEND_OFF ||
	    unit->event.type == LIVE_GAME_EVENT_INJURY ||
	    unit->event.type == LIVE_GAME_EVENT_TEMP_INJURY ||     
	    unit->event.type == LIVE_GAME_EVENT_SUBSTITUTION)
    {
	pl1 = player_of_id_team(fix->teams[unit->event.team],
				unit->event.player);
	if(unit->event.player2 != -1)
	    pl2 = player_of_id_team(fix->teams[unit->event.team],
				    unit->event.player2);
    }
    else if(unit->event.type == LIVE_GAME_EVENT_LOST_POSSESSION ||
	    unit->event.type == LIVE_GAME_EVENT_GOAL ||
	    unit->event.type == LIVE_GAME_EVENT_MISS ||
	    unit->event.type == LIVE_GAME_EVENT_SAVE ||
	    unit->event.type == LIVE_GAME_EVENT_POST ||
	    unit->event.type == LIVE_GAME_EVENT_PLAYER_PUSHED_IN_CORNER ||
	    unit->event.type == LIVE_GAME_EVENT_KEEPER_PUSHED_IN_CORNER ||
	    unit->event.type == LIVE_GAME_EVENT_CROSS_BAR)
    {
	pl1 = player_of_id_team(fix->teams[unit->possession],
				unit->event.player);
	pl2 = player_of_id_team(fix->teams[!unit->possession],
				    unit->event.player2);
    }
    else if(unit->event.type == LIVE_GAME_EVENT_OWN_GOAL)           
	pl1 = player_of_id_team(fix->teams[!unit->possession],
				unit->event.player);
    else if(unit->event.type == LIVE_GAME_EVENT_CORNER_KICK)
        pl1 = player_of_id_team(fix->teams[unit->event.team],
				unit->event.player);
    else if(unit->event.type == LIVE_GAME_EVENT_FOUL ||
	    unit->event.type == LIVE_GAME_EVENT_FOUL_RED ||
	    unit->event.type == LIVE_GAME_EVENT_FOUL_RED_INJURY ||
	    unit->event.type == LIVE_GAME_EVENT_FOUL_YELLOW)
    {
	pl1 = player_of_id_team(fix->teams[!unit->event.team],
				unit->event.player);
	pl2 = player_of_id_team(fix->teams[unit->event.team],
				unit->event.player2);
    }

    if(pl1 != NULL)
    {
	misc_token_add(token_rep,
		       option_int("string_token_player0", &tokens), 
		       player_get_last_name(pl1->name));
	misc_token_add(token_rep,
		       option_int("string_token_player_goals0", &tokens), 
		       misc_int_to_char(player_games_goals_get(
					    pl1, fix->clid, PLAYER_VALUE_GOALS)));
	misc_token_add(token_rep,
		       option_int("string_token_player_goals_all0", &tokens), 
		       misc_int_to_char(player_all_games_goals(pl1, PLAYER_VALUE_GOALS)));
    }

    if(pl2 != NULL)
    {
	misc_token_add(token_rep,
		       option_int("string_token_player1", &tokens), 
		       player_get_last_name(pl2->name));
	misc_token_add(token_rep,
		       option_int("string_token_player_goals1", &tokens), 
		       misc_int_to_char(player_games_goals_get(pl2, fix->clid, PLAYER_VALUE_GOALS)));
	misc_token_add(token_rep,
		       option_int("string_token_player_goals_all1", &tokens), 
		       misc_int_to_char(player_all_games_goals(pl2, PLAYER_VALUE_GOALS)));
	misc_token_add(token_rep,
		       option_int("string_token_player_yellows", &tokens), 
		       misc_int_to_char(player_card_get(pl2, fix->clid, PLAYER_VALUE_CARD_YELLOW)));
    }
}

/** Return some extra data depending on the unit type. */
gchar*
lg_commentary_get_extra_data(const LiveGameUnit *unit, const Fixture *fix)
{
#ifdef DEBUG
    printf("lg_commentary_get_extra_data\n");
#endif

#ifdef DEBUG
    printf("lg_commentary_get_extra_data\n");
#endif

    gchar buf[SMALL];

    switch(unit->event.type)
    {
	default:
	    return NULL;
	    break;
	case LIVE_GAME_EVENT_STRUCTURE_CHANGE:
	    sprintf(buf, "%d",
		    fix->teams[unit->event.team]->structure);
	    break;
	case LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_DEFEND:
	    strcpy(buf, _("ALL OUT DEFEND"));
	    break;
	case LIVE_GAME_EVENT_STYLE_CHANGE_DEFEND:
	    strcpy(buf, _("DEFEND"));
	    break;
	case LIVE_GAME_EVENT_STYLE_CHANGE_BALANCED:
	    strcpy(buf, _("BALANCED"));
	    break;
	case LIVE_GAME_EVENT_STYLE_CHANGE_ATTACK:
	    strcpy(buf, _("ATTACK"));
	    break;
	case LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_ATTACK:
	    strcpy(buf, _("ALL OUT ATTACK"));
	    break;
	case LIVE_GAME_EVENT_BOOST_CHANGE_ANTI:
	    strcpy(buf, _("ANTI"));
	    break;
	case LIVE_GAME_EVENT_BOOST_CHANGE_OFF:
	    strcpy(buf, _("OFF"));
	    break;
	case LIVE_GAME_EVENT_BOOST_CHANGE_ON:
	    strcpy(buf, _("ON"));
	    break;
    }

    return g_strdup(buf);
}

/** Allocate memory for the token array and fill some tokens
    at the beginning of the live game that don't change later. */
void
lg_commentary_initialize(const Fixture *fix)
{
#ifdef DEBUG
    printf("lg_commentary_initialize\n");
#endif

    gchar buf[SMALL];

    token_rep[0] = g_ptr_array_new();
    token_rep[1] = g_ptr_array_new();

    misc_token_add(token_rep,
		   option_int("string_token_team_home", &tokens), 
		   g_strdup(fix->teams[0]->name));
    misc_token_add(token_rep,
		   option_int("string_token_team_away", &tokens), 
		   g_strdup(fix->teams[1]->name));

    if(fix->teams[0]->clid < ID_CUP_START)
	misc_token_add(token_rep,
		       option_int("string_token_team_layer0", &tokens), 
		       misc_int_to_char(league_from_clid(fix->teams[0]->clid)->layer));
    if(fix->teams[1]->clid < ID_CUP_START)
	misc_token_add(token_rep,
		       option_int("string_token_team_layer1", &tokens), 
		       misc_int_to_char(league_from_clid(fix->teams[1]->clid)->layer));
	
    if(fix->teams[0]->clid < ID_CUP_START &&
       fix->teams[1]->clid < ID_CUP_START)
	misc_token_add(token_rep,
		       option_int("string_token_team_layerdiff", &tokens),
		       misc_int_to_char(league_from_clid(fix->teams[0]->clid)->layer -
					league_from_clid(fix->teams[1]->clid)->layer));

    misc_token_add(token_rep,
		   option_int("string_token_league_cup_name", &tokens), 
		   g_strdup(league_cup_get_name_string(fix->clid)));

    if(fix->clid >= ID_CUP_START)
    {
	cup_get_round_name(cup_from_clid(fix->clid), fix->round, buf);
	misc_token_add(token_rep,
		       option_int("string_token_cup_round_name", &tokens), 
		       g_strdup(buf));
    }

    misc_print_grouped_int(fix->attendance, buf);
    misc_token_add(token_rep,
		   option_int("string_token_attendance", &tokens),
		   g_strdup(buf));

    misc_token_add(token_rep,
		   option_int("string_token_yellow_limit", &tokens), 
		   misc_int_to_char(league_cup_get_yellow_red(fix->clid)));
}

/** Free the memory occupied by the tokens array and the permanent tokens. */
void
lg_commentary_free_tokens(void)
{
#ifdef DEBUG
    printf("lg_commentary_free_tokens\n");
#endif

    gint i;

    for(i=token_rep[0]->len - 1;i >= 0; i--)
    {
	g_free(g_ptr_array_index(token_rep[0], i));
	g_free(g_ptr_array_index(token_rep[1], i));
    }

    g_ptr_array_free(token_rep[0], TRUE);
    g_ptr_array_free(token_rep[1], TRUE);
}

/** Load the appropriate commentary file by evaluating
    the language option. */
void
lg_commentary_load_commentary_file_from_option(void)
{
#ifdef DEBUG
    printf("lg_commentary_load_commentary_file_from_option\n");
#endif

    gchar buf[SMALL], commentary_file[SMALL];

    language_get_code(buf);

    sprintf(commentary_file, "lg_commentary_%s.xml", buf);

    lg_commentary_load_commentary_file(commentary_file, FALSE);
}

/** Load the commentary file given. Abort if not found and abort=TRUE, 
    load default english file otherwise. */
void
lg_commentary_load_commentary_file(const gchar *commentary_file, gboolean abort)
{
#ifdef DEBUG
    printf("lg_commentary_load_commentary_file\n");
#endif

    gchar *file_name = NULL;

    if(g_file_test(commentary_file, G_FILE_TEST_EXISTS))
    {
	xml_lg_commentary_read(commentary_file);
	return;
    }
    
    file_name = file_find_support_file(commentary_file, FALSE);

    if(file_name != NULL)
    {
	xml_lg_commentary_read(file_name);
	g_free(file_name);
    }
    else
    {
	if(abort)
	    main_exit_program(EXIT_FILE_NOT_FOUND, 
			      "lg_commentary_load_commentary_file: file %s not found \n", 
			      file_name);
	else
	    lg_commentary_load_commentary_file("lg_commentary_en.xml", TRUE);
    }
}

/** Load a file with tokens for commentary testing purposes. */
void
lg_commentary_test_load_token_file(const gchar *token_file)
{
#ifdef DEBUG
    printf("lg_commentary_test_load_token_file\n");
#endif

    gchar token_name[SMALL], token_value[SMALL];
    FILE *fil = NULL;

    file_my_fopen(token_file, "r", &fil, TRUE);

    token_rep[0] = g_ptr_array_new();
    token_rep[1] = g_ptr_array_new();

    while(file_get_next_opt_line(fil, token_name, token_value))
    {
	g_ptr_array_add(token_rep[0], (gpointer)g_strdup(token_name));
	g_ptr_array_add(token_rep[1], (gpointer)g_strdup(token_value));
    }
}

/** Test a live game commentary file.
    @param commentary_file the commentary file to test.
    @param token_file the file containing the token values to use.
    @param event_name the event to test (or 'all' to test all events).
    @param number_of_passes how many commentaries to generate. */
void
lg_commentary_test(const gchar *commentary_file, const gchar* token_file,
		   const gchar *event_name, gint number_of_passes)
{
#ifdef DEBUG
    printf("lg_commentary_test\n");
#endif

    gint i, j;
    LiveGame dummy;

    lg_commentary_load_commentary_file(commentary_file, TRUE);

    lg_commentary_test_load_token_file(token_file);

    dummy.units = g_array_new(FALSE, FALSE, sizeof(gint));
    statp = (gpointer)&dummy;

    if(event_name == NULL)
	for(i=0;i<LIVE_GAME_EVENT_END;i++)
	    for(j=0;j<number_of_passes;j++)
		lg_commentary_generate(NULL, NULL, NULL, i);
    else
	for(i=0;i<number_of_passes;i++)
	    lg_commentary_generate(NULL, NULL, event_name, -1);

    g_array_free(dummy.units, TRUE);
}
