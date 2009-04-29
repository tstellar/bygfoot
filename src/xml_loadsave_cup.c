/*
   xml_loadsave_cup.c

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
#include "misc.h"
#include "table.h"
#include "team.h"
#include "xml.h"
#include "xml_loadsave_cup.h"
#include "xml_loadsave_fixtures.h"
#include "xml_loadsave_table.h"
#include "xml_loadsave_teams.h"

enum
{
    TAG_CUP = TAG_START_CUP,
    TAG_CUP_LAST_WEEK,
    TAG_CUP_ADD_WEEK,
    TAG_CUP_GROUP,
    TAG_CUP_TALENT_DIFF,
    TAG_CUP_CHOOSE_TEAM,
    TAG_CUP_CHOOSE_TEAM_SID,
    TAG_CUP_CHOOSE_TEAM_NUMBER_OF_TEAMS,
    TAG_CUP_CHOOSE_TEAM_START_IDX,
    TAG_CUP_CHOOSE_TEAM_END_IDX,
    TAG_CUP_CHOOSE_TEAM_RANDOMLY,
    TAG_CUP_CHOOSE_TEAM_GENERATE,
    TAG_CUP_CHOOSE_TEAM_SKIP_GROUP_CHECK,
    TAG_CUP_CHOOSE_TEAM_FROM_TABLE,
    TAG_CUP_CHOOSE_TEAM_PRELOAD,
    TAG_CUP_ROUND,
    TAG_CUP_ROUND_NAME,
    TAG_CUP_ROUND_NEW_TEAMS,
    TAG_CUP_ROUND_BYES,
    TAG_CUP_ROUND_TEAMS_FILE,
    TAG_CUP_ROUND_TEAM_PTR_ID,
    TAG_CUP_ROUND_TABLE_FILE,
    TAG_CUP_ROUND_HOME_AWAY,
    TAG_CUP_ROUND_REPLAY,
    TAG_CUP_ROUND_NEUTRAL,
    TAG_CUP_ROUND_RANDOMISE_TEAMS,
    TAG_CUP_ROUND_ROUND_ROBIN_NUMBER_OF_GROUPS,
    TAG_CUP_ROUND_ROUND_ROBIN_NUMBER_OF_ADVANCE,
    TAG_CUP_ROUND_ROUND_ROBIN_NUMBER_OF_BEST_ADVANCE,
    TAG_CUP_ROUND_ROUND_ROBINS,
    TAG_CUP_ROUND_BREAK,
    TAG_CUP_ROUND_WAIT_CUP,
    TAG_CUP_ROUND_WAIT_ROUND,
    TAG_CUP_ROUND_TWO_MATCH_WEEK_START,
    TAG_CUP_ROUND_TWO_MATCH_WEEK_END,
    TAG_CUP_ROUND_TWO_MATCH_WEEK,
    TAG_CUP_TEAM_ID_BYE,
    TAG_CUP_TEAM_NAME,
    TAG_CUP_NEXT_FIXTURE_UPDATE_WEEK,
    TAG_CUP_NEXT_FIXTURE_UPDATE_WEEK_ROUND,
    TAG_CUP_ROUND_DELAY,
    TAG_END
};

gint state;
Cup *new_cup;
CupChooseTeam new_choose_team;
CupRound new_round;
gchar *dirname;
WeekBreak new_week_break;
CupRoundWait new_wait;

void
xml_loadsave_cup_start_element (GMarkupParseContext *context,
				const gchar         *element_name,
				const gchar        **attribute_names,
				const gchar        **attribute_values,
				gpointer             user_data,
				GError             **error)
{
#ifdef DEBUG
    printf("xml_loadsave_cup_start_element\n");
#endif

    gint i;
    gint tag = xml_get_tag_from_name(element_name);
    gboolean valid_tag = FALSE;

    for(i=TAG_CUP;i<TAG_END;i++)
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

    if(tag == TAG_CUP_CHOOSE_TEAM)
	new_choose_team = cup_choose_team_new();
    else if(tag == TAG_CUP_ROUND)
	new_round = cup_round_new();

     if(!valid_tag)
	debug_print_message("xml_loadsave_cup_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_cup_end_element    (GMarkupParseContext *context,
				 const gchar         *element_name,
				 gpointer             user_data,
				 GError             **error)
{
#ifdef DEBUG
    printf("xml_loadsave_cup_end_element\n");
#endif

    gint tag = xml_get_tag_from_name(element_name);

    if(tag == TAG_NAME ||
       tag == TAG_SHORT_NAME ||
       tag == TAG_SYMBOL ||
       tag == TAG_SID ||
       tag == TAG_ID ||
       tag == TAG_YELLOW_RED ||
       tag == TAG_WEEK_GAP ||
       tag == TAG_WEEK_BREAK ||
       tag == TAG_WEEK_BREAK_LENGTH ||
/*        tag == TAG_SKIP_WEEKS_WITH || */
       tag == TAG_PROPERTY ||
       tag == TAG_CUP_LAST_WEEK ||
       tag == TAG_CUP_ADD_WEEK ||
       tag == TAG_CUP_GROUP ||
       tag == TAG_CUP_TALENT_DIFF ||
       tag == TAG_CUP_NEXT_FIXTURE_UPDATE_WEEK ||
       tag == TAG_CUP_NEXT_FIXTURE_UPDATE_WEEK_ROUND ||
       tag == TAG_CUP_ROUND ||
       tag == TAG_CUP_TEAM_NAME ||
       tag == TAG_CUP_TEAM_ID_BYE)
    {
	state = TAG_CUP;
	if(tag == TAG_CUP_ROUND)
	    g_array_append_val(new_cup->rounds, new_round);
    }
    else if(tag == TAG_CUP_CHOOSE_TEAM)
    {
	state = TAG_CUP_ROUND;
	g_array_append_val(new_round.choose_teams, new_choose_team);
    }
    else if(tag == TAG_CUP_CHOOSE_TEAM_NUMBER_OF_TEAMS ||
	    tag == TAG_CUP_CHOOSE_TEAM_START_IDX ||
	    tag == TAG_CUP_CHOOSE_TEAM_END_IDX ||
	    tag == TAG_CUP_CHOOSE_TEAM_SID ||
	    tag == TAG_CUP_CHOOSE_TEAM_GENERATE ||
	    tag == TAG_CUP_CHOOSE_TEAM_SKIP_GROUP_CHECK ||
	    tag == TAG_CUP_CHOOSE_TEAM_FROM_TABLE ||
	    tag == TAG_CUP_CHOOSE_TEAM_PRELOAD ||
	    tag == TAG_CUP_CHOOSE_TEAM_RANDOMLY)
	state = TAG_CUP_CHOOSE_TEAM;
    else if(tag == TAG_CUP_ROUND_HOME_AWAY ||
	    tag == TAG_CUP_ROUND_TEAMS_FILE ||
	    tag == TAG_CUP_ROUND_TEAM_PTR_ID ||
	    tag == TAG_CUP_ROUND_TABLE_FILE ||
	    tag == TAG_CUP_ROUND_NEW_TEAMS ||
	    tag == TAG_CUP_ROUND_NAME ||
	    tag == TAG_CUP_ROUND_BYES ||
	    tag == TAG_CUP_ROUND_REPLAY ||
	    tag == TAG_CUP_ROUND_NEUTRAL ||
	    tag == TAG_CUP_ROUND_DELAY ||
	    tag == TAG_CUP_ROUND_RANDOMISE_TEAMS ||
	    tag == TAG_CUP_ROUND_ROUND_ROBIN_NUMBER_OF_GROUPS ||
	    tag == TAG_CUP_ROUND_ROUND_ROBIN_NUMBER_OF_ADVANCE ||
	    tag == TAG_CUP_ROUND_ROUND_ROBIN_NUMBER_OF_BEST_ADVANCE ||
	    tag == TAG_CUP_ROUND_ROUND_ROBINS ||
	    tag == TAG_CUP_ROUND_BREAK ||
	    tag == TAG_CUP_ROUND_WAIT_CUP ||
	    tag == TAG_CUP_ROUND_WAIT_ROUND ||
	    tag == TAG_CUP_ROUND_TWO_MATCH_WEEK_START ||
	    tag == TAG_CUP_ROUND_TWO_MATCH_WEEK_END ||
	    tag == TAG_CUP_ROUND_TWO_MATCH_WEEK)
	state = TAG_CUP_ROUND;
    else if(tag != TAG_CUP)
	debug_print_message("xml_loadsave_cup_end_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_cup_text         (GMarkupParseContext *context,
			       const gchar         *text,
			       gsize                text_len,  
			       gpointer             user_data,
			       GError             **error)
{
#ifdef DEBUG
    printf("xml_loadsave_cup_text\n");
#endif

    gint i;
    gchar buf[SMALL], buf2[SMALL];
    gint int_value = -1;
    gfloat float_value;
    Table new_table;

    strncpy(buf, text, text_len);
    buf[text_len] = '\0';

    int_value = (gint)g_ascii_strtod(buf, NULL);
    float_value = (gfloat)g_ascii_strtod(buf, NULL) / 10000;

    if(state == TAG_NAME)
	misc_string_assign(&new_cup->name, buf);
    else if(state == TAG_SHORT_NAME)
	misc_string_assign(&new_cup->short_name, buf);
    else if(state == TAG_SYMBOL)
	misc_string_assign(&new_cup->symbol, buf);
    else if(state == TAG_SID)
	misc_string_assign(&new_cup->sid, buf);
    else if(state == TAG_ID)
	new_cup->id = int_value;
    else if(state == TAG_WEEK_GAP)
	new_cup->week_gap = int_value;
    else if(state == TAG_WEEK_BREAK)
        new_week_break.week_number = int_value;
    else if(state == TAG_WEEK_BREAK_LENGTH)
    {
        new_week_break.length = int_value;
        g_array_append_val(new_cup->week_breaks, new_week_break);
    }
/*     else if(state == TAG_SKIP_WEEKS_WITH) */
/*         g_ptr_array_add(new_cup->skip_weeks_with, g_strdup(buf)); */
    else if(state == TAG_YELLOW_RED)
	new_cup->yellow_red = int_value;
    else if(state == TAG_PROPERTY)
	g_ptr_array_add(new_cup->properties, g_strdup(buf));
    else if(state == TAG_CUP_LAST_WEEK)
	new_cup->last_week = int_value;
    else if(state == TAG_CUP_ADD_WEEK)
	new_cup->add_week = int_value;
    else if(state == TAG_CUP_GROUP)
	new_cup->group = int_value;
    else if(state == TAG_CUP_TALENT_DIFF)
	new_cup->talent_diff = float_value;
    else if(state == TAG_CUP_NEXT_FIXTURE_UPDATE_WEEK)
	new_cup->next_fixture_update_week = int_value;
    else if(state == TAG_CUP_NEXT_FIXTURE_UPDATE_WEEK_ROUND)
	new_cup->next_fixture_update_week_round = int_value;
    else if(state == TAG_CUP_TEAM_ID_BYE)
	g_ptr_array_add(new_cup->bye, team_of_id(int_value));
    else if(state == TAG_CUP_TEAM_NAME)
	g_ptr_array_add(new_cup->team_names, g_strdup(buf));
    else if(state == TAG_CUP_CHOOSE_TEAM_SID)
	misc_string_assign(&new_choose_team.sid, buf);
    else if(state == TAG_CUP_CHOOSE_TEAM_NUMBER_OF_TEAMS)
	new_choose_team.number_of_teams = int_value;
    else if(state == TAG_CUP_CHOOSE_TEAM_START_IDX)
	new_choose_team.start_idx = int_value;
    else if(state == TAG_CUP_CHOOSE_TEAM_END_IDX)
	new_choose_team.end_idx = int_value;
    else if(state == TAG_CUP_CHOOSE_TEAM_RANDOMLY)
	new_choose_team.randomly = int_value;
    else if(state == TAG_CUP_CHOOSE_TEAM_GENERATE)
	new_choose_team.generate = int_value;
    else if(state == TAG_CUP_CHOOSE_TEAM_SKIP_GROUP_CHECK)
	new_choose_team.skip_group_check = int_value;
    else if(state == TAG_CUP_CHOOSE_TEAM_FROM_TABLE)
	new_choose_team.from_table = int_value;
    else if(state == TAG_CUP_CHOOSE_TEAM_PRELOAD)
	new_choose_team.preload = int_value;
    else if(state == TAG_CUP_ROUND_HOME_AWAY)
	new_round.home_away = int_value;
    else if(state == TAG_CUP_ROUND_NEW_TEAMS)
	new_round.new_teams = int_value;
    else if(state == TAG_CUP_ROUND_NAME)
	new_round.name = g_strdup(buf);
    else if(state == TAG_CUP_ROUND_BYES)
	new_round.byes = int_value;
    else if(state == TAG_CUP_ROUND_TEAMS_FILE)
    {
	sprintf(buf2, "%s%s%s", dirname, G_DIR_SEPARATOR_S, buf);
	xml_loadsave_teams_read(buf2, new_round.teams);

	for(i=0;i<new_round.teams->len;i++)
	    g_ptr_array_add(new_cup->teams, 
			    &g_array_index(new_round.teams, Team, i));
    }
    else if(state == TAG_CUP_ROUND_TEAM_PTR_ID)
	g_ptr_array_add(new_round.team_ptrs, GINT_TO_POINTER(int_value));
    else if(state == TAG_CUP_ROUND_TABLE_FILE)
    {
	new_table = table_new();

	sprintf(buf2, "%s%s%s", dirname, G_DIR_SEPARATOR_S, buf);
	xml_loadsave_table_read(buf2, &new_table);
	
	g_array_append_val(new_round.tables, new_table);
    }
    else if(state == TAG_CUP_ROUND_REPLAY)
	new_round.replay = int_value;
    else if(state == TAG_CUP_ROUND_NEUTRAL)
	new_round.neutral = int_value;
    else if(state == TAG_CUP_ROUND_DELAY)
	new_round.delay = int_value;
    else if(state == TAG_CUP_ROUND_RANDOMISE_TEAMS)
	new_round.randomise_teams = int_value;
    else if(state == TAG_CUP_ROUND_ROUND_ROBIN_NUMBER_OF_GROUPS)
	new_round.round_robin_number_of_groups = int_value;
    else if(state == TAG_CUP_ROUND_ROUND_ROBIN_NUMBER_OF_ADVANCE)
	new_round.round_robin_number_of_advance = int_value;
    else if(state == TAG_CUP_ROUND_ROUND_ROBIN_NUMBER_OF_BEST_ADVANCE)
	new_round.round_robin_number_of_best_advance = int_value;
    else if(state == TAG_CUP_ROUND_ROUND_ROBINS)
	new_round.round_robins = int_value;
    else if(state == TAG_CUP_ROUND_BREAK)
	g_array_append_val(new_round.rr_breaks, int_value);
    else if(state == TAG_CUP_ROUND_WAIT_CUP)
        new_wait.cup_sid = g_strdup(buf);
    else if(state == TAG_CUP_ROUND_WAIT_ROUND)
    {
        new_wait.cup_round = int_value;   
        g_array_append_val(new_round.waits, new_wait);
    }
    else if(state == TAG_CUP_ROUND_TWO_MATCH_WEEK_START)
	g_array_append_val(new_round.two_match_weeks[0], int_value);
    else if(state == TAG_CUP_ROUND_TWO_MATCH_WEEK_END)
	g_array_append_val(new_round.two_match_weeks[1], int_value);
    else if(state == TAG_CUP_ROUND_TWO_MATCH_WEEK)
	new_round.two_match_week = int_value;
}

void
xml_loadsave_cup_read(const gchar *filename, Cup *cup)
{
#ifdef DEBUG
    printf("xml_loadsave_cup_read\n");
#endif

    GMarkupParser parser = {xml_loadsave_cup_start_element,
			    xml_loadsave_cup_end_element,
			    xml_loadsave_cup_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents = NULL;
    gsize length;
    GError *error = NULL;

    context = 
	g_markup_parse_context_new(&parser, 0, NULL, NULL);

    if(!g_file_get_contents(filename, &file_contents, &length, &error))
    {
	debug_print_message("xml_loadsave_cup_read: error reading file %s\n", filename);
	misc_print_error(&error, TRUE);
    }

    new_cup = cup;
    dirname = g_path_get_dirname(filename);

    if(g_markup_parse_context_parse(context, file_contents, length, &error))
    {
	g_markup_parse_context_end_parse(context, NULL);
	g_markup_parse_context_free(context);
	g_free(file_contents);
    }
    else
    {
	debug_print_message("xml_loadsave_cup_read: error parsing file %s\n", filename);
	misc_print_error(&error, TRUE);
    }

    g_free(dirname);
}

void
xml_loadsave_cup_write(const gchar *prefix, const Cup *cup)
{
#ifdef DEBUG
    printf("xml_loadsave_cup_write\n");
#endif

    gint i;
    gchar buf[SMALL];
    FILE *fil = NULL;

    sprintf(buf, "%s___cup_%d_fixtures.xml", prefix, cup->id);
    xml_loadsave_fixtures_write(buf, cup->fixtures);

    sprintf(buf, "%s___cup_%d.xml", prefix, cup->id);
    file_my_fopen(buf, "w", &fil, TRUE);

    fprintf(fil, "<_%d>\n", TAG_CUP);

    xml_write_string(fil, cup->name, TAG_NAME, I0);
    xml_write_string(fil, cup->short_name, TAG_SHORT_NAME, I0);
    xml_write_string(fil, cup->sid, TAG_SID, I0);
    xml_write_string(fil, cup->symbol, TAG_SYMBOL, I0);

    xml_write_int(fil, cup->id, TAG_ID, I0);
    xml_write_int(fil, cup->last_week, TAG_CUP_LAST_WEEK, I0);
    xml_write_int(fil, cup->add_week, TAG_CUP_ADD_WEEK, I0);
    xml_write_int(fil, cup->group, TAG_CUP_GROUP, I0);
    xml_write_int(fil, cup->week_gap, TAG_WEEK_GAP, I0);
    xml_write_int(fil, cup->yellow_red, TAG_YELLOW_RED, I0);
    xml_write_float(fil, cup->talent_diff, TAG_CUP_TALENT_DIFF, I0);
    xml_write_int(fil, cup->next_fixture_update_week, 
		  TAG_CUP_NEXT_FIXTURE_UPDATE_WEEK, I0);
    xml_write_int(fil, cup->next_fixture_update_week_round, 
		  TAG_CUP_NEXT_FIXTURE_UPDATE_WEEK_ROUND, I0);

    for(i=0;i<cup->properties->len;i++)
	xml_write_string(fil, (gchar*)g_ptr_array_index(cup->properties, i),
			 TAG_PROPERTY, I0);

/*     for(i = 0; i < cup->skip_weeks_with->len; i++) */
/* 	xml_write_string(fil, (gchar*)g_ptr_array_index(cup->skip_weeks_with, i), */
/* 			 TAG_SKIP_WEEKS_WITH, I0); */
        
    for(i=0;i<cup->rounds->len;i++)
	xml_loadsave_cup_write_round(fil, prefix, cup, i);


    for(i = 0; i < cup->week_breaks->len; i++)
    {
        xml_write_int(fil, g_array_index(cup->week_breaks, WeekBreak, i).week_number, TAG_WEEK_BREAK, I0);   
        xml_write_int(fil, g_array_index(cup->week_breaks, WeekBreak, i).length, TAG_WEEK_BREAK_LENGTH, I0);   
    }
        
    if(cup->bye != NULL)
	for(i=0;i<cup->bye->len;i++)
	    xml_write_int(fil, ((Team*)g_ptr_array_index(cup->bye, i))->id,
			  TAG_CUP_TEAM_ID_BYE, I1);

    for(i=0;i<cup->team_names->len;i++)
	xml_write_string(fil, (gchar*)g_ptr_array_index(cup->team_names, i),
			   TAG_CUP_TEAM_NAME, I1);

    fprintf(fil, "</_%d>\n", TAG_CUP);

    fclose(fil);
}

void
xml_loadsave_cup_write_round(FILE *fil, const gchar *prefix, const Cup *cup, gint round)
{
#ifdef DEBUG
    printf("xml_loadsave_cup_write_round\n");
#endif

    gint i;
    gchar buf[SMALL];
    const CupRound *cup_round = &g_array_index(cup->rounds, CupRound, round);
    gchar *basename = g_path_get_basename(prefix);

    fprintf(fil, "<_%d>\n", TAG_CUP_ROUND);

    if(cup_round->teams->len > 0)
    {
	sprintf(buf, "%s___cup_%d_round_%02d_teams.xml",
		basename, cup->id, round);
	xml_write_string(fil, buf, TAG_CUP_ROUND_TEAMS_FILE, I1);

	sprintf(buf, "%s___cup_%d_round_%02d_teams.xml",
		prefix, cup->id, round);
	xml_loadsave_teams_write(buf, cup_round->teams);
    }

    xml_write_string(fil, cup_round->name,
                     TAG_CUP_ROUND_NAME, I1);
    xml_write_int(fil, cup_round->new_teams,
		  TAG_CUP_ROUND_NEW_TEAMS, I1);
    xml_write_int(fil, cup_round->byes,
		  TAG_CUP_ROUND_BYES, I1);
    xml_write_int(fil, cup_round->home_away,
		  TAG_CUP_ROUND_HOME_AWAY, I1);
    xml_write_int(fil, cup_round->replay,
		  TAG_CUP_ROUND_REPLAY, I1);
    xml_write_int(fil, cup_round->neutral,
		  TAG_CUP_ROUND_NEUTRAL, I1);
    xml_write_int(fil, cup_round->delay,
		  TAG_CUP_ROUND_DELAY, I1);
    xml_write_int(fil, cup_round->randomise_teams,
		  TAG_CUP_ROUND_RANDOMISE_TEAMS, I1);
    xml_write_int(fil, cup_round->round_robin_number_of_groups,
		  TAG_CUP_ROUND_ROUND_ROBIN_NUMBER_OF_GROUPS, I1);
    xml_write_int(fil, cup_round->round_robin_number_of_advance,
		  TAG_CUP_ROUND_ROUND_ROBIN_NUMBER_OF_ADVANCE, I1);
    xml_write_int(fil, cup_round->round_robin_number_of_best_advance,
		  TAG_CUP_ROUND_ROUND_ROBIN_NUMBER_OF_BEST_ADVANCE, I1);
    xml_write_int(fil, cup_round->round_robins,
		  TAG_CUP_ROUND_ROUND_ROBINS, I1);
    
    for(i = 0; i < cup_round->rr_breaks->len; i++)
        xml_write_int(fil, g_array_index(cup_round->rr_breaks, gint, i),
                      TAG_CUP_ROUND_BREAK, I1);

    for(i = 0; i < cup_round->waits->len; i++)
    {
        xml_write_string(fil, g_array_index(cup_round->waits, CupRoundWait, i).cup_sid,
                         TAG_CUP_ROUND_WAIT_CUP, I1);
        xml_write_int(fil, g_array_index(cup_round->waits, CupRoundWait, i).cup_round,
                      TAG_CUP_ROUND_WAIT_ROUND, I1);
    }

    xml_write_int(fil, cup_round->two_match_week,
		  TAG_CUP_ROUND_TWO_MATCH_WEEK, I1);

    for(i=0;i<cup_round->two_match_weeks[0]->len;i++)
    {
	xml_write_int(fil, g_array_index(cup_round->two_match_weeks[0], gint, i),
		      TAG_CUP_ROUND_TWO_MATCH_WEEK_START, I1);
	xml_write_int(fil, g_array_index(cup_round->two_match_weeks[0], gint, i),
		      TAG_CUP_ROUND_TWO_MATCH_WEEK_END, I1);
    }

    for(i=0;i<cup_round->tables->len;i++)
    {
	sprintf(buf, "%s___cup_%d_round_%02d_table_%02d.xml", basename, cup->id, round, i);
	xml_write_string(fil, buf, TAG_CUP_ROUND_TABLE_FILE, I1);

	sprintf(buf, "%s___cup_%d_round_%02d_table_%02d.xml", prefix, cup->id, round, i);
	xml_loadsave_table_write(buf, &g_array_index(cup_round->tables, Table, i));
    }

    for(i=0;i<cup_round->choose_teams->len;i++)
	xml_loadsave_cup_write_choose_team(
	    fil, &g_array_index(cup_round->choose_teams, CupChooseTeam, i));

    for(i=0;i<cup_round->team_ptrs->len;i++)
	xml_write_int(fil, ((Team*)g_ptr_array_index(cup_round->team_ptrs, i))->id,
		      TAG_CUP_ROUND_TEAM_PTR_ID, I1);
    
    fprintf(fil, "</_%d>\n", TAG_CUP_ROUND);

    g_free(basename);
}


void
xml_loadsave_cup_write_choose_team(FILE *fil, const CupChooseTeam *choose_team)
{
#ifdef DEBUG
    printf("xml_loadsave_cup_write_choose_team\n");
#endif

    fprintf(fil, "%s<_%d>\n", I1, TAG_CUP_CHOOSE_TEAM);
    
    xml_write_string(fil, choose_team->sid, TAG_CUP_CHOOSE_TEAM_SID, I2);
    xml_write_int(fil, choose_team->number_of_teams, 
		  TAG_CUP_CHOOSE_TEAM_NUMBER_OF_TEAMS, I2);
    xml_write_int(fil, choose_team->start_idx, 
		  TAG_CUP_CHOOSE_TEAM_START_IDX, I2);
    xml_write_int(fil, choose_team->end_idx, 
		  TAG_CUP_CHOOSE_TEAM_END_IDX, I2);
    xml_write_int(fil, choose_team->randomly, 
		  TAG_CUP_CHOOSE_TEAM_RANDOMLY, I2);
    xml_write_int(fil, choose_team->generate, 
		  TAG_CUP_CHOOSE_TEAM_GENERATE, I2);
    xml_write_int(fil, choose_team->skip_group_check, 
		  TAG_CUP_CHOOSE_TEAM_SKIP_GROUP_CHECK, I2);
    xml_write_int(fil, choose_team->from_table, 
		  TAG_CUP_CHOOSE_TEAM_FROM_TABLE, I2);
    xml_write_int(fil, choose_team->preload, 
		  TAG_CUP_CHOOSE_TEAM_PRELOAD, I2);

    fprintf(fil, "%s</_%d>\n", I1, TAG_CUP_CHOOSE_TEAM);
}
