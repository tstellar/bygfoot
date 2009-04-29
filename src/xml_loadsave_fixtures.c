/*
   xml_loadsave_fixtures.c

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
#include "fixture.h"
#include "misc.h"
#include "team.h"
#include "variables.h"
#include "xml.h"
#include "xml_loadsave_fixtures.h"

enum
{
    TAG_FIXTURES = TAG_START_FIXTURES,
    TAG_FIXTURE,
    TAG_FIXTURE_REPLAY_NUMBER,
    TAG_FIXTURE_ID,
    TAG_FIXTURE_WEEK_NUMBER,
    TAG_FIXTURE_WEEK_ROUND_NUMBER,
    TAG_FIXTURE_RESULT,
    TAG_FIXTURE_HOME_ADVANTAGE,
    TAG_FIXTURE_SECOND_LEG,
    TAG_FIXTURE_DECISIVE,
    TAG_FIXTURE_ATTENDANCE,
    TAG_END
};

gint state, residx1, residx2, teamidx, nameidx;
Fixture new_fixture;
GArray *fixtures_array;

void
xml_loadsave_fixtures_start_element (GMarkupParseContext *context,
				    const gchar         *element_name,
				    const gchar        **attribute_names,
				    const gchar        **attribute_values,
				    gpointer             user_data,
				    GError             **error)
{
#ifdef DEBUG
    printf("xml_loadsave_fixtures_start_element\n");
#endif

    gint i;
    gint tag = xml_get_tag_from_name(element_name);
    gboolean valid_tag = FALSE;

    for(i=TAG_FIXTURES;i<TAG_END;i++)
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

    if(state == TAG_FIXTURE)
	residx1 = residx2 = teamidx = nameidx = 0;

    if(!valid_tag)
	debug_print_message("xml_loadsave_fixtures_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_fixtures_end_element    (GMarkupParseContext *context,
				     const gchar         *element_name,
				     gpointer             user_data,
				     GError             **error)
{
#ifdef DEBUG
    printf("xml_loadsave_fixtures_end_element\n");
#endif

    gint tag = xml_get_tag_from_name(element_name);
    
    if(tag == TAG_FIXTURE)
    {
	state = TAG_FIXTURES;
	g_array_append_val(fixtures_array, new_fixture);
    }
    else if(tag == TAG_FIXTURE_REPLAY_NUMBER ||
	    tag == TAG_FIXTURE_ID ||
	    tag == TAG_FIXTURE_WEEK_NUMBER ||
	    tag == TAG_FIXTURE_WEEK_ROUND_NUMBER ||
	    tag == TAG_FIXTURE_RESULT ||
	    tag == TAG_FIXTURE_HOME_ADVANTAGE ||
	    tag == TAG_FIXTURE_SECOND_LEG ||
	    tag == TAG_FIXTURE_DECISIVE ||
	    tag == TAG_FIXTURE_ATTENDANCE ||
	    tag == TAG_TEAM_ID ||
	    tag == TAG_ID ||
	    tag == TAG_ROUND)
    {
	state = TAG_FIXTURE;
	if(tag == TAG_FIXTURE_RESULT)
	{
	    residx2 = (residx2 + 1) % 3;
	    if(residx2 == 0)
		residx1++;
	}
	else if(tag == TAG_TEAM_ID)
	    teamidx++;
    }
    else if(tag != TAG_FIXTURES)
	debug_print_message("xml_loadsave_fixtures_end_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);	
}

void
xml_loadsave_fixtures_text         (GMarkupParseContext *context,
				   const gchar         *text,
				   gsize                text_len,  
				   gpointer             user_data,
				   GError             **error)
{
#ifdef DEBUG
    printf("xml_loadsave_fixtures_text\n");
#endif

    gchar buf[SMALL];
    gint int_value = -1;

    strncpy(buf, text, text_len);
    buf[text_len] = '\0';

    int_value = (gint)g_ascii_strtod(buf, NULL);

    if(state == TAG_ID)
	new_fixture.clid = int_value;
    else if(state == TAG_ROUND)
	new_fixture.round = int_value;
    else if(state == TAG_FIXTURE_REPLAY_NUMBER)
	new_fixture.replay_number = int_value;
    else if(state == TAG_FIXTURE_ID)
	new_fixture.id = int_value;
    else if(state == TAG_FIXTURE_WEEK_NUMBER)
	new_fixture.week_number = int_value;
    else if(state == TAG_FIXTURE_WEEK_ROUND_NUMBER)
	new_fixture.week_round_number = int_value;
    else if(state == TAG_FIXTURE_HOME_ADVANTAGE)
	new_fixture.home_advantage = int_value;
    else if(state == TAG_FIXTURE_SECOND_LEG)
	new_fixture.second_leg = int_value;
    else if(state == TAG_FIXTURE_DECISIVE)
	new_fixture.decisive = int_value;
    else if(state == TAG_FIXTURE_ATTENDANCE)
	new_fixture.attendance = int_value;
    else if(state == TAG_FIXTURE_RESULT)
	new_fixture.result[residx1][residx2] = int_value;
    else if(state == TAG_TEAM_ID)
	new_fixture.team_ids[teamidx] = int_value;
}

void
xml_loadsave_fixtures_read(const gchar *filename, GArray *fixtures)
{
#ifdef DEBUG
    printf("xml_loadsave_fixtures_read\n");
#endif

    GMarkupParser parser = {xml_loadsave_fixtures_start_element,
			    xml_loadsave_fixtures_end_element,
			    xml_loadsave_fixtures_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    gsize length;
    GError *error = NULL;

    context = 
	g_markup_parse_context_new(&parser, 0, NULL, NULL);

    if(!g_file_get_contents(filename, &file_contents, &length, &error))
    {
	debug_print_message("xml_loadsave_fixtures_read: error reading file %s\n", filename);
	misc_print_error(&error, TRUE);
    }

    fixtures_array = fixtures;

    if(g_markup_parse_context_parse(context, file_contents, length, &error))
    {
	g_markup_parse_context_end_parse(context, NULL);
	g_markup_parse_context_free(context);
	g_free(file_contents);
    }
    else
    {
	debug_print_message("xml_loadsave_fixtures_read: error parsing file %s\n", filename);
	misc_print_error(&error, TRUE);
    }
}

void
xml_loadsave_fixtures_write(const gchar *filename, const GArray *fixtures)
{
#ifdef DEBUG
    printf("xml_loadsave_fixtures_write\n");
#endif

    gint i, j;
    FILE *fil = NULL;

    file_my_fopen(filename, "w", &fil, TRUE);

    fprintf(fil, "<_%d>\n", TAG_FIXTURES);

    for(i=0;i<fixtures->len;i++)
    {
	fprintf(fil, "<_%d>\n", TAG_FIXTURE);
	xml_write_int(fil, g_array_index(fixtures, Fixture, i).clid, TAG_ID, I1);
	xml_write_int(fil, g_array_index(fixtures, Fixture, i).round, TAG_ROUND, I1);
	xml_write_int(fil, g_array_index(fixtures, Fixture, i).replay_number, 
		      TAG_FIXTURE_REPLAY_NUMBER, I1);
	xml_write_int(fil, g_array_index(fixtures, Fixture, i).id, 
		      TAG_FIXTURE_ID, I1);
	xml_write_int(fil, g_array_index(fixtures, Fixture, i).week_number, 
		      TAG_FIXTURE_WEEK_NUMBER, I1);
	xml_write_int(fil, g_array_index(fixtures, Fixture, i).week_round_number,
		      TAG_FIXTURE_WEEK_ROUND_NUMBER, I1);
	xml_write_int(fil, g_array_index(fixtures, Fixture, i).home_advantage,
		      TAG_FIXTURE_HOME_ADVANTAGE, I1);
	xml_write_int(fil, g_array_index(fixtures, Fixture, i).second_leg,
		      TAG_FIXTURE_SECOND_LEG, I1);
	xml_write_int(fil, g_array_index(fixtures, Fixture, i).decisive,
		      TAG_FIXTURE_DECISIVE, I1);
	xml_write_int(fil, g_array_index(fixtures, Fixture, i).attendance,
		      TAG_FIXTURE_ATTENDANCE, I1);

	for(j=0;j<2;j++)
	{
	    xml_write_int(fil, g_array_index(fixtures, Fixture, i).result[j][0],
			  TAG_FIXTURE_RESULT, I1);
	    xml_write_int(fil, g_array_index(fixtures, Fixture, i).result[j][1],
			  TAG_FIXTURE_RESULT, I1);
	    xml_write_int(fil, g_array_index(fixtures, Fixture, i).result[j][2],
			  TAG_FIXTURE_RESULT, I1);
            xml_write_int(fil, g_array_index(fixtures, Fixture, i).team_ids[j], 
                          TAG_TEAM_ID, I1);
	}

	fprintf(fil, "</_%d>\n", TAG_FIXTURE);
    }
    fprintf(fil, "</_%d>\n", TAG_FIXTURES);

    fclose(fil);
}
