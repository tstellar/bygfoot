#include "file.h"
#include "misc.h"
#include "stat.h"
#include "team.h"
#include "xml.h"
#include "xml_loadsave_league_stat.h"

enum
{
    TAG_LEAGUE_STAT = TAG_START_LEAGUE_STAT,
    TAG_STAT_TEAMS_OFF,
    TAG_STAT_TEAMS_DEF,
    TAG_STAT_PLAYER_SCORERS,
    TAG_STAT_PLAYER_GOALIES,
    TAG_STAT,
    TAG_STAT_VALUE,
    TAG_STAT_VALUE_STRING,
    TAG_END
};

gint state, in_state, valueidx;
Stat new_stat;
LeagueStat *lig_stat;

void
xml_loadsave_league_stat_start_element (GMarkupParseContext *context,
					const gchar         *element_name,
					const gchar        **attribute_names,
					const gchar        **attribute_values,
					gpointer             user_data,
					GError             **error)
{
    gint i;
    gint tag = xml_get_tag_from_name(element_name);
    gboolean valid_tag = FALSE;

    for(i=TAG_LEAGUE_STAT;i<TAG_END;i++)
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

    if(tag == TAG_STAT)
	valueidx = 0;
    else if(tag == TAG_STAT_TEAMS_OFF ||
	    tag == TAG_STAT_TEAMS_DEF ||
	    tag == TAG_STAT_PLAYER_SCORERS ||
	    tag == TAG_STAT_PLAYER_GOALIES)
	in_state = tag;

    if(!valid_tag)
	g_warning("xml_loadsave_league_stat_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_league_stat_end_element    (GMarkupParseContext *context,
					 const gchar         *element_name,
					 gpointer             user_data,
					 GError             **error)
{
    GArray *stat_array = NULL;
    gint tag = xml_get_tag_from_name(element_name);

    if(tag == TAG_ID ||
       tag == TAG_STAT_TEAMS_OFF ||
       tag == TAG_STAT_TEAMS_DEF ||
       tag == TAG_STAT_PLAYER_SCORERS ||
       tag == TAG_STAT_PLAYER_GOALIES)
	state = TAG_LEAGUE_STAT;
    else if(tag == TAG_STAT)
    {	
	state = in_state;
	if(in_state == TAG_STAT_TEAMS_OFF)
	    stat_array = lig_stat->teams_off;
	else if(in_state == TAG_STAT_TEAMS_DEF)
	    stat_array = lig_stat->teams_def;
	else if(in_state == TAG_STAT_PLAYER_SCORERS)
	    stat_array = lig_stat->player_scorers;
	else if(in_state == TAG_STAT_PLAYER_GOALIES)
	    stat_array = lig_stat->player_goalies;
	else
	{
	    g_warning("xml_loadsave_league_stat_end_element: unknown in_state %d \n", 
		      in_state);
	    return;
	}

	g_array_append_val(stat_array, new_stat);
    }
    else if(tag == TAG_STAT_VALUE_STRING ||
	    tag == TAG_STAT_VALUE ||
	    tag == TAG_STAT_VALUE_STRING ||
	    tag == TAG_TEAM_ID)
    {
	state = TAG_STAT;
	if(tag == TAG_STAT_VALUE)
	    valueidx++;	
    }
    else if(tag != TAG_LEAGUE_STAT)
	g_warning("xml_loadsave_league_stat_end_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_league_stat_text         (GMarkupParseContext *context,
				       const gchar         *text,
				       gsize                text_len,  
				       gpointer             user_data,
				       GError             **error)
{
    gchar buf[SMALL];
    gint int_value = -1;

    strncpy(buf, text, text_len);
    buf[text_len] = '\0';

    int_value = (gint)g_ascii_strtod(buf, NULL);

    if(state == TAG_ID)
	lig_stat->clid = int_value;
    else if(state == TAG_TEAM_ID)
	new_stat.team_id = int_value;
    else if(state == TAG_STAT_VALUE)
    {
	if(valueidx == 0)
	    new_stat.value1 = int_value;
	else if(valueidx == 1)
	    new_stat.value2 = int_value;
	else
	    new_stat.value3 = int_value;
    }
    else if(state == TAG_STAT_VALUE_STRING)
	new_stat.value_string = g_strdup(buf);
}

void
xml_loadsave_league_stat_read(const gchar *filename, LeagueStat *league_stat)
{
    GMarkupParser parser = {xml_loadsave_league_stat_start_element,
			    xml_loadsave_league_stat_end_element,
			    xml_loadsave_league_stat_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    guint length;
    GError *error = NULL;

    context = 
	g_markup_parse_context_new(&parser, 0, NULL, NULL);

    if(!g_file_get_contents(filename, &file_contents, &length, &error))
    {
	g_warning("xml_loadsave_league_stat_read: error reading file %s\n", filename);
	misc_print_error(&error, TRUE);
    }

    lig_stat = league_stat;

    if(g_markup_parse_context_parse(context, file_contents, length, &error))
    {
	g_markup_parse_context_end_parse(context, NULL);
	g_markup_parse_context_free(context);
	g_free(file_contents);
    }
    else
    {
	g_warning("xml_loadsave_league_stat_read: error parsing file %s\n", filename);
	misc_print_error(&error, TRUE);
    }
}

void
xml_loadsave_league_stat_write(const gchar *filename, const LeagueStat *league_stat)
{
    gint i;
    FILE *fil = NULL;

    file_my_fopen(filename, "w", &fil, TRUE);

    fprintf(fil, "<_%d>\n", TAG_LEAGUE_STAT);
    
    xml_write_int(fil, league_stat->clid, TAG_ID, I0);

    fprintf(fil, "<_%d>\n", TAG_STAT_TEAMS_OFF);
    for(i=0;i<league_stat->teams_off->len;i++)
	xml_loadsave_league_stat_write_stat(
	    fil, &g_array_index(league_stat->teams_off, Stat, i));
    fprintf(fil, "</_%d>\n", TAG_STAT_TEAMS_OFF);

    fprintf(fil, "<_%d>\n", TAG_STAT_TEAMS_DEF);
    for(i=0;i<league_stat->teams_def->len;i++)
	xml_loadsave_league_stat_write_stat(
	    fil, &g_array_index(league_stat->teams_def, Stat, i));
    fprintf(fil, "</_%d>\n", TAG_STAT_TEAMS_DEF);

    fprintf(fil, "<_%d>\n", TAG_STAT_PLAYER_SCORERS);
    for(i=0;i<league_stat->player_scorers->len;i++)
	xml_loadsave_league_stat_write_stat(
	    fil, &g_array_index(league_stat->player_scorers, Stat, i));
    fprintf(fil, "</_%d>\n", TAG_STAT_PLAYER_SCORERS);

    fprintf(fil, "<_%d>\n", TAG_STAT_PLAYER_GOALIES);
    for(i=0;i<league_stat->player_goalies->len;i++)
	xml_loadsave_league_stat_write_stat(
	    fil, &g_array_index(league_stat->player_goalies, Stat, i));
    fprintf(fil, "</_%d>\n", TAG_STAT_PLAYER_GOALIES);

    fprintf(fil, "</_%d>\n", TAG_LEAGUE_STAT);

    fclose(fil);
}

void
xml_loadsave_league_stat_write_stat(FILE *fil, const Stat *stat)
{
    fprintf(fil, "%s<_%d>\n", I1, TAG_STAT);

    xml_write_int(fil, stat->team_id, TAG_TEAM_ID, I1);
    xml_write_int(fil, stat->value1, TAG_STAT_VALUE, I1);
    xml_write_int(fil, stat->value2, TAG_STAT_VALUE, I1);
    xml_write_int(fil, stat->value3, TAG_STAT_VALUE, I1);
    xml_write_string(fil, stat->value_string, TAG_STAT_VALUE_STRING, I1);

    fprintf(fil, "%s</_%d>\n", I1, TAG_STAT);
}
