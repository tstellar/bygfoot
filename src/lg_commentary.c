#include <locale.h>

#include "cup.h"
#include "file.h"
#include "fixture.h"
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
gchar **token_rep;
/** Whether there was a commentary repetition rejection. */
gboolean repetition;

/** Generate commentary for the live game event in the unit.
    @param unit The unit we comment.
    @param fix The fixture being calculated. */
void
lg_commentary_generate(const LiveGame *live_game, LiveGameUnit *unit)
{
    gchar buf[SMALL];
    GArray **commentaries = NULL;
    gint i;

    lg_commentary_set_basic_tokens(unit, live_game->fix);
    lg_commentary_set_team_tokens(unit, live_game->fix);
    lg_commentary_set_player_tokens(unit, live_game->fix);
    lg_commentary_set_stats_tokens(&live_game->stats);

    if(unit->event.type == LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_DEFEND ||
       unit->event.type == LIVE_GAME_EVENT_STYLE_CHANGE_DEFEND ||
       unit->event.type == LIVE_GAME_EVENT_STYLE_CHANGE_BALANCED ||
       unit->event.type == LIVE_GAME_EVENT_STYLE_CHANGE_ATTACK ||
       unit->event.type == LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_ATTACK)
	commentaries = &lg_commentary[LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_DEFEND];
    else if(unit->event.type == LIVE_GAME_EVENT_BOOST_CHANGE_ANTI ||
	    unit->event.type == LIVE_GAME_EVENT_BOOST_CHANGE_OFF ||
	    unit->event.type == LIVE_GAME_EVENT_BOOST_CHANGE_ON)
	commentaries = &lg_commentary[LIVE_GAME_EVENT_BOOST_CHANGE_ANTI];
    else
	commentaries = &lg_commentary[unit->event.type];

    gint order[(*commentaries)->len];
    lg_commentary_get_order(*commentaries, order);

    repetition = FALSE;

    for(i=0;i<(*commentaries)->len;i++)
	if(lg_commentary_check_commentary(&g_array_index(*commentaries, LGCommentary, order[i]), buf))
	    break;

    if(i == (*commentaries)->len)
    {
	repetition = TRUE;
	for(i=0;i<(*commentaries)->len;i++)
	    if(lg_commentary_check_commentary(&g_array_index(*commentaries, LGCommentary, order[i]), buf))
		break;
    }

    if(i == (*commentaries)->len)
    {
	g_warning("lg_commentary_generate: didn't find fitting commentary for unit type %d \n",
		  unit->event.type);
	strcpy(buf, "FIXME!");
    }

    unit->event.commentary = g_string_new(buf);
    unit->event.commentary_id = (i == (*commentaries)->len) ? 
	-1 : g_array_index(*commentaries, LGCommentary, order[i]).id;

    for(i=0;i<lg_tokens.list->len;i++)
	if(i != option_int("string_lg_commentary_token_team_home", &lg_tokens) &&
	   i != option_int("string_lg_commentary_token_team_away", &lg_tokens) &&
	   i != option_int("string_lg_commentary_token_attendance", &lg_tokens) &&
	   i != option_int("string_lg_commentary_token_cup_round_name", &lg_tokens) &&
	   i != option_int("string_lg_commentary_token_league_cup_name", &lg_tokens) &&
	   i != option_int("string_lg_commentary_token_yellow_limit", &lg_tokens) &&
	   i != option_int("string_lg_commentary_token_team_layer0", &lg_tokens) &&
	   i != option_int("string_lg_commentary_token_team_layer1", &lg_tokens))
	{
	    g_free(token_rep[i]);
	    token_rep[i] = NULL;
	}
}

/** Check whether the commentary conditions are fulfilled and whether
    we can replace all tokens in the commentary. */
gboolean
lg_commentary_check_commentary(const LGCommentary *commentary, gchar *dest)
{
    gchar buf[SMALL];

    if(strlen(commentary->text->str) == 0 ||
       (commentary->condition != NULL &&
	!lg_commentary_parse_condition(commentary->condition->str)) ||
       (repetition == FALSE && query_lg_commentary_is_repetition(commentary->id)))
	return FALSE;

    strcpy(dest, commentary->text->str);
    
    do
    {
	strcpy(buf, dest);
	if(!lg_commentary_replace_tokens(dest))
	    return FALSE;

	lg_commentary_replace_expressions(dest);
    }
    while(strcmp(buf, dest) != 0);

    return TRUE;
}

/** Check whether a commentary with given id has been used in the last
    few commentaries. Return TRUE if so and FALSE otherwise. */
gboolean
query_lg_commentary_is_repetition(gint id)
{
    const GArray *units = ((LiveGame*)statp)->units;
    gint units_min = units->len - const_int("int_lg_commentary_check_backwards") - 1;
    gint i, min = MAX(units_min, 0);

    for(i=units->len - 2; i>= min; i--)
	if(g_array_index(units, LiveGameUnit, i).event.commentary_id == id)
	    return TRUE;

    return FALSE;
}



/** Choose one of strings separated with '|' */
void lg_commentary_choose_random(gchar* s)
{
   const gchar STR_SEP = '|';
   gint i = 0;
   gint count = 1;
   const gchar* start;
   
   for (i = 0; s[i]; i++)
      count += (s[i] == STR_SEP);

   if (count == 1)
      return;
      
   count = math_rndi(0, count-1) + 1;
   start = s;
   for (i = 0; s[i]; i++)
      if (s[i] == STR_SEP)
      {
         count--;
	 if (count == 1)
	    start = s + i + 1;
	 else if (!count)
	 {
	    s[i] = '\0';
	    break;
         }
      }

   strcpy(s, start);
}




/** Replace simple arithmetic expressions like "1 + 2"
    and comparisons like "3 < 4" with the appropriate result. */
void
lg_commentary_replace_expressions(gchar *commentary_text)
{
    gint i, j, last_idx = 0;
    gint value = -1;
    gchar buf[SMALL], buf2[SMALL];

    if(debug > 100)
	printf("lg_commentary_replace_expressions: #%s#\n",
	       commentary_text);

    if(!g_strrstr(commentary_text, "["))
	return;

    strcpy(buf, commentary_text);
    strcpy(commentary_text, "");

    for(i=strlen(buf) - 1; i>=0; i--)
	if(buf[i] == '[')
	{
	    strncpy(buf2, buf, i);
	    buf2[i] = '\0';
	    strcat(commentary_text, buf2);

	    for(j=i + 1;j<strlen(buf);j++)
	    {
		if(buf[j] == ']')
		{
		    strncpy(buf2, buf + i + 1, j - i - 1);
		    buf2[j - i - 1] = '\0';
		    if (g_strrstr(buf2, "|"))
			lg_commentary_choose_random(buf2);
		    else 
		    {
		      if(g_strrstr(buf2, "<") ||
		         g_strrstr(buf2, ">") ||
   		         g_strrstr(buf2, "=") ||
		         g_strrstr(buf2, " G ") ||
		         g_strrstr(buf2, " L ") ||
		         g_strrstr(buf2, " GE ") ||
		         g_strrstr(buf2, " LE "))
			    misc_parse(buf2, &value);
		       else
		  	  misc_parse_expression(buf2, &value);
  		       sprintf(buf2, "%d", value);
		    }
		    strcat(commentary_text, buf2);
		    value = -1;

		    last_idx = j + 1;

		    break;
		}
	    }

	    break;
	}

    if(last_idx < strlen(buf))
    {
	strncpy(buf2, buf + last_idx, strlen(buf) - last_idx);
	buf2[strlen(buf) - last_idx] = '\0';
	strcat(commentary_text, buf2);
    }
}

/** Try to replace all special tokens in the string and write the result to dest.
    @param commentary The commentary with tokens.
    @param dest The destination string.
    @return TRUE if we could replace all tokens and the commentary condition
    was fulfilled, FALSE otherwise. */
gboolean
lg_commentary_replace_tokens(gchar *commentary_text)
{
    gint i;

    for(i=0;i<lg_tokens.list->len;i++)
    {
	if(g_strrstr(commentary_text,
		     g_array_index(lg_tokens.list, Option, i).string_value->str))
	{
	    if(token_rep[i] == NULL)
		return FALSE;
	    else
		misc_string_replace_token(commentary_text, 
					  g_array_index(lg_tokens.list, Option, i).string_value->str,
					  token_rep[i]);
	}
    }

    return TRUE;
}

/** Find out whether the conditions of the commentary are fulfilled. */
gboolean
lg_commentary_parse_condition(const gchar *condition)
{
    gboolean return_value = FALSE;
    gchar buf[SMALL], buf2[SMALL];
    
    strcpy(buf, condition);

    do
    {
	strcpy(buf2, buf);
	if(!lg_commentary_replace_tokens(buf))
	    return FALSE;

	lg_commentary_replace_expressions(buf);
    }
    while(strcmp(buf2, buf) != 0);

    misc_parse(buf, &return_value);

    return return_value;
}

/** Write a random order of indices into the integer array
    (only depending on the priority values of the commentaries). 
    I don't like this implementation of ordering the commentaries
    according to their priority :-P can't think of a better one, though. */
void
lg_commentary_get_order(const GArray *commentaries, gint *order)
{
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
		if(!query_integer_is_in_array(j, order, 0, commentaries->len))
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
lg_commentary_set_stats_tokens(const LiveGameStats *stats)
{
    token_rep[option_int("string_lg_commentary_token_stat_shots0", &lg_tokens)] =
	misc_int_to_char(stats->values[0][LIVE_GAME_STAT_VALUE_SHOTS]);
    token_rep[option_int("string_lg_commentary_token_stat_shot_per0", &lg_tokens)] =
	misc_int_to_char(stats->values[0][LIVE_GAME_STAT_VALUE_SHOT_PERCENTAGE]);
    token_rep[option_int("string_lg_commentary_token_stat_poss0", &lg_tokens)] =
	misc_int_to_char((gint)rint(100 * ((gfloat)stats->values[0][LIVE_GAME_STAT_VALUE_POSSESSION] /
					   ((gfloat)stats->values[0][LIVE_GAME_STAT_VALUE_POSSESSION] + 
					    (gfloat)stats->values[1][LIVE_GAME_STAT_VALUE_POSSESSION]))));
    token_rep[option_int("string_lg_commentary_token_stat_pen0", &lg_tokens)] =
	misc_int_to_char(stats->values[0][LIVE_GAME_STAT_VALUE_PENALTIES]);
    token_rep[option_int("string_lg_commentary_token_stat_fouls0", &lg_tokens)] =
	misc_int_to_char(stats->values[0][LIVE_GAME_STAT_VALUE_FOULS]);
    token_rep[option_int("string_lg_commentary_token_stat_yellows0", &lg_tokens)] =
	misc_int_to_char(stats->values[0][LIVE_GAME_STAT_VALUE_CARDS]);
    token_rep[option_int("string_lg_commentary_token_stat_reds0", &lg_tokens)] =
	misc_int_to_char(stats->values[0][LIVE_GAME_STAT_VALUE_REDS]);
    token_rep[option_int("string_lg_commentary_token_stat_injs0", &lg_tokens)] =
	misc_int_to_char(stats->values[0][LIVE_GAME_STAT_VALUE_INJURIES]);

    token_rep[option_int("string_lg_commentary_token_stat_shots1", &lg_tokens)] =
	misc_int_to_char(stats->values[1][LIVE_GAME_STAT_VALUE_SHOTS]);
    token_rep[option_int("string_lg_commentary_token_stat_shot_per1", &lg_tokens)] =
	misc_int_to_char(stats->values[1][LIVE_GAME_STAT_VALUE_SHOT_PERCENTAGE]);
    token_rep[option_int("string_lg_commentary_token_stat_poss1", &lg_tokens)] =
	misc_int_to_char((gint)rint(100 * ((gfloat)stats->values[1][LIVE_GAME_STAT_VALUE_POSSESSION] /
					   ((gfloat)stats->values[0][LIVE_GAME_STAT_VALUE_POSSESSION] + 
					    (gfloat)stats->values[1][LIVE_GAME_STAT_VALUE_POSSESSION]))));
    token_rep[option_int("string_lg_commentary_token_stat_pen1", &lg_tokens)] =
	misc_int_to_char(stats->values[1][LIVE_GAME_STAT_VALUE_PENALTIES]);
    token_rep[option_int("string_lg_commentary_token_stat_fouls1", &lg_tokens)] =
	misc_int_to_char(stats->values[1][LIVE_GAME_STAT_VALUE_FOULS]);
    token_rep[option_int("string_lg_commentary_token_stat_yellows1", &lg_tokens)] =
	misc_int_to_char(stats->values[1][LIVE_GAME_STAT_VALUE_CARDS]);
    token_rep[option_int("string_lg_commentary_token_stat_reds1", &lg_tokens)] =
	misc_int_to_char(stats->values[1][LIVE_GAME_STAT_VALUE_REDS]);
    token_rep[option_int("string_lg_commentary_token_stat_injs1", &lg_tokens)] =
	misc_int_to_char(stats->values[1][LIVE_GAME_STAT_VALUE_INJURIES]);
}

/** Fill the tokens that contain general information. */
void
lg_commentary_set_basic_tokens(const LiveGameUnit *unit, const Fixture *fix)
{
    gchar buf[SMALL];
    gint tmp_int = 1, current_min = live_game_unit_get_minute(unit);
    gint avskill0 = (gint)rint(team_get_average_skill(fix->teams[0], TRUE)),
	avskill1 = (gint)rint(team_get_average_skill(fix->teams[1], TRUE));
    
    sprintf(buf, "%d : %d", unit->result[0], unit->result[1]);
    token_rep[option_int("string_lg_commentary_token_result", &lg_tokens)] = 
	g_strdup(buf);

    token_rep[option_int("string_lg_commentary_token_time", &lg_tokens)] = 
	misc_int_to_char(unit->time);
    token_rep[option_int("string_lg_commentary_token_minute", &lg_tokens)] = 
	misc_int_to_char(live_game_unit_get_minute(unit));

    if(unit->time == LIVE_GAME_UNIT_TIME_EXTRA_TIME)
	tmp_int = 120 - current_min;
    else if(unit->time == LIVE_GAME_UNIT_TIME_SECOND_HALF)
	tmp_int = 90 - current_min;
    else if(unit->time == LIVE_GAME_UNIT_TIME_FIRST_HALF)
	tmp_int = 45 - current_min;
    
    if(unit->time != LIVE_GAME_UNIT_TIME_PENALTIES && tmp_int > 0)
	token_rep[option_int("string_lg_commentary_token_minute_remaining", &lg_tokens)] = 
	    misc_int_to_char(tmp_int);

    if(query_fixture_is_draw(fix))
	tmp_int = 120 - current_min;
    else
	tmp_int = 90 - current_min;

    if(tmp_int > 0)
	token_rep[option_int("string_lg_commentary_token_minute_total", &lg_tokens)] =
	    misc_int_to_char(tmp_int);

    token_rep[option_int("string_lg_commentary_token_goals0", &lg_tokens)] = 
	misc_int_to_char(unit->result[0]);
    token_rep[option_int("string_lg_commentary_token_goals1", &lg_tokens)] = 
	misc_int_to_char(unit->result[1]);
    token_rep[option_int("string_lg_commentary_token_goal_diff", &lg_tokens)] = 
	misc_int_to_char(ABS(unit->result[0] - unit->result[1]));
    token_rep[option_int("string_lg_commentary_token_extra", &lg_tokens)] = 
	lg_commentary_get_extra_data(unit, fix);

    token_rep[option_int("string_lg_commentary_token_possession", &lg_tokens)] = 
	misc_int_to_char(unit->possession);
    token_rep[option_int("string_lg_commentary_token_no_possession", &lg_tokens)] = 
	misc_int_to_char(!unit->possession);

    token_rep[option_int("string_lg_commentary_token_team_avskill0", &lg_tokens)] = 
	misc_int_to_char(avskill0);
    token_rep[option_int("string_lg_commentary_token_team_avskill1", &lg_tokens)] = 
	misc_int_to_char(avskill1);
    token_rep[option_int("string_lg_commentary_token_team_avskilldiff", &lg_tokens)] = 
	misc_int_to_char(ABS(avskill0 - avskill1));
}

/** Set the tokens containing team data. */
void
lg_commentary_set_team_tokens(const LiveGameUnit *unit, const Fixture *fix)
{
    if(unit->result[0] != unit->result[1])
    {
	token_rep[option_int("string_lg_commentary_token_team_losing", &lg_tokens)] = 
	    g_strdup(fix->teams[(unit->result[0] > unit->result[1])]->name->str);
	token_rep[option_int("string_lg_commentary_token_team_winning", &lg_tokens)] = 
	    g_strdup(fix->teams[(unit->result[0] < unit->result[1])]->name->str);
	token_rep[option_int("string_lg_commentary_token_team_losingn", &lg_tokens)] = 
	    misc_int_to_char((unit->result[0] > unit->result[1]));
	token_rep[option_int("string_lg_commentary_token_team_winningn", &lg_tokens)] = 
	    misc_int_to_char((unit->result[0] < unit->result[1]));
    }

    if(unit->event.team != -1)
	token_rep[option_int("string_lg_commentary_token_team", &lg_tokens)] = 
	    misc_int_to_char(unit->event.team);
}

/** Set the player tokens. */
void
lg_commentary_set_player_tokens(const LiveGameUnit *unit, const Fixture *fix)
{
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
	token_rep[option_int("string_lg_commentary_token_player0", &lg_tokens)] = 
	    player_get_last_name(pl1->name->str);
	token_rep[option_int("string_lg_commentary_token_player_goals0", &lg_tokens)] = 
	    misc_int_to_char(player_games_goals_get(pl1, fix->clid, PLAYER_VALUE_GOALS));
	token_rep[option_int("string_lg_commentary_token_player_goals_all0", &lg_tokens)] = 
	    misc_int_to_char(player_all_games_goals(pl1, PLAYER_VALUE_GOALS));
    }

    if(pl2 != NULL)
    {
	token_rep[option_int("string_lg_commentary_token_player1", &lg_tokens)] = 
	    player_get_last_name(pl2->name->str);
	token_rep[option_int("string_lg_commentary_token_player_goals1", &lg_tokens)] = 
	    misc_int_to_char(player_games_goals_get(pl2, fix->clid, PLAYER_VALUE_GOALS));
	token_rep[option_int("string_lg_commentary_token_player_goals_all1", &lg_tokens)] = 
	    misc_int_to_char(player_all_games_goals(pl2, PLAYER_VALUE_GOALS));
	token_rep[option_int("string_lg_commentary_token_player_yellows", &lg_tokens)] = 
	    misc_int_to_char(player_card_get(pl2, fix->clid, PLAYER_VALUE_CARD_YELLOW));
    }
}

/** Return some extra data depending on the unit type. */
gchar*
lg_commentary_get_extra_data(const LiveGameUnit *unit, const Fixture *fix)
{
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
    gint i;
    gchar buf[SMALL];

    token_rep = (gchar**)g_malloc(sizeof(gchar*) * lg_tokens.list->len);

    for(i=0;i<lg_tokens.list->len;i++)
	token_rep[i] = NULL;
	
    token_rep[option_int("string_lg_commentary_token_team_home", &lg_tokens)] = 
	g_strdup(fix->teams[0]->name->str);
    token_rep[option_int("string_lg_commentary_token_team_away", &lg_tokens)] = 
	g_strdup(fix->teams[1]->name->str);

    if(fix->teams[0]->clid < ID_CUP_START)
	token_rep[option_int("string_lg_commentary_token_team_layer0", &lg_tokens)] = 
	    misc_int_to_char(league_from_clid(fix->teams[0]->clid)->layer);
    if(fix->teams[1]->clid < ID_CUP_START)
	token_rep[option_int("string_lg_commentary_token_team_layer1", &lg_tokens)] = 
	    misc_int_to_char(league_from_clid(fix->teams[1]->clid)->layer);
	
    token_rep[option_int("string_lg_commentary_token_league_cup_name", &lg_tokens)] = 
	g_strdup(league_cup_get_name_string(fix->clid));

    if(fix->clid >= ID_CUP_START)
    {
	cup_get_round_name(cup_from_clid(fix->clid), fix->round, buf);
	token_rep[option_int("string_lg_commentary_token_cup_round_name", &lg_tokens)] = 
	    g_strdup(buf);
    }

    misc_print_grouped_int(fix->attendance, buf);
    token_rep[option_int("string_lg_commentary_token_attendance", &lg_tokens)] =
	g_strdup(buf);

    token_rep[option_int("string_lg_commentary_token_yellow_limit", &lg_tokens)] = 
	misc_int_to_char(league_cup_get_yellow_red(fix->clid));
}

/** Free the memory occupied by the tokens array and the permanent tokens. */
void
lg_commentary_post_match(void)
{
    g_free(token_rep[option_int("string_lg_commentary_token_team_home", &lg_tokens)]);
    g_free(token_rep[option_int("string_lg_commentary_token_team_away", &lg_tokens)]);
    g_free(token_rep[option_int("string_lg_commentary_token_attendance", &lg_tokens)]);
    g_free(token_rep[option_int("string_lg_commentary_token_league_cup_name", &lg_tokens)]);
    g_free(token_rep[option_int("string_lg_commentary_token_cup_round_name", &lg_tokens)]);
    g_free(token_rep[option_int("string_lg_commentary_token_yellow_limit", &lg_tokens)]);
    g_free(token_rep[option_int("string_lg_commentary_token_team_layer0", &lg_tokens)]);
    g_free(token_rep[option_int("string_lg_commentary_token_team_layer1", &lg_tokens)]);

    g_free(token_rep);
}

/** Load the appropriate commentary file by evaluating
    the language option. */
void
lg_commentary_load_commentary_file_from_option(void)
{
    gchar buf[SMALL], commentary_file[SMALL];
    gchar *cur_locale = NULL;

#ifdef G_OS_UNIX
    cur_locale = setlocale(LC_MESSAGES, NULL);
#endif

    if(strcmp(opt_str("string_opt_language_code"), "C") == 0)
	strcpy(buf, "en");
    else if(strcmp(opt_str("string_opt_language_code"), "") == 0 && cur_locale != NULL)
    {
	strncpy(buf, cur_locale, 2);
	buf[2] = '\0';
    }
    else
	strcpy(buf, opt_str("string_opt_language_code"));

    sprintf(commentary_file, "lg_commentary_%s.xml", buf);

    lg_commentary_load_commentary_file(commentary_file, FALSE);
}

/** Load the commentary file given. Abort if not found and abort=TRUE, 
    load default english file otherwise. */
void
lg_commentary_load_commentary_file(const gchar *commentary_file, gboolean abort)
{
    gchar *file_name = file_find_support_file(commentary_file, FALSE);
    gchar buf[SMALL];

    if(file_name != NULL)
    {
	xml_lg_commentary_read(file_name);
	g_free(file_name);	
    }
    else
    {
	if(abort)
	{
	    sprintf(buf, "lg_commentary_load_commentary_file: file %s not found \n", 
		    file_name);
	    main_exit_program(EXIT_FILE_NOT_FOUND, buf);
	}
	else
	    lg_commentary_load_commentary_file("lg_commentary_en.xml", TRUE);
    }
}
