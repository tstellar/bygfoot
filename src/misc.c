#include "free.h"
#include "main.h"
#include "misc.h"

/**
   Print the contents of a GError (if it was set).
   If abort_program is TRUE, we free the memory
   and exit the game.
   @param error The GError we check.
   @param abort_program Whether or not we continue or exit the program.
*/
void
misc_print_error(GError *error, gboolean abort_program)
{
    gchar buf[SMALL];

    if(error == NULL)
	return;
    
    sprintf(buf, "%s", error->message);

    g_warning("error message: %s\n", buf);
    /*d*/
/*     show_popup_window(buf); */
    g_error_free(error);

    if(abort_program)
	main_exit_program(EXIT_PRINT_ERROR);
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
