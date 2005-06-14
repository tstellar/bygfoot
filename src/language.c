#include <locale.h>

#include "callbacks.h"
#include "free.h"
#include "language.h"
#include "misc.h"
#include "option.h"
#include "variables.h"
#include "window.h"

/** Set the game language to the specified one. */
void
language_set(gint index)
{
    gchar buf[SMALL];
    GPtrArray *codes = 
	misc_separate_strings(const_str("string_language_codes"));
    
    if(index > 0)
	strcpy(buf, ((GString*)g_ptr_array_index(codes, index - 1))->str);
    else
	strcpy(buf, "");

    if(strcmp(buf, opt_str("string_opt_language_code")) != 0 ||
       window.main == NULL)
    {
	setlocale(LC_MESSAGES, buf);
	opt_set_str("string_opt_language_code", buf);

	if(window.main != NULL)
	{
	    window_destroy(&window.main, FALSE);
	    window_create(WINDOW_MAIN);	
	    on_button_back_to_main_clicked(NULL, NULL);
	}
    }

    free_g_string_array(&codes);
}

/** Get the index of the given code in the codes
    array. */
gint
language_get_code_index(const gchar *code)
{
    gint i, return_value = -1;
    GPtrArray *codes = 
	misc_separate_strings(const_str("string_language_codes"));
    
    for(i=0;i<codes->len;i++)
	if(strcmp(opt_str("string_opt_language_code"),
		  ((GString*)g_ptr_array_index(codes, i))->str) == 0)
	{
	    return_value = i;
	    break;
	}

    free_g_string_array(&codes);

    return return_value;
}
