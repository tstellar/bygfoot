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

    new.table.name = g_string_new("");
    new.table.league_id = new.id;
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
