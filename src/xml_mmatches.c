/*
   xml_mmatches.c

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
#include "live_game.h"
#include "misc.h"
#include "user.h"
#include "xml.h"
#include "xml_mmatches.h"
#include "xml_loadsave_live_game.h"

enum
{
    TAG_MMATCHES = 0,
    TAG_MMATCH,
    TAG_MMATCHES_COUNTRY_NAME,
    TAG_MMATCHES_COMP_NAME,
    TAG_MMATCHES_NEUTRAL,
    TAG_MMATCHES_USER_TEAM,
    TAG_MMATCHES_LG_FILE,
    TAG_END
};

/**
 * The state variable used in the XML parsing functions.
 */
gint state;
MemMatch new_match;
gchar *dirname;
GArray *mm_array;

void
xml_mmatches_start_element (GMarkupParseContext *context,
			    const gchar         *element_name,
			    const gchar        **attribute_names,
			    const gchar        **attribute_values,
			    gpointer             user_data,
			    GError             **error)
{
    gint i;
    gint tag = xml_get_tag_from_name(element_name);
    gboolean valid_tag = FALSE;

    for(i=TAG_MMATCHES;i<TAG_END;i++)
	if(tag == i)
	{
	    state = i;
	    valid_tag = TRUE;
	}

    if(tag == TAG_MMATCH)
	new_match.country_name = NULL;

    if(!valid_tag)
	g_warning("xml_loadsave_mmatches_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_mmatches_end_element    (GMarkupParseContext *context,
			     const gchar         *element_name,
			     gpointer             user_data,
			     GError             **error)
{
    gint tag = xml_get_tag_from_name(element_name);

    if(tag == TAG_MMATCH)
    {
	state = TAG_MMATCHES;
	g_array_append_val(mm_array, new_match);
    }
    else if(tag == TAG_MMATCHES_COMP_NAME ||
	    tag == TAG_MMATCHES_COUNTRY_NAME ||
	    tag == TAG_MMATCHES_NEUTRAL ||
	    tag == TAG_MMATCHES_USER_TEAM ||
	    tag == TAG_MMATCHES_LG_FILE)
	state = TAG_MMATCH;
    else if(tag != TAG_MMATCHES)
	g_warning("xml_loadsave_mmatches_end_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_mmatches_text         (GMarkupParseContext *context,
			   const gchar         *text,
			   gsize                text_len,  
			   gpointer             user_data,
			   GError             **error)
{
    gchar buf[SMALL], lg_file[SMALL];
    gint int_value = -1;

    strncpy(buf, text, text_len);
    buf[text_len] = '\0';

    int_value = (gint)g_ascii_strtod(buf, NULL);

    if(state == TAG_MMATCHES_COMP_NAME)
	new_match.competition_name = g_string_new(buf);
    else if(state == TAG_MMATCHES_COUNTRY_NAME)
	misc_string_assign(&new_match.country_name, buf);
    else if(state == TAG_MMATCHES_NEUTRAL)
	new_match.neutral = int_value;
    else if(state == TAG_MMATCHES_USER_TEAM)
	new_match.user_team = int_value;
    else if(state == TAG_MMATCHES_LG_FILE)
    {
	sprintf(lg_file, "%s%s%s", dirname, G_DIR_SEPARATOR_S, buf);
	live_game_reset(&new_match.lg, NULL, FALSE);
	xml_loadsave_live_game_read(lg_file, &new_match.lg);	
    }
}

void
xml_mmatches_read(const gchar *filename, GArray *mmatches)
{
    GMarkupParser parser = {xml_mmatches_start_element,
			    xml_mmatches_end_element,
			    xml_mmatches_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    guint length;
    GError *error = NULL;

    context = 
	g_markup_parse_context_new(&parser, 0, NULL, NULL);

    if(!g_file_get_contents(filename, &file_contents, &length, &error))
    {
	g_warning("xml_loadsave_mmatches_read: error reading file %s\n", filename);
	misc_print_error(&error, TRUE);
    }

    dirname = g_path_get_dirname(filename);
    mm_array = mmatches;

    if(g_markup_parse_context_parse(context, file_contents, length, &error))
    {
	g_markup_parse_context_end_parse(context, NULL);
	g_markup_parse_context_free(context);
	g_free(file_contents);
    }
    else
    {
	g_warning("xml_loadsave_mmatches_read: error parsing file %s\n", filename);
	misc_print_error(&error, TRUE);
    }
}


/** Write the current user's MMs to file. 
    @param prefix The prefix of the file (w/o ".bmm.zip"). */
void
xml_mmatches_write(const gchar *prefix, const GArray *mmatches)
{
    FILE *fil;
    gint i;
    gchar buf[SMALL];
    gchar *basename = g_path_get_basename(prefix);

    sprintf(buf, "%smmatches", prefix);

    file_my_fopen(buf, "w", &fil, TRUE);

    fprintf(fil, "<_%d>\n", TAG_MMATCHES);

    for(i=0;i<mmatches->len;i++)
    {
	fprintf(fil, "<_%d>\n", TAG_MMATCH);

	xml_write_g_string(fil, 
			   g_array_index(mmatches, MemMatch, i).competition_name,
			   TAG_MMATCHES_COMP_NAME, I1);
	xml_write_string(fil,
			 g_array_index(mmatches, MemMatch, i).country_name,
			 TAG_MMATCHES_COUNTRY_NAME, I1);
	xml_write_int(fil, 
		      g_array_index(mmatches, MemMatch, i).neutral,
		      TAG_MMATCHES_NEUTRAL, I1);
	xml_write_int(fil, 
		      g_array_index(mmatches, MemMatch, i).user_team,
		      TAG_MMATCHES_USER_TEAM, I1);
	
	sprintf(buf, "%slg%03d", basename, i);
	xml_write_string(fil, buf, TAG_MMATCHES_LG_FILE, I1);
	sprintf(buf, "%slg%03d", prefix, i);
	xml_loadsave_live_game_write(buf, 
				     &g_array_index(mmatches, MemMatch, i).lg);

	fprintf(fil, "</_%d>\n", TAG_MMATCH);
    }

    fprintf(fil, "</_%d>\n", TAG_MMATCHES);

    fclose(fil);

    g_free(basename);
}
