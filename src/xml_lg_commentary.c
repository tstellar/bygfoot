#include "free.h"
#include "lg_commentary.h"
#include "live_game.h"
#include "misc.h"
#include "variables.h"
#include "xml_lg_commentary.h"

#define TAG_LG_COMMENTARY "lg_commentary"
#define TAG_EVENT "event"
#define TAG_EVENT_NAME "name"
#define TAG_EVENT_COMMENTARY "commentary"

#define ATT_NAME_CONDITION "cond"
#define ATT_NAME_PRIORITY "pri"

#define EVENT_NAME_GENERAL "general"
#define EVENT_NAME_START_MATCH "start_match"
#define EVENT_NAME_HALF_TIME "half_time"
#define EVENT_NAME_EXTRA_TIME "extra_time"
#define EVENT_NAME_END_MATCH "end_match"
#define EVENT_NAME_LOST_POSSESSION "lost_possession"
#define EVENT_NAME_SCORING_CHANCE "scoring_chance"
#define EVENT_NAME_HEADER "header"
#define EVENT_NAME_PENALTY "penalty"
#define EVENT_NAME_FREE_KICK "free_kick"
#define EVENT_NAME_GOAL "goal"
#define EVENT_NAME_OWN_GOAL "own_goal"
#define EVENT_NAME_POST "post"
#define EVENT_NAME_MISS "miss"
#define EVENT_NAME_SAVE "save"
#define EVENT_NAME_CROSS_BAR "cross_bar"
#define EVENT_NAME_FOUL "foul"
#define EVENT_NAME_FOUL_YELLOW "foul_yellow"
#define EVENT_NAME_FOUL_RED "foul_red"
#define EVENT_NAME_FOUL_RED_INJURY "foul_red_injury"
#define EVENT_NAME_SEND_OFF "send_off"
#define EVENT_NAME_INJURY "injury"
#define EVENT_NAME_TEMP_INJURY "temp_injury"
#define EVENT_NAME_PENALTIES "penalties"
#define EVENT_NAME_STADIUM_BREAKDOWN "stadium_breakdown"
#define EVENT_NAME_STADIUM_RIOTS "stadium_riots"
#define EVENT_NAME_STADIUM_FIRE "stadium_fire"
#define EVENT_NAME_SUBSTITUTION "substitution"
#define EVENT_NAME_STRUCTURE_CHANGE "structure_change"
#define EVENT_NAME_STYLE_CHANGE "style_change"
#define EVENT_NAME_BOOST_CHANGE "boost_change"


enum XmlLgCommentaryStates
{
    STATE_LG_COMMENTARY = 0,
    STATE_EVENT,
    STATE_EVENT_NAME,
    STATE_EVENT_COMMENTARY,
    STATE_END
};

gint state, commentary_idx, priority;
gchar *condition;

/**
 * The function called by the parser when an opening tag is read.
 * The state variable is changed in this function and
 * sometimes memory allocated for the information that's going to be read.
 * @see The GLib manual (Simple XML parser).
 */
void
xml_lg_commentary_read_start_element (GMarkupParseContext *context,
				      const gchar         *element_name,
				      const gchar        **attribute_names,
				      const gchar        **attribute_values,
				      gpointer             user_data,
				      GError             **error)
{
    gint atidx = 0;

    if(strcmp(element_name, TAG_EVENT) == 0)
	state = STATE_EVENT;
    else if(strcmp(element_name, TAG_EVENT_NAME) == 0)
	state = STATE_EVENT_NAME;
    else if(strcmp(element_name, TAG_EVENT_COMMENTARY) == 0)
    {
	state = STATE_EVENT_COMMENTARY;

	condition = NULL;
	priority = 1;
	
	while(attribute_names[atidx] != NULL)
	{
	    if(strcmp(attribute_names[atidx], ATT_NAME_CONDITION) == 0)
		condition = g_strdup(attribute_values[atidx]);
	    else if(strcmp(attribute_names[atidx], ATT_NAME_PRIORITY) == 0)
		priority = (gint)g_ascii_strtod(attribute_values[atidx], NULL);

	    atidx++;
	}

    }
    else if(strcmp(element_name, TAG_LG_COMMENTARY) != 0)
	g_warning("xml_lg_commentary_read_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

/**
 * The function called by the parser when a closing tag is read.
 * The state variable is changed in this function.
 * @see The GLib manual (Simple XML parser).
 */
void
xml_lg_commentary_read_end_element    (GMarkupParseContext *context,
				       const gchar         *element_name,
				       gpointer             user_data,
				       GError             **error)
{
    if(strcmp(element_name, TAG_EVENT) == 0)
	state = STATE_LG_COMMENTARY;
    else if(strcmp(element_name, TAG_EVENT_NAME) == 0 ||
	    strcmp(element_name, TAG_EVENT_COMMENTARY) == 0)
	state = STATE_EVENT;
    else if(strcmp(element_name, TAG_LG_COMMENTARY) != 0)
	g_warning("xml_lg_commentary_read_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

/**
 * The function called by the parser when the text between tags is read.
 * This function is responsible for filling in the variables (e.g. team names)
 * when a file gets loaded.
 * @see The GLib manual (Simple XML parser).
 */
void
xml_lg_commentary_read_text         (GMarkupParseContext *context,
				     const gchar         *text,
				     gsize                text_len,  
				     gpointer             user_data,
				     GError             **error)
{
    gchar buf[text_len + 1];
    LGCommentary commentary;

    commentary.text = NULL;
    commentary.condition = NULL;

    strncpy(buf, text, text_len);
    buf[text_len] = '\0';

    if(state == STATE_EVENT_NAME)
    {
	if(strcmp(buf, EVENT_NAME_GENERAL) == 0)
	    commentary_idx = LIVE_GAME_EVENT_GENERAL;
	else if(strcmp(buf, EVENT_NAME_START_MATCH) == 0)
	    commentary_idx = LIVE_GAME_EVENT_START_MATCH;
	else if(strcmp(buf, EVENT_NAME_HALF_TIME) == 0)
	    commentary_idx = LIVE_GAME_EVENT_HALF_TIME;
	else if(strcmp(buf, EVENT_NAME_EXTRA_TIME) == 0)
	    commentary_idx = LIVE_GAME_EVENT_EXTRA_TIME;
	else if(strcmp(buf, EVENT_NAME_END_MATCH) == 0)
	    commentary_idx = LIVE_GAME_EVENT_END_MATCH;
	else if(strcmp(buf, EVENT_NAME_LOST_POSSESSION) == 0)
	    commentary_idx = LIVE_GAME_EVENT_LOST_POSSESSION;
	else if(strcmp(buf, EVENT_NAME_SCORING_CHANCE) == 0)
	    commentary_idx = LIVE_GAME_EVENT_SCORING_CHANCE;
	else if(strcmp(buf, EVENT_NAME_HEADER) == 0)
	    commentary_idx = LIVE_GAME_EVENT_HEADER;
	else if(strcmp(buf, EVENT_NAME_PENALTY) == 0)
	    commentary_idx = LIVE_GAME_EVENT_PENALTY;
	else if(strcmp(buf, EVENT_NAME_FREE_KICK) == 0)
	    commentary_idx = LIVE_GAME_EVENT_FREE_KICK;
	else if(strcmp(buf, EVENT_NAME_GOAL) == 0)
	    commentary_idx = LIVE_GAME_EVENT_GOAL;
	else if(strcmp(buf, EVENT_NAME_OWN_GOAL) == 0)
	    commentary_idx = LIVE_GAME_EVENT_OWN_GOAL;
	else if(strcmp(buf, EVENT_NAME_POST) == 0)
	    commentary_idx = LIVE_GAME_EVENT_POST;
	else if(strcmp(buf, EVENT_NAME_MISS) == 0)
	    commentary_idx = LIVE_GAME_EVENT_MISS;
	else if(strcmp(buf, EVENT_NAME_SAVE) == 0)
	    commentary_idx = LIVE_GAME_EVENT_SAVE;
	else if(strcmp(buf, EVENT_NAME_CROSS_BAR) == 0)
	    commentary_idx = LIVE_GAME_EVENT_CROSS_BAR;
	else if(strcmp(buf, EVENT_NAME_FOUL) == 0)
	    commentary_idx = LIVE_GAME_EVENT_FOUL;
	else if(strcmp(buf, EVENT_NAME_FOUL_YELLOW) == 0)
	    commentary_idx = LIVE_GAME_EVENT_FOUL_YELLOW;
	else if(strcmp(buf, EVENT_NAME_FOUL_RED) == 0)
	    commentary_idx = LIVE_GAME_EVENT_FOUL_RED;
	else if(strcmp(buf, EVENT_NAME_FOUL_RED_INJURY) == 0)
	    commentary_idx = LIVE_GAME_EVENT_FOUL_RED_INJURY;
	else if(strcmp(buf, EVENT_NAME_SEND_OFF) == 0)
	    commentary_idx = LIVE_GAME_EVENT_SEND_OFF;
	else if(strcmp(buf, EVENT_NAME_INJURY) == 0)
	    commentary_idx = LIVE_GAME_EVENT_INJURY;
	else if(strcmp(buf, EVENT_NAME_TEMP_INJURY) == 0)
	    commentary_idx = LIVE_GAME_EVENT_TEMP_INJURY;
	else if(strcmp(buf, EVENT_NAME_PENALTIES) == 0)
	    commentary_idx = LIVE_GAME_EVENT_PENALTIES;
	else if(strcmp(buf, EVENT_NAME_STADIUM_BREAKDOWN) == 0)
	    commentary_idx = LIVE_GAME_EVENT_STADIUM_BREAKDOWN;
	else if(strcmp(buf, EVENT_NAME_STADIUM_RIOTS) == 0)
	    commentary_idx = LIVE_GAME_EVENT_STADIUM_RIOTS;
	else if(strcmp(buf, EVENT_NAME_STADIUM_FIRE) == 0)
	    commentary_idx = LIVE_GAME_EVENT_STADIUM_FIRE;
	else if(strcmp(buf, EVENT_NAME_SUBSTITUTION) == 0)
	    commentary_idx = LIVE_GAME_EVENT_SUBSTITUTION;
	else if(strcmp(buf, EVENT_NAME_STRUCTURE_CHANGE) == 0)
	    commentary_idx = LIVE_GAME_EVENT_STRUCTURE_CHANGE;
	else if(strcmp(buf, EVENT_NAME_STYLE_CHANGE) == 0)
	    commentary_idx = LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_DEFEND;
	else if(strcmp(buf, EVENT_NAME_BOOST_CHANGE) == 0)
	    commentary_idx = LIVE_GAME_EVENT_BOOST_CHANGE_ANTI;
	else
	    g_warning("xml_lg_commentary_read_text: unknown event name %s \n", buf);
    }
    else if(state == STATE_EVENT_COMMENTARY)
    {
	misc_string_assign(&commentary.text, buf);
	commentary.condition = condition;
	commentary.priority = MAX(1, priority);
	commentary.id = lg_commentary_id_new;
	g_array_append_val(lg_commentary[commentary_idx], commentary);
    }
}

/**
 * Function reading an XML file specifying a country.
 * The variable #country gets freed and overwritten afterwards.
 * @param lg_commentary_name name of the xml file (e.g. 'lg_commentary_england.xml')
 * to be read. Full path is not necessary, if the file is located in
 * one of the suppport directories; neither are the prefix 'lg_commentary_'
 * or the suffix '.xml'.
 */
void
xml_lg_commentary_read(const gchar *commentary_file)
{
    GMarkupParser parser = {xml_lg_commentary_read_start_element,
			    xml_lg_commentary_read_end_element,
			    xml_lg_commentary_read_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    gint length;
    GError *error = NULL;

    context = 
	g_markup_parse_context_new(&parser, 0, NULL, NULL);

    if(!g_file_get_contents(commentary_file, &file_contents, &length, &error))
    {
	g_warning("xml_lg_commentary_read: error reading file %s\n", commentary_file);
	if(g_str_has_suffix(commentary_file, "lg_commentary_en.xml"))
	    misc_print_error(&error, TRUE);
	else
	{
	    misc_print_error(&error, FALSE);
	    lg_commentary_load_commentary_file("lg_commentary_en.xml", TRUE);
	}
	return;
    }

    free_lg_commentary(TRUE);

    if(g_markup_parse_context_parse(context, file_contents, length, &error))
    {
	g_markup_parse_context_end_parse(context, NULL);	
	g_markup_parse_context_free(context);
	g_free(file_contents);
    }
    else
    {
	g_warning("xml_lg_commentary_read: error parsing file %s\n", commentary_file);
	if(g_str_has_suffix(commentary_file, "lg_commentary_en.xml"))
	    misc_print_error(&error, TRUE);
	else
	{
	    misc_print_error(&error, FALSE);
	    lg_commentary_load_commentary_file("lg_commentary_en.xml", TRUE);
	}
	return;
    }
}
