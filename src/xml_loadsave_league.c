/*
   xml_loadsave_league.c

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
#include "league.h"
#include "misc.h"
#include "option.h"
#include "table.h"
#include "variables.h"
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
    TAG_LEAGUE_AVERAGE_TALENT,
    TAG_LEAGUE_PROM_REL,
    TAG_LEAGUE_PROM_GAMES,
    TAG_LEAGUE_PROM_REL_PROM_GAMES_DEST_SID,
    TAG_LEAGUE_PROM_REL_PROM_GAMES_CUP_SID,
    TAG_LEAGUE_PROM_REL_PROM_GAMES_LOSER_SID,
    TAG_LEAGUE_PROM_REL_PROM_GAMES_NUMBER_OF_ADVANCE,
    TAG_LEAGUE_PROM_REL_ELEMENTS,
    TAG_LEAGUE_PROM_REL_ELEMENT,
    TAG_LEAGUE_PROM_REL_ELEMENT_RANK,
    TAG_LEAGUE_PROM_REL_ELEMENT_DEST_SID,
    TAG_LEAGUE_PROM_REL_ELEMENT_TYPE,
    TAG_LEAGUE_PROM_REL_ELEMENT_FROM_TABLE,
    TAG_LEAGUE_BREAK,
    TAG_LEAGUE_JOINED_LEAGUE_SID,
    TAG_LEAGUE_JOINED_LEAGUE_RR,
    TAG_LEAGUE_NEW_TABLE_NAME,
    TAG_LEAGUE_NEW_TABLE_ADD_WEEK,
    TAG_LEAGUE_TWO_MATCH_WEEK_START,
    TAG_LEAGUE_TWO_MATCH_WEEK_END,
    TAG_LEAGUE_TABLE_FILE,
    TAG_END
};

gint promrankidx, state;
PromRelElement new_element;
PromGames new_prom_games;
WeekBreak new_week_break;
League *new_league;
gchar *dirname;

void
xml_loadsave_league_start_element (GMarkupParseContext *context,
				    const gchar         *element_name,
				    const gchar        **attribute_names,
				    const gchar        **attribute_values,
				    gpointer             user_data,
				    GError             **error)
{
#ifdef DEBUG
    printf("xml_loadsave_league_start_element\n");
#endif

    gint i;
    gint tag = xml_get_tag_from_name(element_name);
    gboolean valid_tag = FALSE;
    JoinedLeague new_joined_league;
    NewTable new_table;

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

    if(tag == TAG_LEAGUE_PROM_GAMES)
        new_prom_games = prom_games_new();

    if(tag == TAG_LEAGUE_JOINED_LEAGUE_SID)
        g_array_append_val(new_league->joined_leagues, new_joined_league);        

    if(tag == TAG_LEAGUE_NEW_TABLE_NAME)
        g_array_append_val(new_league->new_tables, new_table);        

    if(debug > 100)
        g_print("xml_loadsave_league_start_element: state %d\n", state);

    if(!valid_tag)
	debug_print_message("xml_loadsave_league_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_league_end_element    (GMarkupParseContext *context,
				    const gchar         *element_name,
				    gpointer             user_data,
				    GError             **error)
{
#ifdef DEBUG
    printf("xml_loadsave_league_end_element\n");
#endif

    gint tag = xml_get_tag_from_name(element_name);
    
    if(tag == TAG_LEAGUE_FIRST_WEEK ||
       tag == TAG_LEAGUE_LAYER ||
       tag == TAG_LEAGUE_BREAK ||
       tag == TAG_LEAGUE_JOINED_LEAGUE_SID ||
       tag == TAG_LEAGUE_JOINED_LEAGUE_RR ||
       tag == TAG_LEAGUE_NEW_TABLE_NAME ||
       tag == TAG_LEAGUE_NEW_TABLE_ADD_WEEK ||
       tag == TAG_LEAGUE_TWO_MATCH_WEEK_START ||
       tag == TAG_LEAGUE_TWO_MATCH_WEEK_END ||
       tag == TAG_LEAGUE_TABLE_FILE ||
       tag == TAG_LEAGUE_AVERAGE_TALENT ||
       tag == TAG_LEAGUE_AVERAGE_TALENT ||
       tag == TAG_LEAGUE_ROUND_ROBINS ||
       tag == TAG_PROPERTY ||
       tag == TAG_NAME ||
       tag == TAG_NAMES_FILE ||
       tag == TAG_SHORT_NAME ||
       tag == TAG_SYMBOL ||
       tag == TAG_SID ||
       tag == TAG_ID ||
       tag == TAG_WEEK_GAP ||
       tag == TAG_WEEK_BREAK ||
       tag == TAG_WEEK_BREAK_LENGTH ||
       tag == TAG_SKIP_WEEKS_WITH ||
       tag == TAG_YELLOW_RED ||
       tag == TAG_LEAGUE_PROM_REL)
	state = TAG_LEAGUE;
    else if(tag == TAG_LEAGUE_PROM_GAMES ||
	    tag == TAG_LEAGUE_PROM_REL_ELEMENTS)
    {
     	state = TAG_LEAGUE_PROM_REL;
        if(tag == TAG_LEAGUE_PROM_GAMES)
            g_array_append_val(new_league->prom_rel.prom_games, new_prom_games);
    }
    else if(tag == TAG_LEAGUE_PROM_REL_PROM_GAMES_DEST_SID ||
	    tag == TAG_LEAGUE_PROM_REL_PROM_GAMES_CUP_SID ||
	    tag == TAG_LEAGUE_PROM_REL_PROM_GAMES_LOSER_SID ||
	    tag == TAG_LEAGUE_PROM_REL_PROM_GAMES_NUMBER_OF_ADVANCE)
	state = TAG_LEAGUE_PROM_GAMES;
    else if(tag == TAG_LEAGUE_PROM_REL_ELEMENT)
    {
	state = TAG_LEAGUE_PROM_REL_ELEMENTS;
	g_array_append_val(new_league->prom_rel.elements, new_element);
    }
    else if(tag == TAG_LEAGUE_PROM_REL_ELEMENT_RANK ||
	    tag == TAG_LEAGUE_PROM_REL_ELEMENT_DEST_SID ||
	    tag == TAG_LEAGUE_PROM_REL_ELEMENT_FROM_TABLE ||
	    tag == TAG_LEAGUE_PROM_REL_ELEMENT_TYPE)
    {
	state = TAG_LEAGUE_PROM_REL_ELEMENT;
	if(tag == TAG_LEAGUE_PROM_REL_ELEMENT_RANK)
	    promrankidx++;
    }
    else if(tag != TAG_LEAGUE)
	debug_print_message("xml_league_read_end_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_league_text         (GMarkupParseContext *context,
				   const gchar         *text,
				   gsize                text_len,  
				   gpointer             user_data,
				   GError             **error)
{
#ifdef DEBUG
    printf("xml_loadsave_league_text\n");
#endif

    gchar buf[SMALL], buf2[SMALL];
    gint int_value = -1;
    gfloat float_value = -1;
    Table new_table;

    strncpy(buf, text, text_len);
    buf[text_len] = '\0';

    float_value = (gfloat)g_ascii_strtod(buf, NULL) / 10000;
    int_value = (gint)g_ascii_strtod(buf, NULL);

    if(state == TAG_NAME)
	misc_string_assign(&new_league->name, buf);
    else if(state == TAG_PROPERTY)
	g_ptr_array_add(new_league->properties, g_strdup(buf));
    else if(state == TAG_SHORT_NAME)
	misc_string_assign(&new_league->short_name, buf);
    else if(state == TAG_NAMES_FILE)
	misc_string_assign(&new_league->names_file, buf);
    else if(state == TAG_SYMBOL)
	misc_string_assign(&new_league->symbol, buf);
    else if(state == TAG_SID)
	misc_string_assign(&new_league->sid, buf);
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
    else if(state == TAG_WEEK_BREAK)
        new_week_break.week_number = int_value;
    else if(state == TAG_WEEK_BREAK_LENGTH)
    {
        new_week_break.length = int_value;
        g_array_append_val(new_league->week_breaks, new_week_break);
    }
    else if(state == TAG_SKIP_WEEKS_WITH)
        g_ptr_array_add(new_league->skip_weeks_with, g_strdup(buf));
    else if(state == TAG_YELLOW_RED)
	new_league->yellow_red = int_value;
    else if(state == TAG_LEAGUE_BREAK)
	g_array_append_val(new_league->rr_breaks, int_value);
    else if(state == TAG_LEAGUE_JOINED_LEAGUE_SID)	
        g_array_index(new_league->joined_leagues,
                      JoinedLeague,
                      new_league->joined_leagues->len - 1).sid = g_strdup(buf);
    else if(state == TAG_LEAGUE_JOINED_LEAGUE_RR)
        g_array_index(new_league->joined_leagues,
                      JoinedLeague,
                      new_league->joined_leagues->len - 1).rr = int_value;
    else if(state == TAG_LEAGUE_NEW_TABLE_NAME)	
        g_array_index(new_league->new_tables,
                      NewTable,
                      new_league->new_tables->len - 1).name = g_strdup(buf);
    else if(state == TAG_LEAGUE_NEW_TABLE_ADD_WEEK)
        g_array_index(new_league->new_tables,
                      NewTable,
                      new_league->new_tables->len - 1).add_week = int_value;
    else if(state == TAG_LEAGUE_TWO_MATCH_WEEK_START)
	g_array_append_val(new_league->two_match_weeks[0], int_value);
    else if(state == TAG_LEAGUE_TWO_MATCH_WEEK_END)
	g_array_append_val(new_league->two_match_weeks[1], int_value);
    else if(state == TAG_LEAGUE_TABLE_FILE)
    {
	new_table = table_new();

	sprintf(buf2, "%s%s%s", dirname, G_DIR_SEPARATOR_S, buf);
	xml_loadsave_table_read(buf2, &new_table);
	g_array_append_val(new_league->tables, new_table);
    }
    else if(state == TAG_LEAGUE_AVERAGE_TALENT)
	new_league->average_talent = float_value;
    else if(state == TAG_LEAGUE_PROM_REL_PROM_GAMES_DEST_SID)
	new_prom_games.dest_sid = g_strdup(buf);
    else if(state == TAG_LEAGUE_PROM_REL_PROM_GAMES_CUP_SID)
	new_prom_games.cup_sid = g_strdup(buf);
    else if(state == TAG_LEAGUE_PROM_REL_PROM_GAMES_NUMBER_OF_ADVANCE)
	new_prom_games.number_of_advance = int_value;
    else if(state == TAG_LEAGUE_PROM_REL_PROM_GAMES_LOSER_SID)
	new_prom_games.loser_sid = g_strdup(buf);
    else if(state == TAG_LEAGUE_PROM_REL_ELEMENT_RANK)
	new_element.ranks[promrankidx] = int_value;
    else if(state == TAG_LEAGUE_PROM_REL_ELEMENT_TYPE)
	new_element.type = int_value;
    else if(state == TAG_LEAGUE_PROM_REL_ELEMENT_FROM_TABLE)
	new_element.from_table = int_value;
    else if(state == TAG_LEAGUE_PROM_REL_ELEMENT_DEST_SID)
	misc_string_assign(&new_element.dest_sid, buf);
}

void
xml_loadsave_league_read(const gchar *filename, const gchar *team_file, League *league)
{
#ifdef DEBUG
    printf("xml_loadsave_league_read\n");
#endif

    GMarkupParser parser = {xml_loadsave_league_start_element,
			    xml_loadsave_league_end_element,
			    xml_loadsave_league_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    gsize length;
    GError *error = NULL;

    context = 
	g_markup_parse_context_new(&parser, 0, NULL, NULL);

    if(!g_file_get_contents(filename, &file_contents, &length, &error))
    {
	debug_print_message("xml_loadsave_league_read: error reading file %s\n", filename);
	misc_print_error(&error, TRUE);
    }

    new_league = league;
    dirname = g_path_get_dirname(filename);

    xml_loadsave_teams_read(team_file, new_league->teams);

    if(g_markup_parse_context_parse(context, file_contents, length, &error))
    {
	g_markup_parse_context_end_parse(context, NULL);
	g_markup_parse_context_free(context);
	g_free(file_contents);
    }
    else
    {
	debug_print_message("xml_loadsave_league_read: error parsing file %s\n", filename);
	misc_print_error(&error, TRUE);
    }

    g_free(dirname);
}

void
xml_loadsave_league_write(const gchar *prefix, const League *league)
{
#ifdef DEBUG
    printf("xml_loadsave_league_write\n");
#endif

    gint i;
    gchar buf[SMALL];
    FILE *fil = NULL;
    gchar *basename = g_path_get_basename(prefix);

    sprintf(buf, "%s___league_%d_teams.xml", prefix, league->id);
    xml_loadsave_teams_write(buf, league->teams);

    sprintf(buf, "%s___league_%d_fixtures.xml", prefix, league->id);
    xml_loadsave_fixtures_write(buf, league->fixtures);

    sprintf(buf, "%s___league_%d_stat.xml", prefix, league->id);
    xml_loadsave_league_stat_write(buf, &league->stats);

    sprintf(buf, "%s___league_%d.xml", prefix, league->id);
    file_my_fopen(buf, "w", &fil, TRUE);

    fprintf(fil, "%s<_%d>\n", I0, TAG_LEAGUE);

    xml_write_string(fil, league->name, TAG_NAME, I0);
    xml_write_string(fil, league->short_name, TAG_SHORT_NAME, I0);
    xml_write_string(fil, league->names_file, TAG_NAMES_FILE, I0);
    xml_write_string(fil, league->sid, TAG_SID, I0);
    xml_write_string(fil, league->symbol, TAG_SYMBOL, I0);

    for(i=0;i<league->properties->len;i++)
	xml_write_string(fil, (gchar*)g_ptr_array_index(league->properties, i),
			 TAG_PROPERTY, I0);

    for(i = 0; i < league->skip_weeks_with->len; i++)
	xml_write_string(fil, (gchar*)g_ptr_array_index(league->skip_weeks_with, i),
			 TAG_SKIP_WEEKS_WITH, I0);

    xml_write_int(fil, league->id, TAG_ID, I0);
    xml_write_int(fil, league->layer, TAG_LEAGUE_LAYER, I0);
    xml_write_int(fil, league->first_week, TAG_LEAGUE_FIRST_WEEK, I0);
    xml_write_int(fil, league->round_robins, TAG_LEAGUE_ROUND_ROBINS, I0);
    xml_write_int(fil, league->week_gap, TAG_WEEK_GAP, I0);
    xml_write_int(fil, league->yellow_red, TAG_YELLOW_RED, I0);

    for(i = 0; i < league->rr_breaks->len; i++)
        xml_write_int(fil, g_array_index(league->rr_breaks, gint, i), TAG_LEAGUE_BREAK, I0);

    for(i = 0; i < league->week_breaks->len; i++)
    {
        xml_write_int(fil, g_array_index(league->week_breaks, WeekBreak, i).week_number, TAG_WEEK_BREAK, I0);   
        xml_write_int(fil, g_array_index(league->week_breaks, WeekBreak, i).length, TAG_WEEK_BREAK_LENGTH, I0);   
    }

    for(i=0;i<league->tables->len;i++)
    {
	sprintf(buf, "%s___league_%d_table_%02d.xml", basename, league->id, i);
	xml_write_string(fil, buf, TAG_LEAGUE_TABLE_FILE, I1);

	sprintf(buf, "%s___league_%d_table_%02d.xml", prefix, league->id, i);
	xml_loadsave_table_write(buf, &g_array_index(league->tables, Table, i));
    }

    for(i = 0; i < league->joined_leagues->len; i++)
    {
        xml_write_string(fil, g_array_index(league->joined_leagues, JoinedLeague, i).sid, TAG_LEAGUE_JOINED_LEAGUE_SID, I0);
        xml_write_int(fil, g_array_index(league->joined_leagues, JoinedLeague, i).rr, TAG_LEAGUE_JOINED_LEAGUE_RR, I0);
    }

    for(i = 0; i < league->new_tables->len; i++)
    {
        xml_write_string(fil, g_array_index(league->new_tables, NewTable, i).name, TAG_LEAGUE_NEW_TABLE_NAME, I0);
        xml_write_int(fil, g_array_index(league->new_tables, NewTable, i).add_week, TAG_LEAGUE_NEW_TABLE_ADD_WEEK, I0);
    }
    
    xml_write_float(fil, league->average_talent, TAG_LEAGUE_AVERAGE_TALENT, I0);

    for(i=0;i<league->two_match_weeks[0]->len;i++)
    {
	xml_write_int(fil, g_array_index(league->two_match_weeks[0], gint, i),
		      TAG_LEAGUE_TWO_MATCH_WEEK_START, I0);
	xml_write_int(fil, g_array_index(league->two_match_weeks[1], gint, i),
		      TAG_LEAGUE_TWO_MATCH_WEEK_END, I0);
    }

    xml_loadsave_league_prom_rel_write(fil, league);

    fprintf(fil, "%s</_%d>\n", I0, TAG_LEAGUE);

    fclose(fil);
}

/** Write the prom_rel data into the file. */
void
xml_loadsave_league_prom_rel_write(FILE *fil, const League *league)
{
    gint i;

    fprintf(fil, "%s<_%d>\n", I0, TAG_LEAGUE_PROM_REL);

    for(i = 0; i < league->prom_rel.prom_games->len; i++)
    {
        fprintf(fil, "%s<_%d>\n", I1, TAG_LEAGUE_PROM_GAMES);

        xml_write_string(fil, g_array_index(league->prom_rel.prom_games, PromGames, i).dest_sid,
                         TAG_LEAGUE_PROM_REL_PROM_GAMES_DEST_SID, I2);
        xml_write_string(fil, g_array_index(league->prom_rel.prom_games, PromGames, i).cup_sid,
                         TAG_LEAGUE_PROM_REL_PROM_GAMES_CUP_SID, I2);
        xml_write_int(fil, g_array_index(league->prom_rel.prom_games, PromGames, i).number_of_advance,
                      TAG_LEAGUE_PROM_REL_PROM_GAMES_NUMBER_OF_ADVANCE, I2);
        xml_write_string(fil, g_array_index(league->prom_rel.prom_games, PromGames, i).loser_sid,
                         TAG_LEAGUE_PROM_REL_PROM_GAMES_LOSER_SID, I2);
        
        fprintf(fil, "%s</_%d>\n", I1, TAG_LEAGUE_PROM_GAMES);
    }

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
	xml_write_int(fil, g_array_index(league->prom_rel.elements, PromRelElement, i).from_table,
		      TAG_LEAGUE_PROM_REL_ELEMENT_FROM_TABLE, I2);
	xml_write_string(fil, g_array_index(league->prom_rel.elements, PromRelElement, i).dest_sid,
			   TAG_LEAGUE_PROM_REL_ELEMENT_DEST_SID, I2);
	fprintf(fil, "%s</_%d>\n", I1, TAG_LEAGUE_PROM_REL_ELEMENT);
    }
    fprintf(fil, "%s</_%d>\n", I1, TAG_LEAGUE_PROM_REL_ELEMENTS);

    fprintf(fil, "%s</_%d>\n", I0, TAG_LEAGUE_PROM_REL);

}
