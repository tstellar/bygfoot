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
    TAG_MISC_RESERVE_PROMOTION_RULES,
    TAG_COUNTRIES,
    TAG_COUNTRY,
    TAG_LEAGUES_CUPS,
    TAG_LEAGUE_FILE,
    TAG_CUP_FILE,
    TAG_END
};

typedef struct {
    Country *country;
    const gchar *directory;
    Bygfoot *bygfoot;

} MiscUserData;



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
#ifdef DEBUG
    printf("xml_loadsave_misc_start_element\n");
#endif

    gint i;
    gint tag = xml_get_tag_from_name(element_name);
    MiscUserData *misc_user_data = (MiscUserData*)user_data;
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

    if (tag == TAG_COUNTRY) {
        misc_user_data->country = g_malloc0(sizeof(Country));
        misc_user_data->country->leagues = g_array_new(FALSE, FALSE, sizeof(League));
        misc_user_data->country->cups = g_array_new(FALSE, FALSE, sizeof(Cup));
        misc_user_data->country->allcups = g_ptr_array_new();
    }

    if(!valid_tag)
	debug_print_message("xml_loadsave_misc_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_misc_end_element    (GMarkupParseContext *context,
				     const gchar         *element_name,
				     gpointer             user_data,
				     GError             **error)
{
#ifdef DEBUG
    printf("xml_loadsave_misc_end_element\n");
#endif

    MiscUserData *misc_user_data = (MiscUserData*)user_data;
    gint tag = xml_get_tag_from_name(element_name);

    if(tag == TAG_NAME ||
       tag == TAG_SYMBOL ||
       tag == TAG_SID ||
       tag == TAG_MISC_RESERVE_PROMOTION_RULES ||
       tag == TAG_MISC_VERSION ||
       tag == TAG_MISC_CURRENT_INTEREST ||
       tag == TAG_MISC_RATING ||
       tag == TAG_MISC_ALLCUP ||
       tag == TAG_MISC_COUNTER ||
       tag == TAG_MISC_SEASON ||
       tag == TAG_MISC_WEEK ||
       tag == TAG_MISC_WEEK_ROUND ||
       tag == TAG_MISC_BET0 ||
       tag == TAG_LEAGUES_CUPS ||
       tag == TAG_LEAGUE_FILE ||
       tag == TAG_CUP_FILE ||
       tag == TAG_COUNTRIES ||
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
    } else if (tag == TAG_COUNTRY) {
        g_ptr_array_add(country_list, misc_user_data->country);
        misc_user_data->country = &country;
    } else if(tag != TAG_MISC)
	debug_print_message("xml_loadsave_misc_end_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_misc_text         (GMarkupParseContext *context,
				const gchar         *text,
				gsize                text_len,
				gpointer             user_data,
				GError             **error)
{
#ifdef DEBUG
    printf("xml_loadsave_misc_text\n");
#endif

    MiscUserData *misc_user_data = (MiscUserData*)user_data;
    gchar buf[SMALL];
    gint int_value = -1;
    gfloat float_value = -1;
    gchar *dirname = (gchar*)user_data;

    strncpy(buf, text, text_len);
    buf[text_len] = '\0';

    int_value = (gint)g_ascii_strtod(buf, NULL);
    float_value = (gfloat)g_ascii_strtod(buf, NULL) / 10000;

    if(state == TAG_NAME)
	misc_string_assign(&misc_user_data->country->name, buf);
    else if(state == TAG_MISC_RATING)
	misc_user_data->country->rating = int_value;
    else if(state == TAG_SYMBOL)
	misc_string_assign(&misc_user_data->country->symbol, buf);
    else if(state == TAG_SID)
	misc_string_assign(&misc_user_data->country->sid, buf);
    else if (state == TAG_MISC_RESERVE_PROMOTION_RULES)
        misc_user_data->country->reserve_promotion_rules = int_value;
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
    else if(state == TAG_LEAGUE_FILE) {
        xml_load_league(misc_user_data->bygfoot, misc_user_data->country->leagues,
                        misc_user_data->directory, buf);
    } else if (state == TAG_CUP_FILE) {
        Cup new_cup = cup_new(FALSE);
        g_array_append_val(misc_user_data->country->cups, new_cup);
        xml_load_cup(misc_user_data->bygfoot,
	             &g_array_index(misc_user_data->country->cups, Cup, misc_user_data->country->cups->len - 1),
                     misc_user_data->directory, buf);
    }
}


/** Read a country xml file. */
void
xml_loadsave_misc_read(Bygfoot *bygfoot, const gchar *dirname, const gchar *basename)
{
#ifdef DEBUG
    printf("xml_loadsave_misc_read\n");
#endif

    gchar file[SMALL];
    MiscUserData user_data = { &country, dirname, bygfoot};
    GMarkupParser parser = {xml_loadsave_misc_start_element,
			    xml_loadsave_misc_end_element,
			    xml_loadsave_misc_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    gsize length;
    GError *error = NULL;

    sprintf(file, "%s%s%s___misc.xml", dirname, G_DIR_SEPARATOR_S, basename);

    context =
	g_markup_parse_context_new(&parser, 0, &user_data, NULL);

    if(!g_file_get_contents(file, &file_contents, &length, &error))
    {
	debug_print_message("xml_loadsave_misc_read: error reading file %s\n", file);
	misc_print_error(&error, TRUE);
    }

    g_ptr_array_free(acps, TRUE);
    acps = g_ptr_array_new();

    if (country_list)
        g_ptr_array_free(country_list, TRUE);
    country_list = g_ptr_array_new();
    free_bets(TRUE);

    if(g_markup_parse_context_parse(context, file_contents, length, &error))
    {
	g_markup_parse_context_end_parse(context, NULL);
	g_markup_parse_context_free(context);
	g_free(file_contents);
    }
    else
    {
	debug_print_message("xml_loadsave_misc_read: error parsing file %s\n", file);
	misc_print_error(&error, TRUE);
    }
}

/** Save the country variable into the file. */
void
xml_loadsave_misc_write(const gchar *prefix)
{
#ifdef DEBUG
    printf("xml_loadsave_misc_write\n");
#endif

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
    xml_write_int(fil, country.reserve_promotion_rules, TAG_MISC_RESERVE_PROMOTION_RULES, I0);
    xml_write_int(fil, country.rating, TAG_MISC_RATING, I0);
    xml_write_int(fil, season, TAG_MISC_SEASON, I0);
    xml_write_int(fil, week, TAG_MISC_WEEK, I0);
    xml_write_int(fil, week_round, TAG_MISC_WEEK_ROUND, I0);

    for(i=0;i<COUNT_END;i++)
	xml_write_int(fil, counters[i], TAG_MISC_COUNTER, I0);

    for(i=0;i<acps->len;i++)
	xml_write_int(fil, acp(i)->id, TAG_MISC_ALLCUP, I0);

    xml_loadsave_misc_write_bets(fil);

    xml_loadsave_misc_write_countries(fil, prefix);

    fprintf(fil, "</_%d>\n", TAG_MISC);
    fclose(fil);
}

/** Write the bets arrays into the file. */
void
xml_loadsave_misc_write_bets(FILE *fil)
{
#ifdef DEBUG
    printf("xml_loadsave_misc_write_bets\n");
#endif

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

static void
xml_loadsave_misc_write_country(const Country *country, FILE *fil,
                                const gchar *prefix)
{
    gchar buf[SMALL];
    gchar *basename = g_path_get_basename(prefix);
    gint i;

    fprintf(fil, "%s<_%d>\n", I1, TAG_COUNTRY);
    xml_write_string(fil, country->name, TAG_NAME, I0);
    xml_write_string(fil, country->symbol, TAG_SYMBOL, I0);
    xml_write_string(fil, country->sid, TAG_SID, I0);
    xml_write_int(fil, country->reserve_promotion_rules, TAG_MISC_RESERVE_PROMOTION_RULES, I0);
    xml_write_int(fil, country->rating, TAG_MISC_RATING, I0);

    fprintf(fil, "%s<_%d>\n", I2, TAG_LEAGUES_CUPS);
    for (i = 0; i < country->leagues->len; i++) {
        const League *league = &g_array_index(country->leagues, League, i);
	xml_loadsave_league_write(prefix, league);
	sprintf(buf, "%s___league_%d.xml", basename, league->id);
	xml_write_string(fil, buf, TAG_LEAGUE_FILE, I3);
    }

    for (i = 0; i < country->cups->len; i++) {
        const Cup *cup = &g_array_index(country->cups, Cup, i);
	xml_loadsave_cup_write(prefix, cup);
	sprintf(buf, "%s___cup_%d.xml", basename, cup->id);
	xml_write_string(fil, buf, TAG_CUP_FILE, I3);
    }

    fprintf(fil, "%s</_%d>\n", I2, TAG_LEAGUES_CUPS);
    fprintf(fil, "%s</_%d>\n", I1, TAG_COUNTRY);
}

/** Write non-user countries to the file. */
void
xml_loadsave_misc_write_countries(FILE *fil, const gchar *prefix)
{
    gint i;

    fprintf(fil, "%s<!-- COUNTRIES--><_%d>\n", I0, TAG_COUNTRIES);

    for (i = 0; i < country_list->len; i++) {
        const Country *country = g_ptr_array_index(country_list, i);
	xml_loadsave_misc_write_country(country, fil, prefix);
    }

    fprintf(fil, "%s</_%d>\n", I0, TAG_COUNTRIES);
}
