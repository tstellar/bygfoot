/*
  xml_news.c

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

#include "free.h"
#include "news.h"
#include "misc.h"
#include "variables.h"
#include "xml_news.h"

#define TAG_NEWS "news"
#define TAG_ARTICLE "news_article"
#define TAG_ARTICLE_TYPE "type"
#define TAG_ARTICLE_CONDITION "condition"
#define TAG_ARTICLE_PRIORITY "priority"
#define TAG_ARTICLE_TITLE "title"
#define TAG_ARTICLE_SUBTITLE "subtitle"

#define ATT_NAME_TEXT_PRIORITY "priority"
#define ATT_NAME_TEXT_CONDITION "condition"

#define ARTICLE_TYPE_NAME_MATCH "match"
#define ARTICLE_TYPE_NAME_FINANCES "finances"
#define ARTICLE_TYPE_NAME_STAR_PLAYER_TRANSFER "star_player_transfer"
#define ARTICLE_TYPE_NAME_LEAGUE_CHAMPION "league_champion"
#define ARTICLE_TYPE_NAME_CUP_QUALIFICATION "cup_qualification"
#define ARTICLE_TYPE_NAME_RELEGATION "relegation"

enum XmlNewsStates
{
    STATE_NEWS = 0,
    STATE_ARTICLE,
    STATE_ARTICLE_TYPE,
    STATE_ARTICLE_CONDITION,
    STATE_ARTICLE_PRIORITY,
    STATE_ARTICLE_TITLE,
    STATE_ARTICLE_SUBTITLE,
    STATE_END
};

gint state, article_idx, priority;
gchar *condition;
NewsArticle new_article;
NewsText new_title, new_subtitle;

/** Return the appropriate enum integer going with the type string. */
gint
xml_news_article_type_to_int(const gchar *type_string)
{
#ifdef DEBUG
    printf("xml_news_type_to_int\n");
#endif

    gint return_value = -1;

    if(strcmp(type_string, ARTICLE_TYPE_NAME_MATCH) == 0)
	return_value = NEWS_ARTICLE_TYPE_MATCH;
    else if(strcmp(type_string, ARTICLE_TYPE_NAME_FINANCES) == 0)
	return_value = NEWS_ARTICLE_TYPE_FINANCES;
    else if(strcmp(type_string, ARTICLE_TYPE_NAME_STAR_PLAYER_TRANSFER) == 0)
	return_value = NEWS_ARTICLE_TYPE_STAR_PLAYER_TRANSFER;
    else if(strcmp(type_string, ARTICLE_TYPE_NAME_LEAGUE_CHAMPION) == 0)
	return_value = NEWS_ARTICLE_TYPE_LEAGUE_CHAMPION;
    else if(strcmp(type_string, ARTICLE_TYPE_NAME_CUP_QUALIFICATION) == 0)
	return_value = NEWS_ARTICLE_TYPE_CUP_QUALIFICATION;
    else if(strcmp(type_string, ARTICLE_TYPE_NAME_RELEGATION) == 0)
	return_value = NEWS_ARTICLE_TYPE_RELEGATION;
    else
	g_warning("xml_news_type_to_int: unknown type name %s \n", 
		  type_string);

    return return_value;
}

/**
 * The function called by the parser when an opening tag is read.
 * The state variable is changed in this function and
 * sometimes memory allocated for the information that's going to be read.
 * @see The GLib manual (Simple XML parser).
 */
void
xml_news_read_start_element (GMarkupParseContext *context,
                             const gchar         *element_name,
                             const gchar        **attribute_names,
                             const gchar        **attribute_values,
                             gpointer             user_data,
                             GError             **error)
{
#ifdef DEBUG
    printf("xml_news_read_start_element\n");
#endif
    gint atidx;
    
    atidx = 0;

    if(strcmp(element_name, TAG_NEWS) == 0)
	state = STATE_NEWS;
    else if(strcmp(element_name, TAG_ARTICLE) == 0)
    {
	state = STATE_ARTICLE;
        new_article.titles = g_array_new(FALSE, FALSE, sizeof(NewsText));
        new_article.subtitles = g_array_new(FALSE, FALSE, sizeof(NewsText));
        new_article.condition = g_strdup("0");
	new_article.priority = 1;
        new_article.id = news_article_id_new;
    }
    else if(strcmp(element_name, TAG_ARTICLE_TYPE) == 0)
	state = STATE_ARTICLE_TYPE;
    else if(strcmp(element_name, TAG_ARTICLE_CONDITION) == 0)
	state = STATE_ARTICLE_CONDITION;
    else if(strcmp(element_name, TAG_ARTICLE_PRIORITY) == 0)
	state = STATE_ARTICLE_PRIORITY;
    else if(strcmp(element_name, TAG_ARTICLE_TITLE) == 0)
    {
	state = STATE_ARTICLE_TITLE;
        new_title.id = news_title_id_new;
        new_title.priority = 1;
        new_title.condition = g_strdup("1");

        while(attribute_names[atidx] != NULL)
        {
            if(strcmp(attribute_names[atidx], ATT_NAME_TEXT_PRIORITY) == 0)
                new_title.priority = (gint)g_ascii_strtod(attribute_values[atidx], NULL);
            else if(strcmp(attribute_names[atidx], ATT_NAME_TEXT_CONDITION) == 0)
                misc_string_assign(&new_title.condition, attribute_values[atidx]);
            atidx++;
        }
    }
    else if(strcmp(element_name, TAG_ARTICLE_SUBTITLE) == 0)
    {
	state = STATE_ARTICLE_SUBTITLE;
        new_subtitle.id = news_subtitle_id_new;
        new_subtitle.priority = 1;
        new_subtitle.condition = g_strdup("1");

        while(attribute_names[atidx] != NULL)
        {
            if(strcmp(attribute_names[atidx], ATT_NAME_TEXT_PRIORITY) == 0)
                new_subtitle.priority = (gint)g_ascii_strtod(attribute_values[atidx], NULL);
            else if(strcmp(attribute_names[atidx], ATT_NAME_TEXT_CONDITION) == 0)
                misc_string_assign(&new_subtitle.condition, attribute_values[atidx]);                            
            atidx++;
        }
    }
    else
	g_warning("xml_news_read_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

/**
 * The function called by the parser when a closing tag is read.
 * The state variable is changed in this function.
 * @see The GLib manual (Simple XML parser).
 */
void
xml_news_read_end_element    (GMarkupParseContext *context,
                              const gchar         *element_name,
                              gpointer             user_data,
                              GError             **error)
{
#ifdef DEBUG
    printf("xml_news_read_end_element\n");
#endif

    if(strcmp(element_name, TAG_ARTICLE) == 0)
    {
     	state = STATE_NEWS;
        g_array_append_val(news[article_idx], new_article);
    }
    else if(strcmp(element_name, TAG_ARTICLE_TYPE) == 0 ||
	    strcmp(element_name, TAG_ARTICLE_CONDITION) == 0 ||
	    strcmp(element_name, TAG_ARTICLE_PRIORITY) == 0 ||
	    strcmp(element_name, TAG_ARTICLE_TITLE) == 0 ||
	    strcmp(element_name, TAG_ARTICLE_SUBTITLE) == 0)
	state = STATE_ARTICLE;
    else if(strcmp(element_name, TAG_NEWS) != 0)
	g_warning("xml_news_read_end_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

/**
 * The function called by the parser when the text between tags is read.
 * This function is responsible for filling in the variables (e.g. team names)
 * when a file gets loaded.
 * @see The GLib manual (Simple XML parser).
 */
void
xml_news_read_text         (GMarkupParseContext *context,
                            const gchar         *text,
                            gsize                text_len,  
                            gpointer             user_data,
                            GError             **error)
{
#ifdef DEBUG
    printf("xml_news_read_text\n");
#endif

    gchar buf[text_len + 1];
    gint int_value;

    strncpy(buf, text, text_len);
    buf[text_len] = '\0';

    int_value = (gint)g_ascii_strtod(buf, NULL);

    if(state == STATE_ARTICLE_TYPE)
	article_idx = xml_news_article_type_to_int(buf);
    else if(state == STATE_ARTICLE_CONDITION)
        misc_string_assign(&new_article.condition, buf);
    else if(state == STATE_ARTICLE_PRIORITY)
        new_article.priority = int_value;
    else if(state == STATE_ARTICLE_TITLE)
    {
        new_title.text = g_strdup(buf);
        g_array_append_val(new_article.titles, new_title);
    }
    else if(state == STATE_ARTICLE_SUBTITLE)
    {
        new_subtitle.text = g_strdup(buf);
        g_array_append_val(new_article.subtitles, new_subtitle);
    }
}

/**
 * Function reading an XML file specifying a country.
 * The variable #country gets freed and overwritten afterwards.
 * @param news_file Name of the xml file (e.g. 'news_en.xml')
 * to be read. Full path is not necessary, if the file is located in
 * one of the suppport directories; neither are the prefix 'news_'
 * or the suffix '.xml'.
 */
void
xml_news_read(const gchar *news_file)
{
#ifdef DEBUG
    printf("xml_news_read\n");
#endif

    GMarkupParser parser = {xml_news_read_start_element,
			    xml_news_read_end_element,
			    xml_news_read_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    gsize length;
    GError *error = NULL;

    context = 
	g_markup_parse_context_new(&parser, 0, NULL, NULL);

    if(!g_file_get_contents(news_file, &file_contents, &length, &error))
    {
	g_warning("xml_news_read: error reading file %s\n", news_file);
	if(g_str_has_suffix(news_file, "news_en.xml"))
	    misc_print_error(&error, TRUE);
	else
	{
	    misc_print_error(&error, FALSE);
	    news_load_news_file("news_en.xml", TRUE);
	}
	return;
    }

    free_news(TRUE);

    if(g_markup_parse_context_parse(context, file_contents, length, &error))
    {
	g_markup_parse_context_end_parse(context, NULL);	
	g_markup_parse_context_free(context);
	g_free(file_contents);
    }
    else
    {
	g_warning("xml_news_read: error parsing file %s\n", news_file);
	if(g_str_has_suffix(news_file, "news_en.xml"))
	    misc_print_error(&error, TRUE);
	else
	{
	    misc_print_error(&error, FALSE);
	    news_load_news_file("news_en.xml", TRUE);
	}
	return;
    }
}
