#include "file.h"
#include "free.h"
#include "misc.h"
#include "stat.h"
#include "variables.h"
#include "xml.h"
#include "xml_loadsave_league_stat.h"
#include "xml_loadsave_season_stats.h"

enum
{
    TAG_SEASON_STATS = TAG_START_SEASON_STATS,
    TAG_SEASON_STAT,
    TAG_SEASON_NUMBER,
    TAG_LEAGUE_CHAMPS,
    TAG_CUP_CHAMPS,
    TAG_CHAMP_STAT,
    TAG_CHAMP_STAT_TEAM_NAME,
    TAG_CHAMP_STAT_CL_NAME,
    TAG_LEAGUE_STAT_FILE,
    TAG_END
};

gint state, in_state;
ChampStat new_champ_stat;
SeasonStat new_season_stat;
const gchar *dirname2;

void
xml_loadsave_season_stats_start_element (GMarkupParseContext *context,
					   const gchar         *element_name,
					   const gchar        **attribute_names,
					   const gchar        **attribute_values,
					   gpointer             user_data,
					   GError             **error)
{
    gint i;
    gint tag = xml_get_tag_from_name(element_name);
    gboolean valid_tag = FALSE;

    for(i=TAG_SEASON_STATS;i<TAG_END;i++)
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

    if(tag == TAG_LEAGUE_CHAMPS)
	in_state = TAG_LEAGUE_CHAMPS;
    else if(tag == TAG_CUP_CHAMPS)
	in_state = TAG_CUP_CHAMPS;
    else if(tag == TAG_SEASON_STAT)
	new_season_stat = stat_season_stat_new(-1);

    if(!valid_tag)
	g_warning("xml_loadsave_season_stats_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_season_stats_end_element    (GMarkupParseContext *context,
					    const gchar         *element_name,
					    gpointer             user_data,
					    GError             **error)
{
    gint tag = xml_get_tag_from_name(element_name);

    if(tag == TAG_SEASON_STAT)
    {
	state = TAG_SEASON_STATS;
	g_array_append_val(season_stats, new_season_stat);
    }
    else if(tag == TAG_SEASON_NUMBER ||
	    tag == TAG_LEAGUE_CHAMPS ||
	    tag == TAG_CUP_CHAMPS ||
	    tag == TAG_LEAGUE_STAT_FILE)
	state = TAG_SEASON_STAT;
    else if(tag == TAG_CHAMP_STAT)
    {
	if(in_state == TAG_CUP_CHAMPS)
	    g_array_append_val(new_season_stat.cup_champs, 
			       new_champ_stat);
	else if(in_state == TAG_LEAGUE_CHAMPS)
	    g_array_append_val(new_season_stat.league_champs, 
			       new_champ_stat);
	state = in_state;
    }
    else if(tag == TAG_CHAMP_STAT_TEAM_NAME ||
	    tag == TAG_CHAMP_STAT_CL_NAME)
	state = TAG_CHAMP_STAT;
    else if(tag != TAG_SEASON_STATS)
	g_warning("xml_loadsave_season_stats_end_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_season_stats_text         (GMarkupParseContext *context,
					  const gchar         *text,
					  gsize                text_len,  
					  gpointer             user_data,
					  GError             **error)
{
    gchar buf[SMALL], buf2[SMALL];
    LeagueStat new_league_stat;
    gint int_value = -1;

    strncpy(buf, text, text_len);
    buf[text_len] = '\0';

    int_value = (gint)g_ascii_strtod(buf, NULL);

    if(state == TAG_SEASON_NUMBER)
	new_season_stat.season_number = int_value;
    else if(state == TAG_CHAMP_STAT_TEAM_NAME)
	new_champ_stat.team_name = g_string_new(buf);
    else if(state == TAG_CHAMP_STAT_CL_NAME)
	new_champ_stat.cl_name = g_string_new(buf);
    else if(state == TAG_LEAGUE_STAT_FILE)
    {
	new_league_stat = stat_league_new(-1);
	sprintf(buf2, "%s/%s", dirname2, buf);
	xml_loadsave_league_stat_read(buf2, &new_league_stat);
	g_array_append_val(new_season_stat.league_stats, new_league_stat);
    }
}

void
xml_loadsave_season_stats_read(const gchar *dirname, const gchar *prefix)
{
    gchar filename[SMALL];
    GMarkupParser parser = {xml_loadsave_season_stats_start_element,
			    xml_loadsave_season_stats_end_element,
			    xml_loadsave_season_stats_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    guint length;
    GError *error = NULL;

    context = 
	g_markup_parse_context_new(&parser, 0, NULL, NULL);

    sprintf(filename, "%s/%s___season_stats.xml", dirname, prefix);

    if(!g_file_get_contents(filename, &file_contents, &length, &error))
    {
	g_warning("xml_loadsave_season_stats_read: error reading file %s\n", filename);
	misc_print_error(&error, TRUE);
    }

    free_season_stats(TRUE);
    dirname2 = dirname;

    if(g_markup_parse_context_parse(context, file_contents, length, &error))
    {
	g_markup_parse_context_end_parse(context, NULL);
	g_markup_parse_context_free(context);
	g_free(file_contents);
    }
    else
    {
	g_warning("xml_loadsave_season_stats_read: error parsing file %s\n", filename);
	misc_print_error(&error, TRUE);
    }
}

void
xml_loadsave_season_stats_write(const gchar *prefix)
{
    gint i, j;
    gchar buf[SMALL];
    FILE *fil = NULL;
    gchar *basename = NULL;

    sprintf(buf, "%s___season_stats.xml", prefix);

    file_my_fopen(buf, "w", &fil, TRUE);

    fprintf(fil, "<_%d>\n", TAG_SEASON_STATS);
    
    for(i=0;i<season_stats->len;i++)
    {
	fprintf(fil, "<_%d>\n", TAG_SEASON_STAT);

	xml_write_int(fil, g_array_index(season_stats, SeasonStat, i).season_number,
		      TAG_SEASON_NUMBER, I1);

	fprintf(fil, "%s<_%d>\n", I1, TAG_LEAGUE_CHAMPS);
	for(j=0;j<g_array_index(season_stats, SeasonStat, i).league_champs->len;j++)
	{
	    fprintf(fil, "%s<_%d>\n", I2, TAG_CHAMP_STAT);
	    xml_write_g_string(fil,
			       g_array_index(
				   g_array_index(
				       season_stats, SeasonStat, i).league_champs, ChampStat, j).team_name,
			       TAG_CHAMP_STAT_TEAM_NAME, I2);
	    xml_write_g_string(fil,
			       g_array_index(
				   g_array_index(
				       season_stats, SeasonStat, i).league_champs, ChampStat, j).cl_name,
			       TAG_CHAMP_STAT_CL_NAME, I2);
			       
	    fprintf(fil, "%s</_%d>\n", I2, TAG_CHAMP_STAT);
	}
	fprintf(fil, "%s</_%d>\n", I1, TAG_LEAGUE_CHAMPS);

	fprintf(fil, "%s<_%d>\n", I1, TAG_CUP_CHAMPS);
	for(j=0;j<g_array_index(season_stats, SeasonStat, i).cup_champs->len;j++)
	{
	    fprintf(fil, "%s<_%d>\n", I2, TAG_CHAMP_STAT);
	    xml_write_g_string(fil, g_array_index(
				   g_array_index(
				       season_stats, SeasonStat, i).cup_champs, ChampStat, j).team_name,
			       TAG_CHAMP_STAT_TEAM_NAME, I2);
	    xml_write_g_string(fil, g_array_index(
				   g_array_index(
				       season_stats, SeasonStat, i).cup_champs, ChampStat, j).cl_name,
			       TAG_CHAMP_STAT_CL_NAME, I2);
			       
	    fprintf(fil, "%s</_%d>\n", I2, TAG_CHAMP_STAT);
	}
	fprintf(fil, "%s</_%d>\n", I1, TAG_CUP_CHAMPS);

	for(j=0;j<g_array_index(season_stats, SeasonStat, i).league_stats->len;j++)
	{
	    sprintf(buf, "%s___season_stats_league_stat_%02d-%02d.xml", prefix, i, j);
	    basename = g_path_get_basename(buf);
	    xml_write_string(fil, basename, TAG_LEAGUE_STAT_FILE, I1);
	    xml_loadsave_league_stat_write(buf, &g_array_index(
					       g_array_index(season_stats, SeasonStat, i).league_stats, LeagueStat, j));
	    g_free(basename);
	}

	fprintf(fil, "</_%d>\n", TAG_SEASON_STAT);
    }

    fprintf(fil, "</_%d>\n", TAG_SEASON_STATS);

    fclose(fil);
}
