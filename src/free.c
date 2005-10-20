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

#include "free.h"
#include "lg_commentary_struct.h"
#include "transfer.h"
#include "user.h"
#include "variables.h"

/**
   Free all memory allocated by the program.
   This mainly means we have to free a lot of strings
   and GArrays. */
void
free_memory(void)
{
    free_variables();
    free_names(FALSE);
    free_transfer_list();
    free_country(FALSE);
    free_users(FALSE);
    free_live_game(&live_game_temp);
    free_lg_commentary(FALSE);
    free_support_dirs();
}

/** Free the transfer list. */
void
free_transfer_list(void)
{
    gint i;

    for(i=0;i<transfer_list->len;i++)
	g_array_free(g_array_index(transfer_list, Transfer, i).offers, TRUE);
    
    free_g_array(&transfer_list);
}

/** Free the memory occupied by the season stats. */
void
free_season_stats(gboolean reset)
{
    gint i, j;

    if(season_stats == NULL)
    {
	if(reset)
	    season_stats = g_array_new(FALSE, FALSE, sizeof(SeasonStat));
	return;
    }

    for(i=0;i<season_stats->len;i++)
    {
	for(j=0;j<g_array_index(season_stats, SeasonStat, i).league_stats->len;j++)
	    free_league_stats(
		&g_array_index(g_array_index(season_stats, SeasonStat, i).league_stats,
			       LeagueStat, j));

	for(j=0;j<g_array_index(season_stats, SeasonStat, i).league_champs->len;j++)
	{
	    free_gchar_ptr(
		g_array_index(
		    g_array_index(season_stats, SeasonStat, i).league_champs, ChampStat, j).cl_name);
	    free_gchar_ptr(
		g_array_index(
		    g_array_index(season_stats, SeasonStat, i).league_champs, ChampStat, j).team_name);
	}

	for(j=0;j<g_array_index(season_stats, SeasonStat, i).cup_champs->len;j++)
	{
	    free_gchar_ptr(
		g_array_index(
		    g_array_index(season_stats, SeasonStat, i).cup_champs, ChampStat, j).cl_name);
	    free_gchar_ptr(
		g_array_index(
		    g_array_index(season_stats, SeasonStat, i).cup_champs, ChampStat, j).team_name);
	}

	g_array_free(g_array_index(season_stats, SeasonStat, i).cup_champs, TRUE);
	g_array_free(g_array_index(season_stats, SeasonStat, i).league_champs, TRUE);
    }

    free_g_array(&season_stats);

    if(reset)
	season_stats = g_array_new(FALSE, FALSE, sizeof(SeasonStat));
}

/** Free the users array. */
void
free_users(gboolean reset)
{
    gint i;

    if(users == NULL)
    {
	if(reset)
	    users = g_array_new(FALSE, FALSE, sizeof(User));
	return;
    }

    for(i=0;i<users->len;i++)
	free_user(&usr(i));

    free_g_array(&users);

    if(reset)
	users = g_array_new(FALSE, FALSE, sizeof(User));
}

/** Free the memory the user occupies.
    @param user The user we free. */
void
free_user(User *user)
{
    gint i;

    free_gchar_ptr(user->name);
    free_g_string(&user->sponsor.name);
    free_gchar_ptr(user->mmatches_file);
    free_live_game(&user->live_game);
    free_option_list(&user->options, FALSE);

    for(i=0;i<user->events->len;i++)
	free_event(&g_array_index(user->events, Event, i));
    free_g_array(&user->events);
    
    for(i=0;i<user->history->len;i++)
	free_gchar_ptr(g_array_index(user->history,
				     UserHistory, i).value_string);
    free_g_array(&user->history);

    free_mmatches(&user->mmatches, FALSE);

    free_player_array(&user->youth_academy.players);
}

/** Free the memorable matches array of the user and the memorable matches
    file name . */
void
free_mmatches(GArray **mmatches, gboolean reset)
{
    gint i;

    if(*mmatches == NULL)
    {
	if(reset)
	    *mmatches = g_array_new(FALSE, FALSE, sizeof(MemMatch));
	return;
    }

    for(i=0;i<(*mmatches)->len;i++)
    {
	free_g_string(&g_array_index(*mmatches, MemMatch, i).competition_name);
	free_gchar_ptr(g_array_index(*mmatches, MemMatch, i).country_name);
	free_live_game(&g_array_index(*mmatches, MemMatch, i).lg);
    }
    free_g_array(mmatches);

    if(reset)
	*mmatches = g_array_new(FALSE, FALSE, sizeof(MemMatch));
}

/** Free a user event. */
void
free_event(Event *event)
{
    free_gchar_ptr(event->value_string);
}

/** Free an array of options.
    @param array The array we free.
    @param reset Whether to create the array anew (empty).
    @see #Option */
void
free_option_list(OptionList *optionlist, gboolean reset)
{
    gint i;

    if(optionlist->list == NULL)
    {
	if(reset)
	{
	    optionlist->list = g_array_new(FALSE, FALSE, sizeof(Option));
	    g_datalist_init(&optionlist->datalist);
	}

	return;
    }
    
    for(i=0;i<optionlist->list->len;i++)
    {
	free_gchar_ptr(g_array_index(optionlist->list, Option, i).name);
	free_gchar_ptr(g_array_index(optionlist->list, Option, i).string_value);
    }

    free_g_array(&optionlist->list);

    if(reset)
    {
	optionlist->list = g_array_new(FALSE, FALSE, sizeof(Option));
	g_datalist_init(&optionlist->datalist);
    }
}

/** Free a live game variable. */
void
free_live_game(LiveGame *match)
{
    gint i, j, k;

    if(match->units == NULL || match->started_game == -1)
	return;

    for(i=0;i<match->units->len;i++)
	free_gchar_ptr(g_array_index(match->units, LiveGameUnit, i).event.commentary);

    for(i=0;i<2;i++)
    {
	free_gchar_ptr(match->team_names[i]);
	for(j=0;j<LIVE_GAME_STAT_ARRAY_END;j++)
	{
	    for(k=0;k<match->stats.players[i][j]->len;k++)
		g_free(g_ptr_array_index(match->stats.players[i][j], k));
	
	    free_g_ptr_array(&match->stats.players[i][j]);
	}
    }
    
    free_g_array(&match->units);
}


/**
   Free the main variable of the game, #country.
*/
void
free_country(gboolean reset)
{
    free_gchar_ptr(country.name);
    free_gchar_ptr(country.symbol);
    free_gchar_ptr(country.sid);

    free_leagues_array(&ligs, reset);
    free_cups_array(&cps, reset);
    free_g_ptr_array(&acps);

    if(reset)
	acps = g_ptr_array_new();
}

/**
   Free the memory occupied by a leagues array.
   @param leagues The pointer to the array we free.
*/
void
free_leagues_array(GArray **leagues, gboolean reset)
{
    gint i;

    if(*leagues == NULL)
    {
	if(reset)
	    *leagues = g_array_new(FALSE, FALSE, sizeof(League));
	return;
    }

    for(i=0;i<(*leagues)->len;i++)
	free_league(&g_array_index(*leagues, League, i));

    free_g_array(leagues);

    if(reset)
	*leagues = g_array_new(FALSE, FALSE, sizeof(League));
}

/**
   Free the memory occupied by a league.
   @param league The pointer to the league we free.
*/
void
free_league(League *league)
{
    free_gchar_ptr(league->name);
    free_gchar_ptr(league->short_name);
    free_gchar_ptr(league->symbol);
    free_gchar_ptr(league->sid);
    free_gchar_ptr(league->prom_rel.prom_games_dest_sid);
    free_gchar_ptr(league->prom_rel.prom_games_cup_sid);
 
    if(league->teams != NULL)
	free_teams_array(&league->teams, FALSE);

    free_g_array(&league->teams);
    free_g_array(&league->prom_rel.elements);;
    
    free_table(&league->table);

    free_g_array(&league->fixtures);

    free_league_stats(&league->stats);
}

/** Free the league stats. */
void
free_league_stats(LeagueStat *stats)
{
    gint i;

    for(i=0;i<stats->teams_off->len;i++)
	free_gchar_ptr(g_array_index(stats->teams_off, Stat, i).value_string);
    free_g_array(&stats->teams_off);

    for(i=0;i<stats->teams_def->len;i++)
	free_gchar_ptr(g_array_index(stats->teams_def, Stat, i).value_string);
    free_g_array(&stats->teams_def);

    for(i=0;i<stats->player_scorers->len;i++)
	free_gchar_ptr(g_array_index(stats->player_scorers, Stat, i).value_string);
    free_g_array(&stats->player_scorers);

    for(i=0;i<stats->player_goalies->len;i++)
	free_gchar_ptr(g_array_index(stats->player_goalies, Stat, i).value_string);
    free_g_array(&stats->player_goalies);
}


/** Free a table. */
void
free_table(Table *table)
{
    free_gchar_ptr(table->name);

    free_g_array(&table->elements);
}

/** Free the memory occupied by a teams array.
    @param teams The pointer to the array we free. */
void
free_teams_array(GArray **teams, gboolean reset)
{
    gint i;

    if(*teams == NULL)
    {
	if(reset)
	    *teams = g_array_new(FALSE, FALSE, sizeof(Team));
	return;
    }

    for(i=0;i<(*teams)->len;i++)
	free_team(&g_array_index(*teams, Team, i));

    free_g_array(teams);

    if(reset)
	*teams = g_array_new(FALSE, FALSE, sizeof(Team));
}

/**
   Free the memory occupied by a team.
   @param tm The pointer to the team we free.
*/
void
free_team(Team *tm)
{
    free_gchar_ptr(tm->stadium.name);
    free_gchar_ptr(tm->name);
    free_gchar_ptr(tm->names_file);
    free_gchar_ptr(tm->symbol);
    free_gchar_ptr(tm->def_file);

    free_player_array(&tm->players);
}

/** Free an array containing players. */
void
free_player_array(GArray **players)
{
    gint i;

    if(*players != NULL)
    {
	for(i=0;i<(*players)->len;i++)
	    free_player(&g_array_index((*players), Player, i));

	free_g_array(players);
    }
}

/** Free the memory occupied by a player.
    @param pl The pointer to the player we free. */
void
free_player(Player *pl)
{
    free_gchar_ptr(pl->name);

    free_g_array(&pl->cards);
    free_g_array(&pl->games_goals);
}

/**
   Free the memory occupied by a cups array.
   @param cups The pointer to the array we free.
*/
void
free_cups_array(GArray **cups, gboolean reset)
{
    gint i;

    if(*cups == NULL)
    {
	if(reset)
	    *cups = g_array_new(FALSE, FALSE, sizeof(Cup));
	return;
    }

    for(i=0;i<(*cups)->len;i++)
	free_cup(&g_array_index(*cups, Cup, i));

    free_g_array(cups);

    if(reset)
	*cups = g_array_new(FALSE, FALSE, sizeof(Cup));
}

/**
   Free the memory occupied by a cup.
   @param cup The pointer to the cup we free.
*/
void
free_cup(Cup *cup)
{
    gint i, j;

    free_gchar_ptr(cup->name);
    free_gchar_ptr(cup->short_name);
    free_gchar_ptr(cup->symbol);
    free_gchar_ptr(cup->sid);

    for(i=0;i<cup->rounds->len;i++)
	if(g_array_index(cup->rounds, CupRound, i).round_robin_number_of_groups > 0)
	{
	    for(j=0;j<g_array_index(cup->rounds, CupRound, i).tables->len;j++)
		free_table(&g_array_index(g_array_index(cup->rounds, CupRound, i).tables, Table, j));

	    for(j=0;j<g_array_index(cup->rounds, CupRound, i).choose_teams->len;j++)
		free_cup_choose_team(
		    &g_array_index(g_array_index(cup->rounds, CupRound, i).choose_teams, CupChooseTeam, j));
	    
	    free_g_array(&g_array_index(cup->rounds, CupRound, i).choose_teams);
	    free_teams_array(&g_array_index(cup->rounds, CupRound, i).teams, FALSE);
	    g_ptr_array_free(g_array_index(cup->rounds, CupRound, i).team_ptrs, TRUE);
	}

    free_g_array(&cup->rounds);
    free_g_array(&cup->fixtures);

    free_g_ptr_array(&cup->bye);
    free_gchar_array(&cup->team_names);
    free_g_ptr_array(&cup->teams);
    free_gchar_array(&cup->properties);
}

/**
   Free the memory occupied by a CupChooseTeam.
   @param cup_choose_team The pointer to the team we free.
*/
void
free_cup_choose_team(CupChooseTeam *cup_choose_team)
{
    free_gchar_ptr(cup_choose_team->sid);
}

/** Free some global variables (except for the country variable). */
void
free_variables(void)
{
    free_option_list(&options, FALSE);
    free_option_list(&settings, FALSE);
    free_option_list(&constants, FALSE);
    free_option_list(&constants_app, FALSE);
    free_option_list(&lg_tokens, FALSE);

    free_gchar_ptr(save_file);

    g_rand_free(rand_generator);

    free_season_stats(FALSE);
}

/** Free the list with live game commentary text. */
void
free_lg_commentary(gboolean reset)
{
    gint i, j;

    for(i=0;i<LIVE_GAME_EVENT_END;i++)
	if(lg_commentary[i] != NULL)
	{
	    for(j=0;j<lg_commentary[i]->len;j++)
	    {
		free_gchar_ptr(g_array_index(lg_commentary[i], LGCommentary, j).text);
		free_gchar_ptr(g_array_index(lg_commentary[i], LGCommentary, j).condition);
	    }

	    free_g_array(&lg_commentary[i]);
	}

    if(reset)
	for(i=0;i<LIVE_GAME_EVENT_END;i++)
	    lg_commentary[i] = g_array_new(FALSE, FALSE, sizeof(LGCommentary));
}

/**
   Free a GPtrArray containing strings.
   @param array The array to be freed.
*/
void
free_gchar_array(GPtrArray **array)
{
    gint i;

    if(*array == NULL)
	return;

    for(i=0;i<(*array)->len;i++)
	free_gchar_ptr(g_ptr_array_index(*array, i));

    free_g_ptr_array(array);
    *array = NULL;
}

/** Free a GString if it isn't NULL.
    @param string The string to be freed. */
void
free_g_string(GString **string)
{
    if(*string == NULL)
	return;

    g_string_free(*string, TRUE);

    *string = NULL;
}

/** Free a GArray if it isn't NULL.
    @param array The array to be freed. */
void
free_g_array(GArray **array)
{
    if(*array == NULL)
	return;

    g_array_free(*array, TRUE);

    *array = NULL;
}

/** Free a GPtrArray if it isn't NULL.
    @param array The array to be freed. */
void
free_g_ptr_array(GPtrArray **array)
{
    if(*array == NULL)
	return;

    g_ptr_array_free(*array, TRUE);

    *array = NULL;
}

/** Free the glist containing the support directories. */
void
free_support_dirs(void)
{
  GList *elem = support_directories;

  if(elem == NULL)
      return;

  while(elem)
  {
      free_gchar_ptr(elem->data);
      elem = elem->next;
  }

  g_list_free(support_directories);
  support_directories = NULL;
}

/** Free a list of names. */
void
free_name_list(NameList *namelist, gboolean reset)
{
    gint i;

    if(namelist->sid == NULL)
    {
	if(reset)
	{
	    namelist->sid = NULL;
	    namelist->first_names = g_ptr_array_new();
	    namelist->last_names = g_ptr_array_new();
	}

	return;
    }

    free_gchar_ptr(namelist->sid);

    for(i=0;i<namelist->first_names->len;i++)
	free_gchar_ptr(g_ptr_array_index(namelist->first_names, i));
    free_g_ptr_array(&namelist->first_names);

    for(i=0;i<namelist->last_names->len;i++)
	free_gchar_ptr(g_ptr_array_index(namelist->last_names, i));
    free_g_ptr_array(&namelist->last_names);

    if(reset)
    {
	namelist->sid = NULL;
	namelist->first_names = g_ptr_array_new();
	namelist->last_names = g_ptr_array_new();
    }
}

/** Free the array with the name lists. */
void
free_names(gboolean reset)
{
    gint i;

    if(name_lists == NULL)
    {
	if(reset)
	    name_lists = g_array_new(FALSE, FALSE, sizeof(NameList));

	return;
    }

    for(i=0;i<name_lists->len;i++)
	free_name_list(&g_array_index(name_lists, NameList, i), FALSE);

    free_g_array(&name_lists);

    if(reset)
	name_lists = g_array_new(FALSE, FALSE, sizeof(NameList));
}
