/*
   option.c

   Bygfoot Football Manager -- a small and simple GTK2-based
   football management game.

   http://bygfoot.sourceforge.net

   Copyright (C) 2005  Gyözö Both (gyboth@bygfoot.com)

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "main.h"
#include "misc.h"
#include "option.h"
#include "variables.h"

/** Return the string going with the option
    named 'name'.
    @param name The name of the option.
    @return The string_value of the option.
    @see #Option */
gchar*
option_string(const gchar *name, OptionList *optionlist)
{
#ifdef DEBUG
    printf("option_string\n");
#endif

#ifdef DEBUG
    printf("option_string\n");
#endif

    gpointer element = g_datalist_get_data(&optionlist->datalist, name);
    
    if(element != NULL)
	return ((Option*)element)->string_value;

    main_exit_program(EXIT_OPTION_NOT_FOUND, 
		      "option_string: option named %s not found\nMaybe you should delete the .bygfoot directory from your home dir", name);

    return NULL;
}

/** Return the GString pointer going with the option. */
gchar**
option_string_pointer(const gchar *name, OptionList *optionlist)
{
    gpointer element = g_datalist_get_data(&optionlist->datalist, name);
    
    if(element != NULL)
	return &((Option*)element)->string_value;

    main_exit_program(EXIT_OPTION_NOT_FOUND, 
		      "option_string: option named %s not found\nMaybe you should delete the .bygfoot directory from your home dir", name);

    return NULL;
}

/** Return the integer going with the option
    named 'name'.
    @param name The name of the option.
    @return The value of the option.
    @see #Option */
gint
option_int(const gchar *name, OptionList *optionlist)
{
#ifdef DEBUG
    printf("option_int\n");
#endif

    gpointer element = g_datalist_get_data(&optionlist->datalist, name);
    
    if(element != NULL)
	return ((Option*)element)->value;

    main_exit_program(EXIT_OPTION_NOT_FOUND, 
		      "option_int: option named %s not found\nMaybe you should delete the .bygfoot directory from your home dir", name);

    return -1;
}

/** Return the address of an options variable. */
gint*
option_int_pointer(const gchar *name, OptionList *optionlist)
{
#ifdef DEBUG
    printf("option_int_pointer\n");
#endif

    gpointer element = g_datalist_get_data(&optionlist->datalist, name);
    
    if(element != NULL)
	return &((Option*)element)->value;

    main_exit_program(EXIT_OPTION_NOT_FOUND, 
		      "option_int: option named %s not found\nMaybe you should delete the .bygfoot directory from your home dir", name);

    return NULL;
}

/** Return the int going with the option named 'name'
    cast to float and divided by 1000.
    @param name The name of the option.
    @return The value of the option cast to float and divided by 1000.
    @see #Option */
gfloat
option_float(const gchar *name, OptionList *optionlist)
{
    gpointer element = g_datalist_get_data(&optionlist->datalist, name);
    
    if(element != NULL)
	return (gfloat)((Option*)element)->value / OPTION_FLOAT_DIVISOR;

    main_exit_program(EXIT_OPTION_NOT_FOUND, 
		      "option_float: option named %s not found\nMaybe you should delete the .bygfoot directory from your home dir", name);

    return -1;
}

/** Change the value of a string option in the array.
    @param name The name of the option.
    @param option_array The option array.
    @param new_value The value we set. */
void
option_set_string(const gchar *name, OptionList *optionlist, const gchar *new_value)
{
#ifdef DEBUG
    printf("option_set_string\n");
#endif

    gpointer element = g_datalist_get_data(&optionlist->datalist, name);
    
    if(element == NULL)
	debug_print_message("option_set_string: option named %s not found\nMaybe you should delete the .bygfoot directory from your home dir", name);
    else
	misc_string_assign(&((Option*)element)->string_value, new_value);
}

/** Change the value of an int option in the array.
    @param name The name of the option.
    @param option_array The option array.
    @param new_value The value we set. */
void
option_set_int(const gchar *name, OptionList *optionlist, gint new_value)
{
#ifdef DEBUG
    printf("option_set_int\n");
#endif

    gpointer element = g_datalist_get_data(&optionlist->datalist, name);
    
    if(element == NULL)
	debug_print_message("option_set_int: option named %s not found\nMaybe you should delete the .bygfoot directory from your home dir", name);
    else
	((Option*)element)->value = new_value;
}


/** Add an option to the optionlist with the given values. */
void
option_add(OptionList *optionlist, const gchar *name, 
	   gint int_value, const gchar *string_value)
{
#ifdef DEBUG
    printf("option_add\n");
#endif

    gint i;
    Option new;
    gpointer element = NULL;

    if(optionlist->list != NULL)
	element = g_datalist_get_data(&optionlist->datalist, name);
    
    if(element != NULL)
    {
        if(debug > 0)
            debug_print_message("Option %s already in optionlist\n", name);
        ((Option*)element)->value = int_value;
        ((Option*)element)->string_value = (string_value == NULL) ? NULL : g_strdup(string_value);
        return;
    }

    new.name = g_strdup(name);
    new.value = int_value;
    new.string_value = (string_value == NULL) ? NULL : g_strdup(string_value);

    if(optionlist->list == NULL)
    {
	optionlist->list = g_array_new(FALSE, FALSE, sizeof(Option));
	g_datalist_init(&optionlist->datalist);
    }

    g_array_append_val(optionlist->list, new);
    
    for(i=0;i<optionlist->list->len;i++)
	g_datalist_set_data(&optionlist->datalist, 
			    g_array_index(optionlist->list, Option, i).name,
			    &g_array_index(optionlist->list, Option, i));
}

gint
option_compare_func(gconstpointer a, gconstpointer b)
{
    return misc_alphabetic_compare(((const Option*)a)->name, ((const Option*)b)->name);
}
