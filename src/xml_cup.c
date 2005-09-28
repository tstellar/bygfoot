#include "cup.h"
#include "file.h"
#include "main.h"
#include "misc.h"
#include "option.h"
#include "variables.h"
#include "xml_cup.h"

/**
 * The tags used in the XML files defining cups.
 */
#define TAG_CUP "cup"
#define TAG_NAME "name"
#define TAG_SHORT_NAME "short_name"
#define TAG_SYMBOL "symbol"
#define TAG_SID "sid"
#define TAG_GROUP "group"
#define TAG_PROPERTY "property"
#define TAG_LAST_WEEK "last_week"
#define TAG_ADD_WEEK "add_week"
#define TAG_WEEK_GAP "week_gap"
#define TAG_YELLOW_RED "yellow_red"
#define TAG_TALENT_DIFF "talent_diff"
#define TAG_CUP_ROUNDS "cup_rounds"
#define TAG_CUP_ROUND "cup_round"
#define TAG_CUP_ROUND_NEW_TEAMS "new_teams"
#define TAG_CUP_ROUND_BYES "byes"
#define TAG_CUP_ROUND_HOME_AWAY "home_away"
#define TAG_CUP_ROUND_REPLAY "replay"
#define TAG_CUP_ROUND_NEUTRAL "neutral"
#define TAG_CUP_ROUND_RANDOMISE_TEAMS "randomise_teams"
#define TAG_CUP_ROUND_NUMBER_OF_GROUPS "number_of_groups"
#define TAG_CUP_ROUND_NUMBER_OF_ADVANCE "number_of_advance"
#define TAG_CUP_ROUND_NUMBER_OF_BEST_ADVANCE "number_of_best_advance"
#define TAG_CHOOSE_TEAMS "choose_teams"
#define TAG_CHOOSE_TEAM "choose_team"
#define TAG_CHOOSE_TEAM_SID "choose_team_sid"
#define TAG_CHOOSE_TEAM_NUMBER_OF_TEAMS "number_of_teams"
#define TAG_CHOOSE_TEAM_START_IDX "start_idx"
#define TAG_CHOOSE_TEAM_END_IDX "end_idx"
#define TAG_CHOOSE_TEAM_RANDOMLY "randomly"
#define TAG_CHOOSE_TEAM_GENERATE "generate"

/**
 * Enum with the states used in the XML parser functions.
 */
enum XmlCupStates
{
    STATE_CUP = 0,
    STATE_NAME,
    STATE_SHORT_NAME,
    STATE_SYMBOL,
    STATE_SID,
    STATE_GROUP,
    STATE_PROPERTY,
    STATE_LAST_WEEK,
    STATE_ADD_WEEK,
    STATE_WEEK_GAP,
    STATE_YELLOW_RED,
    STATE_TALENT_DIFF,
    STATE_CUP_ROUNDS,
    STATE_CUP_ROUND,
    STATE_CUP_ROUND_NEW_TEAMS,
    STATE_CUP_ROUND_BYES,
    STATE_CUP_ROUND_HOME_AWAY,
    STATE_CUP_ROUND_REPLAY,
    STATE_CUP_ROUND_NEUTRAL,
    STATE_CUP_ROUND_RANDOMISE_TEAMS,
    STATE_CUP_ROUND_NUMBER_OF_GROUPS,
    STATE_CUP_ROUND_NUMBER_OF_ADVANCE,
    STATE_CUP_ROUND_NUMBER_OF_BEST_ADVANCE,
    STATE_CHOOSE_TEAMS,
    STATE_CHOOSE_TEAM,
    STATE_CHOOSE_TEAM_SID,
    STATE_CHOOSE_TEAM_NUMBER_OF_TEAMS,
    STATE_CHOOSE_TEAM_START_IDX,
    STATE_CHOOSE_TEAM_END_IDX,
    STATE_CHOOSE_TEAM_RANDOMLY,
    STATE_CHOOSE_TEAM_GENERATE,
    STATE_END
};

/**
 * The state variable used in the XML parsing functions.
 */
gint state;

/** The variable we will fill and append to an array. */
Cup new_cup;
CupRound new_round;
CupChooseTeam new_choose_team;

/**
 * The function called by the parser when an opening tag is read.
 * The state variable is changed in this function and
 * sometimes memory allocated for the information that's going to be read.
 * @see The GLib manual (Simple XML parser).
 */
void
xml_cup_read_start_element (GMarkupParseContext *context,
			    const gchar         *element_name,
			    const gchar        **attribute_names,
			    const gchar        **attribute_values,
			    gpointer             user_data,
			    GError             **error)
{
    if(strcmp(element_name, TAG_CUP) == 0)
    {
	new_cup = cup_new(FALSE);
	state = STATE_CUP;
    }
    else if(strcmp(element_name, TAG_NAME) == 0)
	state = STATE_NAME;
    else if(strcmp(element_name, TAG_SHORT_NAME) == 0)
	state = STATE_SHORT_NAME;
    else if(strcmp(element_name, TAG_SYMBOL) == 0)
	state = STATE_SYMBOL;
    else if(strcmp(element_name, TAG_SID) == 0)
	state = STATE_SID;
    else if(strcmp(element_name, TAG_GROUP) == 0)
	state = STATE_GROUP;
    else if(strcmp(element_name, TAG_LAST_WEEK) == 0)
	state = STATE_LAST_WEEK;
    else if(strcmp(element_name, TAG_PROPERTY) == 0)
	state = STATE_PROPERTY;
    else if(strcmp(element_name, TAG_ADD_WEEK) == 0)
	state = STATE_ADD_WEEK;
    else if(strcmp(element_name, TAG_WEEK_GAP) == 0)
	state = STATE_WEEK_GAP;
    else if(strcmp(element_name, TAG_YELLOW_RED) == 0)
	state = STATE_YELLOW_RED;
    else if(strcmp(element_name, TAG_TALENT_DIFF) == 0)
	state = STATE_TALENT_DIFF;
    else if(strcmp(element_name, TAG_CUP_ROUNDS) == 0)
	state = STATE_CUP_ROUNDS;
    else if(strcmp(element_name, TAG_CUP_ROUND) == 0)
    {
	new_round = cup_round_new();
	state = STATE_CUP_ROUND;
    }
    else if(strcmp(element_name, TAG_CUP_ROUND_NEW_TEAMS) == 0)
	state = STATE_CUP_ROUND_NEW_TEAMS;
    else if(strcmp(element_name, TAG_CUP_ROUND_BYES) == 0)
	state = STATE_CUP_ROUND_BYES;
    else if(strcmp(element_name, TAG_CUP_ROUND_HOME_AWAY) == 0)
	state = STATE_CUP_ROUND_HOME_AWAY;
    else if(strcmp(element_name, TAG_CUP_ROUND_REPLAY) == 0)
	state = STATE_CUP_ROUND_REPLAY;
    else if(strcmp(element_name, TAG_CUP_ROUND_NEUTRAL) == 0)
	state = STATE_CUP_ROUND_NEUTRAL;
    else if(strcmp(element_name, TAG_CUP_ROUND_RANDOMISE_TEAMS) == 0)
	state = STATE_CUP_ROUND_RANDOMISE_TEAMS;
    else if(strcmp(element_name, TAG_CUP_ROUND_NUMBER_OF_GROUPS) == 0)
	state = STATE_CUP_ROUND_NUMBER_OF_GROUPS;
    else if(strcmp(element_name, TAG_CUP_ROUND_NUMBER_OF_ADVANCE) == 0)
	state = STATE_CUP_ROUND_NUMBER_OF_ADVANCE;
    else if(strcmp(element_name, TAG_CUP_ROUND_NUMBER_OF_BEST_ADVANCE) == 0)
	state = STATE_CUP_ROUND_NUMBER_OF_BEST_ADVANCE;
    else if(strcmp(element_name, TAG_CHOOSE_TEAMS) == 0)
	state = STATE_CHOOSE_TEAMS;
    else if(strcmp(element_name, TAG_CHOOSE_TEAM) == 0)
    {
	new_choose_team = cup_choose_team_new();
	state = STATE_CHOOSE_TEAM;
    }
    else if(strcmp(element_name, TAG_CHOOSE_TEAM_SID) == 0)
	state = STATE_CHOOSE_TEAM_SID;
    else if(strcmp(element_name, TAG_CHOOSE_TEAM_NUMBER_OF_TEAMS) == 0)
	state = STATE_CHOOSE_TEAM_NUMBER_OF_TEAMS;
    else if(strcmp(element_name, TAG_CHOOSE_TEAM_START_IDX) == 0)
	state = STATE_CHOOSE_TEAM_START_IDX;
    else if(strcmp(element_name, TAG_CHOOSE_TEAM_END_IDX) == 0)
	state = STATE_CHOOSE_TEAM_END_IDX;
    else if(strcmp(element_name, TAG_CHOOSE_TEAM_RANDOMLY) == 0)
	state = STATE_CHOOSE_TEAM_RANDOMLY;
    else if(strcmp(element_name, TAG_CHOOSE_TEAM_GENERATE) == 0)
	state = STATE_CHOOSE_TEAM_GENERATE;
    else
	g_warning("xml_cup_read_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

/**
 * The function called by the parser when a closing tag is read.
 * The state variable is changed in this function.
 * @see The GLib manual (Simple XML parser).
 */
void
xml_cup_read_end_element    (GMarkupParseContext *context,
			     const gchar         *element_name,
			     gpointer             user_data,
			     GError             **error)
{
    if(strcmp(element_name, TAG_NAME) == 0 ||
       strcmp(element_name, TAG_SHORT_NAME) == 0 ||
       strcmp(element_name, TAG_SYMBOL) == 0 ||
       strcmp(element_name, TAG_SID) == 0 ||
       strcmp(element_name, TAG_GROUP) == 0 ||
       strcmp(element_name, TAG_LAST_WEEK) == 0 ||
       strcmp(element_name, TAG_PROPERTY) == 0 ||
       strcmp(element_name, TAG_ADD_WEEK) == 0 ||
       strcmp(element_name, TAG_WEEK_GAP) == 0 ||
       strcmp(element_name, TAG_YELLOW_RED) == 0 ||
       strcmp(element_name, TAG_TALENT_DIFF) == 0 ||
       strcmp(element_name, TAG_CUP_ROUNDS) == 0)
	state = STATE_CUP;
    else if(strcmp(element_name, TAG_CUP_ROUND) == 0)
    {
	state = STATE_CUP_ROUNDS;	
	g_array_append_val(new_cup.rounds, new_round);
    }
    else if(strcmp(element_name, TAG_CUP_ROUND_HOME_AWAY) == 0 ||
	    strcmp(element_name, TAG_CUP_ROUND_REPLAY) == 0 ||
	    strcmp(element_name, TAG_CUP_ROUND_NEUTRAL) == 0 ||
	    strcmp(element_name, TAG_CUP_ROUND_RANDOMISE_TEAMS) == 0 ||
	    strcmp(element_name, TAG_CUP_ROUND_NUMBER_OF_GROUPS) == 0 ||
	    strcmp(element_name, TAG_CUP_ROUND_NUMBER_OF_ADVANCE) == 0 ||
	    strcmp(element_name, TAG_CUP_ROUND_NUMBER_OF_BEST_ADVANCE) == 0 ||
	    strcmp(element_name, TAG_CUP_ROUND_NEW_TEAMS) == 0 ||
	    strcmp(element_name, TAG_CUP_ROUND_BYES) == 0 ||
	    strcmp(element_name, TAG_CHOOSE_TEAMS) == 0)
	state = STATE_CUP_ROUND;
    else if(strcmp(element_name, TAG_CHOOSE_TEAM) == 0)
    {
	state = STATE_CHOOSE_TEAMS;
	g_array_append_val(new_round.choose_teams, new_choose_team);
    }
    else if(strcmp(element_name, TAG_CHOOSE_TEAM_SID) == 0 ||
	    strcmp(element_name, TAG_CHOOSE_TEAM_NUMBER_OF_TEAMS) == 0 ||
	    strcmp(element_name, TAG_CHOOSE_TEAM_START_IDX) == 0 ||
	    strcmp(element_name, TAG_CHOOSE_TEAM_END_IDX) == 0 ||
	    strcmp(element_name, TAG_CHOOSE_TEAM_RANDOMLY) == 0 ||
	    strcmp(element_name, TAG_CHOOSE_TEAM_GENERATE) == 0)
	    state = STATE_CHOOSE_TEAM;
    else if(strcmp(element_name, TAG_CUP) != 0)
	g_warning("xml_cup_read_end_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

/**
 * The function called by the parser when the text between tags is read.
 * This function is responsible for filling in the variables (e.g. team names)
 * when a file gets loaded.
 * @see The GLib manual (Simple XML parser).
 */
void
xml_cup_read_text         (GMarkupParseContext *context,
			   const gchar         *text,
			   gsize                text_len,  
			   gpointer             user_data,
			   GError             **error)
{
    gchar buf[text_len + 1];
    gint int_value;
    gfloat float_value;
    GString *new_property = NULL;

    strncpy(buf, text, text_len);
    buf[text_len] = '\0';

    int_value = (gint)g_ascii_strtod(buf, NULL);
    float_value = (gfloat)g_ascii_strtod(buf, NULL);

    if(state == STATE_NAME)
	g_string_printf(new_cup.name, "%s", buf);
    else if(state == STATE_SHORT_NAME)
	g_string_printf(new_cup.short_name, "%s", buf);
    else if(state == STATE_SYMBOL)
	g_string_printf(new_cup.symbol, "%s", buf);
    else if(state == STATE_SID)
	g_string_printf(new_cup.sid, "%s", buf);
    else if(state == STATE_GROUP)
	new_cup.group = int_value;
    else if(state == STATE_LAST_WEEK)
	new_cup.last_week = int_value;
    else if(state == STATE_PROPERTY)
    {
	new_property = g_string_new(buf);
	g_ptr_array_add(new_cup.properties, new_property);
    }
    else if(state == STATE_ADD_WEEK)
	new_cup.add_week = int_value;
    else if(state == STATE_WEEK_GAP)
	new_cup.week_gap = int_value;
    else if(state == STATE_YELLOW_RED)
	new_cup.yellow_red = int_value;
    else if(state == STATE_TALENT_DIFF)
	new_cup.talent_diff = 
	    (float_value / 10000) * const_float("float_player_max_skill");
    else if(state == STATE_CUP_ROUND_NEW_TEAMS)
	new_round.new_teams = int_value;
    else if(state == STATE_CUP_ROUND_BYES)
	new_round.byes = int_value;
    else if(state == STATE_CUP_ROUND_HOME_AWAY)
	new_round.home_away = int_value;
    else if(state == STATE_CUP_ROUND_REPLAY)
	new_round.replay = int_value;
    else if(state == STATE_CUP_ROUND_NEUTRAL)
	new_round.neutral = int_value;
    else if(state == STATE_CUP_ROUND_RANDOMISE_TEAMS)
	new_round.randomise_teams = int_value;
    else if(state == STATE_CUP_ROUND_NUMBER_OF_GROUPS)
	new_round.round_robin_number_of_groups = int_value;
    else if(state == STATE_CUP_ROUND_NUMBER_OF_ADVANCE)
	new_round.round_robin_number_of_advance = int_value;
    else if(state == STATE_CUP_ROUND_NUMBER_OF_BEST_ADVANCE)
	new_round.round_robin_number_of_best_advance = int_value;
    else if(state == STATE_CHOOSE_TEAM_SID)
	g_string_printf(new_choose_team.sid, "%s", buf);
    else if(state == STATE_CHOOSE_TEAM_NUMBER_OF_TEAMS)
	new_choose_team.number_of_teams = int_value;
    else if(state == STATE_CHOOSE_TEAM_START_IDX)
	new_choose_team.start_idx = int_value;
    else if(state == STATE_CHOOSE_TEAM_END_IDX)
	new_choose_team.end_idx = int_value;
    else if(state == STATE_CHOOSE_TEAM_RANDOMLY)
	new_choose_team.randomly = int_value;
    else if(state == STATE_CHOOSE_TEAM_GENERATE)
	new_choose_team.generate = int_value;
}

/**
 * Function reading an XML file specifying a cup.
 * @param cup_name name of the xml file (e.g. 'cup_england_fa.xml')
 * to be read. Full path is not necessary, if the file is located in
 * one of the suppport directories; neither are the prefix 'cup_'
 * or the suffix '.xml'.
 * @param cups The array we append the new cup to.
 */
void
xml_cup_read(const gchar *cup_name, GArray *cups)
{
    gchar *file_name = file_find_support_file(cup_name, FALSE);
    GMarkupParser parser = {xml_cup_read_start_element,
			    xml_cup_read_end_element,
			    xml_cup_read_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    gint length;
    GError *error = NULL;
    gchar buf[SMALL];

    context = 
	g_markup_parse_context_new(&parser, 0, NULL, NULL);

    if(file_name == NULL)
    {
	sprintf(buf, "cup_%s.xml", cup_name);
	file_name = file_find_support_file(buf, TRUE);
    }

    if(!g_file_get_contents(file_name, &file_contents, &length, &error))
    {
	g_warning("xml_cup_read: error reading file %s\n", file_name);
	misc_print_error(&error, FALSE);
	return;
    }

    state = STATE_CUP;
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
	g_critical("xml_cup_read: error parsing file %s\n", buf);
	misc_print_error(&error, TRUE);
    }

    new_cup.id = cup_id_new;
    g_array_append_val(cups, new_cup);
}
