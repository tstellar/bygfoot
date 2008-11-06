/*
   xml_loadsave_misc.c

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

#include "bet_struct.h"
#include "cup.h"
#include "file.h"
#include "fixture.h"
#include "free.h"
#include "misc.h"
#include "xml.h"
#include "variables.h"
#include "xml_loadsave_misc.h"
#include "xml_loadsave_cup.h"
#include "xml_loadsave_league.h"

enum XmlLoadSaveCountryTags
{
    TAG_MISC = TAG_START_MISC,
    TAG_MISC_RATING,
    TAG_MISC_SEASON,
    TAG_MISC_WEEK,
    TAG_MISC_WEEK_ROUND,
    TAG_MISC_COUNTER,
    TAG_MISC_ALLCUP,
    TAG_MISC_BET0,
    TAG_MISC_BET1,
    TAG_MISC_BET_FIX_ID,
    TAG_MISC_BET_ODD,
    TAG_MISC_VERSION,
    TAG_MISC_CURRENT_INTEREST,
    TAG_END
};

gint state, countidx, oddidx, betidx;
BetMatch new_bet;

void
xml_loadsave_misc_start_element (GMarkupParseContext *context,
				    const gchar         *element_name,
				    const gchar        **attribute_names,
				    const gchar        **attribute_values,
				    gpointer             user_data,
				    GError             **error)
{
    gint i;
    gint tag = xml_get_tag_from_name(element_name);
    gboolean valid_tag = FALSE;

    for(i=TAG_MISC;i<TAG_END;i++)
	if(tag == i)
	{
	    state = i;
	    valid_tag = TRUE;
	}

    for(i=TAG_NAME;i<=TAG_ROUND;i++)
	if(tag == i)
	{
	    state = i;
	    valid_tag = TRUE;
	}

    if(tag == TAG_MISC)
	countidx = 0;

    if(tag == TAG_MISC_BET0 ||
       tag == TAG_MISC_BET1)
    {
	oddidx = 0;
	betidx = (tag == TAG_MISC_BET1);
    }

    if(!valid_tag)
	g_warning("xml_loadsave_misc_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_misc_end_element    (GMarkupParseContext *context,
				     const gchar         *element_name,
				     gpointer             user_data,
				     GError             **error)
{
    gint tag = xml_get_tag_from_name(element_name);

    if(tag == TAG_NAME ||
       tag == TAG_SYMBOL ||
       tag == TAG_SID ||
       tag == TAG_MISC_VERSION ||
       tag == TAG_MISC_CURRENT_INTEREST ||
       tag == TAG_MISC_RATING ||
       tag == TAG_MISC_ALLCUP ||
       tag == TAG_MISC_COUNTER ||
       tag == TAG_MISC_SEASON ||
       tag == TAG_MISC_WEEK ||
       tag == TAG_MISC_WEEK_ROUND ||
       tag == TAG_MISC_BET0 ||
       tag == TAG_MISC_BET1)
    {
	state = TAG_MISC;
	if(tag == TAG_MISC_COUNTER)
	    countidx++;
	else if(tag == TAG_MISC_BET0 ||
		tag == TAG_MISC_BET1)
	    g_array_append_val(bets[betidx], new_bet);
    }
    else if(tag == TAG_MISC_BET_ODD ||
	    tag == TAG_MISC_BET_FIX_ID)
    {
	state = (betidx == 0) ? TAG_MISC_BET0 : TAG_MISC_BET1;
	if(tag == TAG_MISC_BET_ODD)
	    oddidx++;
    }
    else if(tag != TAG_MISC)
	g_warning("xml_loadsave_misc_end_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_misc_text         (GMarkupParseContext *context,
				const gchar         *text,
				gsize                text_len,
				gpointer             user_data,
				GError             **error)
{
    gchar buf[SMALL];
    gint int_value = -1;
    gfloat float_value = -1;

    strncpy(buf, text, text_len);
    buf[text_len] = '\0';

    int_value = (gint)g_ascii_strtod(buf, NULL);
    float_value = (gfloat)g_ascii_strtod(buf, NULL) / 10000;

    if(state == TAG_NAME)
	misc_string_assign(&country.name, buf);
    else if(state == TAG_MISC_RATING)
	country.rating = int_value;
    else if(state == TAG_SYMBOL)
	misc_string_assign(&country.symbol, buf);
    else if(state == TAG_SID)
	misc_string_assign(&country.sid, buf);
    else if(state == TAG_MISC_SEASON)
	season = int_value;
    else if(state == TAG_MISC_WEEK)
	week = int_value;
    else if(state == TAG_MISC_WEEK_ROUND)
	week_round = int_value;
    else if(state == TAG_MISC_COUNTER)
	counters[countidx] = int_value;
    else if(state == TAG_MISC_ALLCUP)
	g_ptr_array_add(acps, cup_from_clid(int_value));
    else if(state == TAG_MISC_BET_ODD)
	new_bet.odds[oddidx] = float_value;
    else if(state == TAG_MISC_BET_FIX_ID)
	new_bet.fix_id = int_value;
    else if(state == TAG_MISC_CURRENT_INTEREST)
      current_interest = float_value;
}


/** Read a country xml file. */
void
xml_loadsave_misc_read(const gchar *dirname, const gchar *basename)
{
    gchar file[SMALL];
    GMarkupParser parser = {xml_loadsave_misc_start_element,
			    xml_loadsave_misc_end_element,
			    xml_loadsave_misc_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    gsize length;
    GError *error = NULL;

    sprintf(file, "%s%s%s___misc.xml", dirname, G_DIR_SEPARATOR_S, basename);

    context =
	g_markup_parse_context_new(&parser, 0, NULL, NULL);

    if(!g_file_get_contents(file, &file_contents, &length, &error))
    {
	g_warning("xml_loadsave_misc_read: error reading file %s\n", file);
	misc_print_error(&error, TRUE);
    }

    g_ptr_array_free(acps, TRUE);
    acps = g_ptr_array_new();

    free_bets(TRUE);

    if(g_markup_parse_context_parse(context, file_contents, length, &error))
    {
	g_markup_parse_context_end_parse(context, NULL);
	g_markup_parse_context_free(context);
	g_free(file_contents);
    }
    else
    {
	g_warning("xml_loadsave_misc_read: error parsing file %s\n", file);
	misc_print_error(&error, TRUE);
    }
}

/** Save the country variable into the file. */
void
xml_loadsave_misc_write(const gchar *prefix)
{
    gint i;
    gchar buf[SMALL];
    FILE *fil = NULL;

    sprintf(buf, "%s___misc.xml", prefix);

    file_my_fopen(buf, "w", &fil, TRUE);

    fprintf(fil, "<_%d>\n", TAG_MISC);

    xml_write_string(fil, VERS, TAG_MISC_VERSION, I0);
    xml_write_float(fil, current_interest, TAG_MISC_CURRENT_INTEREST, I0);
    xml_write_string(fil, country.name, TAG_NAME, I0);
    xml_write_string(fil, country.symbol, TAG_SYMBOL, I0);
    xml_write_string(fil, country.sid, TAG_SID, I0);
    xml_write_int(fil, country.rating, TAG_MISC_RATING, I0);
    xml_write_int(fil, season, TAG_MISC_SEASON, I0);
    xml_write_int(fil, week, TAG_MISC_WEEK, I0);
    xml_write_int(fil, week_round, TAG_MISC_WEEK_ROUND, I0);

    for(i=0;i<COUNT_END;i++)
	xml_write_int(fil, counters[i], TAG_MISC_COUNTER, I0);

    for(i=0;i<acps->len;i++)
	xml_write_int(fil, acp(i)->id, TAG_MISC_ALLCUP, I0);

    xml_loadsave_misc_write_bets(fil);

    fprintf(fil, "</_%d>\n", TAG_MISC);
    fclose(fil);
}

/** Write the bets arrays into the file. */
void
xml_loadsave_misc_write_bets(FILE *fil)
{
    gint i, j, k;

    for(i=0;i<2;i++)
	for(j=0;j<bets[i]->len;j++)
	{
	    fprintf(fil, "%s<_%d>\n", I0, (i == 0) ? TAG_MISC_BET0 : TAG_MISC_BET1);
	    xml_write_int(fil, g_array_index(bets[i], BetMatch, j).fix_id,
			  TAG_MISC_BET_FIX_ID, I1);
	    for(k=0;k<3;k++)
		xml_write_float(fil, g_array_index(bets[i], BetMatch, j).odds[k],
				TAG_MISC_BET_ODD, I1);
	    fprintf(fil, "%s</_%d>\n", I0, (i == 0) ? TAG_MISC_BET0 : TAG_MISC_BET1);
	}
}
