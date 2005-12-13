/*
   xml_loadsave_users.c

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

#include "bet_struct.h"
#include "file.h"
#include "fixture.h"
#include "misc.h"
#include "team.h"
#include "user.h"
#include "variables.h"
#include "xml.h"
#include "xml_loadsave_live_game.h"
#include "xml_loadsave_players.h"
#include "xml_loadsave_users.h"

enum
{
    TAG_USERS = TAG_START_USERS,
    TAG_USER,
    TAG_USER_COUNTER,
    TAG_USER_MONEY,
    TAG_USER_DEBT,
    TAG_USER_MONEY_INS,
    TAG_USER_MONEY_IN,
    TAG_USER_MONEY_OUTS,
    TAG_USER_MONEY_OUT,
    TAG_USER_SCOUT,
    TAG_USER_PHYSIO,
    TAG_USER_HISTORY,
    TAG_USER_HISTORY_SEASON,
    TAG_USER_HISTORY_WEEK,
    TAG_USER_HISTORY_TYPE,
    TAG_USER_HISTORY_TEAM_NAME,
    TAG_USER_HISTORY_STRING,
    TAG_USER_EVENT,
    TAG_USER_EVENT_TYPE,
    TAG_USER_EVENT_VALUE1,
    TAG_USER_EVENT_VALUE2,
    TAG_USER_EVENT_VALUE_STRING,
    TAG_USER_SPONSOR_NAME,
    TAG_USER_SPONSOR_CONTRACT,
    TAG_USER_SPONSOR_BENEFIT,
    TAG_USER_YA_COACH,
    TAG_USER_YA_PERCENTAGE,
    TAG_USER_YA_AV_COACH,
    TAG_USER_YA_AV_PERCENTAGE,
    TAG_USER_YA_COUNTER,
    TAG_USER_BET0,
    TAG_USER_BET1,
    TAG_USER_BET_WAGER,
    TAG_USER_BET_OUTCOME,
    TAG_USER_BET_FIX_ID,
    TAG_END
};

gint state, idx_mon_in, idx_mon_out, idx,
    idx_bet, idx_cnt;
User new_user;
UserHistory new_history;
Event new_event;
BetUser new_bet;

void
xml_loadsave_users_start_element (GMarkupParseContext *context,
				    const gchar         *element_name,
				    const gchar        **attribute_names,
				    const gchar        **attribute_values,
				    gpointer             user_data,
				    GError             **error)
{
    gint i;
    gint tag = xml_get_tag_from_name(element_name);
    gboolean valid_tag = FALSE;

    for(i=TAG_USERS;i<TAG_END;i++)
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

    if(tag == TAG_USER)
    {
	new_user = user_new();
	idx = idx_mon_out = idx_mon_in = idx_cnt = 0;
    }
    else if(tag >= TAG_START_PLAYERS && tag <= TAG_END_PLAYERS)
    {
	state = TAG_START_PLAYERS;
	valid_tag = TRUE;

	if(tag == TAG_START_PLAYERS)
	    new_user.youth_academy.players = 
		g_array_new(FALSE, FALSE, sizeof(Player));

	xml_loadsave_players_start_element(tag, new_user.tm);
    }
    else if(tag == TAG_USER_HISTORY)
    {
	idx = 0;
	new_history.team_name =
	    new_history.string[0] =
	    new_history.string[1] =
	    new_history.string[2] = NULL;
    }
    else if(tag == TAG_USER_EVENT)
	new_event.value_string = NULL;
    else if(tag == TAG_USER_MONEY_OUTS ||
	    tag == TAG_USER_MONEY_INS)
	idx = 0;
    else if(tag == TAG_USER_BET0 ||
       tag == TAG_USER_BET1)
	idx_bet = (tag == TAG_USER_BET1);

    if(!valid_tag)
	g_warning("xml_loadsave_users_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_users_end_element    (GMarkupParseContext *context,
				   const gchar         *element_name,
				   gpointer             user_data,
				   GError             **error)
{
    gint tag = xml_get_tag_from_name(element_name);

    if(tag == TAG_USER)
    {
	state = TAG_USERS;
	g_array_append_val(users, new_user);
    }
    else if(tag == TAG_USER_COUNTER ||
	    tag == TAG_USER_MONEY ||
	    tag == TAG_USER_DEBT ||
	    tag == TAG_USER_MONEY_INS ||
	    tag == TAG_USER_MONEY_OUTS ||
	    tag == TAG_USER_SCOUT ||
	    tag == TAG_USER_PHYSIO ||
	    tag == TAG_USER_SPONSOR_NAME ||
	    tag == TAG_USER_SPONSOR_CONTRACT ||
	    tag == TAG_USER_SPONSOR_BENEFIT ||
	    tag == TAG_NAME ||
	    tag == TAG_TEAM_ID ||
	    tag == TAG_USER_HISTORY ||
	    tag == TAG_USER_EVENT ||
	    tag == TAG_USER_YA_COACH ||
	    tag == TAG_USER_YA_PERCENTAGE ||
	    tag == TAG_USER_YA_AV_COACH ||
	    tag == TAG_USER_YA_AV_PERCENTAGE ||
	    tag == TAG_USER_YA_COUNTER ||
	    tag == TAG_USER_BET0 ||
	    tag == TAG_USER_BET1)
    {
	state = TAG_USER;
	if(tag == TAG_USER_COUNTER)
	    idx_cnt++;
	else if(tag == TAG_USER_MONEY_OUTS)
	    idx_mon_out++;
	else if(tag == TAG_USER_MONEY_INS)
	    idx_mon_in++;
	else if(tag == TAG_USER_HISTORY)
	    g_array_append_val(new_user.history, new_history);
	else if(tag == TAG_USER_EVENT)
	    g_array_append_val(new_user.events, new_event);
	else if(tag == TAG_USER_BET0 ||
		tag == TAG_USER_BET1)
	    g_array_append_val(new_user.bets[idx_bet], new_bet);
    }
    else if(tag == TAG_USER_MONEY_OUT)
    {
	state = TAG_USER_MONEY_OUTS;
	idx++;
    }
    else if(tag == TAG_USER_MONEY_IN)
    {
	state = TAG_USER_MONEY_INS;
	idx++;
    }
    else if(tag == TAG_USER_HISTORY_SEASON ||
	    tag == TAG_USER_HISTORY_WEEK ||
	    tag == TAG_USER_HISTORY_TYPE ||
	    tag == TAG_USER_HISTORY_TEAM_NAME ||
	    tag == TAG_USER_HISTORY_STRING)
    {
	state = TAG_USER_HISTORY;
	
	if(tag == TAG_USER_HISTORY_STRING)
	    idx++;
    }
    else if(tag == TAG_USER_EVENT_TYPE ||
	    tag == TAG_USER_EVENT_VALUE1 ||
	    tag == TAG_USER_EVENT_VALUE2 ||
	    tag == TAG_USER_EVENT_VALUE_STRING)
	state = TAG_USER_EVENT;
    else if(tag == TAG_USER_BET_FIX_ID ||
	    tag == TAG_USER_BET_WAGER ||
	    tag == TAG_USER_BET_OUTCOME)
	state = (idx_bet == 0) ? TAG_USER_BET0 : TAG_USER_BET1;
    else if(tag >= TAG_START_PLAYERS && tag <= TAG_END_PLAYERS)
    {
	xml_loadsave_players_end_element(tag, new_user.youth_academy.players);
	if(tag == TAG_START_PLAYERS)
	    state = TAG_USER;
    }
    else if(tag != TAG_USERS)
	g_warning("xml_loadsave_users_end_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_users_text         (GMarkupParseContext *context,
				 const gchar         *text,
				 gsize                text_len,  
				 gpointer             user_data,
				 GError             **error)
{
    gchar buf[SMALL];
    gint int_value = -1;
    gfloat float_value = -1;

    strncpy(buf, text, text_len);
    buf[text_len] = '\0';

    int_value = (gint)g_ascii_strtod(buf, NULL);
    float_value = (gfloat)g_ascii_strtod(text, NULL) / 10000;

    if(state == TAG_NAME)
	misc_string_assign(&new_user.name, buf);
    else if(state == TAG_TEAM_ID)
    {
	new_user.tm = team_of_id(int_value);
	new_user.team_id = int_value;
	new_user.youth_academy.tm = new_user.tm;
    }
    else if(state == TAG_USER_MONEY)
	new_user.money = int_value;
    else if(state == TAG_USER_DEBT)
	new_user.debt = int_value;
    else if(state == TAG_USER_SCOUT)
	new_user.scout = int_value;
    else if(state == TAG_USER_PHYSIO)
	new_user.physio = int_value;
    else if(state == TAG_USER_SPONSOR_NAME)
    {
	misc_string_replace_token(buf, "AND", "&amp;");
	g_string_printf(new_user.sponsor.name, buf);
    }
    else if(state == TAG_USER_SPONSOR_CONTRACT)
	new_user.sponsor.contract = int_value;
    else if(state == TAG_USER_SPONSOR_BENEFIT)
	new_user.sponsor.benefit = int_value;
    else if(state == TAG_USER_COUNTER)
	new_user.counters[idx_cnt] = int_value;
    else if(state == TAG_USER_MONEY_IN)
	new_user.money_in[idx_mon_in][idx] = int_value;
    else if(state == TAG_USER_MONEY_OUT)
	new_user.money_out[idx_mon_out][idx] = int_value;
    else if(state == TAG_USER_HISTORY_SEASON)
	new_history.season = int_value;
    else if(state == TAG_USER_HISTORY_WEEK)
	new_history.week = int_value;
    else if(state == TAG_USER_HISTORY_TYPE)
	new_history.type = int_value;
    else if(state == TAG_USER_HISTORY_TEAM_NAME)
	new_history.team_name = g_strdup(buf);
    else if(state == TAG_USER_HISTORY_STRING)	
	new_history.string[idx] = (strlen(buf) == 0) ? NULL : g_strdup(buf);
    else if(state == TAG_USER_EVENT_TYPE)
	new_event.type = int_value;
    else if(state == TAG_USER_EVENT_VALUE1)
	new_event.value1 = int_value;
    else if(state == TAG_USER_EVENT_VALUE2)
	new_event.value2 = int_value;
    else if(state == TAG_USER_EVENT_VALUE_STRING)
	new_event.value_string = g_strdup(buf);
    else if(state == TAG_USER_YA_COACH)
	new_user.youth_academy.coach = int_value;
    else if(state == TAG_USER_YA_PERCENTAGE)
	new_user.youth_academy.percentage = int_value;
    else if(state == TAG_USER_YA_AV_COACH)
	new_user.youth_academy.av_coach = float_value;
    else if(state == TAG_USER_YA_AV_PERCENTAGE)
	new_user.youth_academy.av_percentage = float_value;
    else if(state == TAG_USER_YA_COUNTER)
	new_user.youth_academy.counter_youth = float_value;
    else if(state == TAG_USER_BET_WAGER)
	new_bet.wager = int_value;
    else if(state == TAG_USER_BET_OUTCOME)
	new_bet.outcome = int_value;
    else if(state == TAG_USER_BET_FIX_ID)
	new_bet.fix_id = int_value;
    else if(state >= TAG_START_PLAYERS && state <= TAG_END_PLAYERS)
	xml_loadsave_players_text(buf);
}

void
xml_loadsave_users_read(const gchar *dirname, const gchar *basename)
{
    GMarkupParser parser = {xml_loadsave_users_start_element,
			    xml_loadsave_users_end_element,
			    xml_loadsave_users_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    guint length;
    GError *error = NULL;
    gchar file[SMALL];

    sprintf(file, "%s%s%s___users.xml", dirname, G_DIR_SEPARATOR_S, basename);

    context = 
	g_markup_parse_context_new(&parser, 0, NULL, NULL);

    if(!g_file_get_contents(file, &file_contents, &length, &error))
    {
	g_warning("xml_loadsave_users_read: error reading file %s\n", file);
	misc_print_error(&error, TRUE);
    }

    if(g_markup_parse_context_parse(context, file_contents, length, &error))
    {
	g_markup_parse_context_end_parse(context, NULL);
	g_markup_parse_context_free(context);
	g_free(file_contents);
    }
    else
    {
	g_warning("xml_loadsave_users_read: error parsing file %s\n", file);
	misc_print_error(&error, TRUE);
    }
}

void
xml_loadsave_users_write(const gchar *prefix)
{
    gint i, j, k;
    gchar buf[SMALL];
    FILE *fil = NULL;

    for(i=0;i<users->len;i++)
    {
	sprintf(buf, "%s___user_%02d_options", prefix, i);
	file_save_opt_file(buf, &usr(i).options);

	sprintf(buf, "%s___user_%02d_live_game.xml", prefix, i);
	xml_loadsave_live_game_write(buf, &usr(i).live_game);
    }

    sprintf(buf, "%s___users.xml", prefix);
    file_my_fopen(buf, "w", &fil, TRUE);

    fprintf(fil, "<_%d>\n", TAG_USERS);

    for(i=0;i<users->len;i++)
    {
	fprintf(fil, "<_%d>\n", TAG_USER);

	xml_write_string(fil, usr(i).name, TAG_NAME, I1);
	xml_write_int(fil, usr(i).team_id, TAG_TEAM_ID, I1);
	xml_write_int(fil, usr(i).money, TAG_USER_MONEY, I1);
	xml_write_int(fil, usr(i).debt, TAG_USER_DEBT, I1);
	xml_write_int(fil, usr(i).scout, TAG_USER_SCOUT, I1);
	xml_write_int(fil, usr(i).physio, TAG_USER_PHYSIO, I1);

	strcpy(buf, usr(i).sponsor.name->str);
	misc_string_replace_token(buf, "&amp;", "AND");
	xml_write_string(fil, buf, TAG_USER_SPONSOR_NAME, I1);
	xml_write_int(fil, usr(i).sponsor.contract, TAG_USER_SPONSOR_CONTRACT, I1);
	xml_write_int(fil, usr(i).sponsor.benefit, TAG_USER_SPONSOR_BENEFIT, I1);

	xml_user_write_history(fil, usr(i).history);
	xml_user_write_events(fil, usr(i).events);
	xml_user_write_bets(fil, usr(i).bets);

	for(j=0;j<COUNT_USER_END;j++)
	    xml_write_int(fil, usr(i).counters[j], TAG_USER_COUNTER, I1);

	for(j=0;j<2;j++)
	{
	    fprintf(fil, "%s<_%d>\n", I1, TAG_USER_MONEY_INS);
	    for(k=0;k<MON_IN_END;k++)
		xml_write_int(fil, usr(i).money_in[j][k], TAG_USER_MONEY_IN, I2);	    
	    fprintf(fil, "%s</_%d>\n", I1, TAG_USER_MONEY_INS);
	}

	for(j=0;j<2;j++)
	{
	    fprintf(fil, "%s<_%d>\n", I1, TAG_USER_MONEY_OUTS);
	    for(k=0;k<MON_OUT_END;k++)
		xml_write_int(fil, usr(i).money_out[j][k], TAG_USER_MONEY_OUT, I2);
	    fprintf(fil, "%s</_%d>\n", I1, TAG_USER_MONEY_OUTS);
	}

	xml_write_int(fil, usr(i).youth_academy.coach, TAG_USER_YA_COACH, I1);
	xml_write_int(fil, usr(i).youth_academy.percentage, TAG_USER_YA_PERCENTAGE, I1);
	xml_write_float(fil, usr(i).youth_academy.av_coach, TAG_USER_YA_AV_COACH, I1);
	xml_write_float(fil, usr(i).youth_academy.av_percentage, 
			TAG_USER_YA_AV_PERCENTAGE, I1);
	xml_write_float(fil, usr(i).youth_academy.counter_youth, TAG_USER_YA_COUNTER, I1);

	xml_loadsave_players_write(fil, usr(i).youth_academy.players);

	fprintf(fil, "</_%d>\n", TAG_USER);
    }

    fprintf(fil, "</_%d>\n", TAG_USERS);

    fclose(fil);
}

/** Write the history of a user into the file. */
void
xml_user_write_history(FILE *fil, const GArray *history)
{
    gint i, j;

    for(i=0;i<history->len;i++)
    {
	fprintf(fil, "%s<_%d>\n", I1, TAG_USER_HISTORY);

	xml_write_int(fil, g_array_index(history, UserHistory, i).season,
		      TAG_USER_HISTORY_SEASON, I2);
	xml_write_int(fil, g_array_index(history, UserHistory, i).week,
		      TAG_USER_HISTORY_WEEK, I2);
	xml_write_int(fil, g_array_index(history, UserHistory, i).type,
		      TAG_USER_HISTORY_TYPE, I2);
	xml_write_string(fil, g_array_index(history, UserHistory, i).team_name,
			 TAG_USER_HISTORY_TEAM_NAME, I2);
	for(j=0;j<3;j++)
	    if(g_array_index(history, UserHistory, i).string[j] != NULL)
		xml_write_string(fil, g_array_index(history, UserHistory, i).string[j],
				 TAG_USER_HISTORY_STRING, I2);
	    else
		xml_write_string(fil, "", TAG_USER_HISTORY_STRING, I2);

	fprintf(fil, "%s</_%d>\n", I1, TAG_USER_HISTORY);
    }
}

/** Write the events of a user into the file. */
void
xml_user_write_events(FILE *fil, const GArray *events)
{
    gint i;

    for(i=0;i<events->len;i++)
    {
	fprintf(fil, "%s<_%d>\n", I1, TAG_USER_EVENT);

	xml_write_int(fil, g_array_index(events, Event, i).type,
		      TAG_USER_EVENT_TYPE, I2);
	xml_write_int(fil, g_array_index(events, Event, i).value1,
		      TAG_USER_EVENT_VALUE1, I2);
	xml_write_int(fil, g_array_index(events, Event, i).value2,
		      TAG_USER_EVENT_VALUE2, I2);
	xml_write_string(fil, g_array_index(events, Event, i).value_string,
			 TAG_USER_EVENT_VALUE_STRING, I2);
	
	fprintf(fil, "%s</_%d>\n", I1, TAG_USER_EVENT);
    }
}

/** Write the bets of a user into the file. */
void
xml_user_write_bets(FILE *fil, GArray **bets)
{
    gint i, j;

    for(i=0;i<2;i++)
	for(j=0;j<bets[i]->len;j++)
	{
	    fprintf(fil, "%s<_%d>\n", I1, 
		    (i == 0) ? TAG_USER_BET0 : TAG_USER_BET1);
	    xml_write_int(fil, g_array_index(bets[i], BetUser, j).wager,
			  TAG_USER_BET_WAGER, I2);
	    xml_write_int(fil, g_array_index(bets[i], BetUser, j).outcome,
			  TAG_USER_BET_OUTCOME, I2);
	    xml_write_int(fil, g_array_index(bets[i], BetUser, j).fix_id,
			  TAG_USER_BET_FIX_ID, I2);
	    fprintf(fil, "%s</_%d>\n", I1, 
		    (i == 0) ? TAG_USER_BET0 : TAG_USER_BET1);
	}
}
