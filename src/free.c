/*
   free.c

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
#include "free.h"
#include "lg_commentary_struct.h"
#include "news_struct.h"
#include "strategy_struct.h"
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
#ifdef DEBUG
    printf("free_memory\n");
#endif

    free_variables();
    free_names(FALSE);
    free_transfer_list();
    free_strategies();
    free_country(&country, FALSE);
    free_users(FALSE);
    free_bets(FALSE);
    free_lg_commentary(FALSE);
    free_news(FALSE);
    free_newspaper(FALSE);
    free_support_dirs();
    free_jobs(FALSE);

    free_g_array(&live_games);
}

/** Free the transfer list. */
void
free_transfer_list(void)
{
#ifdef DEBUG
    printf("free_transfer_list\n");
#endif

    gint i;

    for(i=0;i<transfer_list->len;i++)
	g_array_free(g_array_index(transfer_list, Transfer, i).offers, TRUE);
    
    free_g_array(&transfer_list);
}

/** Free the memory occupied by the season stats. */
void
free_season_stats(gboolean reset)
{
#ifdef DEBUG
    printf("free_season_stats\n");
#endif

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
		    g_array_index(season_stats, SeasonStat, i).league_champs,
		    ChampStat, j).cl_name);
	    free_gchar_ptr(
		g_array_index(
		    g_array_index(season_stats, SeasonStat, i).league_champs, 
		    ChampStat, j).team_name);
	}

	for(j=0;j<g_array_index(season_stats, SeasonStat, i).cup_champs->len;j++)
	{
	    free_gchar_ptr(
		g_array_index(
		    g_array_index(season_stats, SeasonStat, i).cup_champs, 
		    ChampStat, j).cl_name);
	    free_gchar_ptr(
		g_array_index(
		    g_array_index(season_stats, SeasonStat, i).cup_champs, 
		    ChampStat, j).team_name);
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
#ifdef DEBUG
    printf("free_users\n");
#endif

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
#ifdef DEBUG
    printf("free_user\n");
#endif

    gint i, j;

    free_gchar_ptr(user->name);
    free_g_string(&user->sponsor.name);
    free_gchar_ptr(user->mmatches_file);
    free_live_game(&user->live_game);
    free_option_list(&user->options, FALSE);

    for(i=0;i<user->events->len;i++)
	free_event(&g_array_index(user->events, Event, i));
    free_g_array(&user->events);
    
    for(i=0;i<user->history->len;i++)
    {
	free_gchar_ptr(g_array_index(user->history,
				     UserHistory, i).team_name);
	for(j=0;j<3;j++)
	    free_gchar_ptr(g_array_index(user->history,
					 UserHistory, i).string[j]);
    }

    free_g_array(&user->history);

    free_mmatches(&user->mmatches, FALSE);

    free_player_array(&user->youth_academy.players);

    free_g_array(&user->bets[0]);
    free_g_array(&user->bets[1]);
    g_array_free(user->default_team, TRUE);
}

/** Free the memorable matches array of the user and the memorable matches
    file name . */
void
free_mmatches(GArray **mmatches, gboolean reset)
{
#ifdef DEBUG
    printf("free_mmatches\n");
#endif

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
#ifdef DEBUG
    printf("free_event\n");
#endif

    free_gchar_ptr(event->value_string);
}

/** Free an array of options.
    @param array The array we free.
    @param reset Whether to create the array anew (empty).
    @see #Option */
void
free_option_list(OptionList *optionlist, gboolean reset)
{
#ifdef DEBUG
    printf("free_option_list\n");
#endif

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
#ifdef DEBUG
    printf("free_live_game\n");
#endif

    gint i, j, k;

    if(match->units == NULL || match->started_game == -1)
	return;

    for(i=0;i<match->units->len;i++)
	free_gchar_ptr(g_array_index(match->units, LiveGameUnit, i).event.commentary);

    for(i=0;i<2;i++)
    {
	free_g_array(&match->action_ids[i]);
	
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
   Free a country variable.
*/
void
free_country(Country *cntry, gboolean reset)
{
#ifdef DEBUG
    printf("free_country\n");
#endif

    free_gchar_ptr(cntry->name);
    free_gchar_ptr(cntry->symbol);
    free_gchar_ptr(cntry->sid);

    free_leagues_array(&cntry->leagues, reset);
    free_cups_array(&cntry->cups, reset);
    free_g_ptr_array(&cntry->allcups);

    if(reset)
    {
	cntry->allcups = g_ptr_array_new();
	cntry->sid = g_strdup("NONAME");
    }
}

/**
   Free the memory occupied by a leagues array.
   @param leagues The pointer to the array we free.
*/
void
free_leagues_array(GArray **leagues, gboolean reset)
{
#ifdef DEBUG
    printf("free_leagues_array\n");
#endif

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
#ifdef DEBUG
    printf("free_league\n");
#endif

    free_gchar_ptr(league->name);
    free_gchar_ptr(league->short_name);
    free_gchar_ptr(league->symbol);
    free_gchar_ptr(league->sid);
 
    free_gchar_array(&league->skip_weeks_with);

    if(league->teams != NULL)
	free_teams_array(&league->teams, FALSE);

    free_joined_leagues(&league->joined_leagues);
    free_prom_rel(&league->prom_rel);

    free_g_array(&league->teams);
    
    free_tables(&league->tables);
    free_new_tables(&league->new_tables);

    free_g_array(&league->fixtures);
    free_g_array(&league->rr_breaks);
    free_g_array(&league->week_breaks);

    free_g_array(&league->two_match_weeks[0]);
    free_g_array(&league->two_match_weeks[1]);

    free_league_stats(&league->stats);
}

/** Free the promotion/relegation struct of a league. */
void
free_prom_rel(PromRel *prom_rel)
{
    gint i;

    for(i = 0; i < prom_rel->elements->len; i++)
        g_free(g_array_index(prom_rel->elements, PromRelElement, i).dest_sid);

    free_g_array(&prom_rel->elements);

    for(i = 0; i < prom_rel->prom_games->len; i++)
    {
        g_free(g_array_index(prom_rel->prom_games, PromGames, i).dest_sid);
        g_free(g_array_index(prom_rel->prom_games, PromGames, i).loser_sid);
        g_free(g_array_index(prom_rel->prom_games, PromGames, i).cup_sid);
    }

    free_g_array(&prom_rel->prom_games);
}

/** Free the data in the joined leagues array. */
void
free_joined_leagues(GArray **joined_leagues)
{
#ifdef DEBUG
    printf("free_joined_leagues\n");
#endif

    gint i;

    for(i = 0; i < (*joined_leagues)->len; i++)
        free_gchar_ptr(g_array_index(*joined_leagues, JoinedLeague, i).sid);

    free_g_array(joined_leagues);
}

/** Free the league stats. */
void
free_league_stats(LeagueStat *stats)
{
#ifdef DEBUG
    printf("free_league_stats\n");
#endif

    gint i;

    free_gchar_ptr(stats->league_name);
    free_gchar_ptr(stats->league_symbol);

    for(i=0;i<stats->teams_off->len;i++)
    {
	free_gchar_ptr(g_array_index(stats->teams_off, Stat, i).team_name);
	free_gchar_ptr(g_array_index(stats->teams_off, Stat, i).value_string);
    }
    free_g_array(&stats->teams_off);

    for(i=0;i<stats->teams_def->len;i++)
    {
	free_gchar_ptr(g_array_index(stats->teams_def, Stat, i).team_name);
	free_gchar_ptr(g_array_index(stats->teams_def, Stat, i).value_string);
    }
    free_g_array(&stats->teams_def);

    for(i=0;i<stats->player_scorers->len;i++)
	free_gchar_ptr(g_array_index(stats->player_scorers, Stat, i).value_string);
    free_g_array(&stats->player_scorers);

    for(i=0;i<stats->player_goalies->len;i++)
	free_gchar_ptr(g_array_index(stats->player_goalies, Stat, i).value_string);
    free_g_array(&stats->player_goalies);
}


/** Free a tables array. */
void
free_tables(GArray **tables)
{
#ifdef DEBUG
    printf("free_tables\n");
#endif

    gint i;

    for(i = 0; i < (*tables)->len; i++)
    {
        free_gchar_ptr(g_array_index(*tables, Table, i).name);        
        free_g_array(&g_array_index(*tables, Table, i).elements);
    }

    g_array_free(*tables, TRUE);
    *tables = NULL;
}

/** Free a new_tables array. */
void
free_new_tables(GArray **new_tables)
{
#ifdef DEBUG
    printf("free_new_tables\n");
#endif

    gint i;

    for(i = 0; i < (*new_tables)->len; i++)
        free_gchar_ptr(g_array_index(*new_tables, NewTable, i).name);        

    g_array_free(*new_tables, TRUE);
    *new_tables = NULL;
}

/** Free the memory occupied by a teams array.
    @param teams The pointer to the array we free. */
void
free_teams_array(GArray **teams, gboolean reset)
{
#ifdef DEBUG
    printf("free_teams_array\n");
#endif

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
#ifdef DEBUG
    printf("free_team\n");
#endif

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
#ifdef DEBUG
    printf("free_player_array\n");
#endif

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
#ifdef DEBUG
    printf("free_player\n");
#endif

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
#ifdef DEBUG
    printf("free_cups_array\n");
#endif

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
#ifdef DEBUG
    printf("free_cup\n");
#endif

    gint i;

    free_gchar_ptr(cup->name);
    free_gchar_ptr(cup->short_name);
    free_gchar_ptr(cup->symbol);
    free_gchar_ptr(cup->sid);

    free_gchar_array(&cup->skip_weeks_with);

    for(i=0;i<cup->rounds->len;i++)
        free_cup_round(&g_array_index(cup->rounds, CupRound, i));
    free_g_array(&cup->rounds);

    free_g_array(&cup->fixtures);
    free_g_array(&cup->week_breaks);

    free_g_ptr_array(&cup->bye);
    free_gchar_array(&cup->team_names);
    free_g_ptr_array(&cup->teams);
    free_gchar_array(&cup->properties);
}

void
free_cup_round(CupRound *cup_round)
{
#ifdef DEBUG
    printf("free_cup_round\n");
#endif

    gint j;

    g_free(cup_round->name);

    free_g_array(&cup_round->two_match_weeks[0]);
    free_g_array(&cup_round->two_match_weeks[1]);

    for(j = 0; j < cup_round->waits->len; j++)
        g_free(g_array_index(cup_round->waits, CupRoundWait, j).cup_sid);

    free_g_array(&cup_round->waits);

    if(cup_round->round_robin_number_of_groups > 0)
    {
        free_tables(&cup_round->tables);

        for(j=0;j<cup_round->choose_teams->len;j++)
            free_cup_choose_team(
                &g_array_index(cup_round->choose_teams, CupChooseTeam, j));
	    
        free_g_array(&cup_round->choose_teams);
        free_teams_array(&cup_round->teams, FALSE);
        g_ptr_array_free(cup_round->team_ptrs, TRUE);
    }    
}

/**
   Free the memory occupied by a CupChooseTeam.
   @param cup_choose_team The pointer to the team we free.
*/
void
free_cup_choose_team(CupChooseTeam *cup_choose_team)
{
#ifdef DEBUG
    printf("free_cup_choose_team\n");
#endif

    free_gchar_ptr(cup_choose_team->sid);
}

/** Free some global variables (except for the country variable). */
void
free_variables(void)
{
#ifdef DEBUG
    printf("free_variables\n");
#endif

    free_option_list(&options, FALSE);
    free_option_list(&settings, FALSE);
    free_option_list(&constants, FALSE);
    free_option_list(&constants_app, FALSE);
    free_option_list(&tokens, FALSE);

    free_gchar_ptr(save_file);

    g_rand_free(rand_generator);

    free_season_stats(FALSE);
}

/** Free the list with live game commentary text. */
void
free_lg_commentary(gboolean reset)
{
#ifdef DEBUG
    printf("free_lg_commentary\n");
#endif

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

/** Free the news variables. */
void
free_news(gboolean reset)
{
#ifdef DEBUG
    printf("free_news\n");
#endif

    gint i, j, k;

    for(i=0;i<NEWS_ARTICLE_TYPE_END;i++)
	if(news[i] != NULL)
	{
	    for(j=0;j<news[i]->len;j++)
	    {
		g_free(g_array_index(news[i], NewsArticle, j).condition);
                
                for(k = 0; k < g_array_index(news[i], NewsArticle, j).titles->len; k++)
                {
                    g_free(g_array_index(g_array_index(news[i], NewsArticle, j).titles, NewsText, k).text);
                    g_free(g_array_index(g_array_index(news[i], NewsArticle, j).titles, NewsText, k).condition);
                }

                g_array_free(g_array_index(news[i], NewsArticle, j).titles, TRUE);

                for(k = 0; k < g_array_index(news[i], NewsArticle, j).subtitles->len; k++)
                {
                    g_free(g_array_index(g_array_index(news[i], NewsArticle, j).subtitles, NewsText, k).text);
                    g_free(g_array_index(g_array_index(news[i], NewsArticle, j).subtitles, NewsText, k).condition);
                }

                g_array_free(g_array_index(news[i], NewsArticle, j).subtitles, TRUE);
	    }

	    free_g_array(&news[i]);
	}

    if(reset)
	for(i=0;i<NEWS_ARTICLE_TYPE_END;i++)
	    news[i] = g_array_new(FALSE, FALSE, sizeof(NewsArticle));
}

void
free_newspaper(gboolean reset)
{
    gint i;

    for(i = 0; i < newspaper.articles->len; i++)
        free_newspaper_article(&g_array_index(newspaper.articles, NewsPaperArticle, i));

    g_array_free(newspaper.articles, TRUE);

    if(reset)
        newspaper.articles = g_array_new(FALSE, FALSE, sizeof(NewsPaperArticle));
}

void
free_newspaper_article(NewsPaperArticle *article)
{
    g_free(article->title_small);
    g_free(article->title);
    g_free(article->subtitle);
}

/**
   Free a GPtrArray containing strings.
   @param array The array to be freed.
*/
void
free_gchar_array(GPtrArray **array)
{
#ifdef DEBUG
    printf("_array\n");
#endif

#ifdef DEBUG
    printf("free_gchar_array\n");
#endif

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
#ifdef DEBUG
    printf("free_g_string\n");
#endif

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
#ifdef DEBUG
    printf("free_g_array\n");
#endif

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
#ifdef DEBUG
    printf("free_g_ptr_array\n");
#endif

    if(*array == NULL)
	return;

    g_ptr_array_free(*array, TRUE);

    *array = NULL;
}

/** Free the glist containing the support directories. */
void
free_support_dirs(void)
{
#ifdef DEBUG
    printf("free_support_dirs\n");
#endif

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
#ifdef DEBUG
    printf("free_name_list\n");
#endif

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
#ifdef DEBUG
    printf("free_names\n");
#endif

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

/** Free the array with the CPU strategies. */
void
free_strategies(void)
{
#ifdef DEBUG
    printf("free_strategies\n");
#endif

    gint i, j;

    for(i=0;i<strategies->len;i++)
    {
	g_free(g_array_index(strategies, Strategy, i).sid);
	g_free(g_array_index(strategies, Strategy, i).desc);

	for(j=0;j<g_array_index(strategies, Strategy, i).prematch->len;j++)
	{
	    g_free(
		g_array_index(
		    g_array_index(
			strategies, Strategy, i).prematch, StrategyPrematch, j).condition);
	    g_array_free(
		g_array_index(
		    g_array_index(strategies, Strategy, i).prematch, StrategyPrematch, j).formations, 
		TRUE);
	}
	g_array_free(g_array_index(strategies, Strategy, i).prematch, TRUE);

	for(j=0;j<g_array_index(strategies, Strategy, i).match_action->len;j++)
	{
	    g_free(
		g_array_index(
		    g_array_index(
			strategies, Strategy, i).match_action, StrategyMatchAction, j).condition);
	    g_free(
		g_array_index(
		    g_array_index(
			strategies, Strategy, i).match_action, StrategyMatchAction, j).sub_condition);
	}
	g_array_free(g_array_index(strategies, Strategy, i).match_action, TRUE);
    }

    g_array_free(strategies, TRUE);
}

/** Free the betting arrays. */
void
free_bets(gboolean reset)
{
#ifdef DEBUG
    printf("free_bets\n");
#endif

    free_g_array(&(bets[0]));
    free_g_array(&(bets[1]));

    if(reset)
    {
	bets[0] = g_array_new(FALSE, FALSE, sizeof(BetMatch));
	bets[1] = g_array_new(FALSE, FALSE, sizeof(BetMatch));
    }
}

/** Free a job struct.
    @param free_team Whether to free the team associated
    with the job if it's an international offer. */
void
free_job(Job *job, gboolean free_tm)
{
#ifdef DEBUG
    printf("free_job\n");
#endif

    gint i;

    if(job->type == JOB_TYPE_INTERNATIONAL)
    {
	free_gchar_ptr(job->country_file);
	free_gchar_ptr(job->country_name);
	free_gchar_ptr(job->league_name);

	for(i=0;i<job_teams->len;i++)
	    if(g_array_index(job_teams, Team, i).id == job->team_id)
	    {
		if(free_tm)
		    free_team(&g_array_index(job_teams, Team, i));
		g_array_remove_index(job_teams, i);
	    }
    }
}

/** Free the jobs and job_teams arrays. */
void
free_jobs(gboolean reset)
{
#ifdef DEBUG
    printf("free_jobs\n");
#endif

    gint i;

    if(jobs == NULL)
    {
	if(reset)
	{
	    jobs = g_array_new(FALSE, FALSE, sizeof(Job));
	    job_teams = g_array_new(FALSE, FALSE, sizeof(Team));
	}

	return;
    }

    for(i=0;i<jobs->len;i++)
	free_job(&g_array_index(jobs, Job, i), TRUE);

    free_g_array(&jobs);
    free_g_array(&job_teams);

    if(reset)
    {
	jobs = g_array_new(FALSE, FALSE, sizeof(Job));
	job_teams = g_array_new(FALSE, FALSE, sizeof(Team));
    }
}
