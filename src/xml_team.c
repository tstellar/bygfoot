#include "file.h"
#include "free.h"
#include "main.h"
#include "misc.h"
#include "option.h"
#include "player.h"
#include "team.h"
#include "variables.h"
#include "xml_team.h"

/**
 * The tags used in the XML files defining teams.
 */
#define TAG_TEAM "team"
#define TAG_TEAM_NAME "team_name"
#define TAG_STADIUM_NAME "stadium_name"
#define TAG_SYMBOL "symbol"
#define TAG_AVERAGE_TALENT "average_talent"
#define TAG_FORMATION "formation"
#define TAG_NAMES_FILE "names_file"
#define TAG_PLAYER "player"
#define TAG_PLAYER_NAME "player_name"
#define TAG_PLAYER_BIRTH_YEAR "birth_year"
#define TAG_PLAYER_BIRTH_MONTH "birth_month"
#define TAG_PLAYER_SKILL "skill"
#define TAG_PLAYER_TALENT "talent"
#define TAG_PLAYER_POSITION "position"

enum XmlTeamStates
{
    STATE_TEAM = 0,
    STATE_TEAM_NAME,
    STATE_STADIUM_NAME,
    STATE_SYMBOL,
    STATE_AVERAGE_TALENT,
    STATE_FORMATION,
    STATE_NAMES_FILE,
    STATE_PLAYER,
    STATE_PLAYER_NAME,
    STATE_PLAYER_BIRTH_YEAR,
    STATE_PLAYER_BIRTH_MONTH,
    STATE_PLAYER_SKILL,
    STATE_PLAYER_TALENT,
    STATE_PLAYER_POSITION,
    STATE_END
};

gint state, birth_year;
Player new_player;
Team *team;
const gchar *d_file;

void
xml_team_read_start_element (GMarkupParseContext *context,
			    const gchar         *element_name,
			    const gchar        **attribute_names,
			    const gchar        **attribute_values,
			    gpointer             user_data,
			    GError             **error)
{
    if(strcmp(element_name, TAG_TEAM) == 0)
	state = STATE_TEAM;
    else if(strcmp(element_name, TAG_TEAM_NAME) == 0)
	state = STATE_TEAM_NAME;
    else if(strcmp(element_name, TAG_STADIUM_NAME) == 0)
	state = STATE_STADIUM_NAME;
    else if(strcmp(element_name, TAG_SYMBOL) == 0)
	state = STATE_SYMBOL;
    else if(strcmp(element_name, TAG_AVERAGE_TALENT) == 0)
	state = STATE_AVERAGE_TALENT;
    else if(strcmp(element_name, TAG_FORMATION) == 0)
	state = STATE_FORMATION;
    else if(strcmp(element_name, TAG_NAMES_FILE) == 0)
	state = STATE_NAMES_FILE;
    else if(strcmp(element_name, TAG_PLAYER) == 0)
    {
	state = STATE_PLAYER;
	new_player = player_new(team, ((gfloat)team->average_talent / 10000) * 
				const_float("float_player_max_skill"), TRUE);
    }
    else if(strcmp(element_name, TAG_PLAYER_NAME) == 0)
	state = STATE_PLAYER_NAME;
    else if(strcmp(element_name, TAG_PLAYER_BIRTH_YEAR) == 0)
	state = STATE_PLAYER_BIRTH_YEAR;
    else if(strcmp(element_name, TAG_PLAYER_BIRTH_MONTH) == 0)
	state = STATE_PLAYER_BIRTH_MONTH;
    else if(strcmp(element_name, TAG_PLAYER_SKILL) == 0)
	state = STATE_PLAYER_SKILL;
    else if(strcmp(element_name, TAG_PLAYER_TALENT) == 0)
	state = STATE_PLAYER_TALENT;
    else if(strcmp(element_name, TAG_PLAYER_POSITION) == 0)
	state = STATE_PLAYER_POSITION;
    else
	g_warning("xml_team_read_start_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

/**
 * The function called by the parser when a closing tag is read.
 * The state variable is changed in this function.
 * @see The GLib manual (Simple XML parser).
 */
void
xml_team_read_end_element    (GMarkupParseContext *context,
			      const gchar         *element_name,
			      gpointer             user_data,
			      GError             **error)
{
    gchar buf[SMALL];

    if(strcmp(element_name, TAG_TEAM_NAME) == 0 ||
       strcmp(element_name, TAG_STADIUM_NAME) == 0 ||
       strcmp(element_name, TAG_SYMBOL) == 0 ||
       strcmp(element_name, TAG_AVERAGE_TALENT) == 0 ||
       strcmp(element_name, TAG_FORMATION) == 0 ||
       strcmp(element_name, TAG_NAMES_FILE) == 0 ||
       strcmp(element_name, TAG_PLAYER) == 0)
    {
	state = STATE_TEAM;
	if(strcmp(element_name, TAG_PLAYER) == 0)
	{
	    if(team->players->len == const_int("int_team_cpu_players"))
	    {
		free_player(&new_player);
		sprintf(buf, "xml_team_read_end_element: too many players in team definition '%s' (only %d allowed).",
			d_file, const_int("int_team_cpu_players"));
		main_exit_program(EXIT_LOAD_TEAM_DEF, buf);
	    }
	    else
		g_array_append_val(team->players, new_player);
	}
    }
    else if(strcmp(element_name, TAG_PLAYER_NAME) == 0 ||
	    strcmp(element_name, TAG_PLAYER_BIRTH_YEAR) == 0 ||
	    strcmp(element_name, TAG_PLAYER_BIRTH_MONTH) == 0 ||
	    strcmp(element_name, TAG_PLAYER_SKILL) == 0 ||
	    strcmp(element_name, TAG_PLAYER_TALENT) == 0 ||
	    strcmp(element_name, TAG_PLAYER_POSITION) == 0)
	state = STATE_PLAYER;
    else if(strcmp(element_name, TAG_TEAM) != 0)
	g_warning("xml_team_read_end_element: unknown tag: %s; I'm in state %d\n",
		  element_name, state);
}

/**
 * The function called by the parser when the text between tags is read.
 * This function is responsible for filling in the variables (e.g. team names)
 * when a file gets loaded.
 * @see The GLib manual (Simple XML parser).
 */
void
xml_team_read_text         (GMarkupParseContext *context,
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

    if(state == STATE_TEAM_NAME)
	g_string_printf(team->name, "%s", buf);
    else if(state == STATE_STADIUM_NAME)
    {
	free_g_string(&team->stadium.name);
	team->stadium.name = g_string_new(buf);
    }
    else if(state == STATE_SYMBOL)
	g_string_printf(team->symbol, "%s", buf);
    else if(state == STATE_AVERAGE_TALENT && opt_int("int_opt_load_defs") == 1)
	team->average_talent = 
	    (float_value / 10000) * const_float("float_player_max_skill");
    else if(state == STATE_FORMATION)
	team->structure = int_value;
    else if(state == STATE_NAMES_FILE)
	g_string_printf(team->names_file, "%s", buf);
    else if(state == STATE_PLAYER_NAME)
	g_string_printf(new_player.name, "%s", buf);
    else if(state == STATE_PLAYER_BIRTH_YEAR && opt_int("int_opt_load_defs") == 1)
	birth_year = int_value;
    else if(state == STATE_PLAYER_BIRTH_MONTH && opt_int("int_opt_load_defs") == 1)
	new_player.age = misc_get_age_from_birth(birth_year, int_value);
    else if(state == STATE_PLAYER_SKILL && opt_int("int_opt_load_defs") == 1)
	new_player.skill = ((gfloat)int_value / 10000) * 
	    const_float("float_player_max_skill");
    else if(state == STATE_PLAYER_TALENT && opt_int("int_opt_load_defs") == 1)
	new_player.talent = ((gfloat)int_value / 10000) * 
	    const_float("float_player_max_skill");
    else if(state == STATE_PLAYER_POSITION)
	new_player.pos = int_value;
}

/** Parse a team definition file and write the team accordingly. */
void
xml_team_read(Team *tm, const gchar *def_file)
{
    GMarkupParser parser = {xml_team_read_start_element,
			    xml_team_read_end_element,
			    xml_team_read_text, NULL, NULL};
    GMarkupParseContext *context;
    gchar *file_contents;
    gint length;
    GError *error = NULL;

    team = tm;
    d_file = def_file;

    context = 
	g_markup_parse_context_new(&parser, 0, NULL, NULL);

    if(!g_file_get_contents(def_file, &file_contents, &length, &error))
    {
	g_warning("xml_team_read: error reading file %s\n", def_file);
	misc_print_error(&error, FALSE);
	return;
    }

    if(g_markup_parse_context_parse(context, file_contents, length, &error))
    {
	g_markup_parse_context_end_parse(context, NULL);	
	g_markup_parse_context_free(context);
	g_free(file_contents);
    }
    else
    {
	g_critical("xml_team_read: error parsing file %s\n", def_file);
	misc_print_error(&error, TRUE);
    }

    team_complete_def(team);
}
