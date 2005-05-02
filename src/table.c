#include "cup.h"
#include "league.h"
#include "table.h"
#include "variables.h"

/** Return a newly allocated empty table. */
Table
table_new(void)
{
    Table new;

    new.name = g_string_new("");
    new.clid = -1;
    new.round = -1;
    new.elements = g_array_new(FALSE, FALSE, sizeof(TableElement));

    return new;
}

/** Return a nullified table element.
    @param team The team pointer of the element.
    @param old_rank The old_rank value of the element.
    @see #TableElement */
TableElement
table_element_new(Team *team, gint old_rank)
{
    gint i;
    TableElement new;

    new.team = team;
    new.team_id = team->id;
    new.old_rank = old_rank;

    for(i=0;i<TABLE_END;i++)
	new.values[i] = 0;

    return new;
}

/** Update the appropriate table entries after
    a fixture has been calculated. 
    @param fix The fixture that's just been calculated. */
void
table_update(const Fixture *fix)
{
    gint i;
    gint idx = (fix->result[0][0] < fix->result[1][0]);
    TableElement *elements[2] = {NULL, NULL};

    table_update_get_elements(elements, fix);

    for(i=0;i<2;i++)
    {
	elements[i]->values[TABLE_PLAYED]++;
	elements[i]->values[TABLE_GF] += fix->result[i][0];
	elements[i]->values[TABLE_GA] += fix->result[!i][0];
	elements[i]->values[TABLE_GD] = 
	    elements[i]->values[TABLE_GF] - elements[i]->values[TABLE_GA];
    }
    
    if(fix->result[0][0] == fix->result[1][0])
	for(i=0;i<2;i++)
	{
	    elements[i]->values[TABLE_DRAW]++;
	    elements[i]->values[TABLE_PTS] += 1;
	}
    else
    {
	elements[idx]->values[TABLE_WON]++;
	elements[idx]->values[TABLE_PTS] += 3;
	elements[!idx]->values[TABLE_LOST]++;
    }
}

/** Get the pointers to the table entries
    representing the two teams from the fixture. 
    @param elements The table entries.
    @fix The fixture. */
void
table_update_get_elements(TableElement **elements, const Fixture *fix)
{
    gint i, j;
    Table *table;

    if(fix->clid < ID_CUP_START)
    {
	table = &league_from_clid(fix->clid)->table;
	for(i=0;i<table->elements->len;i++)
	{
	    if(g_array_index(table->elements, TableElement, i).team == fix->teams[0])
		elements[0] = &g_array_index(table->elements, TableElement, i);
	    else if(g_array_index(table->elements, TableElement, i).team == fix->teams[1])
		elements[1] = &g_array_index(table->elements, TableElement, i);
	}
    }
    else
	for(i=0;i<cup_get_last_tables(fix->clid)->len;i++)
	{
	    table = &g_array_index(cup_get_last_tables(fix->clid), Table, i);

	    if(elements[0] == NULL || elements[1] == NULL)
		for(j=0;j<table->elements->len;j++)
		{
		    if(g_array_index(table->elements, TableElement, j).team == fix->teams[0])
			elements[0] = &g_array_index(table->elements, TableElement, j);
		    else if(g_array_index(table->elements, TableElement, j).team == fix->teams[1])
			elements[1] = &g_array_index(table->elements, TableElement, j);
		}
	}
}

/** Compare function used to sort the tables.
    @param a The first table element.
    @param b The second element.
    @param clid_round The clid and the cup round of the table
    encoded as a gpointer. */
gint
table_element_compare_func(gconstpointer a,
			   gconstpointer b,
			   gpointer clid_pointer)
{
    gint i;
    gint clid, cup_round, value;
    TableElement *element1 = (TableElement*)a,
	*element2 = (TableElement*)b;
    GArray *fixtures;
    const Fixture *fix[2] = {NULL, NULL};

    if(element1->team_id == element2->team_id)
	return 0;

    clid = GPOINTER_TO_INT(clid_pointer);

    fixtures = league_cup_get_fixtures(clid);
    if(clid < ID_CUP_START)
	cup_round = -1;
    else
	cup_round = cup_has_tables(clid);
    
    /*todo use misc_int_compare*/
    if(element1->values[TABLE_PTS] > element2->values[TABLE_PTS])
	value = -1;

    else if(element1->values[TABLE_PTS] < element2->values[TABLE_PTS])
	value = 1;
    
    else if(element1->values[TABLE_GF] - element1->values[TABLE_GA] >
	    element2->values[TABLE_GF] - element2->values[TABLE_GA])
	value = -1;

    else if(element1->values[TABLE_GF] - element1->values[TABLE_GA] <
	    element2->values[TABLE_GF] - element2->values[TABLE_GA])
	value = 1;

    else if(element1->values[TABLE_GA] > element2->values[TABLE_GA])
	value = -1;

    else if(element1->values[TABLE_GA] < element2->values[TABLE_GA])
	value = 1;

    else
    {
	for(i=0;i<fixtures->len;i++)
	{
	    if(g_array_index(fixtures, Fixture, i).round == cup_round &&
	       g_array_index(fixtures, Fixture, i).week_number <= week &&
	       g_array_index(fixtures, Fixture, i).week_round_number <= week_round)
	    {
		if(g_array_index(fixtures, Fixture, i).team_ids[0] == element1->team_id &&
		   g_array_index(fixtures, Fixture, i).team_ids[1] == element2->team_id)
		    fix[0] = &g_array_index(fixtures, Fixture, i);
		else if(g_array_index(fixtures, Fixture, i).team_ids[1] == element1->team_id &&
			g_array_index(fixtures, Fixture, i).team_ids[0] == element2->team_id)
		    fix[1] = &g_array_index(fixtures, Fixture, i);
	    }
	}
	
	if(fix[0] == NULL || fix[1] == NULL)
	    value = 0;
	else
	{
	    if(fix[0]->result[0][0] + fix[1]->result[1][0] >
	       fix[0]->result[1][0] + fix[1]->result[0][0])
		value = -1;
	    else if(fix[0]->result[0][0] + fix[1]->result[1][0] <
		    fix[0]->result[1][0] + fix[1]->result[0][0])
		value = 1;
	    else if(fix[1]->result[1][0] > fix[0]->result[1][0])
		value = -1;
	    else
		value = 1;
	}
    }
    
    return value;
}
