/*
   xml_loadsave_newspaper.c

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

#include "file.h"
#include "free.h"
#include "misc.h"
#include "xml.h"
#include "xml_loadsave_newspaper.h"
#include "variables.h"

enum
{    
    TAG_NEWS_PAPER = TAG_START_NEWS_PAPER,
    TAG_NEWS_PAPER_NAME,
    TAG_NEWS_PAPER_ARTICLE,
    TAG_NEWS_PAPER_ARTICLE_ID,
    TAG_NEWS_PAPER_ARTICLE_WEEK,
    TAG_NEWS_PAPER_ARTICLE_WEEK_ROUND,
    TAG_NEWS_PAPER_ARTICLE_TITLE_ID,
    TAG_NEWS_PAPER_ARTICLE_SUBTITLE_ID,
    TAG_NEWS_PAPER_ARTICLE_TITLE_SMALL,
    TAG_NEWS_PAPER_ARTICLE_TITLE,
    TAG_NEWS_PAPER_ARTICLE_SUBTITLE,
    TAG_NEWS_PAPER_ARTICLE_USER_IDX,
    TAG_NEWS_PAPER_ARTICLE_CLID,
    TAG_NEWS_PAPER_ARTICLE_CUP_ROUND,
    TAG_END
};

gint state;
NewsPaperArticle new_article;

void
xml_loadsave_newspaper_start_element (GMarkupParseContext *context,
				    const gchar         *element_name,
				    const gchar        **attribute_names,
				    const gchar        **attribute_values,
				    gpointer             user_data,
				    GError             **error)
{
#ifdef DEBUG
    printf("xml_loadsave_newspaper_start_element\n");
#endif

    gint i;
    gint tag = xml_get_tag_from_name(element_name);
    gboolean valid_tag = FALSE;

    for(i=TAG_NEWS_PAPER;i<TAG_END;i++)
	if(tag == i)
	{
	    state = i;
	    valid_tag = TRUE;
	}
	
    if(!valid_tag)
	debug_print_message("xml_loadsave_newspaper_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_newspaper_end_element    (GMarkupParseContext *context,
				     const gchar         *element_name,
				     gpointer             user_data,
				     GError             **error)
{
#ifdef DEBUG
    printf("xml_loadsave_newspaper_end_element\n");
#endif

    gint tag = xml_get_tag_from_name(element_name);

    if(tag == TAG_NEWS_PAPER_NAME ||
       tag == TAG_NEWS_PAPER_ARTICLE)
    {
	state = TAG_NEWS_PAPER;
	if(tag == TAG_NEWS_PAPER_ARTICLE)
	    g_array_append_val(newspaper.articles, new_article);
    }
    else if(tag == TAG_NEWS_PAPER_ARTICLE_WEEK ||
	    tag == TAG_NEWS_PAPER_ARTICLE_WEEK_ROUND ||
	    tag == TAG_NEWS_PAPER_ARTICLE_ID ||
	    tag == TAG_NEWS_PAPER_ARTICLE_TITLE_SMALL ||
	    tag == TAG_NEWS_PAPER_ARTICLE_TITLE ||
	    tag == TAG_NEWS_PAPER_ARTICLE_TITLE_ID ||
	    tag == TAG_NEWS_PAPER_ARTICLE_SUBTITLE ||
	    tag == TAG_NEWS_PAPER_ARTICLE_USER_IDX ||
	    tag == TAG_NEWS_PAPER_ARTICLE_CLID ||
	    tag == TAG_NEWS_PAPER_ARTICLE_CUP_ROUND ||
	    tag == TAG_NEWS_PAPER_ARTICLE_SUBTITLE_ID)
    {
	state = TAG_NEWS_PAPER_ARTICLE;
    }
    else if(tag != TAG_NEWS_PAPER)
	debug_print_message("xml_loadsave_newspaper_end_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_newspaper_text         (GMarkupParseContext *context,
				   const gchar         *text,
				   gsize                text_len,  
				   gpointer             user_data,
				   GError             **error)
{
#ifdef DEBUG
    printf("xml_loadsave_newspaper_text\n");
#endif

    gchar buf[SMALL];
    gint int_value = -1;

    strncpy(buf, text, text_len);
    buf[text_len] = '\0';

    int_value = (gint)g_ascii_strtod(buf, NULL);

    if(state == TAG_NEWS_PAPER_ARTICLE_WEEK)
	new_article.week_number = int_value;
    else if(state == TAG_NEWS_PAPER_ARTICLE_WEEK_ROUND)
	new_article.week_round_number = int_value;
    else if(state == TAG_NEWS_PAPER_ARTICLE_ID)
	new_article.id = int_value;
    else if(state == TAG_NEWS_PAPER_ARTICLE_TITLE_SMALL)
	new_article.title_small = g_strdup(buf);
    else if(state == TAG_NEWS_PAPER_ARTICLE_TITLE)
	new_article.title = g_strdup(buf);
    else if(state == TAG_NEWS_PAPER_ARTICLE_SUBTITLE)
	new_article.subtitle = g_strdup(buf);
    else if(state == TAG_NEWS_PAPER_ARTICLE_TITLE_ID)
	new_article.title_id = int_value;
    else if(state == TAG_NEWS_PAPER_ARTICLE_SUBTITLE_ID)
	new_article.subtitle_id = int_value;
    else if(state == TAG_NEWS_PAPER_ARTICLE_USER_IDX)
	new_article.user_idx = int_value;
    else if(state == TAG_NEWS_PAPER_ARTICLE_CLID)
	new_article.clid = int_value;
    else if(state == TAG_NEWS_PAPER_ARTICLE_CUP_ROUND)
	new_article.cup_round = int_value;
}

void
xml_loadsave_newspaper_read(const gchar *dirname, const gchar *prefix)
{
#ifdef DEBUG
    printf("xml_loadsave_newspaper_read\n");
#endif

    gchar filename[SMALL];
    GMarkupParser parser = {xml_loadsave_newspaper_start_element,
			    xml_loadsave_newspaper_end_element,
			    xml_loadsave_newspaper_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    gsize length;
    GError *error = NULL;

    free_newspaper(TRUE);

    sprintf(filename, "%s%s%s___newspaper.xml", dirname, G_DIR_SEPARATOR_S, prefix);

    context = 
	g_markup_parse_context_new(&parser, 0, NULL, NULL);

    if(!g_file_get_contents(filename, &file_contents, &length, &error))
    {
	debug_print_message("xml_loadsave_news_read: error reading file %s\n", filename);
	misc_print_error(&error, TRUE);
    }

    if(g_markup_parse_context_parse(context, file_contents, length, &error))
    {
	g_markup_parse_context_end_parse(context, NULL);
	g_markup_parse_context_free(context);
	g_free(file_contents);
    }
    else
    {
	debug_print_message("xml_loadsave_news_read: error parsing file %s\n", filename);
	misc_print_error(&error, TRUE);
    }
}

void
xml_loadsave_newspaper_write(const gchar *prefix)
{
#ifdef DEBUG
    printf("xml_loadsave_newspaper_write\n");
#endif

    gint i;
    FILE *fil = NULL;
    gchar buf[SMALL];

    sprintf(buf, "%s___newspaper.xml", prefix);

    file_my_fopen(buf, "w", &fil, TRUE);

    fprintf(fil, "<_%d>\n", TAG_NEWS_PAPER);
    
    for(i=0;i<newspaper.articles->len;i++)
    {
	fprintf(fil, "%s<_%d>\n", I0, TAG_NEWS_PAPER_ARTICLE);
	
	xml_write_int(fil, g_array_index(newspaper.articles, NewsPaperArticle, i).id, 
		      TAG_NEWS_PAPER_ARTICLE_ID, I1);
	xml_write_int(fil, g_array_index(newspaper.articles, NewsPaperArticle, i).week_number, 
		      TAG_NEWS_PAPER_ARTICLE_WEEK, I1);
	xml_write_int(fil, g_array_index(newspaper.articles, NewsPaperArticle, i).week_round_number, 
		      TAG_NEWS_PAPER_ARTICLE_WEEK_ROUND, I1);
        xml_write_string(fil, g_array_index(newspaper.articles, NewsPaperArticle, i).title_small,
                         TAG_NEWS_PAPER_ARTICLE_TITLE_SMALL, I1);
        xml_write_string(fil, g_array_index(newspaper.articles, NewsPaperArticle, i).title,
                         TAG_NEWS_PAPER_ARTICLE_TITLE, I1);
        xml_write_string(fil, g_array_index(newspaper.articles, NewsPaperArticle, i).subtitle,
                         TAG_NEWS_PAPER_ARTICLE_SUBTITLE, I1);
	xml_write_int(fil, g_array_index(newspaper.articles, NewsPaperArticle, i).title_id, 
		      TAG_NEWS_PAPER_ARTICLE_TITLE_ID, I1);
	xml_write_int(fil, g_array_index(newspaper.articles, NewsPaperArticle, i).subtitle_id, 
		      TAG_NEWS_PAPER_ARTICLE_SUBTITLE_ID, I1);
	xml_write_int(fil, g_array_index(newspaper.articles, NewsPaperArticle, i).user_idx, 
		      TAG_NEWS_PAPER_ARTICLE_USER_IDX, I1);
	xml_write_int(fil, g_array_index(newspaper.articles, NewsPaperArticle, i).clid, 
		      TAG_NEWS_PAPER_ARTICLE_CLID, I1);
	xml_write_int(fil, g_array_index(newspaper.articles, NewsPaperArticle, i).cup_round, 
		      TAG_NEWS_PAPER_ARTICLE_CUP_ROUND, I1);

	fprintf(fil, "%s</_%d>\n", I0, TAG_NEWS_PAPER_ARTICLE);
    }

    fprintf(fil, "</_%d>\n", TAG_NEWS_PAPER);

    fclose(fil);
}
