#include "cup.h"
#include "free.h"
#include "option.h"
#include "player.h"
#include "stat.h"
#include "table_struct.h"
#include "team.h"
#include "variables.h"

/** Return a newly allocated league stat with given clid. */
LeagueStat
stat_league_new(gint clid)
{
    LeagueStat new;

    new.clid = clid;
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
	if(g_array_index(lig(i).fixtures, Fixture, lig(i).fixtures->len - 1).week_number >= week)
	{
	    free_league_stats(&lig(i).stats);
	    lig(i).stats.teams_off = stat_update_league_teams(lig(i).teams, TEAM_COMPARE_OFFENSIVE);
	    lig(i).stats.teams_def = stat_update_league_teams(lig(i).teams, TEAM_COMPARE_DEFENSE);
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
	    new_stat.team_id = pl->team->id;
	    new_stat.value_string = g_string_new(pl->name->str);
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
	new_stat.team_id = ((Team*)g_ptr_array_index(teams, i))->id;
	new_stat.value1 = team_get_table_value((Team*)g_ptr_array_index(teams, i), TABLE_GF);
	new_stat.value2 = team_get_table_value((Team*)g_ptr_array_index(teams, i), TABLE_GA);
	new_stat.value3 = -1;
	new_stat.value_string = g_string_new("");

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
	new_champ.cl_name = g_string_new(lig(i).name->str);
	new_champ.team_name = 
	    g_string_new(g_array_index(lig(i).table.elements, TableElement, 0).team->name->str);
	g_array_append_val(new.league_champs, new_champ);

	g_array_append_val(new.league_stats, lig(i).stats);
	lig(i).stats = stat_league_new(lig(i).id);
    }

    for(i=0;i<acps->len;i++)
    {
	new_champ.cl_name = g_string_new(acp(i)->name->str);
	new_champ.team_name = 
	    g_string_new(cup_get_winner(acp(i))->name->str);
	g_array_append_val(new.cup_champs, new_champ);
    }

    g_array_append_val(season_stats, new);
}
