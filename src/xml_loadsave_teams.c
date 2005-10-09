#include "file.h"
#include "misc.h"
#include "player.h"
#include "team.h"
#include "xml.h"
#include "xml_loadsave_players.h"
#include "xml_loadsave_teams.h"

enum
{
    TAG_TEAMS = TAG_START_TEAMS,
    TAG_TEAM,
    TAG_TEAM_NAMES_FILE,
    TAG_TEAM_STRUCTURE,
    TAG_TEAM_STYLE,
    TAG_TEAM_BOOST,
    TAG_TEAM_CLID,
    TAG_TEAM_STADIUM,
    TAG_TEAM_STADIUM_NAME,
    TAG_TEAM_STADIUM_CAPACITY,
    TAG_TEAM_STADIUM_AVERAGE_ATTENDANCE,
    TAG_TEAM_STADIUM_POSSIBLE_ATTENDANCE,
    TAG_TEAM_STADIUM_GAMES,
    TAG_TEAM_STADIUM_SAFETY,
    TAG_END
};

gint state, etalidx, careeridx;
GArray *teams_array;
Team new_team;
Player new_player;
PlayerGamesGoals new_games_goals;
PlayerCard new_card;

void
xml_loadsave_teams_start_element (GMarkupParseContext *context,
				  const gchar         *element_name,
				  const gchar        **attribute_names,
				  const gchar        **attribute_values,
				  gpointer             user_data,
				  GError             **error)
{
    gint i;
    gint tag = xml_get_tag_from_name(element_name);
    gboolean valid_tag = FALSE;

    for(i=TAG_TEAMS;i<TAG_END;i++)
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

    if(tag == TAG_TEAM)
	new_team = team_new(FALSE);
    else if(tag >= TAG_START_PLAYERS && tag <= TAG_END_PLAYERS)
    {
	valid_tag = TRUE;
	state = TAG_START_PLAYERS;
	xml_loadsave_players_start_element(tag, &new_team);
    }

    if(!valid_tag)
	g_warning("xml_loadsave_teams_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_teams_end_element    (GMarkupParseContext *context,
				   const gchar         *element_name,
				   gpointer             user_data,
				   GError             **error)
{
    gint tag = xml_get_tag_from_name(element_name);

    if(tag == TAG_TEAM)
    {
	state = TAG_TEAMS;
	g_array_append_val(teams_array, new_team);
    }
    else if(tag == TAG_NAME ||
	    tag == TAG_SYMBOL ||
	    tag == TAG_TEAM_NAMES_FILE ||
	    tag == TAG_TEAM_ID ||
	    tag == TAG_TEAM_CLID ||
	    tag == TAG_TEAM_STRUCTURE ||
	    tag == TAG_TEAM_STYLE ||
	    tag == TAG_TEAM_BOOST ||
	    tag == TAG_TEAM_STADIUM)
	state = TAG_TEAM;
    else if(tag == TAG_TEAM_STADIUM_NAME ||
	    tag == TAG_TEAM_STADIUM_CAPACITY ||
	    tag == TAG_TEAM_STADIUM_AVERAGE_ATTENDANCE || 
	    tag == TAG_TEAM_STADIUM_POSSIBLE_ATTENDANCE ||
	    tag == TAG_TEAM_STADIUM_GAMES ||
	    tag == TAG_TEAM_STADIUM_SAFETY)
	state = TAG_TEAM_STADIUM;
    else if(tag >= TAG_START_PLAYERS && tag <= TAG_END_PLAYERS)
    {
	xml_loadsave_players_end_element(tag, new_team.players);
	if(tag == TAG_START_PLAYERS)
	    state = TAG_TEAM;
    }
    else if(tag != TAG_TEAMS)
	g_warning("xml_loadsave_teams_end_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

void
xml_loadsave_teams_text         (GMarkupParseContext *context,
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
    float_value = (gfloat)g_ascii_strtod(buf, NULL) / 10000;

    if(state == TAG_NAME)
	misc_string_assign(&new_team.name, buf);
    else if(state == TAG_SYMBOL)
	misc_string_assign(&new_team.symbol, buf);
    else if(state == TAG_TEAM_NAMES_FILE)
	misc_string_assign(&new_team.names_file, buf);
    else if(state == TAG_TEAM_CLID)
	new_team.clid = int_value;
    else if(state == TAG_TEAM_ID)
	new_team.id = int_value;
    else if(state == TAG_TEAM_STRUCTURE)
	new_team.structure = int_value;
    else if(state == TAG_TEAM_STYLE)
	new_team.style = int_value;
    else if(state == TAG_TEAM_BOOST)
	new_team.boost = int_value;
    else if(state == TAG_TEAM_STADIUM_NAME)
	misc_string_assign(&new_team.stadium.name, buf);
    else if(state == TAG_TEAM_STADIUM_CAPACITY)
	new_team.stadium.capacity = int_value;
    else if(state == TAG_TEAM_STADIUM_AVERAGE_ATTENDANCE)
	new_team.stadium.average_attendance = int_value;
    else if(state == TAG_TEAM_STADIUM_POSSIBLE_ATTENDANCE)
	new_team.stadium.possible_attendance = int_value;
    else if(state == TAG_TEAM_STADIUM_GAMES)
	new_team.stadium.games = int_value;
    else if(state == TAG_TEAM_STADIUM_SAFETY)
	new_team.stadium.safety = float_value;
    else if(state >= TAG_START_PLAYERS && state <= TAG_END_PLAYERS)
	xml_loadsave_players_text(buf);
}

void
xml_loadsave_teams_read(const gchar *filename, GArray *teams)
{
    gint i, j;
    GMarkupParser parser = {xml_loadsave_teams_start_element,
			    xml_loadsave_teams_end_element,
			    xml_loadsave_teams_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    guint length;
    GError *error = NULL;

    context = 
	g_markup_parse_context_new(&parser, 0, NULL, NULL);

    if(!g_file_get_contents(filename, &file_contents, &length, &error))
    {
	g_warning("xml_loadsave_teams_read: error reading file %s\n", filename);
	misc_print_error(&error, TRUE);
    }

    teams_array = teams;

    if(g_markup_parse_context_parse(context, file_contents, length, &error))
    {
	g_markup_parse_context_end_parse(context, NULL);
	g_markup_parse_context_free(context);
	g_free(file_contents);
    }
    else
    {
	g_warning("xml_loadsave_teams_read: error parsing file %s\n", filename);
	misc_print_error(&error, TRUE);
    }

    for(i=0;i<teams->len;i++)
	for(j=0;j<g_array_index(teams, Team, i).players->len;j++)
	    g_array_index(g_array_index(teams, Team, i).players, Player, j).team = 
		&g_array_index(teams, Team, i);
}

void
xml_loadsave_teams_write(const gchar *filename, const GArray *teams)
{
    gint i;
    FILE *fil = NULL;

    file_my_fopen(filename, "w", &fil, TRUE);

    fprintf(fil, "<_%d>\n", TAG_TEAMS);
    for(i=0;i<teams->len;i++)
	xml_loadsave_teams_write_team(fil, &g_array_index(teams, Team, i));

    fprintf(fil, "</_%d>\n", TAG_TEAMS);

    fclose(fil);
}

void
xml_loadsave_teams_write_team(FILE *fil, const Team* team)
{
    fprintf(fil, "<_%d>\n", TAG_TEAM);

    xml_write_string(fil, team->name, TAG_NAME, I1);
    xml_write_string(fil, team->symbol, TAG_SYMBOL, I1);
    xml_write_string(fil, team->names_file, TAG_TEAM_NAMES_FILE, I1);
    
    xml_write_int(fil, team->clid, TAG_TEAM_CLID, I1);
    xml_write_int(fil, team->id, TAG_TEAM_ID, I1);
    xml_write_int(fil, team->structure, TAG_TEAM_STRUCTURE, I1);
    xml_write_int(fil, team->style, TAG_TEAM_STYLE, I1);
    xml_write_int(fil, team->boost, TAG_TEAM_BOOST, I1);
    
    fprintf(fil, "%s<_%d>\n", I1, TAG_TEAM_STADIUM);

    if(team->stadium.name != NULL)
	xml_write_string(fil, team->stadium.name, TAG_TEAM_STADIUM_NAME, I2);

    xml_write_int(fil, team->stadium.capacity, TAG_TEAM_STADIUM_CAPACITY, I2);
    xml_write_int(fil, team->stadium.average_attendance, TAG_TEAM_STADIUM_AVERAGE_ATTENDANCE, I2);
    xml_write_int(fil, team->stadium.possible_attendance, TAG_TEAM_STADIUM_POSSIBLE_ATTENDANCE, I2);
    xml_write_int(fil, team->stadium.games, TAG_TEAM_STADIUM_GAMES, I2);
    xml_write_float(fil, team->stadium.safety, TAG_TEAM_STADIUM_SAFETY, I2);

    fprintf(fil, "%s</_%d>\n", I1, TAG_TEAM_STADIUM);

    xml_loadsave_players_write(fil, team->players);

    fprintf(fil, "</_%d>\n", TAG_TEAM);
}
