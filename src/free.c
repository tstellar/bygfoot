#include "cup.h"
#include "fixture.h"
#include "free.h"
#include "league.h"
#include "player.h"
#include "variables.h"


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

/**
   Free all memory allocated by the program.
   This mainly means we have to free a lot of GStrings
   and GArrays. */
void
free_memory(void)
{
    free_variables();
    free_country();
    free_g_string(&font_name);
}

/**
   Free the main variable of the game, #country.
*/
void
free_country(void)
{
    gint i;
    GString **strings[3] = 
	{&country.name, &country.symbol, &country.sid};

    for(i=0;i<3;i++)
	free_g_string(strings[i]);

    free_leagues_array(&ligs);

    free_cups_array(&cps);
}

/**
   Free the memory occupied by a leagues array.
   @param leagues The pointer to the array we free.
*/
void
free_leagues_array(GArray **leagues)
{
    gint i;

    if(*leagues == NULL)
	return;

    for(i=0;i<(*leagues)->len;i++)
	free_league(&g_array_index(*leagues, League, i));

    free_g_array(leagues);
}

/**
   Free the memory occupied by a league.
   @param league The pointer to the league we free.
*/
void
free_league(League *league)
{
    gint i;
    GString **strings[7] = 
	{&league->name,
	 &league->short_name,
	 &league->symbol,
	 &league->sid,
	 &league->prom_rel.prom_games_cup_sid,
	 &league->prom_rel.prom_games_dest_sid,
	 &league->table.name};
    GArray **arrays[3] = 
	{&league->teams,
	 &league->prom_rel.elements,
	 &league->table.elements};

    for(i=0;i<7;i++)
	free_g_string(strings[i]);

    if(league->teams != NULL)
	free_teams_array(&league->teams);

    for(i=0;i<3;i++)
	free_g_array(arrays[i]);
}

/** Free the memory occupied by a teams array.
    @param teams The pointer to the array we free. */
void
free_teams_array(GArray **teams)
{
    gint i;

    if(*teams == NULL)
	return;

    for(i=0;i<(*teams)->len;i++)
	free_team(&g_array_index(*teams, Team, i));

    free_g_array(teams);
}

/**
   Free the memory occupied by a team.
   @param tm The pointer to the team we free.
*/
void
free_team(Team *tm)
{
    gint i;

    free_g_string(&tm->name);

    if(tm->players != NULL)
    {
	for(i=0;i<tm->players->len;i++)
	    free_player(&g_array_index(tm->players, Player, i));

	free_g_array(&tm->players);
    }
}

/** Free the memory occupied by a player.
    @param pl The pointer to the player we free. */
void
free_player(Player *pl)
{
    free_g_string(&pl->name);

    free_g_array(&pl->cards);

    /* todo: free history */
}

/**
   Free the memory occupied by a cups array.
   @param cups The pointer to the array we free.
*/
void
free_cups_array(GArray **cups)
{
    gint i;

    if(*cups == NULL)
	return;

    for(i=0;i<(*cups)->len;i++)
	free_cup(&g_array_index(*cups, Cup, i));

    free_g_array(cups);
}

/**
   Free the memory occupied by a cup.
   @param cup The pointer to the cup we free.
*/
void
free_cup(Cup *cup)
{
    gint i;
    GString **strings[4] =
	{&cup->name,
	 &cup->short_name,
	 &cup->symbol,
	 &cup->sid};
    GArray **arrays[3] = 
	{&cup->choose_teams,
	 &cup->rounds,
	 &cup->teams};

    for(i=0;i<4;i++)
	free_g_string(strings[i]);

    if(cup->choose_teams != NULL)
	for(i=0;i<cup->choose_teams->len;i++)
	    free_cup_choose_team(&g_array_index(cup->choose_teams, CupChooseTeam, i));
    free_cup_choose_team(&cup->choose_team_user);

    if(cup->teams != NULL)
	for(i=0;i<cup->teams->len;i++)
	    free_team(&g_array_index(cup->teams, Team, i));

    for(i=0;i<3;i++)
	free_g_array(arrays[i]);
}

/**
   Free the memory occupied by a CupChooseTeam.
   @param cup_choose_team The pointer to the team we free.
*/
void
free_cup_choose_team(CupChooseTeam *cup_choose_team)
{
    free_g_string(&cup_choose_team->sid);
}

/** Free some global variables (except for the country variable). */
void
free_variables(void)
{
    gint i, j;
    GArray **arrays[2] = 
	{&transfer_list,
	 &fixtures};
    
    free_g_string_array(&player_names);

    if(fixtures != NULL)	
	for(i=0;i<fixtures->len;i++)
	    for(j=0;j<g_array_index(fixtures, Fixture, i).goals->len;j++)
		free_g_string(&g_array_index(
				  g_array_index(
				      fixtures, Fixture, i).goals, Goal, j).scorer_name);

    for(i=0;i<2;i++)
	free_g_array(arrays[i]);
}

/**
   Free a GPtrArray containing GStrings.
   @param array The array to be freed.
*/
void
free_g_string_array(GPtrArray **array)
{
    gint i;

    if(*array == NULL)
	return;

    for(i=0;i<(*array)->len;i++)
	free_g_string((GString**)&g_ptr_array_index(*array, i));

    g_ptr_array_free(*array, TRUE);

    *array = NULL;
}
