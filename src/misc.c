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
    This array must be freed with free_gchar_array(). */
GPtrArray*
misc_separate_strings(gchar *string)
{
    gint i, cnt = 0, start = 0;
    gchar buf[BIG];
    GPtrArray *string_array = g_ptr_array_new();

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
		g_ptr_array_add(string_array, (gpointer)g_strdup(buf));
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
    @buf The buffer to hold the number. */
void
misc_print_grouped_int(gint number, gchar *buf)
{
    gint i;
    gchar buf2[SMALL];
    gint length = 0;
    gfloat copy = (gfloat)(abs(number));
    gint number2 = abs(number);

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

/** Check whether the string is in the string array. */
gboolean
query_misc_string_in_array(const gchar *string, GPtrArray *array)
{
    gint i;

    for(i=0;i<array->len;i++)
	if(strcmp(string, (gchar*)g_ptr_array_index(array, i)) == 0)
	    return TRUE;

    return FALSE;
}

/** Replace a token in a string by another string. 
    The replacement should NOT CONTAIN THE TOKEN otherwise
    we end up in an infinite loop. */
void
misc_string_replace_token(gchar *string, const gchar *token, const gchar *replacement)
{
    gchar buf[SMALL], buf2[SMALL];
    gchar *occurrence = NULL;

    if(strlen(string) < strlen(token))
	return;

    occurrence = g_strrstr(string, token);

    if(occurrence != NULL)
    {
	strncpy(buf, string, strlen(string) - strlen(occurrence));
	buf[strlen(string) - strlen(occurrence)] = '\0';
	strcpy(buf2, occurrence + strlen(token));
	sprintf(string, "%s%s%s", buf, replacement, buf2);
	misc_string_replace_token(string, token, replacement);
    }
}

/** Replace sums of the form [1 + 2 - 3] in the string. */
void
misc_replace_sums(gchar *string)
{
    gint i, result = -1;
    gchar buf[SMALL], buf2[SMALL];
    const gchar *buf_return = NULL;    

    strcpy(buf, string);

    for(i=0;i<strlen(buf);i++)
	if(buf[i] == '[')
	{
	    strncpy(buf2, buf, i);
	    buf2[i] = '\0';
	    buf_return = misc_parse_expression(buf + i + 1, &result);
	    sprintf(string, "%s%d%s", buf2, result, buf_return + 1);
	    
	    misc_replace_sums(string);
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

/* skip spaces */
const gchar*
misc_skip_spaces(const gchar* s)
{
   while (g_ascii_isspace(*s)) s++;
   return s;
}

/* read number, skip all leading and trailing spaces */
const gchar*
misc_parse_value(const gchar *s, gint *value)
{
   s = misc_skip_spaces(s);
   *value = 0;
   while (g_ascii_isdigit(*s)) {
      *value = *value * 10 + (*s - '0');
      s++;
   }
   return s;
}

/* parse numeric expression (supports + and -) */
const gchar*
misc_parse_expression(const gchar *s, gint *result)
{
    gint value = 0,
	loop = 1;

    s = misc_parse_value(s, &value);
    *result = value;

    while (loop) {
	s = misc_skip_spaces(s);
	switch(*s) {
	    case '+': 
		s = misc_parse_value(s+1, &value);
		*result += value;
		break;
	    case '-': 
		s = misc_parse_value(s+1, &value);
		*result -= value;
		break;
	    default:
		loop = 0;
	}
    }
    return s;
}

/* parse comparison (supports '<', '>' and '=') */
const gchar*
misc_parse_comparison(const gchar *s, gint *result)
{
   gint value = 0;
   s = misc_parse_expression(s, result);
   s = misc_skip_spaces(s);
   switch(*s) {
      case '<':
            if (*(s+1) == '=')
	    {
	       s = misc_parse_expression(s+2, &value);
	       *result = *result <= value;
	    }
	    else 
	    {
               s = misc_parse_expression(s+1, &value);
	       *result = *result < value;
	    }
	    break;
      case 'L':
            if (*(s+1) == 'E')
	    {
	       s = misc_parse_expression(s+2, &value);
	       *result = *result <= value;
	    }
	    else 
	    {
               s = misc_parse_expression(s+1, &value);
	       *result = *result < value;
	    }
	    break;
       case '=':
	       s = misc_parse_expression(s+1, &value);
	       *result = *result == value;
	   break;
       case '>': 
	   if (*(s+1) == '=')
	   {
	       s = misc_parse_expression(s+2, &value);
	       *result = *result >= value;
	    }
	    else 
	    {
               s = misc_parse_expression(s+1, &value);
	       *result = *result > value;
	    }
	    break;
       case 'G': 
	   if (*(s+1) == 'E')
	   {
	       s = misc_parse_expression(s+2, &value);
	       *result = *result >= value;
	    }
	    else 
	    {
               s = misc_parse_expression(s+1, &value);
	       *result = *result > value;
	    }
	    break;
       case '!':
	   if(*(s + 1) == '=')
	   {
               s = misc_parse_expression(s+2, &value);
	       *result = *result != value;
	   }
	   break;
   }
   return s;
}

const gchar*
misc_parse_and(const gchar *s, gint *result)
{
   gint value = 0;
   s = misc_parse_comparison(s, result);
   s = misc_skip_spaces(s);
   while (*s == 'a' && *(s+1) == 'n' && *(s+2) == 'd') {
      s = misc_parse_comparison(s + 3, &value);
      *result = *result && value;
      s = misc_skip_spaces(s);
   }

   return s;
}

const gchar*
misc_parse(const gchar *s, gint *result)
{
   gint value = 0;
   s = misc_parse_and(s, result);
   s = misc_skip_spaces(s);
   while (*s == 'o' && *(s+1) == 'r') {
       s = misc_parse_and(s + 2, &value);
       *result = *result || value;
       s = misc_skip_spaces(s);
   }
   return s;
}

/** Return a freshly allocated copy of the array. */
GPtrArray*
misc_copy_ptr_array(const GPtrArray *array)
{
    gint i;
    GPtrArray *copy = NULL;

    if(array != NULL)
	copy = g_ptr_array_new();
    else
	return NULL;

    for(i=0;i<array->len;i++)
	g_ptr_array_add(copy, g_ptr_array_index(array, i));

    return copy;
}

/** Free the string if it's non-NULL and assign the contents to it. */
void
misc_string_assign(gchar **string, const gchar *contents)
{
    if(*string != NULL)
	g_free(*string);

    *string = g_strdup(contents);
}
