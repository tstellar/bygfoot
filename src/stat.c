/*
   stat.c

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
#include "free.h"
#include "league.h"
#include "option.h"
#include "player.h"
#include "stat.h"
#include "table_struct.h"
#include "team.h"
#include "variables.h"

/** Return a newly allocated league stat with specified
    league name and symbol. */
LeagueStat
stat_league_new(const gchar *league_name, const gchar *league_symbol)
{
    LeagueStat new;
    
    new.league_name = g_strdup(league_name);    
    new.league_symbol = g_strdup(league_symbol);
    new.teams_off = g_array_new(FALSE, FALSE, sizeof(Stat));
    new.teams_def = g_array_new(FALSE, FALSE, sizeof(Stat));
    new.player_scorers = g_array_new(FALSE, FALSE, sizeof(Stat));
    new.player_goalies = g_array_new(FALSE, FALSE, sizeof(Stat));

    return new;
}

/** Update the league stats. */
void
stat_update_leagues(void)
{
    gint i;

    for(i=0;i<ligs->len;i++)
	if(lig(i).active && 
	   g_array_index(
	       lig(i).fixtures, Fixture, lig(i).fixtures->len - 1).week_number >= week)
	{
	    free_league_stats(&lig(i).stats);
	    lig(i).stats.league_name = g_strdup(lig(i).name);
	    lig(i).stats.league_symbol = g_strdup(lig(i).symbol);
	    lig(i).stats.teams_off = 
		stat_update_league_teams(lig(i).teams, TEAM_COMPARE_OFFENSIVE);
	    lig(i).stats.teams_def = 
		stat_update_league_teams(lig(i).teams, TEAM_COMPARE_DEFENSE);
	    stat_update_league_players(&lig(i));
	}
}

/** Update the league player stats. */
void
stat_update_league_players(League *league)
{
    gint i, j;
    GPtrArray *players_sorted[2] = {g_ptr_array_new(),
				    g_ptr_array_new()};
    GArray *players[2] = {g_array_new(FALSE, FALSE, sizeof(Stat)),
			  g_array_new(FALSE, FALSE, sizeof(Stat))};
    Player *pl = NULL;
    gint maxlen = const_int("int_stat_players_len");
    Stat new_stat;
        
    for(i=0;i<league->teams->len;i++)
	for(j=0;j<g_array_index(league->teams, Team, i).players->len;j++)
	{
	    pl = &g_array_index(g_array_index(league->teams, Team, i).players, Player, j);
	    if(pl->pos != PLAYER_POS_GOALIE)
		g_ptr_array_add(players_sorted[0], pl);
	    else if(player_games_goals_get(pl, pl->team->clid, PLAYER_VALUE_GAMES) >=
		    const_float("float_stat_goalie_percentage") * 
		    (gfloat)team_get_table_value(pl->team, TABLE_PLAYED))
		g_ptr_array_add(players_sorted[1], pl);
	}

    g_ptr_array_sort_with_data(players_sorted[0], player_compare_func, 
			       GINT_TO_POINTER(PLAYER_COMPARE_ATTRIBUTE_LEAGUE_GOALS));
    g_ptr_array_sort_with_data(players_sorted[1], player_compare_func, 
			       GINT_TO_POINTER(-PLAYER_COMPARE_ATTRIBUTE_LEAGUE_GOALS));

    maxlen = MIN(maxlen, players_sorted[0]->len);
    maxlen = MIN(maxlen, players_sorted[1]->len);

    for(i=0;i<2;i++)
	for(j=0;j<maxlen;j++)
	{
	    pl = (Player*)g_ptr_array_index(players_sorted[i], j);
	    new_stat.team_name = g_strdup(pl->team->name);
	    new_stat.value_string = g_strdup(pl->name);
	    new_stat.value1 = 
		player_games_goals_get(pl, pl->team->clid, PLAYER_VALUE_GOALS);
	    new_stat.value2 = 
		player_games_goals_get(pl, pl->team->clid, PLAYER_VALUE_GAMES);
	    new_stat.value3 = 
		player_games_goals_get(pl, pl->team->clid, PLAYER_VALUE_SHOTS);

	    g_array_append_val(players[i], new_stat);
	}

    league->stats.player_scorers = players[0];
    league->stats.player_goalies = players[1];
}

/** Update the stats of the league. */
GArray*
stat_update_league_teams(const GArray *teams_array, gint compare_type)
{
    gint i;
    GArray *stats = g_array_new(FALSE, FALSE, sizeof(Stat));
    GPtrArray *teams = g_ptr_array_new();
    Stat new_stat;
    gint maxlen = const_int("int_stat_teams_len");
    
    maxlen = MIN(maxlen, teams_array->len);

    for(i=0;i<teams_array->len;i++)
	g_ptr_array_add(teams, &g_array_index(teams_array, Team, i));

    g_ptr_array_sort_with_data(teams, team_compare_func, GINT_TO_POINTER(compare_type));

    for(i=0;i<maxlen;i++)
    {
	new_stat.team_name = g_strdup(((Team*)g_ptr_array_index(teams, i))->name);
	new_stat.value1 = 
	    team_get_table_value((Team*)g_ptr_array_index(teams, i), TABLE_GF);
	new_stat.value2 = 
	    team_get_table_value((Team*)g_ptr_array_index(teams, i), TABLE_GA);
	new_stat.value3 = -1;
	new_stat.value_string = NULL;

	g_array_append_val(stats, new_stat);
    }

    g_ptr_array_free(teams, TRUE);

    return stats;
}

/** Create a new empty season stat with specified season number. */
SeasonStat
stat_season_stat_new(gint season_number)
{
    SeasonStat new;

    new.season_number = season_number;
    new.league_champs = g_array_new(FALSE, FALSE, sizeof(ChampStat));
    new.cup_champs = g_array_new(FALSE, FALSE, sizeof(ChampStat));
    new.league_stats = g_array_new(FALSE, FALSE, sizeof(LeagueStat));

    return new;
}

/** Create a seasonstat struct at the end of a season. */
void
stat_create_season_stat(void)
{
    gint i;
    SeasonStat new = stat_season_stat_new(season);
    ChampStat new_champ;

    for(i=0;i<ligs->len;i++)
    {
	new_champ.cl_name = g_strdup(lig(i).name);
	new_champ.team_name = 
	    g_strdup(g_array_index(lig(i).table.elements, TableElement, 0).team->name);
	g_array_append_val(new.league_champs, new_champ);

	g_array_append_val(new.league_stats, lig(i).stats);
	lig(i).stats = stat_league_new(lig(i).name, lig(i).symbol);
    }

    for(i=0;i<acps->len;i++)
    {
	new_champ.cl_name = g_strdup(acp(i)->name);
	new_champ.team_name = 
	    g_strdup(cup_get_winner(acp(i))->name);
	g_array_append_val(new.cup_champs, new_champ);
    }

    g_array_append_val(season_stats, new);
}

/** Function printing goals stats from the fixture array.
    Used for fine-tuning the game. */
void
stat_show_av_goals(GArray *fixtures)
{
    gint i;
    gfloat games = 0,
	allgoals = 0,
	homegoals = 0,
	awaygoals = 0,
	goaldiff = 0,
	homewon = 0,
	homeloss = 0,
	draw = 0;

    for(i=0;i<fixtures->len;i++)
	if(g_array_index(fixtures, Fixture, i).attendance != -1)
	{
	    games++;
	    allgoals += (g_array_index(fixtures, Fixture, i).result[0][0] +
			 g_array_index(fixtures, Fixture, i).result[1][0]);
	    homegoals += g_array_index(fixtures, Fixture, i).result[0][0];
	    awaygoals += g_array_index(fixtures, Fixture, i).result[1][0];
	    goaldiff += ABS(g_array_index(fixtures, Fixture, i).result[0][0] -
			    g_array_index(fixtures, Fixture, i).result[1][0]);

	    if(g_array_index(fixtures, Fixture, i).result[0][0] >
	       g_array_index(fixtures, Fixture, i).result[1][0])
		homewon++;
	    else if(g_array_index(fixtures, Fixture, i).result[0][0] <
		    g_array_index(fixtures, Fixture, i).result[1][0])
		homeloss++;
	    else
		draw++;
	}

    g_print("------------------------------------ \n");
    g_print("%s\nGoals/G HomeG/G AwayG/G Away %% GD/G \n",
	   league_cup_get_name_string(g_array_index(fixtures, Fixture, 0).clid));
    g_print("%.2f \t %.2f \t %.2f \t %.2f \t %.2f \n", allgoals/games, homegoals/games, awaygoals/games,
	   awaygoals / allgoals, goaldiff/games);
    g_print("Home win %%: %.2f Loss %%: %.2f Draw %%: %.2f \n",
	   homewon / games, homeloss / games, draw / games);
}

/** Show league goals stats (debug). */
void
stat_show_av_league_goals(void)
{
    gint i;

    g_print("\n\n");
    for(i=0;i<ligs->len;i++)
	stat_show_av_goals(league_cup_get_fixtures(lig(i).id));
}
