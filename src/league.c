#include "cup.h"
#include "fixture_struct.h"
#include "league.h"
#include "team.h"
#include "variables.h"

/**
   Create a new league with some default values.
   @return The league we created.
   @see League
*/
League
league_new(void)
{
    League new;

    new.name = g_string_new("");
    new.sid = g_string_new("");
    new.short_name = g_string_new("");
    new.symbol = g_string_new("");
    
    new.id = league_new_id();

    new.average_skill = -1;

    new.prom_rel.prom_games_dest_sid = g_string_new("");
    new.prom_rel.elements = g_array_new(FALSE, FALSE, sizeof(PromRelElement));

    new.teams = g_array_new(FALSE, FALSE, sizeof(Team));
    
    new.fixtures = g_array_new(FALSE, FALSE, sizeof(Fixture));

    new.table.name = g_string_new("");
    new.table.clid = new.id;
    new.table.round = -1;
    new.table.elements = g_array_new(FALSE, FALSE, sizeof(TableElement));

    new.first_week = new.week_gap = 1;
    new.yellow_red = 1000;

    return new;
}

/**
   Create a new PromRelElement with default values.
   @return The PromRelElement we created.
   @see PromRelElement
*/
PromRelElement
prom_rel_element_new(void)
{
    PromRelElement new;

    new.ranks[0] = new.ranks[1] = 0;
    new.dest_sid = g_string_new("");
    new.type = PROM_REL_NONE;

    return new;
}

/** Return a new numerical id for a league.
    We browse through the existing leagues and take the
    first free nid.
*/
gint
league_new_id(void)
{
    gint i, j;

    if(ligs == NULL)
	return ID_LEAGUE_START;

    for(i=ID_LEAGUE_START;i<ID_LEAGUE_START+1000;i++)
    {
	for(j=0;j<ligs->len;j++)
	    if(lig(j).id == i)
		break;

	if(j == ligs->len)
	    return i;
    }

    g_warning("league_new_id: didn't find a free numerical id.");
    return -1;
}

/** Get the array index of the given league or cup id.
    @param clid The id of the league or cup.
    @return The index in the leagues or cups array. */
gint
league_cup_get_index_from_clid(gint clid)
{
    gint i;
    gint index = -1;

    if(clid < ID_CUP_START)
    {
	for(i=0;i<ligs->len;i++)
	    if(lig(i).id == clid)
	    {
		index = i;
		break;
	    }
    }
    else
	for(i=0;i<cps->len;i++)
	    if(cp(i).id == clid)
	    {
		index = i;
		break;
	    }

    if(index == -1)
	g_warning("league_cup_get_index_from_clid: couldn't find league or cup with index %d\n", clid);

    return index;
}

/** Return the league pointer belonging to the id.
    @param clid The id we look for.
    @return The league pointer or NULL if failed. */
League*
league_from_clid(gint clid)
{
    gint i;

    for(i=0;i<ligs->len;i++)
	if(lig(i).id == clid)
	    return &lig(i);

    g_warning("league_from_clid: didn't find league with id %d\n", clid);
    return NULL;
}

/** Return the id of the next league (or cup if we are
    at the end of the leagues array).
    @param clid The id of the current league or cup. */
gint
league_cup_get_next_clid(gint clid)
{
    gint i, return_value = -1;

    if(clid < ID_CUP_START)
    {
	for(i=0;i<ligs->len;i++)
	    if(lig(i).id == clid)
		break;

	if(i != ligs->len - 1)
	    return_value = lig(i + 1).id;
	else if(cps->len > 0)
	    return_value = cp(0).id;
	else
	    return_value = clid;
    }
    else
    {
	for(i=0;i<cps->len;i++)
	    if(cp(i).id == clid)
		break;

	if(i != cps->len - 1)
	    return_value = cp(i + 1).id;
	else
	    return_value = lig(0).id;
    }

    return return_value;
}


/** Return the id of the previous league or cup.
    @param clid The id of the current league or cup. */
gint
league_cup_get_previous_clid(gint clid)
{
    gint i, return_value = -1;

    if(clid < ID_CUP_START)
    {
	for(i=ligs->len - 1;i>=0;i--)
	    if(lig(i).id == clid)
		break;

	if(i != 0)
	    return_value = lig(i - 1).id;
	else if(cps->len > 0)
	    return_value = cp(cps->len - 1).id;
	else
	    return_value = clid;
    }
    else
    {
	for(i=cps->len - 1;i>=0;i--)
	    if(cp(i).id == clid)
		break;

	if(i != 0)
	    return_value = cp(i - 1).id;
	else
	    return_value = lig(ligs->len - 1).id;
    }

    return return_value;
}

/** Return the fixture coming after the specified week and round. 
    @param league The league the fixtures of which we examine. 
    @return A fixture pointer or NULL. */
Fixture*
league_cup_get_next_fixture(gint clid, gint week_number, gint week_round_number)
{
    gint i;
    GArray *fixtures = (clid < ID_CUP_START) ?
	league_from_clid(clid)->fixtures :
	cup_from_clid(clid)->fixtures;

    for(i=0;i<fixtures->len;i++)
	if(g_array_index(fixtures, Fixture, i).week_number > week_number ||
	   (g_array_index(fixtures, Fixture, i).week_number == week_number &&
	    g_array_index(fixtures, Fixture, i).week_round_number > week_round_number))
	    return &g_array_index(fixtures, Fixture, i);

    return NULL;
}

/** Return the fixture coming just before the specified week and round. 
    @param league The league the fixtures of which we examine. 
    @return A fixture pointer or NULL. */
Fixture*
league_cup_get_previous_fixture(gint clid, gint week_number, gint week_round_number)
{
    gint i;
    GArray *fixtures = (clid < ID_CUP_START) ?
	league_from_clid(clid)->fixtures :
	cup_from_clid(clid)->fixtures;

    for(i=fixtures->len - 1;i>=0;i--)
	if(g_array_index(fixtures, Fixture, i).week_number < week_number ||
	   (g_array_index(fixtures, Fixture, i).week_number == week_number &&
	    g_array_index(fixtures, Fixture, i).week_round_number < week_round_number))
	    return &g_array_index(fixtures, Fixture, i);

    return NULL;
}


/** Return the number of league in the leagues array. */
gint
league_get_index(gint clid)
{
    gint i;

    for(i=0;i<ligs->len;i++)
	if(lig(i).id == clid)
	    return i;

    g_warning("league_get_index: reached end of leagues array; clid is %d\n", clid);

    return -1;
}

/** Return the average stadium capacity of cpu teams
    in the specified league or cup. */
gint
league_cup_average_capacity(gint clid)
{
    gint i;
    gfloat sum = 0;
    const GArray *teams = (clid < ID_CUP_START) ?
	league_from_clid(clid)->teams :
	cup_from_clid(clid)->teams;

    for(i=0;i<teams->len;i++)
	if(team_is_user(&g_array_index(teams, Team, i)) == -1)
	    sum += g_array_index(teams, Team, i).stadium.capacity;

    return sum / (gfloat)teams->len;
}
