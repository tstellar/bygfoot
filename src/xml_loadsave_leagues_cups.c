/*
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

#include "cup.h"
#include "file.h"
#include "free.h"
#include "league_struct.h"
#include "misc.h"
#include "xml.h"
#include "xml_loadsave_cup.h"
#include "xml_loadsave_league.h"
#include "xml_loadsave_leagues_cups.h"

enum
{
    TAG_LEAGUES_CUPS = TAG_START_LEAGUES_CUPS,
    TAG_LEAGUE_FILE,
    TAG_CUP_FILE,
    TAG_END
};

gint state;
const gchar *dir;

void
xml_loadsave_leagues_cups_start_element (GMarkupParseContext *context,
					 const gchar         *element_name,
					 const gchar        **attribute_names,
					 const gchar        **attribute_values,
					 gpointer             user_data,
					 GError             **error)
{
    gint i;
    gint tag = xml_get_tag_from_name(element_name);
    gboolean valid_tag = FALSE;

    for(i=TAG_LEAGUES_CUPS;i<TAG_END;i++)
	if(tag == i)
	{
	    state = i;
	    valid_tag = TRUE;
	}

    if(!valid_tag)
	g_warning("xml_loadsave_leagues_cups_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_leagues_cups_end_element    (GMarkupParseContext *context,
					  const gchar         *element_name,
					  gpointer             user_data,
					  GError             **error)
{
    gint tag = xml_get_tag_from_name(element_name);

    if(tag == TAG_LEAGUE_FILE ||
       tag == TAG_CUP_FILE)
	state = TAG_LEAGUES_CUPS;
    else if(tag != TAG_LEAGUES_CUPS)
	g_warning("xml_loadsave_leagues_cups_end_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_leagues_cups_text         (GMarkupParseContext *context,
					const gchar         *text,
					gsize                text_len,  
					gpointer             user_data,
					GError             **error)
{
    gchar buf[SMALL];
    Cup new_cup;

    strncpy(buf, text, text_len);
    buf[text_len] = '\0';

    if(state == TAG_LEAGUE_FILE)
	xml_load_league(dir, buf);
    else if(state == TAG_CUP_FILE)
    {
	new_cup = cup_new(FALSE);
	g_array_append_val(cps, new_cup);
	xml_load_cup(&g_array_index(cps, Cup, cps->len - 1), dir, buf);	
    }
}

/** Load the leagues and cups given in the leagues_cups.xml file. */
void
xml_loadsave_leagues_cups_read(const gchar *dirname, const gchar *prefix)
{
    gchar file[SMALL];
    GMarkupParser parser = {xml_loadsave_leagues_cups_start_element,
			    xml_loadsave_leagues_cups_end_element,
			    xml_loadsave_leagues_cups_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    guint length;
    GError *error = NULL;

    sprintf(file, "%s/%s___leagues_cups.xml", dirname, prefix);

    context = 
	g_markup_parse_context_new(&parser, 0, NULL, NULL);

    if(!g_file_get_contents(file, &file_contents, &length, &error))
    {
	g_warning("xml_loadsave_misc_read: error reading file %s\n", file);
	misc_print_error(&error, TRUE);
    }

    free_leagues_array(&ligs, TRUE);
    free_cups_array(&cps, TRUE);

    dir = dirname;

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

/** Write the leagues into xml files with the given prefix. */
void
xml_loadsave_leagues_cups_write(const gchar *prefix)
{
    gint i;
    gchar buf[SMALL],
	*basename = g_path_get_basename(prefix);
    FILE *fil = NULL;

    sprintf(buf, "%s___leagues_cups.xml", prefix);
    file_my_fopen(buf, "w", &fil, TRUE);

    fprintf(fil, "%s<_%d>\n", I0, TAG_LEAGUES_CUPS);

    for(i=0;i<ligs->len;i++)
    {
	xml_loadsave_league_write(prefix, &lig(i));
	sprintf(buf, "%s___league_%d.xml", basename, lig(i).id);
	xml_write_string(fil, buf, TAG_LEAGUE_FILE, I1);
    }

    for(i=0;i<cps->len;i++)
    {
	xml_loadsave_cup_write(prefix, &cp(i));
	sprintf(buf, "%s___cup_%d.xml", basename, cp(i).id);
	xml_write_string(fil, buf, TAG_CUP_FILE, I1);
    }

    fprintf(fil, "%s</_%d>\n", I0, TAG_LEAGUES_CUPS);
    
    fclose(fil);

    g_free(basename);
}
