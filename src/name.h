#ifndef NAME_H
#define NAME_H

#include "bygfoot.h"
#include "name_struct.h"
#include "maths.h"

#define nli(i) g_array_index(name_lists, NameList, i)
#define name_get_random_first_name(namelist) (gchar*)g_ptr_array_index(namelist->first_names, math_rndi(0, namelist->first_names->len - 1))
#define name_get_random_last_name(namelist) (gchar*)g_ptr_array_index(namelist->last_names, math_rndi(0, namelist->last_names->len - 1))

/** Get a name from a randomly picked name list. */
#define name_get_from_random_list() name_get_from_list(&nli(math_rndi(0, name_lists->len - 1)))

void
name_shorten_list(NameList *namelist);

gchar*
name_get(const gchar *names_file);

gchar*
name_get_from_list(const NameList *namelist);

NameList*
name_get_list_from_sid(const gchar *sid);

#endif
