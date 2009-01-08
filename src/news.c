/*
   news.c

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
#include <math.h>

#include "cup.h"
#include "file.h"
#include "fixture.h"
#include "language.h"
#include "lg_commentary.h"
#include "live_game.h"
#include "league.h"
#include "main.h"
#include "maths.h"
#include "misc.h"
#include "news.h"
#include "option.h"
#include "team.h"
#include "user.h"
#include "variables.h"
#include "xml_news.h"

/** The replacement strings for the news tokens. */
GPtrArray *token_rep_news[2];

/** Generate news for a user live game or a CPU fixture. */
void
news_generate_match(const LiveGame *live_game)
{
#ifdef DEBUG
    printf("news_generate_match\n");
#endif

    gchar title_small[SMALL], buf[SMALL],
	title[SMALL], subtitle[SMALL];
    gint title_id, subtitle_id, article_id;
    NewsPaperArticle new_article;
    
    token_rep_news[0] = g_ptr_array_new();
    token_rep_news[1] = g_ptr_array_new();

    news_set_match_tokens(live_game);

    news_select(news[NEWS_ARTICLE_TYPE_MATCH], title, subtitle,
                &article_id, &title_id, &subtitle_id);

    if(title_id != -1 && subtitle_id != -1)
    {
        new_article.id = article_id;
        new_article.week_number = week;
        new_article.week_round_number = week_round;

	fixture_result_to_buf(live_game->fix, buf, FALSE);
	sprintf(title_small, "%s %s %s", 
		live_game->fix->teams[0]->name, buf, live_game->fix->teams[1]->name);
        new_article.title_small = g_strdup(title_small);

        new_article.title = g_strdup(title);
        new_article.subtitle = g_strdup(subtitle);
        new_article.title_id = title_id;
        new_article.subtitle_id = subtitle_id;
        new_article.user_idx = fixture_user_team_involved(live_game->fix);
        new_article.clid = live_game->fix->clid;
        new_article.cup_round = live_game->fix->round;

        if(counters[COUNT_NEW_NEWS] == 0)
            counters[COUNT_NEW_NEWS] = 2;

        if(new_article.user_idx != -1)
            counters[COUNT_NEW_NEWS] = 1;

        g_array_append_val(newspaper.articles, new_article);
    }

    news_free_tokens();
}

/** Select a news from the array depending on the tokens
    available and write the texts and ids into the variables. */
void
news_select(const GArray *news_array, gchar *title, gchar *subtitle,
            gint *article_id, gint *title_id, gint *subtitle_id)
{
#ifdef DEBUG
    printf("news_select\n");
#endif

    gint i;
    const NewsArticle *article;
    gint order_articles[news_array->len];
    gint to_check;

    news_articles_get_order(news_array, order_articles);

    *title_id = *subtitle_id = -1;

    for(i=0;i<news_array->len;i++)
    {
	if((!news_check_article_for_repetition(g_array_index(news_array, NewsArticle, order_articles[i]).id) ||
            g_array_index(news_array, NewsArticle, order_articles[i]).priority > 20) &&
           misc_parse_condition(g_array_index(news_array, NewsArticle, order_articles[i]).condition, token_rep_news))
	    break;        
    }

    if(i == news_array->len)
        for(i=0;i<news_array->len;i++)
            if(misc_parse_condition(g_array_index(news_array, NewsArticle, order_articles[i]).condition, token_rep_news))
                break;

    if(i == news_array->len)
	return;

    article = &g_array_index(news_array, NewsArticle, order_articles[i]);

    *article_id = article->id;

    gint order_titles[article->titles->len],
        order_subtitles[article->subtitles->len];
    news_titles_get_order(article->titles, order_titles);
    news_titles_get_order(article->subtitles, order_subtitles);

    *title_id = news_get_title(article->titles, title, order_titles, TRUE, FALSE, 
                               const_int("int_news_repetition_max_check_number"));
    if(*title_id == -1)
        for(to_check = const_int("int_news_repetition_max_check_number") - 1; 
            to_check >= const_int("int_news_repetition_min_check_number");
            to_check--)
        {
            *title_id = news_get_title(article->titles, title, order_titles, TRUE, 
                                       (to_check == const_int("int_news_repetition_min_check_number")), to_check);   
            if(*title_id == -1)
                break;
        }

    *subtitle_id = news_get_title(article->subtitles, subtitle, order_subtitles, FALSE, FALSE,
                                  const_int("int_news_repetition_max_check_number"));
    if(*subtitle_id == -1)
        for(to_check = const_int("int_news_repetition_max_check_number") - 1; 
            to_check >= const_int("int_news_repetition_min_check_number");
            to_check--)
        {
            *subtitle_id = news_get_title(article->subtitles, subtitle, order_subtitles, FALSE, 
                                       (to_check == const_int("int_news_repetition_min_check_number")), to_check);   
            if(*subtitle_id == -1)
                break;
        }

    return;
}

/** Try to find a news article title with valid tokens. */
gint
news_get_title(const GArray *titles, gchar *title, gint *order, 
               gboolean is_title, gboolean ignore_repetition, gint to_check)
{
#ifdef DEBUG
    printf("news_get_title\n");
#endif

    gint i;
    gint result;

    for(i = 0; i < titles->len; i++)
    {
        if(misc_parse_condition(g_array_index(titles, NewsText, order[i]).condition, token_rep_news) &&
           misc_string_replace_all_tokens(token_rep_news, g_array_index(titles, NewsText, order[i]).text, title))
        {
            result = g_array_index(titles, NewsText, order[i]).id;
            if(ignore_repetition || !news_check_title_for_repetition(result, is_title, to_check))
                return result;                
            else
                continue;
        }
    }

    return -1;
}

/** Check whether a news article text has occurred in the paper recently. */
gboolean
news_check_title_for_repetition(gint id, gboolean is_title, gint to_check)
{
#ifdef DEBUG
    printf("news_check_title_for_repetition\n");
#endif

    gint i;
    gint end;

    end = (newspaper.articles->len < to_check) ?
        0 : newspaper.articles->len - to_check;

    for(i = newspaper.articles->len - 1; i >= end; i--)
        if((is_title && g_array_index(newspaper.articles, NewsPaperArticle, i).title_id == id) ||
           (!is_title && g_array_index(newspaper.articles, NewsPaperArticle, i).subtitle_id == id))
            return TRUE;

    return FALSE;
}

/** Check recent news for article repetition. */
gboolean
news_check_article_for_repetition(gint id)
{
#ifdef DEBUG
    printf("news_check_article_for_repetition\n");
#endif

    gint i;
    gint end;

    end = (newspaper.articles->len < const_int("int_news_repetition_max_check_number")) ?
        0 : newspaper.articles->len - const_int("int_news_repetition_max_check_number");

    for(i = newspaper.articles->len - 1; i >= end; i--)
        if(g_array_index(newspaper.articles, NewsPaperArticle, i).id == id)
            return TRUE;

    return FALSE;
}

/** Write a random order of indices into the integer array
    (only depending on the priority values of the news titles).
    I don't like this implementation of ordering the titles
    according to their priority :-P can't think of a better one, though. */
void
news_titles_get_order(const GArray *titles, gint *order)
{
#ifdef DEBUG
    printf("news_titles_get_order\n");
#endif

    gint i, j, order_idx = 0;
    gint priority_sum = 0, bounds[titles->len + 1];

    bounds[0] = 0;

    for(i=0;i<titles->len;i++)
    {
	priority_sum += g_array_index(titles, NewsText, i).priority;
	bounds[i + 1] = priority_sum;
	order[i] = -1;
    }

    gint permutation[priority_sum];

    math_generate_permutation(permutation, 1, priority_sum);

    for(i=0;i<priority_sum;i++)
    {
	if(order_idx == titles->len)
	    break;

	for(j=0;j<titles->len;j++)
	    if(bounds[j] < permutation[i] && permutation[i] <= bounds[j + 1])
	    {
		if(!query_integer_is_in_array(j, order, titles->len))
		{
		    order[order_idx] = j;
		    order_idx++;
		}

		break;
	    }
    }
}

/** Write a random order of indices into the integer array
    (only depending on the priority values of the news articles).
    I don't like this implementation of ordering the articles
    according to their priority :-P can't think of a better one, though. */
void
news_articles_get_order(const GArray *articles, gint *order)
{
#ifdef DEBUG
    printf("news_articles_get_order\n");
#endif

    gint i, j, order_idx = 0;
    gint priority_sum = 0, bounds[articles->len + 1];

    bounds[0] = 0;

    for(i=0;i<articles->len;i++)
    {
	priority_sum += g_array_index(articles, NewsArticle, i).priority;
	bounds[i + 1] = priority_sum;
	order[i] = -1;
    }

    gint permutation[priority_sum];

    math_generate_permutation(permutation, 1, priority_sum);

    for(i=0;i<priority_sum;i++)
    {
	if(order_idx == articles->len)
	    break;

	for(j=0;j<articles->len;j++)
	    if(bounds[j] < permutation[i] && permutation[i] <= bounds[j + 1])
	    {
		if(!query_integer_is_in_array(j, order, articles->len))
		{
		    order[order_idx] = j;
		    order_idx++;
		}

		break;
	    }
    }
}

/** Set match-related tokens for the news. */
void
news_set_match_tokens(const LiveGame *live_game)
{
#ifdef DEBUG
    printf("news_set_match_tokens\n");
#endif

    lg_commentary_set_stats_tokens(&live_game->stats, token_rep_news);
    news_set_fixture_tokens(live_game->fix);
    news_set_league_cup_tokens(live_game->fix);
    news_set_streak_tokens(live_game->fix);
    news_set_scorer_tokens(&live_game->stats);

    if(live_game->fix->clid < ID_CUP_START)
        news_set_rank_tokens(live_game->fix);
}

/** Set overall and league streak tokens for the two teams. */
void
news_set_streak_tokens(const Fixture *fix)
{
#ifdef DEBUG
    printf("news_set_streak_tokens\n");
#endif

    gint i, j, k,
	streak_won, streak_lost, streak_unbeaten;
    gboolean draw;
    gint res[2];
    gchar buf[SMALL], buf2[SMALL];
    GPtrArray *latest_fixtures;
    
    for(i = 0; i < 2; i++)
    {
	for(k = 0; k < 2; k++)
	{
	    latest_fixtures = fixture_get_latest(fix->teams[i], (k == 0));
	    streak_won = streak_lost = streak_unbeaten = 0;
            draw = FALSE;

	    for(j=latest_fixtures->len - 1;j>=0;j--)
	    {                
		res[0] = math_sum_int_array(((Fixture*)g_ptr_array_index(latest_fixtures, j))->result[0], 3);
		res[1] = math_sum_int_array(((Fixture*)g_ptr_array_index(latest_fixtures, j))->result[1], 3);

		if(res[0] == res[1])
                {
                    if(streak_lost == 0)
                    {
                        draw = TRUE;
                        streak_unbeaten++;                        
                    }
                    else
                        break;
                }
		else if(res[(((Fixture*)g_ptr_array_index(latest_fixtures, j))->teams[0] == fix->teams[i])] >
			res[(((Fixture*)g_ptr_array_index(latest_fixtures, j))->teams[0] != fix->teams[i])])
		{
		    if(streak_won == 0)
			streak_lost++;
		    else
			break;
		}
		else
		{
		    if(streak_lost == 0)
                    {
                        streak_unbeaten++;

                        if(!draw)
                            streak_won++;
                    }
		    else
			break;
		}	    	    
	    }

	    if(k == 0)
		strcpy(buf2, "");
	    else
		strcpy(buf2, "league_");

	    sprintf(buf, "string_token_streak_%swon%d", buf2, i);
	    misc_token_add(token_rep_news,
			   option_int(buf, &tokens),
			   misc_int_to_char(streak_won));
	    sprintf(buf, "string_token_streak_%slost%d", buf2, i);
	    misc_token_add(token_rep_news,
			   option_int(buf, &tokens),
			   misc_int_to_char(streak_lost));
	    sprintf(buf, "string_token_streak_%sunbeaten%d", buf2, i);
	    misc_token_add(token_rep_news,
			   option_int(buf, &tokens),
			   misc_int_to_char(streak_unbeaten));

	    g_ptr_array_free(latest_fixtures, TRUE);
	}
    }
}

void
news_set_scorer_tokens(const LiveGameStats *stats)
{
#ifdef DEBUG
    printf("news_set_scorer_tokens\n");
#endif

    gint i, j, k;
    GPtrArray *scorers[2];
    GArray *goals[2];
    gchar buf[SMALL];
    gchar scorer_str[SMALL];
    const gchar *scorer;
    gchar high_scorer[SMALL];
    gint scorer_goals;
    gint max_goals;

    for(i = 0; i < 2; i++)
    {
        scorers[i] = g_ptr_array_new();
        goals[i] = g_array_new(FALSE, FALSE, sizeof(gint));
 
       for(j = 0; j < stats->players[i][LIVE_GAME_STAT_ARRAY_SCORERS]->len; j++)
        {
            scorer = (gchar*)g_ptr_array_index(stats->players[i][LIVE_GAME_STAT_ARRAY_SCORERS], j);

            for(k=0;k<scorers[i]->len;k++)
            {
                if(strcmp(scorer, (gchar*)g_ptr_array_index(scorers[i], k)) == 0)
                {
                    g_array_index(goals[i], gint, k) = g_array_index(goals[i], gint, k) + 1;
                    break;
                }           
            }
            
            if(k == scorers[i]->len)
            {
                gint onegoal = 1;
                g_ptr_array_add(scorers[i], (gpointer)scorer);
                g_array_append_val(goals[i], onegoal);
            }            
        }
        
        max_goals = 0;
        strcpy(buf, "");
        strcpy(scorer_str, "");
        strcpy(high_scorer, "");

        for(j = 0; j < scorers[i]->len; j++)
        {
            scorer_goals = g_array_index(goals[i], gint, j);
            scorer = (gchar*)g_ptr_array_index(scorers[i], j);

            if(scorer_goals > max_goals)
            {
                max_goals = scorer_goals;
                strcpy(high_scorer, scorer);
            }

            if(j == 0)
            {
                if(scorer_goals > 1)
                    sprintf(scorer_str, "%s (%d)", scorer, scorer_goals);
                else
                    sprintf(scorer_str, "%s", scorer);                    
            }
            else if(j == scorers[i]->len - 1 && j != 0)
            {
                if(scorer_goals > 1)
                    sprintf(scorer_str, _("%s and %s (%d)"), buf, scorer, scorer_goals);
                else
                    sprintf(scorer_str, _("%s and %s"), buf, scorer);
            }
            else
            {
                if(scorer_goals > 1)
                    sprintf(scorer_str, "%s, %s (%d)", buf, scorer, scorer_goals);
                else
                    sprintf(scorer_str, "%s, %s", buf, scorer);                    
            }

            strcpy(buf, scorer_str);
        }
        
        if(strcmp(scorer_str, "") != 0)
        {
            sprintf(buf, "string_token_bool_multiple_scorers%d", i);
            misc_token_add_bool(token_rep_news,
                                option_int(buf, &tokens),
                                (scorers[i]->len > 1));

            sprintf(buf, "string_token_scorers%d", i);
            misc_token_add(token_rep_news,
                           option_int(buf, &tokens),
                           g_strdup(scorer_str));
            sprintf(buf, "string_token_highscorer%d", i);
            misc_token_add(token_rep_news,
                           option_int(buf, &tokens),
                           g_strdup(high_scorer));
            sprintf(buf, "string_token_highscorer_goals%d", i);
            misc_token_add(token_rep_news,
                           option_int(buf, &tokens),
                           misc_int_to_char(max_goals));            
        }

        g_ptr_array_free(scorers[i], TRUE);
        g_array_free(goals[i], TRUE);
    }
}

void
news_set_league_cup_tokens(const Fixture *fix)
{
#ifdef DEBUG
    printf("news_set_league_cup_tokens\n");
#endif

    gchar buf[SMALL];
    const Cup *cup;
    const CupRound *cupround;
    const Team *tm;

    if(fix->teams[0]->clid < ID_CUP_START)
	misc_token_add(token_rep_news,
		       option_int("string_token_team_layer0", &tokens),
		       misc_int_to_char(league_from_clid(fix->teams[0]->clid)->layer));
    if(fix->teams[1]->clid < ID_CUP_START)
	misc_token_add(token_rep_news,
		       option_int("string_token_team_layer1", &tokens),
		       misc_int_to_char(league_from_clid(fix->teams[1]->clid)->layer));
	
    if(fix->teams[0]->clid < ID_CUP_START &&
       fix->teams[1]->clid < ID_CUP_START)
	misc_token_add(token_rep_news,
		       option_int("string_token_team_layerdiff", &tokens),
		       misc_int_to_char(ABS(league_from_clid(fix->teams[0]->clid)->layer -
                                            league_from_clid(fix->teams[1]->clid)->layer)));

    misc_token_add(token_rep_news,
		   option_int("string_token_league_cup_name", &tokens),
		   g_strdup(league_cup_get_name_string(fix->clid)));

    misc_token_add_bool(token_rep_news,
                        option_int("string_token_bool_cup", &tokens),
                        (fix->clid >= ID_CUP_START));

    if(fix->clid >= ID_CUP_START)
    {
        cup = cup_from_clid(fix->clid);
        cupround = &g_array_index(cup->rounds, CupRound, fix->round);

        if(query_league_cup_has_property(cup->id, "international"))
            misc_token_add(token_rep_news,
                           option_int("string_token_team_layerdiff", &tokens),
                           misc_int_to_char(0));
        
        misc_token_add(token_rep_news,
                       option_int("string_token_cup_stage", &tokens),
                       misc_int_to_char(cup->rounds->len - fix->round));

        misc_token_add(token_rep_news,
                       option_int("string_token_bool_cup_neutral", &tokens),
                       misc_int_to_char(cupround->neutral));

	cup_get_round_name(cup, fix->round, buf);
	misc_token_add(token_rep_news,
		       option_int("string_token_cup_round_name", &tokens),
		       g_strdup(buf));

        misc_token_add_bool(token_rep_news,
                           option_int("string_token_bool_cup_knockout", &tokens),
                            (cupround->tables->len == 0));
        
        misc_token_add_bool(token_rep_news,
                            option_int("string_token_bool_cup_home_away", &tokens),
                            cupround->home_away);
        
        misc_token_add_bool(token_rep_news, 
                            option_int("string_token_bool_cup_first_leg", &tokens),
                            (cupround->home_away && !fix->second_leg));
        misc_token_add_bool(token_rep_news, 
                            option_int("string_token_bool_cup_second_leg", &tokens),
                            (cupround->home_away && fix->second_leg));

        if(fix->decisive)
        {
            tm = (Team*)fixture_winner_of(fix, FALSE);
            misc_token_add(token_rep_news,
                           option_int("string_token_cup_match_winner", &tokens),
                           g_strdup(tm->name));
            misc_token_add(token_rep_news,
                           option_int("string_token_cup_match_loser", &tokens),
                           g_strdup(fix->teams[fix->teams[1] != tm]->name));
            misc_token_add(token_rep_news,
                           option_int("string_token_cup_match_winnern", &tokens),
                           misc_int_to_char(fix->teams[1] == tm));
            misc_token_add(token_rep_news,
                           option_int("string_token_cup_match_losern", &tokens),
                           misc_int_to_char(fix->teams[1] != tm));
        }
        
        misc_token_add_bool(token_rep_news,
                            option_int("string_token_bool_cup_national", &tokens),
                            query_league_cup_has_property(cup->id, "national"));
        misc_token_add_bool(token_rep_news,
                            option_int("string_token_bool_cup_international", &tokens),
                            query_league_cup_has_property(cup->id, "international"));
        misc_token_add_bool(token_rep_news,
                            option_int("string_token_bool_cup_promrel", &tokens),
                            query_league_cup_has_property(cup->id, "promotion"));
        misc_token_add_bool(token_rep_news,
                            option_int("string_token_bool_cup_aux", &tokens),
                            query_league_cup_has_property(cup->id, "hide") ||
                            query_league_cup_has_property(cup->id, "omit_from_history"));
    }
    else
    {
        misc_token_add_bool(token_rep_news,
                            option_int("string_token_bool_cup_knockout", &tokens),
                            FALSE);
        
        misc_token_add_bool(token_rep_news,
                            option_int("string_token_bool_cup_home_away", &tokens),
                            FALSE);
        
        misc_token_add_bool(token_rep_news, 
                            option_int("string_token_bool_cup_first_leg", &tokens),
                            FALSE);
        misc_token_add_bool(token_rep_news, 
                            option_int("string_token_bool_cup_second_leg", &tokens),
                            FALSE);
    }
}

/** Set the news tokens related to the fixture. */
void
news_set_fixture_tokens(const Fixture *fix)
{
#ifdef DEBUG
    printf("news_set_fixture_tokens\n");
#endif

    gchar buf[SMALL];
    gint res[2];
    gint avskill0, avskill1;
    const Fixture *first_leg;
    gint goaldiffaggr;

    avskill0 = (gint)rint(team_get_average_skill(fix->teams[0], TRUE));
    avskill1 = (gint)rint(team_get_average_skill(fix->teams[1], TRUE));
    res[0] = math_sum_int_array(fix->result[0], 3);
    res[1] = math_sum_int_array(fix->result[1], 3);
    goaldiffaggr = ABS(res[0] - res[1]);

    fixture_result_to_buf(fix, buf, FALSE);
    misc_token_add(token_rep_news, 
		   option_int("string_token_result", &tokens),
		   g_strdup(buf));

    fixture_result_to_buf(fix, buf, (res[0] < res[1]));
    misc_token_add(token_rep_news, 
		   option_int("string_token_result_rew", &tokens),
		   g_strdup(buf));

    fixture_result_to_buf(fix, buf, (res[0] > res[1]));
    misc_token_add(token_rep_news, 
		   option_int("string_token_result_rel", &tokens),
		   g_strdup(buf));

    misc_token_add_bool(token_rep_news, 
                        option_int("string_token_bool_cup_penalties", &tokens),
                        (fix->result[0][2] + fix->result[1][2] != 0));

    misc_token_add_bool(token_rep_news, 
                        option_int("string_token_bool_cup_extra", &tokens),
                        (fix->result[0][2] + fix->result[1][2] == 0 &&
                         fix->result[0][1] + fix->result[1][1] != 0));

    misc_print_grouped_int(math_round_integer(fix->attendance, 2), buf);
    misc_token_add(token_rep_news,
		   option_int("string_token_attendance", &tokens),
		   g_strdup(buf));    

    misc_token_add(token_rep_news,
		   option_int("string_token_team_avskill0", &tokens), 
		   misc_int_to_char(avskill0));
    misc_token_add(token_rep_news,
		   option_int("string_token_team_avskill1", &tokens), 
		   misc_int_to_char(avskill1));
    misc_token_add(token_rep_news,
		   option_int("string_token_team_avskilldiff", &tokens), 
		   misc_int_to_char(ABS(avskill0 - avskill1)));

    misc_token_add(token_rep_news,
		   option_int("string_token_goals0", &tokens), 
		   misc_int_to_char(fix->result[0][0]));
    misc_token_add(token_rep_news,
		   option_int("string_token_goals1", &tokens), 
		   misc_int_to_char(fix->result[1][0]));
    misc_token_add(token_rep_news,
		   option_int("string_token_goal_diff", &tokens), 
		   misc_int_to_char(goaldiffaggr));

    if(fix->clid >= ID_CUP_START)
    {
        first_leg = fixture_get_first_leg(fix, TRUE);

        if(first_leg != NULL)
            goaldiffaggr = ABS(fix->result[0][0] + fix->result[0][1] + first_leg->result[1][0] -
                               fix->result[1][0] - fix->result[1][1] - first_leg->result[0][0]);
    }

    misc_token_add(token_rep_news,
                   option_int("string_token_goal_diff_aggregate", &tokens), 
                   misc_int_to_char(goaldiffaggr));   
    
    misc_token_add(token_rep_news,
		   option_int("string_token_team_home", &tokens),
		   g_strdup(fix->teams[0]->name));
    misc_token_add(token_rep_news,
		   option_int("string_token_team_away", &tokens),
		   g_strdup(fix->teams[1]->name));

    if(fix->result[0][0] != fix->result[1][0])
    {
	misc_token_add(token_rep_news,
		       option_int("string_token_team_losing", &tokens), 
		       g_strdup(fix->teams[(fix->result[0][0] > fix->result[1][0])]->name));
	misc_token_add(token_rep_news,
		       option_int("string_token_team_winning", &tokens), 
		       g_strdup(fix->teams[(fix->result[0][0] < fix->result[1][0])]->name));
	misc_token_add(token_rep_news,
		       option_int("string_token_team_losingn", &tokens), 
		       misc_int_to_char((fix->result[0][0] > fix->result[1][0])));
	misc_token_add(token_rep_news,
		       option_int("string_token_team_winningn", &tokens), 
		       misc_int_to_char((fix->result[0][0] < fix->result[1][0])));
    }    
}

/** Set the rank tokens of the involved teams. */
void
news_set_rank_tokens(const Fixture *fix)
{
#ifdef DEBUG
    printf("news_set_rank_tokens\n");
#endif
    
    gint i, j;
    gchar buf[SMALL];
    const Table *table;

    for(i = 0; i < 2; i++)
    {
        table = league_table(league_from_clid(fix->teams[i]->clid));
        for(j = 0; j < table->elements->len; j++)
            if(g_array_index(table->elements, TableElement, j).team == fix->teams[i])
            {                
                sprintf(buf, "string_token_rank%d", i);
                misc_token_add(token_rep_news, option_int(buf, &tokens), misc_int_to_char(j + 1));
                sprintf(buf, "string_token_oldrank%d", i);
                misc_token_add(token_rep_news, option_int(buf, &tokens), 
                               misc_int_to_char(g_array_index(table->elements, TableElement, j).old_rank));
                break;
            }
    }
}

/** Free the memory occupied by the tokens array and the permanent tokens. */
void
news_free_tokens(void)
{
#ifdef DEBUG
    printf("news_free_tokens\n");
#endif

    gint i;

/*     printf("-------------------------------------\n"); */
    for(i=token_rep_news[0]->len - 1;i >= 0; i--)
    {
/*         printf("%s // %s\n", */
/*                (gchar*)g_ptr_array_index(token_rep_news[0], i), */
/*                (gchar*)g_ptr_array_index(token_rep_news[1], i)); */
	g_free(g_ptr_array_index(token_rep_news[0], i));
	g_free(g_ptr_array_index(token_rep_news[1], i));
    }

    g_ptr_array_free(token_rep_news[0], TRUE);
    g_ptr_array_free(token_rep_news[1], TRUE);
}

/** Load the appropriate news file by evaluating
    the language option. */
void
news_load_news_file_from_option(void)
{
#ifdef DEBUG
    printf("news_load_news_file_from_option\n");
#endif

    gchar buf[SMALL], news_file[SMALL];

    language_get_code(buf);

    sprintf(news_file, "news_%s.xml", buf);

    news_load_news_file(news_file, FALSE);
}

/** Load the news file given. Abort if not found and abort=TRUE,
    load default english file otherwise. */
void
news_load_news_file(const gchar *news_file, gboolean abort)
{
#ifdef DEBUG
    printf("news_load_news_file\n");
#endif

    gchar *file_name = NULL;

    if(g_file_test(news_file, G_FILE_TEST_EXISTS))
    {
	xml_news_read(news_file);
	return;
    }
    
    file_name = file_find_support_file(news_file, FALSE);

    if(file_name != NULL)
    {
	xml_news_read(file_name);
	g_free(file_name);
    }
    else
    {
	if(abort)
	    main_exit_program(EXIT_FILE_NOT_FOUND,
			      "news_load_news_file: file %s not found \n",
			      file_name);
	else
	    news_load_news_file("news_en.xml", TRUE);
    }
}

/** Find out if the match is interesting from a newspaper article
    generation point of view. */
gboolean
news_check_match_relevant(const LiveGame *live_game)
{
#ifdef DEBUG
    printf("news_check_match_relevant\n");
#endif

    gint i;
    GArray *user_leagues;

    if(fixture_user_team_involved(live_game->fix) != -1 &&
       opt_int("int_opt_news_create_user"))
        return TRUE;

    if(live_game->fix->clid >= ID_CUP_START &&
       opt_int("int_opt_news_create_cup"))
        return TRUE;

    user_leagues = g_array_new(FALSE, FALSE, sizeof(gint));

    for(i = 0; i < users->len; i++)
        if(!query_misc_integer_is_in_g_array(usr(i).tm->clid, user_leagues))
            g_array_append_val(user_leagues, usr(i).tm->clid);

    if(query_misc_integer_is_in_g_array(live_game->fix->clid, user_leagues) &&
       opt_int("int_opt_news_create_league"))
    {
        g_array_free(user_leagues, TRUE);
        return TRUE;
    }

    g_array_free(user_leagues, TRUE);
    return FALSE;
}
