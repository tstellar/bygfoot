/*
   xml_league.c

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

#include "cup_struct.h"
#include "file.h"
#include "free.h"
#include "league.h"
#include "misc.h"
#include "option.h"
#include "team.h"
#include "table.h"
#include "variables.h"
#include "xml_league.h"
#include "xml_cup.h"
#include "xml.h"

/**
 * The tags used in the XML files defining leagues.
 */
#define TAG_LEAGUE "league"
#define TAG_LAYER "layer"
#define TAG_FIRST_WEEK "first_week"
#define TAG_ROUND_ROBINS "round_robins"
#define TAG_AVERAGE_TALENT "average_talent"
#define TAG_NAMES_FILE "names_file"
#define TAG_BREAK "break"
#define TAG_JOINED_LEAGUE "joined_league"
#define TAG_NEW_TABLE "new_table"
#define TAG_PROM_REL "prom_rel"
#define TAG_PROM_GAMES "prom_games"
#define TAG_PROM_GAMES_DEST_SID "prom_games_dest_sid"
#define TAG_PROM_GAMES_LOSER_SID "prom_games_loser_sid"
#define TAG_PROM_GAMES_NUMBER_OF_ADVANCE "prom_games_number_of_advance"
#define TAG_PROM_GAMES_CUP_SID "prom_games_cup_sid"
#define TAG_PROM_REL_ELEMENT "prom_rel_element"
#define TAG_PROM_REL_ELEMENT_RANK_START "rank_start"
#define TAG_PROM_REL_ELEMENT_RANK_END "rank_end"
#define TAG_PROM_REL_ELEMENT_DEST_SID "dest_sid"
#define TAG_PROM_REL_ELEMENT_TYPE "prom_rel_type"
#define TAG_PROM_REL_ELEMENT_FROM_TABLE "from_table"
#define TAG_TEAMS "teams"
#define TAG_TEAM "team"
#define TAG_TEAM_NAME "team_name"
#define TAG_TEAM_SYMBOL "team_symbol"
#define TAG_TEAM_NAMES_FILE "team_names_file"
#define TAG_TEAM_AVERAGE_TALENT "team_average_talent"
#define TAG_TEAM_DEF_FILE "def_file"
#define TAG_TWO_MATCH_WEEK_START "two_match_week_start"
#define TAG_TWO_MATCH_WEEK_END "two_match_week_end"

#define ATT_NAME_JOINED_LEAGUE_RR "rr"
#define ATT_NAME_NEW_TABLE_NAME "name"

/**
 * Enum with the states used in the XML parser functions.
 */
enum XmlLeagueStates
{
    STATE_LEAGUE = 0,
    STATE_SID,
    STATE_NAME,
    STATE_SHORT_NAME,
    STATE_SYMBOL,
    STATE_LAYER,
    STATE_FIRST_WEEK,
    STATE_WEEK_GAP,
    STATE_WEEK_BREAK,
    STATE_SKIP_WEEKS_WITH,
    STATE_ROUND_ROBINS,
    STATE_YELLOW_RED,
    STATE_AVERAGE_TALENT,
    STATE_NAMES_FILE,
    STATE_PROM_REL,
    STATE_PROM_GAMES,
    STATE_PROM_GAMES_DEST_SID,
    STATE_PROM_GAMES_LOSER_SID,
    STATE_PROM_GAMES_NUMBER_OF_ADVANCE,
    STATE_PROM_GAMES_CUP_SID,
    STATE_PROM_REL_ELEMENT,
    STATE_PROM_REL_ELEMENT_RANK_START,
    STATE_PROM_REL_ELEMENT_RANK_END,
    STATE_PROM_REL_ELEMENT_DEST_SID,
    STATE_PROM_REL_ELEMENT_TYPE,
    STATE_PROM_REL_ELEMENT_FROM_TABLE,
    STATE_TEAMS,
    STATE_TEAM,
    STATE_TEAM_NAME,
    STATE_TEAM_SYMBOL,
    STATE_TEAM_NAMES_FILE,
    STATE_TEAM_AVERAGE_TALENT,
    STATE_TEAM_DEF_FILE,
    STATE_BREAK,
    STATE_JOINED_LEAGUE,
    STATE_PROPERTY,
    STATE_NEW_TABLE,
    STATE_TWO_MATCH_WEEK_START,
    STATE_TWO_MATCH_WEEK_END,
    STATE_END
};

/**
 * The state variable used in the XML parsing functions.
 */
gint state;

/** The new league we create and append to an array. */
League new_league;

/**
 * The function called by the parser when an opening tag is read.
 * The state variable is changed in this function and
 * sometimes memory allocated for the information that's going to be read.
 * @see The GLib manual (Simple XML parser).
 */
void
xml_league_read_start_element (GMarkupParseContext *context,
				const gchar         *element_name,
				const gchar        **attribute_names,
				const gchar        **attribute_values,
				gpointer             user_data,
				GError             **error)
{
#ifdef DEBUG
    printf("xml_league_read_start_element\n");
#endif

    PromRelElement new_element;
    PromGames new_prom_games;
    Team new_team;
    JoinedLeague new_joined_league;
    NewTable new_table;
    WeekBreak new_week_break;

    if(strcmp(element_name, TAG_LEAGUE) == 0)
    {
	new_league = league_new(TRUE);
	state = STATE_LEAGUE;
    }
    else if(strcmp(element_name, TAG_DEF_NAME) == 0)
	state = STATE_NAME;
    else if(strcmp(element_name, TAG_DEF_SHORT_NAME) == 0)
	state = STATE_SHORT_NAME;
    else if(strcmp(element_name, TAG_DEF_SID) == 0)
	state = STATE_SID;
    else if(strcmp(element_name, TAG_DEF_SYMBOL) == 0)
	state = STATE_SYMBOL;
    else if(strcmp(element_name, TAG_FIRST_WEEK) == 0)
	state = STATE_FIRST_WEEK;
    else if(strcmp(element_name, TAG_LAYER) == 0)
	state = STATE_LAYER;
    else if(strcmp(element_name, TAG_DEF_WEEK_GAP) == 0)
	state = STATE_WEEK_GAP;
    else if(strcmp(element_name, TAG_DEF_WEEK_BREAK) == 0)
    {
     	state = STATE_WEEK_BREAK;
        if(attribute_names[0] != NULL && strcmp(attribute_names[0], ATT_DEF_NAME_WEEK_BREAK_LENGTH) == 0)
            new_week_break.length = (gint)g_ascii_strtod(attribute_values[0], NULL);
        else
            new_week_break.length = -1000;

        g_array_append_val(new_league.week_breaks, new_week_break);
    }
    else if(strcmp(element_name, TAG_DEF_SKIP_WEEKS_WITH) == 0)
     	state = STATE_SKIP_WEEKS_WITH;
    else if(strcmp(element_name, TAG_ROUND_ROBINS) == 0)
	state = STATE_ROUND_ROBINS;
    else if(strcmp(element_name, TAG_DEF_YELLOW_RED) == 0)
	state = STATE_YELLOW_RED;
    else if(strcmp(element_name, TAG_AVERAGE_TALENT) == 0)
	state = STATE_AVERAGE_TALENT;
    else if(strcmp(element_name, TAG_NAMES_FILE) == 0)
	state = STATE_NAMES_FILE;
    else if(strcmp(element_name, TAG_BREAK) == 0)
	state = STATE_BREAK;
    else if(strcmp(element_name, TAG_DEF_PROPERTY) == 0)
	state = STATE_PROPERTY;
    else if(strcmp(element_name, TAG_JOINED_LEAGUE) == 0)
    {
	state = STATE_JOINED_LEAGUE;
        new_joined_league.sid = NULL;
        if(attribute_names[0] != NULL && strcmp(attribute_names[0], ATT_NAME_JOINED_LEAGUE_RR) == 0)
            new_joined_league.rr = (gint)g_ascii_strtod(attribute_values[0], NULL);
        else
            new_joined_league.rr = 2;

        g_array_append_val(new_league.joined_leagues, new_joined_league);
    }
    else if(strcmp(element_name, TAG_NEW_TABLE) == 0)
    {
     	state = STATE_NEW_TABLE;
        if(attribute_names[0] != NULL && strcmp(attribute_names[0], ATT_NAME_NEW_TABLE_NAME) == 0)
            new_table.name = g_strdup(attribute_values[0]);
        else
            new_table.name = g_strdup(new_league.name);

        g_array_append_val(new_league.new_tables, new_table);        
    }
    else if(strcmp(element_name, TAG_TWO_MATCH_WEEK_START) == 0)
	state = STATE_TWO_MATCH_WEEK_START;
    else if(strcmp(element_name, TAG_TWO_MATCH_WEEK_END) == 0)
	state = STATE_TWO_MATCH_WEEK_END;
    else if(strcmp(element_name, TAG_PROM_REL) == 0)
	state = STATE_PROM_REL;
    else if(strcmp(element_name, TAG_PROM_GAMES) == 0)
    {
        state = STATE_PROM_GAMES;
        new_prom_games = prom_games_new();
        g_array_append_val(new_league.prom_rel.prom_games, new_prom_games);
    }
    else if(strcmp(element_name, TAG_PROM_GAMES_DEST_SID) == 0)
	state = STATE_PROM_GAMES_DEST_SID;
    else if(strcmp(element_name, TAG_PROM_GAMES_LOSER_SID) == 0)
	state = STATE_PROM_GAMES_LOSER_SID;
    else if(strcmp(element_name, TAG_PROM_GAMES_NUMBER_OF_ADVANCE) == 0)
	state = STATE_PROM_GAMES_NUMBER_OF_ADVANCE;
    else if(strcmp(element_name, TAG_PROM_GAMES_CUP_SID) == 0)
	state = STATE_PROM_GAMES_CUP_SID;
    else if(strcmp(element_name, TAG_PROM_REL_ELEMENT) == 0)
    {
	new_element = prom_rel_element_new();
	g_array_append_val(new_league.prom_rel.elements, new_element);
	state = STATE_PROM_REL_ELEMENT;	
    }
    else if(strcmp(element_name, TAG_PROM_REL_ELEMENT_RANK_START) == 0)
	state = STATE_PROM_REL_ELEMENT_RANK_START;
    else if(strcmp(element_name, TAG_PROM_REL_ELEMENT_RANK_END) == 0)
	state = STATE_PROM_REL_ELEMENT_RANK_END;
    else if(strcmp(element_name, TAG_PROM_REL_ELEMENT_DEST_SID) == 0)
	state = STATE_PROM_REL_ELEMENT_DEST_SID;
    else if(strcmp(element_name, TAG_PROM_REL_ELEMENT_TYPE) == 0)
	state = STATE_PROM_REL_ELEMENT_TYPE;
    else if(strcmp(element_name, TAG_PROM_REL_ELEMENT_FROM_TABLE) == 0)
	state = STATE_PROM_REL_ELEMENT_FROM_TABLE;
    else if(strcmp(element_name, TAG_TEAMS) == 0)
	state = STATE_TEAMS;
    else if(strcmp(element_name, TAG_TEAM) == 0)
    {
	new_team = team_new(TRUE);
	misc_string_assign(&(new_team.symbol), new_league.symbol);
	misc_string_assign(&(new_team.names_file), new_league.names_file);
	new_team.clid = new_league.id;
	g_array_append_val(new_league.teams, new_team);	
	state = STATE_TEAM;
    }
    else if(strcmp(element_name, TAG_TEAM_NAME) == 0)
	state = STATE_TEAM_NAME;
    else if(strcmp(element_name, TAG_TEAM_SYMBOL) == 0)
	state = STATE_TEAM_SYMBOL;
    else if(strcmp(element_name, TAG_TEAM_NAMES_FILE) == 0)
	state = STATE_TEAM_NAMES_FILE;
    else if(strcmp(element_name, TAG_TEAM_AVERAGE_TALENT) == 0)
	state = STATE_TEAM_AVERAGE_TALENT;
    else if(strcmp(element_name, TAG_TEAM_DEF_FILE) == 0)
	state = STATE_TEAM_DEF_FILE;
    else
	debug_print_message("xml_league_read_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
	
}

/**
 * The function called by the parser when a closing tag is read.
 * The state variable is changed in this function.
 * @see The GLib manual (Simple XML parser).
 */
void
xml_league_read_end_element    (GMarkupParseContext *context,
				const gchar         *element_name,
				gpointer             user_data,
				GError             **error)
{
#ifdef DEBUG
    printf("xml_league_read_end_element\n");
#endif

    if(strcmp(element_name, TAG_DEF_NAME) == 0 ||
       strcmp(element_name, TAG_DEF_SHORT_NAME) == 0 ||
       strcmp(element_name, TAG_DEF_SID) == 0 ||
       strcmp(element_name, TAG_DEF_SYMBOL) == 0 ||
       strcmp(element_name, TAG_LAYER) == 0 ||
       strcmp(element_name, TAG_FIRST_WEEK) == 0 ||
       strcmp(element_name, TAG_DEF_WEEK_GAP) == 0 ||
       strcmp(element_name, TAG_DEF_WEEK_BREAK) == 0 ||
       strcmp(element_name, TAG_DEF_SKIP_WEEKS_WITH) == 0 ||
       strcmp(element_name, TAG_ROUND_ROBINS) == 0 ||
       strcmp(element_name, TAG_DEF_YELLOW_RED) == 0 ||
       strcmp(element_name, TAG_AVERAGE_TALENT) == 0 ||
       strcmp(element_name, TAG_NAMES_FILE) == 0 ||
       strcmp(element_name, TAG_BREAK) == 0 ||
       strcmp(element_name, TAG_JOINED_LEAGUE) == 0 ||
       strcmp(element_name, TAG_DEF_PROPERTY) == 0 ||
       strcmp(element_name, TAG_NEW_TABLE) == 0 ||
       strcmp(element_name, TAG_TWO_MATCH_WEEK_START) == 0 ||
       strcmp(element_name, TAG_TWO_MATCH_WEEK_END) == 0 ||
       strcmp(element_name, TAG_PROM_REL) == 0 ||
       strcmp(element_name, TAG_TEAMS) == 0)
	state = STATE_LEAGUE;
    else if(strcmp(element_name, TAG_PROM_GAMES) == 0 ||
	    strcmp(element_name, TAG_PROM_REL_ELEMENT) == 0)
	state = STATE_PROM_REL;
    else if(strcmp(element_name, TAG_PROM_GAMES_DEST_SID) == 0 ||
	    strcmp(element_name, TAG_PROM_GAMES_LOSER_SID) == 0 ||
	    strcmp(element_name, TAG_PROM_GAMES_NUMBER_OF_ADVANCE) == 0 ||
	    strcmp(element_name, TAG_PROM_GAMES_CUP_SID) == 0)
	state = STATE_PROM_GAMES;
    else if(strcmp(element_name, TAG_PROM_REL_ELEMENT_RANK_START) == 0 ||
	    strcmp(element_name, TAG_PROM_REL_ELEMENT_RANK_END) == 0 ||
	    strcmp(element_name, TAG_PROM_REL_ELEMENT_DEST_SID) == 0 ||
	    strcmp(element_name, TAG_PROM_REL_ELEMENT_FROM_TABLE) == 0 ||
	    strcmp(element_name, TAG_PROM_REL_ELEMENT_TYPE) == 0)
	state = STATE_PROM_REL_ELEMENT;
    else if(strcmp(element_name, TAG_TEAM) == 0)
	state = STATE_TEAMS;
    else if(strcmp(element_name, TAG_TEAM_NAME) == 0 ||
	    strcmp(element_name, TAG_TEAM_DEF_FILE) == 0 ||
	    strcmp(element_name, TAG_TEAM_AVERAGE_TALENT) == 0 ||
	    strcmp(element_name, TAG_TEAM_SYMBOL) == 0 ||
	    strcmp(element_name, TAG_TEAM_NAMES_FILE) == 0)
	state = STATE_TEAM;
    else if(strcmp(element_name, TAG_LEAGUE) != 0)
	debug_print_message("xml_league_end_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

/**
 * The function called by the parser when the text between tags is read.
 * This function is responsible for filling in the variables (e.g. team names)
 * when a file gets loaded.
 * @see The GLib manual (Simple XML parser).
 */
void
xml_league_read_text         (GMarkupParseContext *context,
			       const gchar         *text,
			       gsize                text_len,  
			       gpointer             user_data,
			       GError             **error)
{
#ifdef DEBUG
    printf("xml_league_read_text\n");
#endif

    gchar buf[text_len + 1];
    gint int_value;
    gfloat float_value;

    strncpy(buf, text, text_len);
    buf[text_len] = '\0';

    int_value = (gint)g_ascii_strtod(buf, NULL);
    float_value = (gfloat)g_ascii_strtod(buf, NULL);

    if(state == STATE_NAME)
	misc_string_assign(&new_league.name, buf);
    else if(state == STATE_SHORT_NAME)
	misc_string_assign(&new_league.short_name, buf);
    else if(state == STATE_SID)
	misc_string_assign(&new_league.sid, buf);
    else if(state == STATE_SYMBOL)
	misc_string_assign(&new_league.symbol, buf);
    else if(state == STATE_LAYER)
	new_league.layer = int_value;
    else if(state == STATE_FIRST_WEEK)
	new_league.first_week = int_value;
    else if(state == STATE_WEEK_GAP)
	new_league.week_gap = int_value;
    else if(state == STATE_WEEK_BREAK)
	g_array_index(new_league.week_breaks, WeekBreak, 
                      new_league.week_breaks->len - 1).week_number = int_value;
    else if(state == STATE_SKIP_WEEKS_WITH)
        g_ptr_array_add(new_league.skip_weeks_with, g_strdup(buf));
    else if(state == STATE_ROUND_ROBINS)
	new_league.round_robins = int_value;
    else if(state == STATE_YELLOW_RED)
	new_league.yellow_red = int_value;
    else if(state == STATE_AVERAGE_TALENT)
	new_league.average_talent = 
	    (float_value / 10000) * const_float("float_player_max_skill");
    else if(state == STATE_NAMES_FILE)
	misc_string_assign(&new_league.names_file, buf);
    else if(state == STATE_BREAK)
        league_cup_fill_rr_breaks(new_league.rr_breaks, buf);
    else if(state == STATE_PROPERTY)
	g_ptr_array_add(new_league.properties, g_strdup(buf));
    else if(state == STATE_JOINED_LEAGUE)
	misc_string_assign(
            &g_array_index(new_league.joined_leagues,
                           JoinedLeague,
                           new_league.joined_leagues->len - 1).sid, buf);
    else if(state == STATE_NEW_TABLE)
        g_array_index(new_league.new_tables, NewTable, new_league.new_tables->len - 1).add_week = int_value;
    else if(state == STATE_TWO_MATCH_WEEK_START)
	g_array_append_val(new_league.two_match_weeks[0], int_value);
    else if(state == STATE_TWO_MATCH_WEEK_END)
	g_array_append_val(new_league.two_match_weeks[1], int_value);
    else if(state == STATE_PROM_GAMES_DEST_SID)
	g_array_index(new_league.prom_rel.prom_games, PromGames,
                      new_league.prom_rel.prom_games->len - 1).dest_sid = g_strdup(buf);
    else if(state == STATE_PROM_GAMES_LOSER_SID)
	g_array_index(new_league.prom_rel.prom_games, PromGames,
                      new_league.prom_rel.prom_games->len - 1).loser_sid = g_strdup(buf);
    else if(state == STATE_PROM_GAMES_NUMBER_OF_ADVANCE)
	g_array_index(new_league.prom_rel.prom_games, PromGames,
                      new_league.prom_rel.prom_games->len - 1).number_of_advance = int_value;
    else if(state == STATE_PROM_GAMES_CUP_SID)
	g_array_index(new_league.prom_rel.prom_games, PromGames,
                      new_league.prom_rel.prom_games->len - 1).cup_sid = g_strdup(buf);
    else if(state == STATE_PROM_REL_ELEMENT_RANK_START)
	g_array_index(new_league.prom_rel.elements,
		      PromRelElement,
		      new_league.prom_rel.elements->len - 1).ranks[0] = int_value;
    else if(state == STATE_PROM_REL_ELEMENT_RANK_END)
	g_array_index(new_league.prom_rel.elements,
		      PromRelElement,
		      new_league.prom_rel.elements->len - 1).ranks[1] = int_value;
    else if(state == STATE_PROM_REL_ELEMENT_DEST_SID)
	misc_string_assign(&g_array_index(new_league.prom_rel.elements,
				      PromRelElement,
				      new_league.prom_rel.elements->len - 1).dest_sid, buf);
    else if(state == STATE_PROM_REL_ELEMENT_FROM_TABLE)
	g_array_index(new_league.prom_rel.elements,
		      PromRelElement,
		      new_league.prom_rel.elements->len - 1).from_table = int_value;
    else if(state == STATE_PROM_REL_ELEMENT_TYPE)
    {
	if(strcmp(buf, "promotion") == 0)
	    g_array_index(new_league.prom_rel.elements,
			  PromRelElement,
			  new_league.prom_rel.elements->len - 1).type = PROM_REL_PROMOTION;
	else if(strcmp(buf, "relegation") == 0)
	    g_array_index(new_league.prom_rel.elements,
			  PromRelElement,
			  new_league.prom_rel.elements->len - 1).type = PROM_REL_RELEGATION;
    }	
    else if(state == STATE_TEAM_NAME)
	misc_string_assign(&g_array_index(new_league.teams, Team,
				      new_league.teams->len - 1).name, buf);
    else if(state == STATE_TEAM_SYMBOL)
	misc_string_assign(&g_array_index(new_league.teams, Team,
				      new_league.teams->len - 1).symbol, buf);
    else if(state == STATE_TEAM_NAMES_FILE)
	misc_string_assign(&g_array_index(new_league.teams, Team,
				      new_league.teams->len - 1).names_file, buf);
    else if(state == STATE_TEAM_AVERAGE_TALENT)
	g_array_index(new_league.teams, Team,
		      new_league.teams->len - 1).average_talent = 
	    (float_value / 10000) * const_float("float_player_max_skill");
    else if(state == STATE_TEAM_DEF_FILE)
	misc_string_assign(&g_array_index(new_league.teams, Team, new_league.teams->len - 1).def_file, buf);
}

/**
 * Function reading an XML file specifying a league.
 * A new league struct is filled in and appended to the
 * _Country::leagues array.
 * @param league_name name of the xml file ( e.g. 'league_england1.xml')
 * to be read. Full path is not necessary, if the file is located in
 * one of the suppport directories; neither are the prefix 'league_'
 * or the suffix '.xml'.
 * @param leagues The array we write the league into.
 */
void
xml_league_read(const gchar *league_name, GArray *leagues)
{
#ifdef DEBUG
    printf("xml_league_read\n");
#endif

    gchar *file_name = file_find_support_file(league_name, FALSE);
    GMarkupParser parser = {xml_league_read_start_element,
			    xml_league_read_end_element,
			    xml_league_read_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    gsize length;
    GError *error = NULL;
    gchar buf[SMALL];

    context = 
	g_markup_parse_context_new(&parser, 0, NULL, NULL);

    if(file_name == NULL)
    {
	sprintf(buf, "league_%s.xml", league_name);
	file_name = file_find_support_file(buf, TRUE);
    }

    if(!g_file_get_contents(file_name, &file_contents, &length, &error))
    {
	debug_print_message("xml_league_read: error reading file %s\n", file_name);
	misc_print_error(&error, FALSE);
	return;
    }

    state = STATE_LEAGUE;
    strcpy(buf, file_name);
    g_free(file_name);

    if(g_markup_parse_context_parse(context, file_contents, length, &error))
    {
	g_markup_parse_context_end_parse(context, NULL);	
	g_markup_parse_context_free(context);
	g_free(file_contents);

        league_cup_adjust_rr_breaks(new_league.rr_breaks, new_league.round_robins, new_league.week_gap);
        league_cup_adjust_week_breaks(new_league.week_breaks, new_league.week_gap);
	g_array_append_val(leagues, new_league);
    }
    else
    {
	g_critical("xml_league_read: error parsing file %s\n", buf);
	misc_print_error(&error, TRUE);
    }
}
