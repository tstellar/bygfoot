#ifndef NAME_STRUCT_H
#define NAME_STRUCT_H

#include "bygfoot.h"

/** A list of first names and last names
    from a file. */
typedef struct
{
    /** The file id (the part between 'player_names_' and '.xml'). */
    gchar *sid;
    /** Arrays of strings holding the names. */
    GPtrArray *first_names, 
	*last_names;

} NameList;

#endif
