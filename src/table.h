#ifndef TABLE_H
#define TABLE_H

#include "bygfoot.h"
#include "fixture_struct.h"
#include "table_struct.h"

Table
table_new(void);

TableElement
table_element_new(Team *team);

void
table_update(const Fixture *fix);

void
table_update_get_elements(TableElement **elements, const Fixture *fix);

gint
table_element_compare_func(gconstpointer a,
			   gconstpointer b,
			   gpointer clid_round);

#endif

