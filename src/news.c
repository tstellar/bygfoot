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

    gchar title[SMALL], subtitle[SMALL];
    gint title_id, subtitle_id;
    NewsPaperArticle new_article;
    
    token_rep_news[0] = g_ptr_array_new();
    token_rep_news[1] = g_ptr_array_new();

    news_set_match_tokens(live_game);

    news_select(news[NEWS_ARTICLE_TYPE_MATCH], title, subtitle,
                &title_id, &subtitle_id);

    if(title_id != -1 && subtitle_id != -1)
    {
        new_article.week_number = week;
        new_article.week_round_number = week_round;
        new_article.title = g_strdup(title);
        new_article.subtitle = g_strdup(subtitle);
        new_article.title_id = title_id;
        new_article.subtitle_id = subtitle_id;
//        printf("%s / %s -- %d %d\n", title, subtitle, title_id, subtitle_id);
        g_array_append_val(newspaper.articles, new_article);

/*         gint i; */
/*         for(i = 0; i < newspaper.articles->len; i++) */
/*             printf("%s\n%s\n", g_array_index(newspaper.articles, NewsPaperArticle, i).title,  */
/*                    g_array_index(newspaper.articles, NewsPaperArticle, i).subtitle); */
    }

    news_free_tokens();
}

/** Select a news from the array depending on the tokens
    available and write the texts and ids into the variables. */
void
news_select(const GArray *news_array, gchar *title, gchar *subtitle,
            gint *title_id, gint *subtitle_id)
{
#ifdef DEBUG
    printf("news_select\n");
#endif

    gint i;
    const NewsArticle *article;

    *title_id = *subtitle_id = -1;

    for(i=0;i<news_array->len;i++)
	if(misc_parse_condition(g_array_index(news_array, NewsArticle, i).condition, token_rep_news))
	    break;

    if(i == news_array->len)
	return;

    article = &g_array_index(news_array, NewsArticle, i);

    gint order_titles[article->titles->len],
        order_subtitles[article->subtitles->len];
    news_titles_get_order(article->titles, order_titles);
    news_titles_get_order(article->subtitles, order_subtitles);

    *title_id = news_get_title(article->titles, title, order_titles, TRUE, FALSE);
    if(*title_id == -1)
        *title_id = news_get_title(article->titles, title, order_titles, TRUE, TRUE);

    *subtitle_id = news_get_title(article->subtitles, subtitle, order_subtitles, FALSE, FALSE);
    if(*subtitle_id == -1)
        *subtitle_id = news_get_title(article->subtitles, subtitle, order_subtitles, FALSE, TRUE);

    return;
}

/** Try to find a news article title with valid tokens. */
gint
news_get_title(const GArray *titles, gchar *title, gint *order, 
               gboolean is_title, gboolean ignore_repetition)
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
            if(ignore_repetition || !news_check_for_repetition(result, is_title))
                return result;
            else
                continue;
        }
    }

    return -1;
}

/** Check whether a news article text has occurred in the paper recently. */
gboolean
news_check_for_repetition(gint id, gboolean is_title)
{
#ifdef DEBUG
    printf("news_check_for_repetition\n");
#endif

    gint i;
    gint end;

    end = (newspaper.articles->len < const_int("int_news_repetition_check_number")) ?
        0 : newspaper.articles->len - const_int("int_news_repetition_check_number");

    for(i = newspaper.articles->len - 1; i >= end; i--)
        if((is_title && g_array_index(newspaper.articles, NewsPaperArticle, i).title_id == id) ||
           (!is_title && g_array_index(newspaper.articles, NewsPaperArticle, i).subtitle_id == id))
            return TRUE;

    return FALSE;
}


/** Write a random order of indices into the integer array
    (only depending on the priority values of the commentaries).
    I don't like this implementation of ordering the commentaries
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
    news_set_scorer_tokens(&live_game->stats);
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
                    sprintf(scorer_str, "%s and %s (%d)", buf, scorer, scorer_goals);
                else
                    sprintf(scorer_str, "%s and %s", buf, scorer);
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
        
/*         printf("%d +%s+ +%s+ %d\n", i, scorer_str, high_scorer, max_goals); */

        if(strcmp(scorer_str, "") != 0)
        {
            sprintf(buf, "string_token_multiple_scorers%d", i);
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
		       misc_int_to_char(league_from_clid(fix->teams[0]->clid)->layer -
					league_from_clid(fix->teams[1]->clid)->layer));

    misc_token_add(token_rep_news,
		   option_int("string_token_league_cup_name", &tokens),
		   g_strdup(league_cup_get_name_string(fix->clid)));

    misc_token_add_bool(token_rep_news,
                        option_int("string_token_cup", &tokens),
                        (fix->clid >= ID_CUP_START));

    if(fix->clid >= ID_CUP_START)
    {
        cup = cup_from_clid(fix->clid);
        cupround = &g_array_index(cup->rounds, CupRound, fix->round);

	cup_get_round_name(cup, fix->round, buf);
	misc_token_add(token_rep_news,
		       option_int("string_token_cup_round_name", &tokens),
		       g_strdup(buf));

        misc_token_add_bool(token_rep_news,
                           option_int("string_token_cup_knockout", &tokens),
                            (cupround->tables->len == 0));
        
        if(fix->decisive)
            misc_token_add(token_rep_news,
                           option_int("string_token_cup_match_winner", &tokens),
                           g_strdup(((Team*)fixture_winner_of(fix, FALSE))->name));
    }
}

void
news_set_fixture_tokens(const Fixture *fix)
{
#ifdef DEBUG
    printf("news_set_fixture_tokens\n");
#endif

    gchar buf[SMALL];
    gint avskill0, avskill1;

    avskill0 = (gint)rint(team_get_average_skill(fix->teams[0], TRUE));
    avskill1 = (gint)rint(team_get_average_skill(fix->teams[1], TRUE));

    fixture_result_to_buf(fix, buf, FALSE);
    misc_token_add(token_rep_news, 
		   option_int("string_token_result", &tokens),
		   g_strdup(buf));

    misc_print_grouped_int(fix->attendance, buf);
    misc_token_add(token_rep_news,
		   option_int("string_token_attendance", &tokens),
		   g_strdup(buf));    

    misc_token_add(token_rep_news,
		   option_int("string_token_goals0", &tokens), 
		   misc_int_to_char(fix->result[0][0]));
    misc_token_add(token_rep_news,
		   option_int("string_token_goals1", &tokens), 
		   misc_int_to_char(fix->result[1][0]));
    misc_token_add(token_rep_news,
		   option_int("string_token_goal_diff", &tokens), 
		   misc_int_to_char(ABS(fix->result[0][0] - fix->result[1][0])));

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

/** Free the memory occupied by the tokens array and the permanent tokens. */
void
news_free_tokens(void)
{
#ifdef DEBUG
    printf("news_free_tokens\n");
#endif

    gint i;

    for(i=token_rep_news[0]->len - 1;i >= 0; i--)
    {
/*         printf("%s/%s\n", (gchar*)g_ptr_array_index(token_rep_news[0], i), */
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

    if(fixture_user_team_involved(live_game->fix) != -1)
        return TRUE;

    user_leagues = g_array_new(FALSE, FALSE, sizeof(gint));

    if(live_game->fix->clid >= ID_CUP_START &&
       live_game->fix->round >= cup_from_clid(live_game->fix->clid)->rounds->len - 4)
        return TRUE;

    for(i = 0; i < users->len; i++)
        if(!query_misc_integer_is_in_g_array(usr(i).tm->clid, user_leagues))
            g_array_append_val(user_leagues, usr(i).tm->clid);

    if(query_misc_integer_is_in_g_array(live_game->fix->clid, user_leagues))
    {
        g_array_free(user_leagues, TRUE);
        return TRUE;
    }

    g_array_free(user_leagues, TRUE);
    return FALSE;
}
