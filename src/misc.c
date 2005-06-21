#include <time.h>

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

/** Check whether the number is in the array. */
gboolean
query_misc_integer_is_in_g_array(gint item, GArray *array)
{
    gint i;

    for(i=0;i<array->len;i++)
	if(item == g_array_index(array, gint, i))
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

/** Remove some of the first or last characters from src and copy
    the rest to dest; no error-checking is done. */
void
misc_truncate_string(const gchar *src, gchar *dest, gint number_of_chars)
{
    gint i;
    gint num = ABS(number_of_chars);
    
    if(number_of_chars >= 0)
    {
	strncpy(dest, src, strlen(src) - num);
	dest[strlen(src) - num] = '\0';
	return;
    }

    for(i=0;i<strlen(src);i++)
	if(i >= num)
	    dest[i - num] = src[i];
    
    dest[i - num] = '\0';
}

/** Find out whether the first string contains the second string. */
gboolean
query_misc_string_contains(const gchar *string, const gchar *text)
{
    gint i, j;
    gint lens = strlen(string),
	lent = strlen(text);

    if(lent > lens)
	return FALSE;
       
    if(lent == lens)
	return (strcmp(text, string) == 0);

    for(i=0;i<lens - lent + 1;i++)
    {
	for(j=0;j<lent;j++)
	    if(string[i + j] != text[j])
		break;
	
	if(j == lent)
	    return TRUE;
    }

    return FALSE;
}

/** Check whether the string is in the GString array. */
gboolean
query_misc_string_in_array(const gchar *string, GPtrArray *array)
{
    gint i;

    for(i=0;i<array->len;i++)
	if(strcmp(string, ((GString*)g_ptr_array_index(array, i))->str) == 0)
	    return TRUE;

    return FALSE;
}

/** Replace a token in a string by another string. 
    The replacement should NOT CONTAIN THE TOKEN otherwise
    we end up in an infinite loop. */
void
misc_string_replace_token(gchar *string, const gchar *token, const gchar *replacement)
{
    gint i, j;
    gchar buf[SMALL], buf2[SMALL], rest[SMALL];
    
    for(i=strlen(string); i >= strlen(token); i--)
    {
	strcpy(buf, "");
	strcpy(buf2, "");
	strcpy(rest, "");

	strncpy(buf, string, i);
	buf[i] = '\0';

	for(j=i;j<strlen(string);j++)
	    rest[j - i] = string[j];
	rest[j - i] = '\0';

	if(g_str_has_suffix(buf, token))
	{
	    strncpy(buf2, buf, strlen(buf) - strlen(token));
	    buf2[strlen(buf) - strlen(token)] = '\0';
	    strcat(buf2, replacement);

	    sprintf(string, "%s%s", buf2, rest);
	    misc_string_replace_token(string, token, replacement);
	}
    }
}

/** Get a float representation of someone's age
    based on his birth year and month. */
gfloat
misc_get_age_from_birth(gint birth_year, gint birth_month)
{
    GDate *current_date = g_date_new();
    GDate *birth_date = g_date_new();

    g_date_set_time(current_date, time(NULL));
    g_date_set_dmy(birth_date, 15, birth_month, birth_year);

    return (gfloat)g_date_days_between(birth_date, current_date) / 365.25;
}
