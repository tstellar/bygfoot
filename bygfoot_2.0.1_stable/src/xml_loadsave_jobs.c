/*
   xml_loadsave_jobs.c

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

#include "job.h"
#include "file.h"
#include "free.h"
#include "misc.h"
#include "variables.h"
#include "xml.h"
#include "xml_loadsave_jobs.h"
#include "xml_loadsave_teams.h"

enum
{
    TAG_JOBS = TAG_START_JOBS,
    TAG_JOB,
    TAG_JOB_TYPE,
    TAG_JOB_TIME,
    TAG_JOB_COUNTRY_FILE,
    TAG_JOB_COUNTRY_NAME,
    TAG_JOB_COUNTRY_RATING,
    TAG_JOB_LEAGUE_NAME,
    TAG_JOB_LEAGUE_LAYER,
    TAG_JOB_TALENT_PERCENT,
    TAG_JOB_TEAM_ID,
    TAG_END
};

gint state;
Job new_job;

void
xml_loadsave_jobs_start_element (GMarkupParseContext *context,
				 const gchar         *element_name,
				 const gchar        **attribute_names,
				 const gchar        **attribute_values,
				 gpointer             user_data,
				 GError             **error)
{
    gint i;
    gint tag = xml_get_tag_from_name(element_name);
    gboolean valid_tag = FALSE;

    for(i=TAG_JOBS;i<TAG_END;i++)
	if(tag == i)
	{
	    state = i;
	    valid_tag = TRUE;
	}

    if(tag == TAG_JOB)
	new_job.country_file = new_job.country_name =
	    new_job.league_name = NULL;

    if(!valid_tag)
	g_warning("xml_loadsave_jobs_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_jobs_end_element    (GMarkupParseContext *context,
				  const gchar         *element_name,
				  gpointer             user_data,
				  GError             **error)
{
    gint tag = xml_get_tag_from_name(element_name);
    
    if(tag == TAG_JOB)
    {
	state = TAG_JOBS;
	g_array_append_val(jobs, new_job);
    }
    else if(tag == TAG_JOB_TYPE ||
	    tag == TAG_JOB_TIME ||
	    tag == TAG_JOB_COUNTRY_FILE ||
	    tag == TAG_JOB_COUNTRY_NAME ||
	    tag == TAG_JOB_COUNTRY_RATING ||
	    tag == TAG_JOB_LEAGUE_NAME ||
	    tag == TAG_JOB_LEAGUE_LAYER ||
	    tag == TAG_JOB_TALENT_PERCENT ||
	    tag == TAG_JOB_TEAM_ID)
	state = TAG_JOB;
    else if(tag != TAG_JOBS)
	g_warning("xml_loadsave_jobs_end_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_jobs_text         (GMarkupParseContext *context,
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
    float_value = (gfloat)g_ascii_strtod(text, NULL) / 10000;

    if(state == TAG_JOB_TYPE)
	new_job.type = int_value;
    else if(state == TAG_JOB_TIME)
	new_job.time = int_value;
    else if(state == TAG_JOB_COUNTRY_FILE)
	new_job.country_file = g_strdup(buf);
    else if(state == TAG_JOB_COUNTRY_NAME)
	new_job.country_name = g_strdup(buf);
    else if(state == TAG_JOB_COUNTRY_RATING)
	new_job.country_rating = int_value;
    else if(state == TAG_JOB_LEAGUE_NAME)
	new_job.league_name = g_strdup(buf);
    else if(state == TAG_JOB_LEAGUE_LAYER)
	new_job.league_layer = int_value;
    else if(state == TAG_JOB_TALENT_PERCENT)
	new_job.talent_percent = int_value;
    else if(state == TAG_JOB_TEAM_ID)
	new_job.team_id = int_value;
}

void
xml_loadsave_jobs_read(const gchar *dirname, const gchar *basename)
{
    GMarkupParser parser = {xml_loadsave_jobs_start_element,
			    xml_loadsave_jobs_end_element,
			    xml_loadsave_jobs_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    gsize length;
    GError *error = NULL;
    gchar file[SMALL];

    free_jobs(TRUE);

    sprintf(file, "%s%s%s___job_teams.xml", dirname, G_DIR_SEPARATOR_S, basename);
    xml_loadsave_teams_read(file, job_teams);

    sprintf(file, "%s%s%s___jobs.xml", dirname, G_DIR_SEPARATOR_S, basename);

    context = 
	g_markup_parse_context_new(&parser, 0, NULL, NULL);

    if(!g_file_get_contents(file, &file_contents, &length, &error))
    {
	g_warning("xml_loadsave_jobs_read: error reading file %s\n", file);
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
	g_warning("xml_loadsave_jobs_read: error parsing file %s\n", file);
	misc_print_error(&error, TRUE);
    }
}

void
xml_loadsave_jobs_write(const gchar *prefix)
{
    gint i;
    gchar buf[SMALL];
    FILE *fil = NULL;

    sprintf(buf, "%s___job_teams.xml", prefix);
    xml_loadsave_teams_write(buf, job_teams);    

    sprintf(buf, "%s___jobs.xml", prefix);
    file_my_fopen(buf, "w", &fil, TRUE);

    fprintf(fil, "<_%d>\n", TAG_JOBS);

    for(i=0;i<jobs->len;i++)
    {
	fprintf(fil, "<_%d>\n", TAG_JOB);

	xml_write_int(fil, g_array_index(jobs, Job, i).type, 
		      TAG_JOB_TYPE, I1);
	xml_write_int(fil, g_array_index(jobs, Job, i).time, 
		      TAG_JOB_TIME, I1);

	if(g_array_index(jobs, Job, i).country_file != NULL)
	    xml_write_string(fil, g_array_index(jobs, Job, i).country_file,
			     TAG_JOB_COUNTRY_FILE, I1);
	if(g_array_index(jobs, Job, i).country_name != NULL)
	    xml_write_string(fil, g_array_index(jobs, Job, i).country_name,
			     TAG_JOB_COUNTRY_NAME, I1);

	xml_write_int(fil, g_array_index(jobs, Job, i).country_rating, 
		      TAG_JOB_COUNTRY_RATING, I1);

	if(g_array_index(jobs, Job, i).league_name != NULL)
	    xml_write_string(fil, g_array_index(jobs, Job, i).league_name,
			     TAG_JOB_LEAGUE_NAME, I1);

	xml_write_int(fil, g_array_index(jobs, Job, i).league_layer, 
		      TAG_JOB_LEAGUE_LAYER, I1);

	xml_write_int(fil, g_array_index(jobs, Job, i).talent_percent, 
		      TAG_JOB_TALENT_PERCENT, I1);
	xml_write_int(fil, g_array_index(jobs, Job, i).team_id, 
		      TAG_JOB_TEAM_ID, I1);

	fprintf(fil, "</_%d>\n", TAG_JOB);
    }

    fprintf(fil, "</_%d>\n", TAG_JOBS);

    fclose(fil);
}
