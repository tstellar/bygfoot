#include "league.h"

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
    new.id = g_string_new("");
    new.short_name = g_string_new("");
    new.symbol = g_string_new("");
    
    new.prom_rel.prom_games_dest_id = g_string_new("");
    new.prom_rel.prom_games_cup_id = g_string_new("");
    new.prom_rel.elements = g_array_new(FALSE, FALSE, sizeof(PromRelElement));

    new.teams = g_array_new(FALSE, FALSE, sizeof(Team));

    new.table.name = g_string_new("");
    new.table.league_id = g_string_new("");
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
    new.dest_id = g_string_new("");

    return new;
}
