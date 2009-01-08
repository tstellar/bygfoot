/*
   xml_cup.c

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

#include "cup.h"
#include "file.h"
#include "league.h"
#include "main.h"
#include "misc.h"
#include "option.h"
#include "variables.h"
#include "xml_cup.h"
#include "xml.h"

/**
 * The tags used in the XML files defining cups.
 */
#define TAG_CUP "cup"
#define TAG_GROUP "group"
#define TAG_LAST_WEEK "last_week"
#define TAG_ADD_WEEK "add_week"
#define TAG_TALENT_DIFF "talent_diff"
#define TAG_CUP_ROUNDS "cup_rounds"
#define TAG_CUP_ROUND "cup_round"
#define TAG_CUP_ROUND_NAME "round_name"
#define TAG_CUP_ROUND_NEW_TEAMS "new_teams"
#define TAG_CUP_ROUND_BYES "byes"
#define TAG_CUP_ROUND_HOME_AWAY "home_away"
#define TAG_CUP_ROUND_REPLAY "replay"
#define TAG_CUP_ROUND_NEUTRAL "neutral"
#define TAG_CUP_ROUND_DELAY "delay"
#define TAG_CUP_ROUND_RANDOMISE_TEAMS "randomise_teams"
#define TAG_CUP_ROUND_NUMBER_OF_GROUPS "number_of_groups"
#define TAG_CUP_ROUND_NUMBER_OF_ADVANCE "number_of_advance"
#define TAG_CUP_ROUND_NUMBER_OF_BEST_ADVANCE "number_of_best_advance"
#define TAG_CUP_ROUND_ROUND_ROBINS "round_robins"
#define TAG_CUP_ROUND_BREAK "break"
#define TAG_CUP_ROUND_WAIT "wait_for_cup"
#define TAG_CUP_ROUND_TWO_MATCH_WEEK_START "two_match_week_start"
#define TAG_CUP_ROUND_TWO_MATCH_WEEK_END "two_match_week_end"
#define TAG_CUP_ROUND_TWO_MATCH_WEEK "two_match_week"
#define TAG_CHOOSE_TEAMS "choose_teams"
#define TAG_CHOOSE_TEAM "choose_team"
#define TAG_CHOOSE_TEAM_SID "choose_team_sid"
#define TAG_CHOOSE_TEAM_NUMBER_OF_TEAMS "number_of_teams"
#define TAG_CHOOSE_TEAM_START_IDX "start_idx"
#define TAG_CHOOSE_TEAM_END_IDX "end_idx"
#define TAG_CHOOSE_TEAM_RANDOMLY "randomly"
#define TAG_CHOOSE_TEAM_GENERATE "generate"
#define TAG_CHOOSE_TEAM_SKIP_GROUP_CHECK "skip_group_check"
#define TAG_CHOOSE_TEAM_FROM_TABLE "from_table"
#define TAG_CHOOSE_TEAM_PRELOAD "preload"

#define ATT_NAME_CUP_ROUND_WAIT_ROUND "round"

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
    STATE_WEEK_BREAK,
    STATE_SKIP_WEEKS_WITH,
    STATE_YELLOW_RED,
    STATE_TALENT_DIFF,
    STATE_CUP_ROUNDS,
    STATE_CUP_ROUND,
    STATE_CUP_ROUND_NAME,
    STATE_CUP_ROUND_NEW_TEAMS,
    STATE_CUP_ROUND_BYES,
    STATE_CUP_ROUND_HOME_AWAY,
    STATE_CUP_ROUND_REPLAY,
    STATE_CUP_ROUND_NEUTRAL,
    STATE_CUP_ROUND_DELAY,
    STATE_CUP_ROUND_RANDOMISE_TEAMS,
    STATE_CUP_ROUND_NUMBER_OF_GROUPS,
    STATE_CUP_ROUND_NUMBER_OF_ADVANCE,
    STATE_CUP_ROUND_NUMBER_OF_BEST_ADVANCE,
    STATE_CUP_ROUND_ROUND_ROBINS,
    STATE_CUP_ROUND_BREAK,
    STATE_CUP_ROUND_WAIT,
    STATE_CUP_ROUND_TWO_MATCH_WEEK_START,
    STATE_CUP_ROUND_TWO_MATCH_WEEK_END,
    STATE_CUP_ROUND_TWO_MATCH_WEEK,
    STATE_CHOOSE_TEAMS,
    STATE_CHOOSE_TEAM,
    STATE_CHOOSE_TEAM_SID,
    STATE_CHOOSE_TEAM_NUMBER_OF_TEAMS,
    STATE_CHOOSE_TEAM_START_IDX,
    STATE_CHOOSE_TEAM_END_IDX,
    STATE_CHOOSE_TEAM_RANDOMLY,
    STATE_CHOOSE_TEAM_GENERATE,
    STATE_CHOOSE_TEAM_SKIP_GROUP_CHECK,
    STATE_CHOOSE_TEAM_FROM_TABLE,
    STATE_CHOOSE_TEAM_PRELOAD,
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
WeekBreak new_week_break;
CupRoundWait new_wait;

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
#ifdef DEBUG
    printf("xml_cup_read_start_element\n");
#endif

    if(strcmp(element_name, TAG_CUP) == 0)
    {
	new_cup = cup_new(FALSE);
	state = STATE_CUP;
    }
    else if(strcmp(element_name, TAG_DEF_NAME) == 0)
	state = STATE_NAME;
    else if(strcmp(element_name, TAG_DEF_SHORT_NAME) == 0)
	state = STATE_SHORT_NAME;
    else if(strcmp(element_name, TAG_DEF_SYMBOL) == 0)
	state = STATE_SYMBOL;
    else if(strcmp(element_name, TAG_DEF_SID) == 0)
	state = STATE_SID;
    else if(strcmp(element_name, TAG_GROUP) == 0)
	state = STATE_GROUP;
    else if(strcmp(element_name, TAG_LAST_WEEK) == 0)
	state = STATE_LAST_WEEK;
    else if(strcmp(element_name, TAG_DEF_PROPERTY) == 0)
	state = STATE_PROPERTY;
    else if(strcmp(element_name, TAG_ADD_WEEK) == 0)
	state = STATE_ADD_WEEK;
    else if(strcmp(element_name, TAG_DEF_WEEK_GAP) == 0)
	state = STATE_WEEK_GAP;
    else if(strcmp(element_name, TAG_DEF_WEEK_BREAK) == 0)
    {
     	state = STATE_WEEK_BREAK;
        if(attribute_names[0] != NULL && strcmp(attribute_names[0], ATT_DEF_NAME_WEEK_BREAK_LENGTH) == 0)
            new_week_break.length = (gint)g_ascii_strtod(attribute_values[0], NULL);
        else
            new_week_break.length = -1000;
    }
    else if(strcmp(element_name, TAG_DEF_SKIP_WEEKS_WITH) == 0)
        state = STATE_SKIP_WEEKS_WITH;
    else if(strcmp(element_name, TAG_DEF_YELLOW_RED) == 0)
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
    else if(strcmp(element_name, TAG_CUP_ROUND_NAME) == 0)
	state = STATE_CUP_ROUND_NAME;
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
    else if(strcmp(element_name, TAG_CUP_ROUND_DELAY) == 0)
	state = STATE_CUP_ROUND_DELAY;
    else if(strcmp(element_name, TAG_CUP_ROUND_RANDOMISE_TEAMS) == 0)
	state = STATE_CUP_ROUND_RANDOMISE_TEAMS;
    else if(strcmp(element_name, TAG_CUP_ROUND_NUMBER_OF_GROUPS) == 0)
	state = STATE_CUP_ROUND_NUMBER_OF_GROUPS;
    else if(strcmp(element_name, TAG_CUP_ROUND_NUMBER_OF_ADVANCE) == 0)
	state = STATE_CUP_ROUND_NUMBER_OF_ADVANCE;
    else if(strcmp(element_name, TAG_CUP_ROUND_NUMBER_OF_BEST_ADVANCE) == 0)
	state = STATE_CUP_ROUND_NUMBER_OF_BEST_ADVANCE;
    else if(strcmp(element_name, TAG_CUP_ROUND_ROUND_ROBINS) == 0)
	state = STATE_CUP_ROUND_ROUND_ROBINS;
    else if(strcmp(element_name, TAG_CUP_ROUND_BREAK) == 0)
	state = STATE_CUP_ROUND_BREAK;
    else if(strcmp(element_name, TAG_CUP_ROUND_WAIT) == 0)
    {
     	state = STATE_CUP_ROUND_WAIT;
        if(attribute_names[0] != NULL && strcmp(attribute_names[0], ATT_NAME_CUP_ROUND_WAIT_ROUND) == 0)
            new_wait.cup_round = (gint)g_ascii_strtod(attribute_values[0], NULL) - 1;
        else
        {
            new_wait.cup_round = -1;
            g_warning("xml_cup_read_start_element: No round number specified for cup round wait in cup %s\n", new_cup.name);            
        }
    }
    else if(strcmp(element_name, TAG_CUP_ROUND_TWO_MATCH_WEEK_START) == 0)
	state = STATE_CUP_ROUND_TWO_MATCH_WEEK_START;
    else if(strcmp(element_name, TAG_CUP_ROUND_TWO_MATCH_WEEK_END) == 0)
	state = STATE_CUP_ROUND_TWO_MATCH_WEEK_END;
    else if(strcmp(element_name, TAG_CUP_ROUND_TWO_MATCH_WEEK) == 0)
	state = STATE_CUP_ROUND_TWO_MATCH_WEEK;
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
    else if(strcmp(element_name, TAG_CHOOSE_TEAM_SKIP_GROUP_CHECK) == 0)
	state = STATE_CHOOSE_TEAM_SKIP_GROUP_CHECK;
    else if(strcmp(element_name, TAG_CHOOSE_TEAM_FROM_TABLE) == 0)
	state = STATE_CHOOSE_TEAM_FROM_TABLE;
    else if(strcmp(element_name, TAG_CHOOSE_TEAM_PRELOAD) == 0)
	state = STATE_CHOOSE_TEAM_PRELOAD;
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
#ifdef DEBUG
    printf("xml_cup_read_end_element\n");
#endif

    if(strcmp(element_name, TAG_DEF_NAME) == 0 ||
       strcmp(element_name, TAG_DEF_SHORT_NAME) == 0 ||
       strcmp(element_name, TAG_DEF_SYMBOL) == 0 ||
       strcmp(element_name, TAG_DEF_SID) == 0 ||
       strcmp(element_name, TAG_GROUP) == 0 ||
       strcmp(element_name, TAG_LAST_WEEK) == 0 ||
       strcmp(element_name, TAG_DEF_PROPERTY) == 0 ||
       strcmp(element_name, TAG_ADD_WEEK) == 0 ||
       strcmp(element_name, TAG_DEF_WEEK_GAP) == 0 ||
       strcmp(element_name, TAG_DEF_SKIP_WEEKS_WITH) == 0 ||
       strcmp(element_name, TAG_DEF_YELLOW_RED) == 0 ||
       strcmp(element_name, TAG_TALENT_DIFF) == 0 ||
       strcmp(element_name, TAG_CUP_ROUNDS) == 0)
	state = STATE_CUP;
    else if(strcmp(element_name, TAG_DEF_WEEK_BREAK) == 0)
    {
	state = STATE_CUP;
        g_array_append_val(new_cup.week_breaks, new_week_break);
    }
    else if(strcmp(element_name, TAG_CUP_ROUND) == 0)
    {
	state = STATE_CUP_ROUNDS;
        
        if(new_round.home_away == 0)
            new_round.round_robins = 1;
	
        league_cup_adjust_rr_breaks(new_round.rr_breaks, new_round.round_robins, new_cup.week_gap);
	g_array_append_val(new_cup.rounds, new_round);
    }
    else if(strcmp(element_name, TAG_CUP_ROUND_HOME_AWAY) == 0 ||
	    strcmp(element_name, TAG_CUP_ROUND_REPLAY) == 0 ||
	    strcmp(element_name, TAG_CUP_ROUND_NEUTRAL) == 0 ||
	    strcmp(element_name, TAG_CUP_ROUND_DELAY) == 0 ||
	    strcmp(element_name, TAG_CUP_ROUND_RANDOMISE_TEAMS) == 0 ||
	    strcmp(element_name, TAG_CUP_ROUND_NUMBER_OF_GROUPS) == 0 ||
	    strcmp(element_name, TAG_CUP_ROUND_NUMBER_OF_ADVANCE) == 0 ||
	    strcmp(element_name, TAG_CUP_ROUND_NUMBER_OF_BEST_ADVANCE) == 0 ||
	    strcmp(element_name, TAG_CUP_ROUND_ROUND_ROBINS) == 0 ||
	    strcmp(element_name, TAG_CUP_ROUND_BREAK) == 0 ||
	    strcmp(element_name, TAG_CUP_ROUND_WAIT) == 0 ||
	    strcmp(element_name, TAG_CUP_ROUND_TWO_MATCH_WEEK_START) == 0 ||
	    strcmp(element_name, TAG_CUP_ROUND_TWO_MATCH_WEEK_END) == 0 ||
	    strcmp(element_name, TAG_CUP_ROUND_TWO_MATCH_WEEK) == 0 ||
	    strcmp(element_name, TAG_CUP_ROUND_NEW_TEAMS) == 0 ||
	    strcmp(element_name, TAG_CUP_ROUND_NAME) == 0 ||
	    strcmp(element_name, TAG_CUP_ROUND_BYES) == 0 ||
	    strcmp(element_name, TAG_CHOOSE_TEAMS) == 0)
    {
     	state = STATE_CUP_ROUND;
        if(strcmp(element_name, TAG_CUP_ROUND_WAIT) == 0)
            g_array_append_val(new_round.waits, new_wait);
    }
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
	    strcmp(element_name, TAG_CHOOSE_TEAM_SKIP_GROUP_CHECK) == 0 ||
	    strcmp(element_name, TAG_CHOOSE_TEAM_FROM_TABLE) == 0 ||
	    strcmp(element_name, TAG_CHOOSE_TEAM_PRELOAD) == 0 ||
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
#ifdef DEBUG
    printf("xml_cup_read_text\n");
#endif

    gchar buf[text_len + 1];
    gint int_value;
    gfloat float_value;

    strncpy(buf, text, text_len);
    buf[text_len] = '\0';

    int_value = (gint)g_ascii_strtod(buf, NULL);
    float_value = (gfloat)g_ascii_strtod(buf, NULL);

    if(state == STATE_NAME)
	misc_string_assign(&new_cup.name, buf);
    else if(state == STATE_SHORT_NAME)
	misc_string_assign(&new_cup.short_name, buf);
    else if(state == STATE_SYMBOL)
	misc_string_assign(&new_cup.symbol, buf);
    else if(state == STATE_SID)
	misc_string_assign(&new_cup.sid, buf);
    else if(state == STATE_GROUP)
	new_cup.group = int_value;
    else if(state == STATE_LAST_WEEK)
	new_cup.last_week = int_value;
    else if(state == STATE_PROPERTY)
	g_ptr_array_add(new_cup.properties, g_strdup(buf));
    else if(state == STATE_ADD_WEEK)
	new_cup.add_week = int_value;
    else if(state == STATE_WEEK_GAP)
	new_cup.week_gap = int_value;
    else if(state == STATE_WEEK_BREAK)
	new_week_break.week_number = int_value;
    else if(state == STATE_SKIP_WEEKS_WITH)
        g_ptr_array_add(new_cup.skip_weeks_with, g_strdup(buf));
    else if(state == STATE_YELLOW_RED)
	new_cup.yellow_red = int_value;
    else if(state == STATE_TALENT_DIFF)
	new_cup.talent_diff = 
	    (float_value / 10000);
    else if(state == STATE_CUP_ROUND_NAME)
	new_round.name = g_strdup(buf);
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
    else if(state == STATE_CUP_ROUND_DELAY)
	new_round.delay = int_value;
    else if(state == STATE_CUP_ROUND_RANDOMISE_TEAMS)
	new_round.randomise_teams = int_value;
    else if(state == STATE_CUP_ROUND_NUMBER_OF_GROUPS)
	new_round.round_robin_number_of_groups = int_value;
    else if(state == STATE_CUP_ROUND_NUMBER_OF_ADVANCE)
	new_round.round_robin_number_of_advance = int_value;
    else if(state == STATE_CUP_ROUND_NUMBER_OF_BEST_ADVANCE)
	new_round.round_robin_number_of_best_advance = int_value;
    else if(state == STATE_CUP_ROUND_ROUND_ROBINS)
	new_round.round_robins = int_value;
    else if(state == STATE_CUP_ROUND_BREAK)
	league_cup_fill_rr_breaks(new_round.rr_breaks, buf);
    else if(state == STATE_CUP_ROUND_WAIT)
        new_wait.cup_sid = g_strdup(buf);
    else if(state == STATE_CUP_ROUND_TWO_MATCH_WEEK_START)
	g_array_append_val(new_round.two_match_weeks[0], int_value);
    else if(state == STATE_CUP_ROUND_TWO_MATCH_WEEK_END)
	g_array_append_val(new_round.two_match_weeks[1], int_value);
    else if(state == STATE_CUP_ROUND_TWO_MATCH_WEEK)
	new_round.two_match_week = int_value;
    else if(state == STATE_CHOOSE_TEAM_SID)
	misc_string_assign(&new_choose_team.sid, buf);
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
    else if(state == STATE_CHOOSE_TEAM_SKIP_GROUP_CHECK)
	new_choose_team.skip_group_check = int_value;
    else if(state == STATE_CHOOSE_TEAM_FROM_TABLE)
	new_choose_team.from_table = int_value;
    else if(state == STATE_CHOOSE_TEAM_PRELOAD)
	new_choose_team.preload = int_value;
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
#ifdef DEBUG
    printf("xml_cup_read\n");
#endif

    gchar *file_name = file_find_support_file(cup_name, FALSE);
    GMarkupParser parser = {xml_cup_read_start_element,
			    xml_cup_read_end_element,
			    xml_cup_read_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    gsize length;
    GError *error = NULL;
    gchar buf[SMALL];
    gint i;

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
    league_cup_adjust_week_breaks(new_cup.week_breaks, new_cup.week_gap);

    for(i = 0; i < new_cup.rounds->len; i++)
        if(g_array_index(new_cup.rounds, CupRound, i).name == NULL)
        {
            cup_get_round_name(&new_cup, i, buf);
            g_array_index(new_cup.rounds, CupRound, i).name = g_strdup(buf);
        }

    g_array_append_val(cups, new_cup);
}
