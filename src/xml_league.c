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

/**
 * The tags used in the XML files defining leagues.
 */
#define TAG_LEAGUE "league"
#define TAG_NAME "name"
#define TAG_SHORT_NAME "short_name"
#define TAG_SID "sid"
#define TAG_SYMBOL "symbol"
#define TAG_LAYER "layer"
#define TAG_FIRST_WEEK "first_week"
#define TAG_WEEK_GAP "week_gap"
#define TAG_ROUND_ROBINS "round_robins"
#define TAG_YELLOW_RED "yellow_red"
#define TAG_AVERAGE_TALENT "average_talent"
#define TAG_NAMES_FILE "names_file"
#define TAG_ACTIVE "active"
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
#define TAG_TEAMS "teams"
#define TAG_TEAM "team"
#define TAG_TEAM_NAME "team_name"
#define TAG_TEAM_SYMBOL "team_symbol"
#define TAG_TEAM_NAMES_FILE "team_names_file"
#define TAG_TEAM_AVERAGE_TALENT "team_average_talent"
#define TAG_TEAM_DEF_FILE "def_file"

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
    STATE_ROUND_ROBINS,
    STATE_YELLOW_RED,
    STATE_AVERAGE_TALENT,
    STATE_NAMES_FILE,
    STATE_ACTIVE,
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
    STATE_TEAMS,
    STATE_TEAM,
    STATE_TEAM_NAME,
    STATE_TEAM_SYMBOL,
    STATE_TEAM_NAMES_FILE,
    STATE_TEAM_AVERAGE_TALENT,
    STATE_TEAM_DEF_FILE,
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
    PromRelElement new_element;
    Team new_team;
    
    if(strcmp(element_name, TAG_LEAGUE) == 0)
    {
	new_league = league_new(TRUE);
	state = STATE_LEAGUE;
    }
    else if(strcmp(element_name, TAG_NAME) == 0)
	state = STATE_NAME;
    else if(strcmp(element_name, TAG_SHORT_NAME) == 0)
	state = STATE_SHORT_NAME;
    else if(strcmp(element_name, TAG_SID) == 0)
	state = STATE_SID;
    else if(strcmp(element_name, TAG_SYMBOL) == 0)
	state = STATE_SYMBOL;
    else if(strcmp(element_name, TAG_FIRST_WEEK) == 0)
	state = STATE_FIRST_WEEK;
    else if(strcmp(element_name, TAG_LAYER) == 0)
	state = STATE_LAYER;
    else if(strcmp(element_name, TAG_WEEK_GAP) == 0)
	state = STATE_WEEK_GAP;
    else if(strcmp(element_name, TAG_ROUND_ROBINS) == 0)
	state = STATE_ROUND_ROBINS;
    else if(strcmp(element_name, TAG_YELLOW_RED) == 0)
	state = STATE_YELLOW_RED;
    else if(strcmp(element_name, TAG_AVERAGE_TALENT) == 0)
	state = STATE_AVERAGE_TALENT;
    else if(strcmp(element_name, TAG_NAMES_FILE) == 0)
	state = STATE_NAMES_FILE;
    else if(strcmp(element_name, TAG_ACTIVE) == 0)
	state = STATE_ACTIVE;
    else if(strcmp(element_name, TAG_PROM_REL) == 0)
	state = STATE_PROM_REL;
    else if(strcmp(element_name, TAG_PROM_GAMES) == 0)
	state = STATE_PROM_GAMES;
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
    else if(strcmp(element_name, TAG_TEAMS) == 0)
	state = STATE_TEAMS;
    else if(strcmp(element_name, TAG_TEAM) == 0)
    {
	new_team = team_new(TRUE);
	misc_string_assign(&new_team.symbol, new_league.symbol);
	misc_string_assign(&new_team.names_file, new_league.names_file);
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
	g_warning("xml_league_read_start_element: unknown tag: %s; I'm in state %d\n",
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
    if(strcmp(element_name, TAG_NAME) == 0 ||
       strcmp(element_name, TAG_SHORT_NAME) == 0 ||
       strcmp(element_name, TAG_SID) == 0 ||
       strcmp(element_name, TAG_SYMBOL) == 0 ||
       strcmp(element_name, TAG_LAYER) == 0 ||
       strcmp(element_name, TAG_FIRST_WEEK) == 0 ||
       strcmp(element_name, TAG_WEEK_GAP) == 0 ||
       strcmp(element_name, TAG_ROUND_ROBINS) == 0 ||
       strcmp(element_name, TAG_YELLOW_RED) == 0 ||
       strcmp(element_name, TAG_AVERAGE_TALENT) == 0 ||
       strcmp(element_name, TAG_NAMES_FILE) == 0 ||
       strcmp(element_name, TAG_ACTIVE) == 0 ||
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
	g_warning("xml_league_end_start_element: unknown tag: %s; I'm in state %d\n",
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
    gchar buf[text_len + 1];
    gint int_value;
    gfloat float_value;

    strncpy(buf, text, text_len);
    buf[text_len] = '\0';

    int_value = (gint)g_ascii_strtod(buf, NULL);
    float_value = (gfloat)g_ascii_strtod(buf, NULL);

    if(state == STATE_NAME)
    {
	misc_string_assign(&new_league.name, buf);
	misc_string_assign(&new_league.table.name, buf);
    }
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
    else if(state == STATE_ROUND_ROBINS)
	new_league.round_robins = int_value;
    else if(state == STATE_YELLOW_RED)
	new_league.yellow_red = int_value;
    else if(state == STATE_AVERAGE_TALENT)
	new_league.average_talent = 
	    (float_value / 10000) * const_float("float_player_max_skill");
    else if(state == STATE_NAMES_FILE)
	misc_string_assign(&new_league.names_file, buf);
    else if(state == STATE_ACTIVE)
	new_league.active = int_value;
    else if(state == STATE_PROM_GAMES_DEST_SID)
	misc_string_assign(&new_league.prom_rel.prom_games_dest_sid, buf);
    else if(state == STATE_PROM_GAMES_LOSER_SID)
	misc_string_assign(&new_league.prom_rel.prom_games_loser_sid, buf);
    else if(state == STATE_PROM_GAMES_NUMBER_OF_ADVANCE)
	new_league.prom_rel.prom_games_number_of_advance = int_value;
    else if(state == STATE_PROM_GAMES_CUP_SID)
	misc_string_assign(&new_league.prom_rel.prom_games_cup_sid, buf);
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
    gint i;
    TableElement new_table_element;
    gchar *file_name = file_find_support_file(league_name, FALSE);
    GMarkupParser parser = {xml_league_read_start_element,
			    xml_league_read_end_element,
			    xml_league_read_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    gint length;
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
	g_warning("xml_league_read: error reading file %s\n", file_name);
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

	g_array_append_val(leagues, new_league);
	for(i=0;i<g_array_index(leagues, League, leagues->len - 1).teams->len;i++)
	{
	    new_table_element = 
		table_element_new(
		    &g_array_index(
			g_array_index(
			    leagues, League, leagues->len - 1).teams, Team, i), i);
	    g_array_append_val(g_array_index(leagues, League, leagues->len - 1).
			       table.elements, new_table_element);
	}
    }
    else
    {
	g_critical("xml_league_read: error parsing file %s\n", buf);
	misc_print_error(&error, TRUE);
    }
}
