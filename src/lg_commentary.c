#include <locale.h>

#include "file.h"
#include "free.h"
#include "lg_commentary.h"
#include "live_game.h"
#include "main.h"
#include "misc.h"
#include "option.h"
#include "player.h"
#include "variables.h"
#include "xml_lg_commentary.h"

/** Generate commentary for the live game event in the unit.
    @param unit The unit we comment.
    @param fix The fixture being calculated. */
void
lg_commentary_generate(LiveGameUnit *unit, const Fixture *fix)
{
    gchar *strings[LG_TOKEN_END][2];
    gchar buf[SMALL];
    GPtrArray **commentaries = NULL;
    gint i;

    lg_commentary_set_strings(strings, unit, fix);
    
    if(unit->event.type == LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_DEFEND ||
       unit->event.type == LIVE_GAME_EVENT_STYLE_CHANGE_DEFEND ||
       unit->event.type == LIVE_GAME_EVENT_STYLE_CHANGE_BALANCED ||
       unit->event.type == LIVE_GAME_EVENT_STYLE_CHANGE_ATTACK ||
       unit->event.type == LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_ATTACK)
	commentaries = &lg_commentary[LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_DEFEND];
    else if(unit->event.type == LIVE_GAME_EVENT_BOOST_CHANGE_ANTI ||
	    unit->event.type == LIVE_GAME_EVENT_BOOST_CHANGE_OFF ||
	    unit->event.type == LIVE_GAME_EVENT_BOOST_CHANGE_ON)
	commentaries = &lg_commentary[LIVE_GAME_EVENT_BOOST_CHANGE_ANTI];
    else
	commentaries = &lg_commentary[unit->event.type];

    *commentaries = misc_randomise_g_pointer_array(*commentaries);

    for(i=0;i<(*commentaries)->len;i++)
    {
	if(lg_commentary_replace_tokens(
	       ((GString*)g_ptr_array_index((*commentaries), i))->str, strings, buf))
	    break;
    }

    if(i == (*commentaries)->len)
    {
	g_warning("lg_commentary_generate: didn't find fitting commentary for unit type %d \n",
		  unit->event.type);
	strcpy(buf, "FIXME!");
    }

    unit->event.commentary = g_string_new(buf);

    for(i=LG_TOKEN_ATTENDANCE;i<LG_TOKEN_END;i++)
	g_free(strings[i][1]);
}

/** Try to replace all special tokens in the string and write the result to dest.
    @param string The commentary string with tokens.
    @param strings The replacements of the tokens.
    @param dest The destination string.
    @return TRUE if we could replace all tokens, FALSE otherwise. */
gboolean
lg_commentary_replace_tokens(const gchar *string, gchar *strings[][2], gchar *dest)
{
    gint i;

    strcpy(dest, string);

    for(i=0;i<LG_TOKEN_END;i++)
    {
	if(query_misc_string_contains(dest, strings[i][0]))
	{
	    if(strings[i][1] == NULL)
		return FALSE;
	    else
		misc_string_replace_token(dest, strings[i][0], strings[i][1]);
	}
    }

    return TRUE;
}

/** Get the strings corresponding to the commentary tokens. */
void
lg_commentary_set_strings(gchar *strings[][2], const LiveGameUnit *unit, const Fixture *fix)
{
    gchar buf[SMALL];

    strings[LG_TOKEN_TEAM_HOME][0] = const_str("string_lg_commentary_token_team_home");
    strings[LG_TOKEN_TEAM_HOME][1] = fix->teams[0]->name->str;

    strings[LG_TOKEN_TEAM_AWAY][0] = const_str("string_lg_commentary_token_team_away");
    strings[LG_TOKEN_TEAM_AWAY][1] = fix->teams[1]->name->str;

    strings[LG_TOKEN_TEAM_IN_POSS][0] = const_str("string_lg_commentary_token_team_in_poss");
    strings[LG_TOKEN_TEAM_IN_POSS][1] = fix->teams[unit->possession]->name->str;


    strings[LG_TOKEN_TEAM_NOT_IN_POSS][0] = const_str("string_lg_commentary_token_team_not_in_poss");
    strings[LG_TOKEN_TEAM_NOT_IN_POSS][1] = fix->teams[!unit->possession]->name->str;

    strings[LG_TOKEN_TEAM_LOSING][0] = const_str("string_lg_commentary_token_team_losing");
    strings[LG_TOKEN_TEAM_LOSING][1] = (unit->result[0] == unit->result[1]) ? NULL :
	fix->teams[(unit->result[0] > unit->result[1])]->name->str;
	
    strings[LG_TOKEN_TEAM_WINNING][0] = const_str("string_lg_commentary_token_team_winning");
    strings[LG_TOKEN_TEAM_WINNING][1] = (unit->result[0] == unit->result[1]) ? NULL :
	fix->teams[(unit->result[0] < unit->result[1])]->name->str;

    strings[LG_TOKEN_TEAM][0] = const_str("string_lg_commentary_token_team");
    strings[LG_TOKEN_TEAM][1] = (unit->event.team == -1) ?
	NULL : fix->teams[unit->event.team]->name->str;

    strings[LG_TOKEN_PLAYER1][0] = const_str("string_lg_commentary_token_player1");
    strings[LG_TOKEN_PLAYER1][1] = (unit->event.player == -1) ?
	NULL : lg_commentary_get_player_name(unit, fix, FALSE);

    strings[LG_TOKEN_PLAYER2][0] = const_str("string_lg_commentary_token_player2");
    strings[LG_TOKEN_PLAYER2][1] = (unit->event.player2 == -1) ?
	NULL : lg_commentary_get_player_name(unit, fix, TRUE);
 
    strings[LG_TOKEN_ATTENDANCE][0] = const_str("string_lg_commentary_token_attendance");
    misc_print_grouped_int(fix->attendance, buf, FALSE);
    strings[LG_TOKEN_ATTENDANCE][1] = g_strdup(buf);
    
    strings[LG_TOKEN_RESULT][0] = const_str("string_lg_commentary_token_result");
    sprintf(buf, "%d : %d", unit->result[0], unit->result[1]);
    strings[LG_TOKEN_RESULT][1] = g_strdup(buf);

    strings[LG_TOKEN_MINUTE][0] = const_str("string_lg_commentary_token_minute");
    sprintf(buf, "%d", live_game_unit_get_minute(unit));
    strings[LG_TOKEN_MINUTE][1] = g_strdup(buf);

    strings[LG_TOKEN_EXTRA][0] = const_str("string_lg_commentary_token_extra");
    strings[LG_TOKEN_EXTRA][1] = lg_commentary_get_extra_data(unit, fix);
}

/** Return the name of a player involved in the unit (or NULL). 
    @param player Which player we look for. */
gchar*
lg_commentary_get_player_name(const LiveGameUnit *unit, const Fixture *fix, gboolean player2)
{
    gchar *return_value = NULL;

    if(unit->event.type == LIVE_GAME_EVENT_GENERAL)
	return_value = (player2) ?
	    player_get_last_name(player_of_id_team(fix->teams[unit->possession],
						   unit->event.player2)->name->str) :
	    player_get_last_name(player_of_id_team(fix->teams[unit->possession],
						   unit->event.player)->name->str);
    else if(unit->event.type == LIVE_GAME_EVENT_LOST_POSSESSION)
    {
	return_value = (player2) ?
	    player_get_last_name(player_of_id_team(fix->teams[!unit->possession], 
						   unit->event.player2)->name->str):
	    player_get_last_name(player_of_id_team(fix->teams[unit->possession], 
						   unit->event.player)->name->str);
    }
    else if(unit->event.type == LIVE_GAME_EVENT_SCORING_CHANCE ||
	    unit->event.type == LIVE_GAME_EVENT_HEADER ||
	    unit->event.type == LIVE_GAME_EVENT_PENALTY ||
	    unit->event.type == LIVE_GAME_EVENT_FREE_KICK)
    {	
	return_value = (player2) ?
	    player_get_last_name(player_of_id_team(fix->teams[unit->possession],
						   unit->event.player2)->name->str) :
	    player_get_last_name(player_of_id_team(fix->teams[unit->possession],
						   unit->event.player)->name->str);
    }
    else if(unit->event.type == LIVE_GAME_EVENT_GOAL ||
	    unit->event.type == LIVE_GAME_EVENT_MISS ||
	    unit->event.type == LIVE_GAME_EVENT_SAVE ||
	    unit->event.type == LIVE_GAME_EVENT_POST ||
	    unit->event.type == LIVE_GAME_EVENT_CROSS_BAR)
    {
	return_value = (player2) ?
	    player_get_last_name(player_of_id_team(fix->teams[!unit->possession], 
						   unit->event.player2)->name->str) :
	    player_get_last_name(player_of_id_team(fix->teams[unit->possession], 
						   unit->event.player)->name->str);
    }
    else if(unit->event.type == LIVE_GAME_EVENT_OWN_GOAL)
	return_value = 
	    player_get_last_name(player_of_id_team(fix->teams[!unit->possession], 
						   unit->event.player)->name->str);
    else if(unit->event.type == LIVE_GAME_EVENT_FOUL ||
	    unit->event.type == LIVE_GAME_EVENT_FOUL_RED ||
	    unit->event.type == LIVE_GAME_EVENT_FOUL_RED_INJURY ||
	    unit->event.type == LIVE_GAME_EVENT_FOUL_YELLOW)
    {
	return_value = (player2) ? 
	    player_get_last_name(player_of_id_team(fix->teams[unit->event.team], 
						   unit->event.player2)->name->str) :
	    player_get_last_name(player_of_id_team(fix->teams[!unit->event.team], 
						   unit->event.player)->name->str);
    }
    else if(unit->event.type == LIVE_GAME_EVENT_SEND_OFF ||
	    unit->event.type == LIVE_GAME_EVENT_INJURY ||
	    unit->event.type == LIVE_GAME_EVENT_TEMP_INJURY ||
	    unit->event.type == LIVE_GAME_EVENT_SUBSTITUTION)
	return_value = (player2) ?
	    player_get_last_name(player_of_id_team(fix->teams[unit->event.team],
						   unit->event.player2)->name->str) :
	    player_get_last_name(player_of_id_team(fix->teams[unit->event.team],
						   unit->event.player)->name->str);
    else
    {
	g_warning("lg_commentary_get_player_name: unknown event type %d\n",
		  unit->event.type);
	main_exit_program(EXIT_INT_NOT_FOUND, NULL);
    }

    return return_value;
}

/** Return some extra data depending on the unit type. */
gchar*
lg_commentary_get_extra_data(const LiveGameUnit *unit, const Fixture *fix)
{
    gchar buf[SMALL];

    switch(unit->event.type)
    {
	default:
	    return NULL;
	    break;
	case LIVE_GAME_EVENT_STRUCTURE_CHANGE:
	    sprintf(buf, "%d",
		    fix->teams[unit->event.team]->structure);
	    break;
	case LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_DEFEND:
	    strcpy(buf, _("ALL OUT DEFEND"));
	    break;
	case LIVE_GAME_EVENT_STYLE_CHANGE_DEFEND:
	    strcpy(buf, _("DEFEND"));
	    break;
	case LIVE_GAME_EVENT_STYLE_CHANGE_BALANCED:
	    strcpy(buf, _("BALANCED"));
	    break;
	case LIVE_GAME_EVENT_STYLE_CHANGE_ATTACK:
	    strcpy(buf, _("ATTACK"));
	    break;
	case LIVE_GAME_EVENT_STYLE_CHANGE_ALL_OUT_ATTACK:
	    strcpy(buf, _("ALL OUT ATTACK"));
	    break;
	case LIVE_GAME_EVENT_BOOST_CHANGE_ANTI:
	    strcpy(buf, _("ANTI"));
	    break;
	case LIVE_GAME_EVENT_BOOST_CHANGE_OFF:
	    strcpy(buf, _("OFF"));
	    break;
	case LIVE_GAME_EVENT_BOOST_CHANGE_ON:
	    strcpy(buf, _("ON"));
	    break;
    }

    return g_strdup(buf);
}

/** Load the appropriate commentary file by evaluating
    the language option. */
void
lg_commentary_load_commentary_file_from_option(void)
{
    gchar buf[SMALL], commentary_file[SMALL];
    gchar *cur_locale = setlocale(LC_MESSAGES, NULL);

    if(strcmp(opt_str("string_opt_language_code"), "") == 0)
    {
	strncpy(buf, cur_locale, 2);
	buf[2] = '\0';
    }
    else if(strcmp(opt_str("string_opt_language_code"), "C") == 0)
	strcpy(buf, "en");
    else
    {
	strncpy(buf, opt_str("string_opt_language_code"), 2);
	buf[2] = '\0';
    }

    sprintf(commentary_file, "lg_commentary_%s.xml", buf);

    lg_commentary_load_commentary_file(commentary_file, FALSE);
}

/** Load the commentary file given. Abort if not found and abort=TRUE, 
    load default english file otherwise. */
void
lg_commentary_load_commentary_file(const gchar *commentary_file, gboolean abort)
{
    gchar *file_name = file_find_support_file(commentary_file, FALSE);
    gchar buf[SMALL];

    if(file_name != NULL)
    {
	xml_lg_commentary_read(file_name);
	g_free(file_name);	
    }
    else
    {
	if(abort)
	{
	    sprintf(buf, "lg_commentary_load_commentary_file: file %s not found \n", 
		    file_name);
	    main_exit_program(EXIT_FILE_NOT_FOUND, buf);
	}
	else
	    lg_commentary_load_commentary_file("lg_commentary_en.xml", TRUE);
    }
}
