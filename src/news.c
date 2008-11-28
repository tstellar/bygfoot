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
#include "league.h"
#include "main.h"
#include "maths.h"
#include "misc.h"
#include "news.h"
#include "option.h"
#include "team.h"
#include "variables.h"
#include "xml_news.h"

/** The replacement strings for the news tokens. */
GPtrArray *token_rep[2];

/** Generate news for a user live game or a CPU fixture. */
void
news_generate_match(const LiveGame *live_game, const Fixture *fix)
{
#ifdef DEBUG
    printf("news_generate_match\n");
#endif

    gchar title[SMALL], subtitle[SMALL];
    gint title_id, subtitle_id;
    NewsPaperArticle new_article;
    
    token_rep[0] = g_ptr_array_new();
    token_rep[1] = g_ptr_array_new();

    news_set_match_tokens(live_game, fix);

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
	if(news_check_article_condition(&g_array_index(news_array, NewsArticle, i)))
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
    gint i;
    gint result;

    for(i = 0; i < titles->len; i++)
    {
        if(misc_string_replace_all_tokens(token_rep, g_array_index(titles, NewsText, order[i]).text, title))
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

/** Check whether the news conditions are fulfilled and whether
    we can replace all tokens in the news. */
gboolean
news_check_article_condition(const NewsArticle *news_article)
{
#ifdef DEBUG
    printf("news_check_article_condition\n");
#endif

    if(!misc_parse_condition(news_article->condition, token_rep))/*  || */
/*        (repetition == FALSE && query_news_is_repetition(news_article->id))) */
	return FALSE;

/*     strcpy(dest, news->text); */
    
/*     do */
/*     { */
/* 	strcpy(buf, dest); */
/* 	misc_string_replace_tokens(dest, token_rep); */
/* 	misc_string_replace_expressions(dest); */
/*     } */
/*     while(strcmp(buf, dest) != 0); */

/*     return (g_strrstr(dest, "_") == NULL); */
    return TRUE;
}

/** Check whether a news with given id has been used in the last
    few commentaries. Return TRUE if so and FALSE otherwise. */
/* gboolean */
/* query_news_is_repetition(gint id) */
/* { */
/* #ifdef DEBUG */
/*     printf("query_news_is_repetition\n"); */
/* #endif */

/*     const GArray *units = ((LiveGame*)statp)->units; */
/*     gint units_min = units->len - const_int("int_news_check_backwards") - 1; */
/*     gint i, min = MAX(units_min, 0); */

/*     for(i=units->len - 2; i>= min; i--) */
/* 	if(g_array_index(units, LiveGameUnit, i).event.news_id == id) */
/* 	    return TRUE; */

/*     return FALSE; */
/* } */

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
news_set_match_tokens(const LiveGame *live_game, const Fixture *fix_)
{
#ifdef DEBUG
    printf("news_set_match_tokens\n");
#endif

    gchar buf[SMALL];
    const Fixture *fix;
    gint avskill0, avskill1;
    const Cup *cup;
    const CupRound *cupround;

    fix = (live_game == NULL) ? fix_ : live_game->fix;

    token_rep[0] = g_ptr_array_new();
    token_rep[1] = g_ptr_array_new();

    avskill0 = (gint)rint(team_get_average_skill(fix->teams[0], TRUE));
    avskill1 = (gint)rint(team_get_average_skill(fix->teams[1], TRUE));

    fixture_result_to_buf(fix, buf, FALSE);
    misc_token_add(token_rep, 
		   option_int("string_token_result", &tokens),
		   g_strdup(buf));

    misc_token_add(token_rep,
		   option_int("string_token_goals0", &tokens), 
		   misc_int_to_char(fix->result[0][0]));
    misc_token_add(token_rep,
		   option_int("string_token_goals1", &tokens), 
		   misc_int_to_char(fix->result[1][0]));
    misc_token_add(token_rep,
		   option_int("string_token_goal_diff", &tokens), 
		   misc_int_to_char(ABS(fix->result[0][0] - fix->result[1][0])));

    misc_token_add(token_rep,
		   option_int("string_token_team_home", &tokens),
		   g_strdup(fix->teams[0]->name));
    misc_token_add(token_rep,
		   option_int("string_token_team_away", &tokens),
		   g_strdup(fix->teams[1]->name));

    if(fix->result[0][0] != fix->result[1][0])
    {
	misc_token_add(token_rep,
		       option_int("string_token_team_losing", &tokens), 
		       g_strdup(fix->teams[(fix->result[0][0] > fix->result[1][0])]->name));
	misc_token_add(token_rep,
		       option_int("string_token_team_winning", &tokens), 
		       g_strdup(fix->teams[(fix->result[0][0] < fix->result[1][0])]->name));
	misc_token_add(token_rep,
		       option_int("string_token_team_losingn", &tokens), 
		       misc_int_to_char((fix->result[0][0] > fix->result[1][0])));
	misc_token_add(token_rep,
		       option_int("string_token_team_winningn", &tokens), 
		       misc_int_to_char((fix->result[0][0] < fix->result[1][0])));
    }

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
        cup = cup_from_clid(fix->clid);
        cupround = &g_array_index(cup->rounds, CupRound, fix->round);

	misc_token_add(token_rep,
		       option_int("string_token_cup", &tokens),
		       g_strdup("1"));

	cup_get_round_name(cup, fix->round, buf);
	misc_token_add(token_rep,
		       option_int("string_token_cup_round_name", &tokens),
		       g_strdup(buf));

        if(cupround->tables->len > 0)
        {
            misc_token_add(token_rep,
                           option_int("string_token_cup_knockout", &tokens),
                           g_strdup("0"));
            misc_token_add(token_rep,
                           option_int("string_token_cup_round_robin", &tokens),
                           g_strdup("1"));
        }            
        else
        {
            misc_token_add(token_rep,
                           option_int("string_token_cup_knockout", &tokens),
                           g_strdup("1"));
            misc_token_add(token_rep,
                           option_int("string_token_cup_round_robin", &tokens),
                           g_strdup("0"));

        }
        
        if(fix->decisive)
            misc_token_add(token_rep,
                           option_int("string_token_cup_match_winner", &tokens),
                           ((Team*)fixture_winner_of(fix, FALSE))->name);
    }

    misc_print_grouped_int(fix->attendance, buf);
    misc_token_add(token_rep,
		   option_int("string_token_attendance", &tokens),
		   g_strdup(buf));
}

/** Free the memory occupied by the tokens array and the permanent tokens. */
void
news_free_tokens(void)
{
#ifdef DEBUG
    printf("news_free_tokens\n");
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
