#include "xml_league.h"

/**
 * The tags used in the XML files defining leagues.
 */
#define TAG_LEAGUE "league"
#define TAG_NAME "name"
#define TAG_SHORT_NAME "short_name"
#define TAG_ID "id"
#define TAG_SYMBOL "symbol"
#define TAG_FIRST_WEEK "first_week"
#define TAG_WEEK_GAP "week_gap"
#define TAG_YELLOW_RED "yellow_red"
#define TAG_PROM_REL "prom_rel"
#define TAG_PROM_GAMES "prom_games"
#define TAG_PROM_GAMES_DEST_ID "prom_games_dest_id"
#define TAG_PROM_GAMES_CUP "cup"
#define TAG_PROM_REL_ELEMENT "prom_rel_element"
#define TAG_PROM_REL_ELEMENT_RANK_START "rank_start"
#define TAG_PROM_REL_ELEMENT_RANK_END "rank_end"
#define TAG_PROM_REL_ELEMENT_DEST_ID "dest_id"
#define TAG_TEAMS "teams"
#define TAG_TEAM "team"
#define TAG_TEAM_NAME "team_name"

/**
 * Enum with the states used in the XML parser functions.
 */
enum XmlLeagueStates
{
    STATE_LEAGUE = 0,
    STATE_ID,
    STATE_NAME,
    STATE_SHORT_NAME,
    STATE_SYMBOL,
    STATE_FIRST_WEEK,
    STATE_WEEK_GAP,
    STATE_YELLOW_RED,
    STATE_PROM_REL,
    STATE_PROM_GAMES,
    STATE_PROM_GAMES_DEST_ID,
    STATE_PROM_GAMES_CUP,
    STATE_PROM_REL_ELEMENT,
    STATE_PROM_REL_ELEMENT_RANK_START,
    STATE_PROM_REL_ELEMENT_RANK_END,
    STATE_PROM_REL_ELEMENT_DEST_ID,
    STATE_TEAMS,
    STATE_TEAM,
    STATE_TEAM_NAME,
    STATE_END
};

/**
 * The state variable used in the XML parsing functions.
 */
gint state;

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
    League new;
    PromRelElement new_element;
    Team new_team;
    
    if(strcmp(element_name, TAG_LEAGUE) == 0)
    {
	new = league_new();
	g_array_append_val(ligs, new);
	state = STATE_LEAGUE;
    }
    else if(strcmp(element_name, TAG_NAME) == 0)
	state = STATE_NAME;
    else if(strcmp(element_name, TAG_SHORT_NAME) == 0)
	state = STATE_SHORT_NAME;
    else if(strcmp(element_name, TAG_ID) == 0)
	state = STATE_ID;
    else if(strcmp(element_name, TAG_SYMBOL) == 0)
	state = STATE_SYMBOL;
    else if(strcmp(element_name, TAG_FIRST_WEEK) == 0)
	state = STATE_FIRST_WEEK;
    else if(strcmp(element_name, TAG_WEEK_GAP) == 0)
	state = STATE_WEEK_GAP;
    else if(strcmp(element_name, TAG_YELLOW_RED) == 0)
	state = STATE_YELLOW_RED;
    else if(strcmp(element_name, TAG_PROM_REL) == 0)
	state = STATE_PROM_REL;
    else if(strcmp(element_name, TAG_PROM_GAMES) == 0)
	state = STATE_PROM_GAMES;
    else if(strcmp(element_name, TAG_PROM_GAMES_DEST_ID) == 0)
	state = STATE_PROM_GAMES_DEST_ID;
    else if(strcmp(element_name, TAG_PROM_GAMES_CUP) == 0)
	state = STATE_PROM_GAMES_CUP;
    else if(strcmp(element_name, TAG_PROM_REL_ELEMENT) == 0)
    {
	new_element = prom_rel_element_new();
	g_array_append_val(lig(ligs->len - 1).prom_rel.elements, new_element);
	state = STATE_PROM_REL_ELEMENT;	
    }
    else if(strcmp(element_name, TAG_PROM_REL_ELEMENT_RANK_START) == 0)
	state = STATE_PROM_REL_ELEMENT_RANK_START;
    else if(strcmp(element_name, TAG_PROM_REL_ELEMENT_RANK_END) == 0)
	state = STATE_PROM_REL_ELEMENT_RANK_END;
    else if(strcmp(element_name, TAG_PROM_REL_ELEMENT_DEST_ID) == 0)
	state = STATE_PROM_REL_ELEMENT_DEST_ID;
    else if(strcmp(element_name, TAG_TEAMS) == 0)
	state = STATE_TEAMS;
    else if(strcmp(element_name, TAG_TEAM) == 0)
    {
	new_team = team_new();
	g_array_append_val(lig(ligs->len - 1).teams, new_team);
	state = STATE_TEAM;
    }
    else if(strcmp(element_name, TAG_TEAM_NAME) == 0)
	state = STATE_TEAM_NAME;

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
       strcmp(element_name, TAG_ID) == 0 ||
       strcmp(element_name, TAG_SYMBOL) == 0 ||
       strcmp(element_name, TAG_FIRST_WEEK) == 0 ||
       strcmp(element_name, TAG_WEEK_GAP) == 0 ||
       strcmp(element_name, TAG_YELLOW_RED) == 0 ||
       strcmp(element_name, TAG_PROM_REL) == 0 ||
       strcmp(element_name, TAG_TEAMS) == 0)
	state = STATE_LEAGUE;
    else if(strcmp(element_name, TAG_PROM_GAMES) == 0 ||
	    strcmp(element_name, TAG_PROM_REL_ELEMENT) == 0)
	state = STATE_PROM_REL;
    else if(strcmp(element_name, TAG_PROM_GAMES_DEST_ID) == 0 ||
	    strcmp(element_name, TAG_PROM_GAMES_CUP) == 0)
	state = STATE_PROM_GAMES;
    else if(strcmp(element_name, TAG_PROM_REL_ELEMENT_RANK_START) == 0 ||
	    strcmp(element_name, TAG_PROM_REL_ELEMENT_RANK_END) == 0 ||
	    strcmp(element_name, TAG_PROM_REL_ELEMENT_DEST_ID) == 0)
	state = STATE_PROM_REL_ELEMENT;
    else if(strcmp(element_name, TAG_TEAM) == 0)
	state = STATE_TEAMS;
    else if(strcmp(element_name, TAG_TEAM_NAME) == 0)
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
    gint value;

    strncpy(buf, text, text_len);
    buf[text_len] = '\0';

    value = (gint)g_ascii_strtod(buf, NULL);

    if(state == STATE_NAME)
	g_string_printf(lig(ligs->len - 1).name, "%s", buf);
    else if(state == STATE_SHORT_NAME)
	g_string_printf(lig(ligs->len - 1).short_name, "%s", buf);
    else if(state == STATE_ID)
	g_string_printf(lig(ligs->len - 1).id, "%s", buf);
    else if(state == STATE_SYMBOL)
	g_string_printf(lig(ligs->len - 1).symbol, "%s", buf);
    else if(state == STATE_FIRST_WEEK)
	lig(ligs->len - 1).first_week = value;
    else if(state == STATE_WEEK_GAP)
	lig(ligs->len - 1).week_gap = value;
    else if(state == STATE_YELLOW_RED)
	lig(ligs->len - 1).yellow_red = value;
    else if(state == STATE_PROM_GAMES_DEST_ID)
	lig(ligs->len - 1).prom_rel.prom_games_dest_id = 
	    g_string_assign(lig(ligs->len - 1).prom_rel.prom_games_dest_id, buf);
    else if(state == STATE_PROM_GAMES_CUP)
	lig(ligs->len - 1).prom_rel.prom_games_cup_id = 
	    g_string_assign(lig(ligs->len - 1).prom_rel.prom_games_cup_id, buf);
    else if(state == STATE_PROM_REL_ELEMENT_RANK_START)
	g_array_index(lig(ligs->len - 1).prom_rel.elements,
		      PromRelElement,
		      lig(ligs->len - 1).prom_rel.elements->len - 1).ranks[0] = value;
    else if(state == STATE_PROM_REL_ELEMENT_RANK_END)
	g_array_index(lig(ligs->len - 1).prom_rel.elements,
		      PromRelElement,
		      lig(ligs->len - 1).prom_rel.elements->len - 1).ranks[1] = value;
    else if(state == STATE_TEAM_NAME)
	g_array_index(lig(ligs->len - 1).teams, Team,
		      lig(ligs->len - 1).teams->len - 1).name =
	    g_string_assign(g_array_index(lig(ligs->len - 1).teams, Team,
					  lig(ligs->len - 1).teams->len - 1).name, buf);
}

/**
 * Function reading an XML file specifying a league.
 * A new league struct is filled in and appended to the
 * _Country::leagues array.
 * @param league_name name of the xml file ( e.g. 'league_england1.xml')
 * to be read. Full path is not necessary, if the file is located in
 * one of the suppport directories; neither are the prefix 'league_'
 * or the suffix '.xml'.
 */
void
xml_league_read(const gchar *league_name)
{
    gchar *file_name = find_support_file(league_name);
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
	file_name = find_support_file(buf);
    }

    if(!g_file_get_contents(file_name, &file_contents, &length, &error))
    {
	g_warning("xml_league_read: error reading file %s\n", file_name);
	misc_print_error(error, FALSE);
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
    }
    else
    {
	g_critical("xml_league_read: error parsing file %s\n", buf);
	misc_print_error(error, TRUE);
    }
}
