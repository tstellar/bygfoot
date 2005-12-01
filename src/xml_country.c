/*
   xml_country.c

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
#include "option.h"
#include "variables.h"
#include "xml_cup.h"
#include "xml_country.h"
#include "xml_league.h"

/**
 * The tags used in the XML files defining countries.
 */
#define TAG_COUNTRY "country"
#define TAG_NAME "name"
#define TAG_RATING "rating"
#define TAG_SYMBOL "symbol"
#define TAG_SID "sid"
#define TAG_SUPERNATIONAL "supernational"
#define TAG_LEAGUES "leagues"
#define TAG_LEAGUE "league"
#define TAG_CUPS "cups"
#define TAG_CUP "cup"

/**
 * Enum with the states used in the XML parser functions.
 */
enum XmlCountryStates
{
    STATE_COUNTRY = 0,
    STATE_NAME,
    STATE_RATING,
    STATE_SYMBOL,
    STATE_SID,
    STATE_SUPERNATIONAL,
    STATE_LEAGUES,
    STATE_LEAGUE,
    STATE_CUPS,
    STATE_CUP,
    STATE_END
};

/**
 * The state variable used in the XML parsing functions.
 */
gint state;
Country *cntry;

/**
 * The function called by the parser when an opening tag is read.
 * The state variable is changed in this function and
 * sometimes memory allocated for the information that's going to be read.
 * @see The GLib manual (Simple XML parser).
 */
void
xml_country_read_start_element (GMarkupParseContext *context,
				const gchar         *element_name,
				const gchar        **attribute_names,
				const gchar        **attribute_values,
				gpointer             user_data,
				GError             **error)
{
    if(strcmp(element_name, TAG_NAME) == 0)
	state = STATE_NAME;
    else if(strcmp(element_name, TAG_RATING) == 0)
	state = STATE_RATING;
    else if(strcmp(element_name, TAG_SYMBOL) == 0)
	state = STATE_SYMBOL;
    else if(strcmp(element_name, TAG_SID) == 0)
	state = STATE_SID;
    else if(strcmp(element_name, TAG_SUPERNATIONAL) == 0)
	state = STATE_SUPERNATIONAL;
    else if(strcmp(element_name, TAG_LEAGUES) == 0)
    {
	state = STATE_LEAGUES;
	if(cntry->leagues == NULL)
	    cntry->leagues = g_array_new(FALSE, FALSE, sizeof(League));
    }
    else if(strcmp(element_name, TAG_LEAGUE) == 0)
	state = STATE_LEAGUE;
    else if(strcmp(element_name, TAG_CUPS) == 0)
    {
	state = STATE_CUPS;
	if(cntry->cups == NULL)
	    cntry->cups = g_array_new(FALSE, FALSE, sizeof(Cup));
    }
    else if(strcmp(element_name, TAG_CUP) == 0)
	state = STATE_CUP;
    else if(strcmp(element_name, TAG_COUNTRY) != 0)
	g_warning("xml_country_read_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

/**
 * The function called by the parser when a closing tag is read.
 * The state variable is changed in this function.
 * @see The GLib manual (Simple XML parser).
 */
void
xml_country_read_end_element    (GMarkupParseContext *context,
				 const gchar         *element_name,
				 gpointer             user_data,
				 GError             **error)
{
    if(strcmp(element_name, TAG_NAME) == 0 ||
       strcmp(element_name, TAG_RATING) == 0 ||
       strcmp(element_name, TAG_SYMBOL) == 0 ||
       strcmp(element_name, TAG_SID) == 0 ||
       strcmp(element_name, TAG_SUPERNATIONAL) == 0 ||
       strcmp(element_name, TAG_LEAGUES) == 0 ||
       strcmp(element_name, TAG_CUPS) == 0)
	state = STATE_COUNTRY;
    else if(strcmp(element_name, TAG_LEAGUE) == 0)
	state = STATE_LEAGUES;
    else if(strcmp(element_name, TAG_CUP) == 0)
	state = STATE_CUPS;

    else if(strcmp(element_name, TAG_COUNTRY) != 0)
	g_warning("xml_country_read_end_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

/**
 * The function called by the parser when the text between tags is read.
 * This function is responsible for filling in the variables (e.g. team names)
 * when a file gets loaded.
 * @see The GLib manual (Simple XML parser).
 */
void
xml_country_read_text         (GMarkupParseContext *context,
			       const gchar         *text,
			       gsize                text_len,  
			       gpointer             user_data,
			       GError             **error)
{
    gchar buf[text_len + 1];
    gint int_value;

    strncpy(buf, text, text_len);
    buf[text_len] = '\0';

    int_value = (gint)g_ascii_strtod(buf, NULL);

    if(state == STATE_NAME)
	misc_string_assign(&cntry->name, buf);
    else if(state == STATE_RATING)
	cntry->rating = int_value;
    else if(state == STATE_SYMBOL)
	misc_string_assign(&cntry->symbol, buf);
    else if(state == STATE_SID)
	misc_string_assign(&cntry->sid, buf);
    else if(state == STATE_SUPERNATIONAL && cntry == &country)
    {
	sett_set_int("int_opt_disable_finances", 1);
	sett_set_int("int_opt_disable_transfers", 1);
	sett_set_int("int_opt_disable_stadium", 1);
	sett_set_int("int_opt_disable_contracts", 1);
	sett_set_int("int_opt_disable_boost_on", 1);
	sett_set_int("int_opt_disable_ya", 1);
    }
    else if(state == STATE_LEAGUE)
	xml_league_read(buf, cntry->leagues);
    else if(state == STATE_CUP)
	xml_cup_read(buf, cntry->cups);
}


/**
  Function reading an XML file specifying a country.
  @param country_name name of the xml file (e.g. 'country_england.xml')
  to be read. Full path is not necessary, if the file is located in
  one of the suppport directories; neither are the prefix 'country_'
  or the suffix '.xml'.
  @param cntry The country variable to write.
 */
void
xml_country_read(const gchar *country_name, Country *cntry_arg)
{
    gchar *file_name = file_find_support_file(country_name, FALSE);
    GMarkupParser parser = {xml_country_read_start_element,
			    xml_country_read_end_element,
			    xml_country_read_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    gint length;
    GError *error = NULL;
    gchar buf[SMALL];
    gint i;

    context = 
	g_markup_parse_context_new(&parser, 0, NULL, NULL);

    if(file_name == NULL)
    {
	sprintf(buf, "country_%s.xml", country_name);
	file_name = file_find_support_file(buf, TRUE);
    }
	
    if(!g_file_get_contents(file_name, &file_contents, &length, &error))
    {
	g_warning("xml_country_read: error reading file %s\n", file_name);
	misc_print_error(&error, TRUE);
	return;
    }

    cntry = (cntry_arg == NULL) ? &country : cntry_arg;

    state = STATE_COUNTRY;
    strcpy(buf, file_name);
    g_free(file_name);

    free_country(cntry, TRUE);

    if(cntry_arg == NULL)
    {
	sett_set_int("int_opt_disable_finances", 0);
	sett_set_int("int_opt_disable_transfers", 0);
	sett_set_int("int_opt_disable_stadium", 0);
	sett_set_int("int_opt_disable_contracts", 0);
	sett_set_int("int_opt_disable_boost_on", 0);
	sett_set_int("int_opt_disable_ya", 0);
    }

    if(g_markup_parse_context_parse(context, file_contents, length, &error))
    {
	g_markup_parse_context_end_parse(context, NULL);	
	g_markup_parse_context_free(context);
	g_free(file_contents);
    }
    else
    {
	g_critical("xml_country_read: error parsing file %s\n", buf);
	misc_print_error(&error, TRUE);
    }

    for(i=0;i<cntry->leagues->len;i++)
	if(g_array_index(cntry->leagues, League, i).layer == -1)
	    g_array_index(cntry->leagues, League, i).layer = i + 1;
}
