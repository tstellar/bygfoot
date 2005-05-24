#include "file.h"
#include "league.h"
#include "misc.h"
#include "xml.h"
#include "xml_loadsave_cup.h"
#include "xml_loadsave_fixtures.h"
#include "xml_loadsave_league.h"
#include "xml_loadsave_league_stat.h"
#include "xml_loadsave_table.h"
#include "xml_loadsave_teams.h"

enum
{
    TAG_LEAGUE = TAG_START_LEAGUE,
    TAG_LEAGUE_LAYER,
    TAG_LEAGUE_FIRST_WEEK,
    TAG_LEAGUE_ROUND_ROBINS,
    TAG_LEAGUE_AVERAGE_SKILL,
    TAG_LEAGUE_PROM_REL,
    TAG_LEAGUE_PROM_REL_PROM_GAMES_DEST_SID,
    TAG_LEAGUE_PROM_REL_PROM_GAMES_CUP_SID,
    TAG_LEAGUE_PROM_REL_PROM_GAMES_LOSER_SID,
    TAG_LEAGUE_PROM_REL_PROM_GAMES_NUMBER_OF_ADVANCE,
    TAG_LEAGUE_PROM_REL_CUP,
    TAG_LEAGUE_PROM_REL_ELEMENTS,
    TAG_LEAGUE_PROM_REL_ELEMENT,
    TAG_LEAGUE_PROM_REL_ELEMENT_RANK,
    TAG_LEAGUE_PROM_REL_ELEMENT_DEST_SID,
    TAG_LEAGUE_PROM_REL_ELEMENT_TYPE,
    TAG_END
};

gint promrankidx, state;
PromRelElement new_element;
League *new_league;

void
xml_loadsave_league_start_element (GMarkupParseContext *context,
				    const gchar         *element_name,
				    const gchar        **attribute_names,
				    const gchar        **attribute_values,
				    gpointer             user_data,
				    GError             **error)
{
    gint i;
    gint tag = xml_get_tag_from_name(element_name);
    gboolean valid_tag = FALSE;

    for(i=TAG_LEAGUE;i<TAG_END;i++)
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

    if(tag == TAG_LEAGUE_PROM_REL_ELEMENT)
    {
	new_element = prom_rel_element_new();
	promrankidx = 0;
    }

    if(!valid_tag)
	g_warning("xml_loadsave_league_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_league_end_element    (GMarkupParseContext *context,
				    const gchar         *element_name,
				    gpointer             user_data,
				    GError             **error)
{
    gint tag = xml_get_tag_from_name(element_name);
    
    if(tag == TAG_LEAGUE_FIRST_WEEK ||
       tag == TAG_LEAGUE_LAYER ||
       tag == TAG_LEAGUE_AVERAGE_SKILL ||
       tag == TAG_LEAGUE_ROUND_ROBINS ||
       tag == TAG_NAME ||
       tag == TAG_SHORT_NAME ||
       tag == TAG_SYMBOL ||
       tag == TAG_SID ||
       tag == TAG_ID ||
       tag == TAG_WEEK_GAP ||
       tag == TAG_YELLOW_RED ||
       tag == TAG_LEAGUE_PROM_REL)
	state = TAG_LEAGUE;
    else if(tag == TAG_LEAGUE_PROM_REL_PROM_GAMES_DEST_SID ||
	    tag == TAG_LEAGUE_PROM_REL_PROM_GAMES_CUP_SID ||
	    tag == TAG_LEAGUE_PROM_REL_PROM_GAMES_LOSER_SID ||
	    tag == TAG_LEAGUE_PROM_REL_PROM_GAMES_NUMBER_OF_ADVANCE ||
	    tag == TAG_LEAGUE_PROM_REL_CUP ||
	    tag == TAG_LEAGUE_PROM_REL_ELEMENTS)
	state = TAG_LEAGUE_PROM_REL;
    else if(tag == TAG_LEAGUE_PROM_REL_ELEMENT)
    {
	state = TAG_LEAGUE_PROM_REL_ELEMENTS;
	g_array_append_val(new_league->prom_rel.elements, new_element);
    }
    else if(tag == TAG_LEAGUE_PROM_REL_ELEMENT_RANK ||
	    tag == TAG_LEAGUE_PROM_REL_ELEMENT_DEST_SID ||
	    tag == TAG_LEAGUE_PROM_REL_ELEMENT_TYPE)
    {
	state = TAG_LEAGUE_PROM_REL_ELEMENT;
	if(tag == TAG_LEAGUE_PROM_REL_ELEMENT_RANK)
	    promrankidx++;
    }
    else if(tag != TAG_LEAGUE)
	g_warning("xml_league_read_end_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_league_text         (GMarkupParseContext *context,
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

    if(state == TAG_NAME)
	g_string_printf(new_league->name, "%s", buf);
    else if(state == TAG_SHORT_NAME)
	g_string_printf(new_league->short_name, "%s", buf);
    else if(state == TAG_SYMBOL)
	g_string_printf(new_league->symbol, "%s", buf);
    else if(state == TAG_SID)
	g_string_printf(new_league->sid, "%s", buf);
    else if(state == TAG_ID)
	new_league->id = int_value;
    else if(state == TAG_LEAGUE_LAYER)
	new_league->layer = int_value;
    else if(state == TAG_LEAGUE_FIRST_WEEK)
	new_league->first_week = int_value;
    else if(state == TAG_LEAGUE_ROUND_ROBINS)
	new_league->round_robins = int_value;
    else if(state == TAG_WEEK_GAP)
	new_league->week_gap = int_value;
    else if(state == TAG_YELLOW_RED)
	new_league->yellow_red = int_value;
    else if(state == TAG_LEAGUE_AVERAGE_SKILL)
	new_league->average_skill = int_value;
    else if(state == TAG_LEAGUE_PROM_REL_PROM_GAMES_DEST_SID)
	g_string_printf(new_league->prom_rel.prom_games_dest_sid, "%s", buf);
    else if(state == TAG_LEAGUE_PROM_REL_PROM_GAMES_CUP_SID)
	g_string_printf(new_league->prom_rel.prom_games_cup_sid, "%s", buf);
    else if(state == TAG_LEAGUE_PROM_REL_PROM_GAMES_NUMBER_OF_ADVANCE)
	new_league->prom_rel.prom_games_number_of_advance = int_value;
    else if(state == TAG_LEAGUE_PROM_REL_PROM_GAMES_LOSER_SID)
	g_string_printf(new_league->prom_rel.prom_games_loser_sid, "%s", buf);
    else if(state == TAG_LEAGUE_PROM_REL_ELEMENT_RANK)
	new_element.ranks[promrankidx] = int_value;
    else if(state == TAG_LEAGUE_PROM_REL_ELEMENT_TYPE)
	new_element.type = int_value;
    else if(state == TAG_LEAGUE_PROM_REL_ELEMENT_DEST_SID)
	g_string_printf(new_element.dest_sid, "%s", buf);
}

void
xml_loadsave_league_read(const gchar *filename, League *league)
{
    GMarkupParser parser = {xml_loadsave_league_start_element,
			    xml_loadsave_league_end_element,
			    xml_loadsave_league_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    guint length;
    GError *error = NULL;

    context = 
	g_markup_parse_context_new(&parser, 0, NULL, NULL);

    if(!g_file_get_contents(filename, &file_contents, &length, &error))
    {
	g_warning("xml_loadsave_league_read: error reading file %s\n", filename);
	misc_print_error(&error, TRUE);
    }

    new_league = league;

    if(g_markup_parse_context_parse(context, file_contents, length, &error))
    {
	g_markup_parse_context_end_parse(context, NULL);
	g_markup_parse_context_free(context);
	g_free(file_contents);
    }
    else
    {
	g_warning("xml_loadsave_league_read: error parsing file %s\n", filename);
	misc_print_error(&error, TRUE);
    }
}

void
xml_loadsave_league_write(const gchar *prefix, const League *league)
{
    gint i;
    gchar buf[SMALL];
    FILE *fil = NULL;

    sprintf(buf, "%s___league_%d_table.xml", prefix, league->id);
    xml_loadsave_table_write(buf, &league->table);

    sprintf(buf, "%s___league_%d_teams.xml", prefix, league->id);
    xml_loadsave_teams_write(buf, league->teams);

    sprintf(buf, "%s___league_%d_fixtures.xml", prefix, league->id);
    xml_loadsave_fixtures_write(buf, league->fixtures);

    sprintf(buf, "%s___league_%d_stat.xml", prefix, league->id);
    xml_loadsave_league_stat_write(buf, &league->stats);

    sprintf(buf, "%s___league_%d.xml", prefix, league->id);
    file_my_fopen(buf, "w", &fil, TRUE);

    fprintf(fil, "%s<_%d>\n", I0, TAG_LEAGUE);

    xml_write_g_string(fil, league->name, TAG_NAME, I0);
    xml_write_g_string(fil, league->short_name, TAG_SHORT_NAME, I0);
    xml_write_g_string(fil, league->sid, TAG_SID, I0);
    xml_write_g_string(fil, league->symbol, TAG_SYMBOL, I0);

    xml_write_int(fil, league->id, TAG_ID, I0);
    xml_write_int(fil, league->layer, TAG_LEAGUE_LAYER, I0);
    xml_write_int(fil, league->first_week, TAG_LEAGUE_FIRST_WEEK, I0);
    xml_write_int(fil, league->round_robins, TAG_LEAGUE_ROUND_ROBINS, I0);
    xml_write_int(fil, league->week_gap, TAG_WEEK_GAP, I0);
    xml_write_int(fil, league->yellow_red, TAG_YELLOW_RED, I0);
    xml_write_int(fil, league->average_skill, TAG_LEAGUE_AVERAGE_SKILL, I0);

    fprintf(fil, "%s<_%d>\n", I0, TAG_LEAGUE_PROM_REL);

    xml_write_g_string(fil, league->prom_rel.prom_games_dest_sid,
		       TAG_LEAGUE_PROM_REL_PROM_GAMES_DEST_SID, I1);
    xml_write_g_string(fil, league->prom_rel.prom_games_cup_sid,
		       TAG_LEAGUE_PROM_REL_PROM_GAMES_CUP_SID, I1);
    xml_write_int(fil, league->prom_rel.prom_games_number_of_advance,
		  TAG_LEAGUE_PROM_REL_PROM_GAMES_NUMBER_OF_ADVANCE, I1);
    xml_write_g_string(fil, league->prom_rel.prom_games_loser_sid,
		       TAG_LEAGUE_PROM_REL_PROM_GAMES_LOSER_SID, I1);

    fprintf(fil, "%s<_%d>\n", I1, TAG_LEAGUE_PROM_REL_ELEMENTS);
    for(i=0;i<league->prom_rel.elements->len;i++)
    {
	fprintf(fil, "%s<_%d>\n", I1, TAG_LEAGUE_PROM_REL_ELEMENT);
	xml_write_int(fil, g_array_index(league->prom_rel.elements, PromRelElement, i).ranks[0],
		      TAG_LEAGUE_PROM_REL_ELEMENT_RANK, I2);
	xml_write_int(fil, g_array_index(league->prom_rel.elements, PromRelElement, i).ranks[1],
		      TAG_LEAGUE_PROM_REL_ELEMENT_RANK, I2);
	xml_write_int(fil, g_array_index(league->prom_rel.elements, PromRelElement, i).type,
		      TAG_LEAGUE_PROM_REL_ELEMENT_TYPE, I2);
	xml_write_g_string(fil, g_array_index(league->prom_rel.elements, PromRelElement, i).dest_sid,
			   TAG_LEAGUE_PROM_REL_ELEMENT_DEST_SID, I2);
	fprintf(fil, "%s</_%d>\n", I1, TAG_LEAGUE_PROM_REL_ELEMENT);
    }
    fprintf(fil, "%s</_%d>\n", I1, TAG_LEAGUE_PROM_REL_ELEMENTS);

    fprintf(fil, "%s</_%d>\n", I0, TAG_LEAGUE_PROM_REL);

    fprintf(fil, "%s</_%d>\n", I0, TAG_LEAGUE);

    fclose(fil);
}


