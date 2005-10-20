/*
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

#include "misc.h"
#include "player.h"
#include "team.h"
#include "xml.h"
#include "xml_loadsave_players.h"

enum
{
    TAG_PLAYERS = TAG_START_PLAYERS,
    TAG_PLAYER,
    TAG_PLAYER_NAME,
    TAG_PLAYER_POS,
    TAG_PLAYER_CPOS,
    TAG_PLAYER_HEALTH,
    TAG_PLAYER_RECOVERY,
    TAG_PLAYER_ID,
    TAG_PLAYER_VALUE,
    TAG_PLAYER_WAGE,
    TAG_PLAYER_OFFERS,
    TAG_PLAYER_SKILL,
    TAG_PLAYER_CSKILL,
    TAG_PLAYER_TALENT,
    TAG_PLAYER_ETAL,
    TAG_PLAYER_FITNESS,
    TAG_PLAYER_LSU,
    TAG_PLAYER_AGE,
    TAG_PLAYER_PEAK_AGE,
    TAG_PLAYER_PEAK_REGION,
    TAG_PLAYER_CONTRACT,
    TAG_PLAYER_PARTICIPATION,
    TAG_PLAYER_GAMES_GOAL,
    TAG_PLAYER_GAMES_GOAL_CLID,
    TAG_PLAYER_GAMES_GOAL_GAMES,
    TAG_PLAYER_GAMES_GOAL_GOALS,
    TAG_PLAYER_GAMES_GOAL_SHOTS,
    TAG_PLAYER_CARD,
    TAG_PLAYER_CARD_CLID,
    TAG_PLAYER_CARD_YELLOW,
    TAG_PLAYER_CARD_RED,
    TAG_PLAYER_CAREER,
    TAG_PLAYER_STREAK,
    TAG_PLAYER_STREAK_COUNT,
    TAG_PLAYER_STREAK_PROB,
    TAG_END
};

gint state, etalidx, careeridx;
Player new_player;
PlayerGamesGoals new_games_goals;
PlayerCard new_card;

void
xml_loadsave_players_start_element(gint tag, Team *tm)
{
    gint i;
    gboolean valid_tag = FALSE;

    for(i=TAG_PLAYERS;i<TAG_END;i++)
	if(tag == i)
	{
	    state = i;
	    valid_tag = TRUE;
	}

    if(tag == TAG_PLAYER)
    {
	new_player = player_new(tm, 80, FALSE);
	etalidx = careeridx = 0;
    }

    if(!valid_tag)
	g_warning("xml_loadsave_players_start_element: unknown tag. I'm in state %d\n",
		  state);
}

void
xml_loadsave_players_end_element(gint tag, GArray *players)
{
    if(tag == TAG_PLAYER)
	g_array_append_val(players, new_player);
    else if(tag == TAG_PLAYER_NAME ||
	    tag == TAG_PLAYER_POS ||
	    tag == TAG_PLAYER_CPOS ||
	    tag == TAG_PLAYER_HEALTH ||
	    tag == TAG_PLAYER_RECOVERY ||
	    tag == TAG_PLAYER_ID ||
	    tag == TAG_PLAYER_VALUE ||
	    tag == TAG_PLAYER_WAGE ||
	    tag == TAG_PLAYER_OFFERS ||
	    tag == TAG_PLAYER_SKILL ||
	    tag == TAG_PLAYER_CSKILL ||
	    tag == TAG_PLAYER_TALENT ||
	    tag == TAG_PLAYER_ETAL ||
	    tag == TAG_PLAYER_FITNESS ||
	    tag == TAG_PLAYER_LSU ||
	    tag == TAG_PLAYER_AGE ||
	    tag == TAG_PLAYER_PEAK_AGE ||
	    tag == TAG_PLAYER_PEAK_REGION ||
	    tag == TAG_PLAYER_CONTRACT ||
	    tag == TAG_PLAYER_PARTICIPATION ||
	    tag == TAG_PLAYER_GAMES_GOAL ||
	    tag == TAG_PLAYER_CAREER ||
	    tag == TAG_PLAYER_CARD ||
	    tag == TAG_PLAYER_STREAK ||
	    tag == TAG_PLAYER_STREAK_COUNT ||
	    tag == TAG_PLAYER_STREAK_PROB)
    {
	state = TAG_PLAYER;
	if(tag == TAG_PLAYER_ETAL)
	    etalidx++;
	else if(tag == TAG_PLAYER_CAREER)
	    careeridx++;
	else if(tag == TAG_PLAYER_CARD)
	    g_array_append_val(new_player.cards, new_card);
	else if(tag == TAG_PLAYER_GAMES_GOAL)
	    g_array_append_val(new_player.games_goals, new_games_goals);
    }
    else if(tag == TAG_PLAYER_GAMES_GOAL_CLID ||
	    tag == TAG_PLAYER_GAMES_GOAL_GAMES ||
	    tag == TAG_PLAYER_GAMES_GOAL_GOALS ||
	    tag == TAG_PLAYER_GAMES_GOAL_SHOTS)
	state = TAG_PLAYER_GAMES_GOAL;
    else if(tag == TAG_PLAYER_CARD_CLID ||
	    tag == TAG_PLAYER_CARD_YELLOW ||
	    tag == TAG_PLAYER_CARD_RED)
	state = TAG_PLAYER_CARD;
    else if(tag != TAG_PLAYERS)
	g_warning("xml_loadsave_players_end_element: unknown tag. I'm in state %d\n",
		  state);
}

void
xml_loadsave_players_text(gchar *text)
{
    gint int_value = -1;
    gfloat float_value = -1;

    int_value = (gint)g_ascii_strtod(text, NULL);
    float_value = (gfloat)g_ascii_strtod(text, NULL) / 10000;

    if(state == TAG_PLAYER_NAME)
	misc_string_assign(&new_player.name, text);
    else if(state == TAG_PLAYER_POS)
	new_player.pos = int_value;
    else if(state == TAG_PLAYER_CPOS)
	new_player.cpos = int_value;
    else if(state == TAG_PLAYER_HEALTH)
	new_player.health = int_value;
    else if(state == TAG_PLAYER_RECOVERY)
	new_player.recovery = int_value;
    else if(state == TAG_PLAYER_ID)
	new_player.id = int_value;
    else if(state == TAG_PLAYER_VALUE)
	new_player.value = int_value;
    else if(state == TAG_PLAYER_WAGE)
	new_player.wage = int_value;
    else if(state == TAG_PLAYER_OFFERS)
	new_player.offers = int_value;
    else if(state == TAG_PLAYER_PARTICIPATION)
	new_player.participation = int_value;
    else if(state == TAG_PLAYER_SKILL)
	new_player.skill = float_value;
    else if(state == TAG_PLAYER_CSKILL)
	new_player.cskill = float_value;
    else if(state == TAG_PLAYER_TALENT)
	new_player.talent = float_value;
    else if(state == TAG_PLAYER_FITNESS)
	new_player.fitness = float_value;
    else if(state == TAG_PLAYER_LSU)
	new_player.lsu = float_value;
    else if(state == TAG_PLAYER_AGE)
	new_player.age = float_value;
    else if(state == TAG_PLAYER_PEAK_AGE)
	new_player.peak_age = float_value;
    else if(state == TAG_PLAYER_PEAK_REGION)
	new_player.peak_region = float_value;
    else if(state == TAG_PLAYER_CONTRACT)
	new_player.contract = float_value;
    else if(state == TAG_PLAYER_ETAL)
	new_player.etal[etalidx] = float_value;
    else if(state == TAG_PLAYER_GAMES_GOAL_CLID)
	new_games_goals.clid = int_value;
    else if(state == TAG_PLAYER_GAMES_GOAL_GAMES)
	new_games_goals.games = int_value;
    else if(state == TAG_PLAYER_GAMES_GOAL_GOALS)
	new_games_goals.goals = int_value;
    else if(state == TAG_PLAYER_GAMES_GOAL_SHOTS)
	new_games_goals.shots = int_value;
    else if(state == TAG_PLAYER_CARD_CLID)
	new_card.clid = int_value;
    else if(state == TAG_PLAYER_CARD_YELLOW)
	new_card.yellow = int_value;
    else if(state == TAG_PLAYER_CARD_RED)
	new_card.red = int_value;
    else if(state == TAG_PLAYER_CAREER)
	new_player.career[careeridx] = int_value;
    else if(state == TAG_PLAYER_STREAK)
	new_player.streak = int_value;
    else if(state == TAG_PLAYER_STREAK_COUNT)
	new_player.streak_count = float_value;
    else if(state == TAG_PLAYER_STREAK_PROB)
	new_player.streak_prob = float_value;
}

void
xml_loadsave_players_write(FILE *fil, const GArray *players)
{
    gint i;

    fprintf(fil, "%s<_%d>\n", I1, TAG_PLAYERS);
   
    for(i=0;i<players->len;i++)
	xml_loadsave_players_write_player(fil, &g_array_index(players, Player, i));

    fprintf(fil, "%s</_%d>\n", I1, TAG_PLAYERS);
}

void
xml_loadsave_players_write_player(FILE *fil, const Player *pl)
{
    gint i;

    fprintf(fil, "%s<_%d>\n", I1, TAG_PLAYER);

    xml_write_string(fil, pl->name, TAG_PLAYER_NAME, I2);
    xml_write_int(fil, pl->pos, TAG_PLAYER_POS, I2);
    xml_write_int(fil, pl->cpos, TAG_PLAYER_CPOS, I2);
    xml_write_int(fil, pl->health, TAG_PLAYER_HEALTH, I2);
    xml_write_int(fil, pl->recovery, TAG_PLAYER_RECOVERY, I2);
    xml_write_int(fil, pl->id, TAG_PLAYER_ID, I2);
    xml_write_int(fil, pl->value, TAG_PLAYER_VALUE, I2);
    xml_write_int(fil, pl->wage, TAG_PLAYER_WAGE, I2);
    xml_write_int(fil, pl->offers, TAG_PLAYER_OFFERS, I2);
    xml_write_int(fil, pl->participation, TAG_PLAYER_PARTICIPATION, I2);

    xml_write_float(fil, pl->skill, TAG_PLAYER_SKILL, I2);
    xml_write_float(fil, pl->cskill, TAG_PLAYER_CSKILL, I2);
    xml_write_float(fil, pl->talent, TAG_PLAYER_TALENT, I2);
    xml_write_float(fil, pl->fitness, TAG_PLAYER_FITNESS, I2);
    xml_write_float(fil, pl->lsu, TAG_PLAYER_LSU, I2);
    xml_write_float(fil, pl->age, TAG_PLAYER_AGE, I2);
    xml_write_float(fil, pl->peak_age, TAG_PLAYER_PEAK_AGE, I2);
    xml_write_float(fil, pl->peak_region, TAG_PLAYER_PEAK_REGION, I2);
    xml_write_float(fil, pl->contract, TAG_PLAYER_CONTRACT, I2);

    for(i=0;i<4;i++)
	xml_write_float(fil, pl->etal[i], TAG_PLAYER_ETAL, I3);
    
    for(i=0;i<PLAYER_VALUE_END;i++)
	xml_write_int(fil, pl->career[i], TAG_PLAYER_CAREER, I2);

    xml_write_int(fil, pl->streak, TAG_PLAYER_STREAK, I2);
    xml_write_float(fil, pl->streak_count, TAG_PLAYER_STREAK_COUNT, I2);
    xml_write_float(fil, pl->streak_prob, TAG_PLAYER_STREAK_PROB, I2);

    for(i=0;i<pl->games_goals->len;i++)
    {
	fprintf(fil, "%s<_%d>\n", I2, TAG_PLAYER_GAMES_GOAL);

	xml_write_int(fil, g_array_index(pl->games_goals, PlayerGamesGoals, i).clid,
		      TAG_PLAYER_GAMES_GOAL_CLID, I3);
	xml_write_int(fil, g_array_index(pl->games_goals, PlayerGamesGoals, i).games,
		      TAG_PLAYER_GAMES_GOAL_GAMES, I3);
	xml_write_int(fil, g_array_index(pl->games_goals, PlayerGamesGoals, i).goals,
		      TAG_PLAYER_GAMES_GOAL_GOALS, I3);
	xml_write_int(fil, g_array_index(pl->games_goals, PlayerGamesGoals, i).shots,
		      TAG_PLAYER_GAMES_GOAL_SHOTS, I3);

	fprintf(fil, "%s</_%d>\n", I2, TAG_PLAYER_GAMES_GOAL);
    }

    for(i=0;i<pl->cards->len;i++)
    {
	fprintf(fil, "%s<_%d>\n", I2, TAG_PLAYER_CARD);

	xml_write_int(fil, g_array_index(pl->cards, PlayerCard, i).clid,
		      TAG_PLAYER_CARD_CLID, I3);
	xml_write_int(fil, g_array_index(pl->cards, PlayerCard, i).yellow,
		      TAG_PLAYER_CARD_YELLOW, I3);
	xml_write_int(fil, g_array_index(pl->cards, PlayerCard, i).red,
		      TAG_PLAYER_CARD_RED, I3);
	
	fprintf(fil, "%s</_%d>\n", I2, TAG_PLAYER_CARD);
    }    

    fprintf(fil, "%s</_%d>\n", I1, TAG_PLAYER);
}
