#include "cup_struct.h"
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

    new.average_skill = new.average_capacity = -1;

    new.prom_rel.prom_games_dest_sid = g_string_new("");
    new.prom_rel.prom_games_cup_sid = g_string_new("");
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
