#include "main.h"
#include "maths.h"
#include "misc.h"

/**
   Print the contents of a GError (if it was set).
   If abort_program is TRUE, we free the memory
   and exit the game.
   @param error The GError we check.
   @param abort_program Whether or not we continue or exit the program.
*/
void
misc_print_error(GError **error, gboolean abort_program)
{
    gchar buf[SMALL];

    if(*error == NULL)
	return;
    
    sprintf(buf, "%s", (*error)->message);

    g_warning("error message: %s\n", buf);
    /*d*/
/*     show_popup_window(buf); */
    g_error_free(*error);
    *error = NULL;

    if(abort_program)
	main_exit_program(EXIT_PRINT_ERROR, NULL);
}

/** Swap two integers.
    @param first The first integer.
    @param second The second integer. */
void
misc_swap_int(gint *first, gint *second)
{
    gint swap = *first;

    *first = *second;
    *second = swap;
}

/** Swap two pointers.
    @param first The first pointer.
    @param second The second pointer. */
void
misc_swap_gpointer(gpointer *first, gpointer *second)
{
    gpointer swap = *first;

    *first = *second;
    *second = swap;
}

/** Transform a string containing white spaces into an array of strings without
    white spaces. 
    @param string The string containing white spaces.
    @return A GPtrArray containing all the strings without white spaces that were part of the original string.
    This array must be freed with free_g_string_array(). */
GPtrArray*
misc_separate_strings(gchar *string)
{
    gint i, cnt = 0, start = 0;
    gchar buf[BIG];
    GPtrArray *string_array = g_ptr_array_new();
    GString *new_string = NULL;

    for(i=0;i<strlen(string);i++)
	if(g_ascii_isspace(string[i]))
	    start++;
	else
	    break;

    if(start == strlen(string))
    {
	g_warning("misc_separate_strings: input string contains only white spaces\n");
	return string_array;
    }

    for(i=start;i<strlen(string) + 1;i++)
    {
	if(i < strlen(string) && !g_ascii_isspace(string[i]))
	    buf[cnt++] = string[i];
	else
	{
	    buf[cnt] = '\0';
	    cnt = 0;
	    if(strlen(buf) > 0)
	    {
		new_string = g_string_new(buf);
		g_ptr_array_add(string_array, (gpointer)new_string);
	    }
	}
    }

    return string_array;
}

/** Write a pointer array randomly into another one and free
    the original one.
    @param array The array to randomise.
    @return A new pointer array containing the items in random order. */
GPtrArray*
misc_randomise_g_pointer_array(GPtrArray *array)
{
    GPtrArray *new = g_ptr_array_new();
    gint order[array->len];
    gint i;

    math_generate_permutation(order, 0, array->len - 1);

    for(i=0;i<array->len;i++)
	g_ptr_array_add(new, g_ptr_array_index(array, order[i]));

    g_ptr_array_free(array, TRUE);

    return new;
}

/** Print a thousands-grouped output of 'number' into 'buf',
    like 2 234 345 instead of 2234345.
    @param number The number to print. 
    @buf The buffer to hold the number.
    @append Whether to overwrite the buffer or append. */
void
misc_print_grouped_int(gint number, gchar *buf, gboolean append)
{
    gint i;
    gchar buf2[SMALL];
    gint length = 0;
    gfloat copy = (gfloat)(abs(number));
    gint number2 = abs(number);

    if(!append)
      strcpy(buf, "");

    while(copy >= 1)
    {
	copy /= 10;
	length++;
    }

    if(length > 9)
    {
	sprintf(buf2, "%d", number);
	strcat(buf, buf2);
	return;
    }

    for(i = length; i > 0; i--)
    {
	sprintf(buf2, "%d", math_get_place(number2, i));
	strcat(buf, buf2);
	if(i % 3 == 1)
	    strcat(buf, " ");
    }

    if(number < 0)
    {
	sprintf(buf2, "- ");
	strcat(buf2, buf);
	sprintf(buf, "%s", buf2);
    }
    else if(number == 0)
	strcat(buf, "0");
}

/** Check whether 'item' is in array 'array' between
    'min' and 'max'.
    @param array The integer list.
    @param min The lower delimitor (inclusive).
    @param max The upper delimitor (exclusive).
    @param item The item we look for. */
gboolean
query_integer_is_in_array(gint item, gint *array, gint min, gint max)
{
    gint i;

    for(i=min;i<max;i++)
	if(item == array[i])
	    return TRUE;
    
    return FALSE;
}

/** Compare two integers. */
gint
misc_int_compare(gint first, gint second)
{
    if(first > second)
	return -1;
    else if(first < second)
	return 1;

    return 0;
}

/** Compare two floats. */
gint
misc_float_compare(gfloat first, gfloat second)
{
    if(first > second)
	return -1;
    else if(first < second)
	return 1;

    return 0;
}
