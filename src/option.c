#include "option.h"
#include "variables.h"

/** Return the string going with the option
    named 'name'.
    @param name The name of the option.
    @return The string_value of the option.
    @see #Option */
gchar*
option_string(gchar *name, GArray *option_array)
{
    gint i;

    for(i=0;i<option_array->len;i++)
	if(strcmp(g_array_index(option_array, Option, i).name->str, name) == 0)
	    return g_array_index(option_array, Option, i).string_value->str;

    g_warning("option_string: option named %s not found\n", name);

    return NULL;
}

/** Return the integer going with the option
    named 'name'.
    @param name The name of the option.
    @return The value of the option.
    @see #Option */
gint
option_int(gchar *name, GArray *option_array)
{
    gint i;

    for(i=0;i<option_array->len;i++)
	if(strcmp(g_array_index(option_array, Option, i).name->str, name) == 0)
	    return g_array_index(option_array, Option, i).value;

    g_warning("option_int: option named %s not found\n", name);

    return -1;
}

/** Return the int going with the option named 'name'
    cast to float and divided by 1000.
    @param name The name of the option.
    @return The value of the option cast to float and divided by 1000.
    @see #Option */
gfloat
option_float(gchar *name, GArray *option_array)
{
    gint i;
    
    for(i=0;i<option_array->len;i++)
	if(strcmp(g_array_index(option_array, Option, i).name->str, name) == 0)
	    return (gfloat)g_array_index(option_array, Option, i).value / 1000;

    g_warning("option_float: option named %s not found\n", name);

    return -1;
}

/** Change the value of a string option in the array.
    @param name The name of the option.
    @param option_array The option array.
    @param new_value The value we set. */
void
option_set_string(gchar *name, GArray *option_array, gchar *new_value)
{
    gint i;

    for(i=0;i<option_array->len;i++)
	if(strcmp(g_array_index(option_array, Option, i).name->str, name) == 0)
	{
		g_string_printf(g_array_index(option_array, Option, i).string_value,
				"%s", new_value);
		return;
	}

    g_warning("option_set_string: option named %s not found\n", name);
}

/** Change the value of an int option in the array.
    @param name The name of the option.
    @param option_array The option array.
    @param new_value The value we set. */
void
option_set_int(gchar *name, GArray *option_array, gint new_value)
{
    gint i;
    
    for(i=0;i<option_array->len;i++)
	if(strcmp(g_array_index(option_array, Option, i).name->str, name) == 0)
	{
	    g_array_index(option_array, Option, i).value = new_value;
	    return;
	}

    g_warning("option_set_int: option named %s not found\n", name);
}
