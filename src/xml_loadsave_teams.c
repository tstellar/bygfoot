#include "file.h"
#include "misc.h"
#include "player.h"
#include "team.h"
#include "xml.h"
#include "xml_loadsave_teams.h"

enum
{
    TAG_TEAMS = TAG_START_TEAMS,
    TAG_TEAM,
    TAG_TEAM_STRUCTURE,
    TAG_TEAM_STYLE,
    TAG_TEAM_BOOST,
    TAG_TEAM_STADIUM,
    TAG_TEAM_STADIUM_CAPACITY,
    TAG_TEAM_STADIUM_AVERAGE_ATTENDANCE,
    TAG_TEAM_STADIUM_POSSIBLE_ATTENDANCE,
    TAG_TEAM_STADIUM_GAMES,
    TAG_TEAM_STADIUM_SAFETY,
    TAG_TEAM_PLAYER,
    TAG_TEAM_PLAYER_NAME,
    TAG_TEAM_PLAYER_POS,
    TAG_TEAM_PLAYER_CPOS,
    TAG_TEAM_PLAYER_HEALTH,
    TAG_TEAM_PLAYER_RECOVERY,
    TAG_TEAM_PLAYER_ID,
    TAG_TEAM_PLAYER_VALUE,
    TAG_TEAM_PLAYER_WAGE,
    TAG_TEAM_PLAYER_OFFERS,
    TAG_TEAM_PLAYER_SKILL,
    TAG_TEAM_PLAYER_CSKILL,
    TAG_TEAM_PLAYER_TALENT,
    TAG_TEAM_PLAYER_ETAL,
    TAG_TEAM_PLAYER_FITNESS,
    TAG_TEAM_PLAYER_LSU,
    TAG_TEAM_PLAYER_AGE,
    TAG_TEAM_PLAYER_PEAK_AGE,
    TAG_TEAM_PLAYER_CONTRACT,
    TAG_TEAM_PLAYER_PARTICIPATION,
    TAG_TEAM_PLAYER_GAMES_GOAL,
    TAG_TEAM_PLAYER_GAMES_GOAL_CLID,
    TAG_TEAM_PLAYER_GAMES_GOAL_GAMES,
    TAG_TEAM_PLAYER_GAMES_GOAL_GOALS,
    TAG_TEAM_PLAYER_GAMES_GOAL_SHOTS,
    TAG_TEAM_PLAYER_CARD,
    TAG_TEAM_PLAYER_CARD_CLID,
    TAG_TEAM_PLAYER_CARD_YELLOW,
    TAG_TEAM_PLAYER_CARD_RED,
    TAG_END
};

gint state, etalidx;
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
	new_team = team_new();
    else if(tag == TAG_TEAM_PLAYER)
    {
	new_player = player_new(&new_team, 80);
	etalidx = 0;
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
	    tag == TAG_TEAM_ID ||
	    tag == TAG_TEAM_CLID ||
	    tag == TAG_TEAM_STRUCTURE ||
	    tag == TAG_TEAM_STYLE ||
	    tag == TAG_TEAM_BOOST ||
	    tag == TAG_TEAM_STADIUM ||
	    tag == TAG_TEAM_PLAYER)
    {
	state = TAG_TEAM;
	if(tag == TAG_TEAM_PLAYER)
	    g_array_append_val(new_team.players, new_player);
    }
    else if(tag == TAG_TEAM_STADIUM_CAPACITY ||
	    tag == TAG_TEAM_STADIUM_AVERAGE_ATTENDANCE || 
	    tag == TAG_TEAM_STADIUM_POSSIBLE_ATTENDANCE ||
	    tag == TAG_TEAM_STADIUM_GAMES ||
	    tag == TAG_TEAM_STADIUM_SAFETY)
	state = TAG_TEAM_STADIUM;
    else if(tag == TAG_TEAM_PLAYER_NAME ||
	    tag == TAG_TEAM_PLAYER_POS ||
	    tag == TAG_TEAM_PLAYER_CPOS ||
	    tag == TAG_TEAM_PLAYER_HEALTH ||
	    tag == TAG_TEAM_PLAYER_RECOVERY ||
	    tag == TAG_TEAM_PLAYER_ID ||
	    tag == TAG_TEAM_PLAYER_VALUE ||
	    tag == TAG_TEAM_PLAYER_WAGE ||
	    tag == TAG_TEAM_PLAYER_OFFERS ||
	    tag == TAG_TEAM_PLAYER_SKILL ||
	    tag == TAG_TEAM_PLAYER_CSKILL ||
	    tag == TAG_TEAM_PLAYER_TALENT ||
	    tag == TAG_TEAM_PLAYER_ETAL ||
	    tag == TAG_TEAM_PLAYER_FITNESS ||
	    tag == TAG_TEAM_PLAYER_LSU ||
	    tag == TAG_TEAM_PLAYER_AGE ||
	    tag == TAG_TEAM_PLAYER_PEAK_AGE ||
	    tag == TAG_TEAM_PLAYER_CONTRACT ||
	    tag == TAG_TEAM_PLAYER_PARTICIPATION ||
	    tag == TAG_TEAM_PLAYER_GAMES_GOAL ||
	    tag == TAG_TEAM_PLAYER_CARD)
    {
	state = TAG_TEAM_PLAYER;
	if(tag == TAG_TEAM_PLAYER_ETAL)
	    etalidx++;
	else if(tag == TAG_TEAM_PLAYER_CARD)
	    g_array_append_val(new_player.cards, new_card);
	else if(tag == TAG_TEAM_PLAYER_GAMES_GOAL)
	    g_array_append_val(new_player.games_goals, new_games_goals);
    }
    else if(tag == TAG_TEAM_PLAYER_GAMES_GOAL_CLID ||
	    tag == TAG_TEAM_PLAYER_GAMES_GOAL_GAMES ||
	    tag == TAG_TEAM_PLAYER_GAMES_GOAL_GOALS ||
	    tag == TAG_TEAM_PLAYER_GAMES_GOAL_SHOTS)
	state = TAG_TEAM_PLAYER_GAMES_GOAL;
    else if(tag == TAG_TEAM_PLAYER_CARD_CLID ||
	    tag == TAG_TEAM_PLAYER_CARD_YELLOW ||
	    tag == TAG_TEAM_PLAYER_CARD_RED)
	state = TAG_TEAM_PLAYER_CARD;
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
	g_string_printf(new_team.name, "%s", buf);
    else if(state == TAG_SYMBOL)
	g_string_printf(new_team.symbol, "%s", buf);
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
    else if(state == TAG_TEAM_PLAYER_NAME)
	g_string_printf(new_player.name, "%s", buf);
    else if(state == TAG_TEAM_PLAYER_POS)
	new_player.pos = int_value;
    else if(state == TAG_TEAM_PLAYER_CPOS)
	new_player.cpos = int_value;
    else if(state == TAG_TEAM_PLAYER_HEALTH)
	new_player.health = int_value;
    else if(state == TAG_TEAM_PLAYER_RECOVERY)
	new_player.recovery = int_value;
    else if(state == TAG_TEAM_PLAYER_ID)
	new_player.id = int_value;
    else if(state == TAG_TEAM_PLAYER_VALUE)
	new_player.value = int_value;
    else if(state == TAG_TEAM_PLAYER_WAGE)
	new_player.wage = int_value;
    else if(state == TAG_TEAM_PLAYER_OFFERS)
	new_player.offers = int_value;
    else if(state == TAG_TEAM_PLAYER_PARTICIPATION)
	new_player.participation = int_value;
    else if(state == TAG_TEAM_PLAYER_SKILL)
	new_player.skill = float_value;
    else if(state == TAG_TEAM_PLAYER_CSKILL)
	new_player.cskill = float_value;
    else if(state == TAG_TEAM_PLAYER_TALENT)
	new_player.talent = float_value;
    else if(state == TAG_TEAM_PLAYER_FITNESS)
	new_player.fitness = float_value;
    else if(state == TAG_TEAM_PLAYER_LSU)
	new_player.lsu = float_value;
    else if(state == TAG_TEAM_PLAYER_AGE)
	new_player.age = float_value;
    else if(state == TAG_TEAM_PLAYER_PEAK_AGE)
	new_player.peak_age = float_value;
    else if(state == TAG_TEAM_PLAYER_CONTRACT)
	new_player.contract = float_value;
    else if(state == TAG_TEAM_PLAYER_ETAL)
	new_player.etal[etalidx] = float_value;
    else if(state == TAG_TEAM_PLAYER_GAMES_GOAL_CLID)
	new_games_goals.clid = int_value;
    else if(state == TAG_TEAM_PLAYER_GAMES_GOAL_GAMES)
	new_games_goals.games = int_value;
    else if(state == TAG_TEAM_PLAYER_GAMES_GOAL_GOALS)
	new_games_goals.goals = int_value;
    else if(state == TAG_TEAM_PLAYER_GAMES_GOAL_SHOTS)
	new_games_goals.shots = int_value;
    else if(state == TAG_TEAM_PLAYER_CARD_CLID)
	new_card.clid = int_value;
    else if(state == TAG_TEAM_PLAYER_CARD_YELLOW)
	new_card.yellow = int_value;
    else if(state == TAG_TEAM_PLAYER_CARD_RED)
	new_card.red = int_value;
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
    gint i;

    fprintf(fil, "<_%d>\n", TAG_TEAM);

    xml_write_g_string(fil, team->name, TAG_NAME, I1);
    xml_write_g_string(fil, team->symbol, TAG_SYMBOL, I1);
    
    xml_write_int(fil, team->clid, TAG_TEAM_CLID, I1);
    xml_write_int(fil, team->id, TAG_TEAM_ID, I1);
    xml_write_int(fil, team->structure, TAG_TEAM_STRUCTURE, I1);
    xml_write_int(fil, team->style, TAG_TEAM_STYLE, I1);
    xml_write_int(fil, team->boost, TAG_TEAM_BOOST, I1);
    
    fprintf(fil, "%s<_%d>\n", I1, TAG_TEAM_STADIUM);

    xml_write_int(fil, team->stadium.capacity, TAG_TEAM_STADIUM_CAPACITY, I2);
    xml_write_int(fil, team->stadium.average_attendance, TAG_TEAM_STADIUM_AVERAGE_ATTENDANCE, I2);
    xml_write_int(fil, team->stadium.possible_attendance, TAG_TEAM_STADIUM_POSSIBLE_ATTENDANCE, I2);
    xml_write_int(fil, team->stadium.games, TAG_TEAM_STADIUM_GAMES, I2);
    xml_write_float(fil, team->stadium.safety, TAG_TEAM_STADIUM_SAFETY, I2);

    fprintf(fil, "%s</_%d>\n", I1, TAG_TEAM_STADIUM);

    for(i=0;i<team->players->len;i++)
	xml_loadsave_teams_write_player(fil, &g_array_index(team->players, Player, i));

    fprintf(fil, "</_%d>\n", TAG_TEAM);
}

void
xml_loadsave_teams_write_player(FILE *fil, const Player *pl)
{
    gint i;

    fprintf(fil, "%s<_%d>\n", I1, TAG_TEAM_PLAYER);

    xml_write_g_string(fil, pl->name, TAG_TEAM_PLAYER_NAME, I2);
    xml_write_int(fil, pl->pos, TAG_TEAM_PLAYER_POS, I2);
    xml_write_int(fil, pl->cpos, TAG_TEAM_PLAYER_CPOS, I2);
    xml_write_int(fil, pl->health, TAG_TEAM_PLAYER_HEALTH, I2);
    xml_write_int(fil, pl->recovery, TAG_TEAM_PLAYER_RECOVERY, I2);
    xml_write_int(fil, pl->id, TAG_TEAM_PLAYER_ID, I2);
    xml_write_int(fil, pl->value, TAG_TEAM_PLAYER_VALUE, I2);
    xml_write_int(fil, pl->wage, TAG_TEAM_PLAYER_WAGE, I2);
    xml_write_int(fil, pl->offers, TAG_TEAM_PLAYER_OFFERS, I2);
    xml_write_int(fil, pl->participation, TAG_TEAM_PLAYER_PARTICIPATION, I2);

    xml_write_float(fil, pl->skill, TAG_TEAM_PLAYER_SKILL, I2);
    xml_write_float(fil, pl->cskill, TAG_TEAM_PLAYER_CSKILL, I2);
    xml_write_float(fil, pl->talent, TAG_TEAM_PLAYER_TALENT, I2);
    xml_write_float(fil, pl->fitness, TAG_TEAM_PLAYER_FITNESS, I2);
    xml_write_float(fil, pl->lsu, TAG_TEAM_PLAYER_LSU, I2);
    xml_write_float(fil, pl->age, TAG_TEAM_PLAYER_AGE, I2);
    xml_write_float(fil, pl->peak_age, TAG_TEAM_PLAYER_PEAK_AGE, I2);
    xml_write_float(fil, pl->contract, TAG_TEAM_PLAYER_CONTRACT, I2);

    for(i=0;i<4;i++)
	xml_write_float(fil, pl->etal[i], TAG_TEAM_PLAYER_ETAL, I3);
    
    for(i=0;i<pl->games_goals->len;i++)
    {
	fprintf(fil, "%s<_%d>\n", I2, TAG_TEAM_PLAYER_GAMES_GOAL);

	xml_write_int(fil, g_array_index(pl->games_goals, PlayerGamesGoals, i).clid,
		      TAG_TEAM_PLAYER_GAMES_GOAL_CLID, I3);
	xml_write_int(fil, g_array_index(pl->games_goals, PlayerGamesGoals, i).games,
		      TAG_TEAM_PLAYER_GAMES_GOAL_GAMES, I3);
	xml_write_int(fil, g_array_index(pl->games_goals, PlayerGamesGoals, i).goals,
		      TAG_TEAM_PLAYER_GAMES_GOAL_GOALS, I3);
	xml_write_int(fil, g_array_index(pl->games_goals, PlayerGamesGoals, i).shots,
		      TAG_TEAM_PLAYER_GAMES_GOAL_SHOTS, I3);

	fprintf(fil, "%s</_%d>\n", I2, TAG_TEAM_PLAYER_GAMES_GOAL);
    }

    for(i=0;i<pl->cards->len;i++)
    {
	fprintf(fil, "%s<_%d>\n", I2, TAG_TEAM_PLAYER_CARD);

	xml_write_int(fil, g_array_index(pl->cards, PlayerCard, i).clid,
		      TAG_TEAM_PLAYER_CARD_CLID, I3);
	xml_write_int(fil, g_array_index(pl->cards, PlayerCard, i).yellow,
		      TAG_TEAM_PLAYER_CARD_YELLOW, I3);
	xml_write_int(fil, g_array_index(pl->cards, PlayerCard, i).red,
		      TAG_TEAM_PLAYER_CARD_RED, I3);
	
	fprintf(fil, "%s</_%d>\n", I2, TAG_TEAM_PLAYER_CARD);
    }

    fprintf(fil, "%s</_%d>\n", I1, TAG_TEAM_PLAYER);
}
