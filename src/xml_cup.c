#include "xml_cup.h"


/**
 * The tags used in the XML files defining cups.
 */
#define TAG_CUP "cup"
#define TAG_NAME "name"
#define TAG_SHORT_NAME "short_name"
#define TAG_SYMBOL "symbol"
#define TAG_ID "id"
#define TAG_TYPE "type"
#define TAG_LAST_WEEK "last_week"
#define TAG_WEEK_GAP "week_gap"
#define TAG_YELLOW_RED "yellow_red"
#define TAG_CUP_ROUNDS "cup_rounds"
#define TAG_CUP_ROUND "cup_round"
#define TAG_CUP_ROUND_HOME_AWAY "home_away"
#define TAG_CUP_ROUND_REPLAY "replay"
#define TAG_CUP_ROUND_NEUTRAL "neutral"
#define TAG_CUP_ROUND_ROUND_ROBIN "round_robin"
#define TAG_CUP_ROUND_ROUND_ROBIN_NUMBER_OF_GROUPS "number_of_groups"
#define TAG_CUP_ROUND_ROUND_ROBIN_NUMBER_OF_ADVANCE "number_of_advance"
#define TAG_CUP_ROUND_ROUND_ROBIN_NUMBER_OF_BEST_ADVANCE "number_of_best_advance"
#define TAG_CHOOSE_TEAMS "choose_teams"
#define TAG_CHOOSE_TEAM "choose_team"
#define TAG_CHOOSE_TEAM_ID "choose_team_id"
#define TAG_CHOOSE_TEAM_NUMBER_OF_TEAMS "number_of_teams"
#define TAG_CHOOSE_TEAM_START_IDX "start_idx"
#define TAG_CHOOSE_TEAM_END_IDX "end_idx"
#define TAG_CHOOSE_TEAM_RANDOMLY "randomly"
#define TAG_CHOOSE_TEAM_USER "choose_team_user"

/**
 * Enum with the states used in the XML parser functions.
 */
enum XmlCupStates
{
    STATE_CUP = 0,
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
xml_cup_read_start_element (GMarkupParseContext *context,
				const gchar         *element_name,
				const gchar        **attribute_names,
				const gchar        **attribute_values,
				gpointer             user_data,
				GError             **error)
{

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

}

/**
 * Function reading an XML file specifying a country.
 * The variable country::cup gets freed and overwritten afterwards.
 * @param cup_name name of the xml file (e.g. 'cup_england_fa.xml')
 * to be read. Full path is not necessary, if the file is located in
 * one of the suppport directories; neither are the prefix 'cup_'
 * or the suffix '.xml'.
 */
void
xml_cup_read(const gchar *cup_name)
{
    gchar *file_name = find_support_file(cup_name);
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
	file_name = find_support_file(buf);
    }

    if(!g_file_get_contents(file_name, &file_contents, &length, &error))
    {
	g_warning("xml_cup_read: error reading file %s\n", file_name);
	misc_print_error(error, FALSE);
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
	misc_print_error(error, TRUE);
    }
}
