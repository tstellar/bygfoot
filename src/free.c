#include "free.h"

/**
   Free all memory allocated by the program.
   This mainly means we have to free a lot of GStrings
   and GArrays. */
void
free_memory(void)
{
    free_country();
    if(font_name != NULL)
    {
	g_string_free(font_name, TRUE);
	font_name = NULL;
    }
}

/**
   Free the main variable of the game, #country.
*/
void
free_country(void)
{
    gint i;
    GString **strings[3] = 
	{&country.name, &country.symbol, &country.id};
    GArray **arrays[3] =
	{&ligs, &cps, &country.supercups};

    for(i=0;i<3;i++)
    {
	if(*strings[i] != NULL)
	{
	    g_string_free(*strings[i], TRUE);
	    *strings[i] = NULL;
	}
    }

    if(ligs != NULL)
	for(i=0;i<ligs->len;i++)
	    free_league(&(g_array_index(ligs, League, i)));

    for(i=0;i<3;i++)
    {
	if(*arrays[i] != NULL)
	{
	    g_array_free(*arrays[i], TRUE);
	    *arrays[i] = NULL;
	}
    }
}

/**
   Free the memory occupied by a league.
   @param league The pointer to the league we free.
*/
void
free_league(League *league)
{
    gint i;
    GString **strings[8] = 
	{&league->name,
	 &league->short_name,
	 &league->symbol,
	 &league->id,
	 &league->prom_rel.prom_games_cup_id,
	 &league->prom_rel.prom_games_dest_id,
	 &league->table.name,
	 &league->table.league_id};
    GArray **arrays[3] = 
	{&league->teams,
	 &league->prom_rel.elements,
	 &league->table.elements};

    for(i=0;i<8;i++)
	if(*strings[i] != NULL)
	{
	    g_string_free(*strings[i], TRUE);
	    *strings[i] = NULL;
	}

    if(league->teams != NULL)
	for(i=0;i<league->teams->len;i++)
	    free_team(&(g_array_index(league->teams, Team, i)));

    for(i=0;i<3;i++)
	if(*arrays[i] != NULL)
	{
	    g_array_free(*arrays[i], TRUE);
	    *arrays[i] = NULL;
	}
}

/**
   Free the memory occupied by a team.
   @param team The pointer to the team we free.
*/
void
free_team(Team *team)
{
    if(team->name != NULL)
    {
	g_string_free(team->name, TRUE);
	team->name = NULL;
    }
}
