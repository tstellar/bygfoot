/*
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

#include <locale.h>


#include "callbacks.h"
#include "free.h"
#include "language.h"
#include "lg_commentary.h"
#include "misc.h"
#include "option.h"
#include "variables.h"
#include "window.h"

#ifndef G_OS_UNIX
extern int _nl_msg_cat_cntr=0;
#endif

/** Set the game language to the specified one. */
void
language_set(gint index)
{
    gchar buf[SMALL], buf2[SMALL];
    gchar *pwd = g_get_current_dir();
    GPtrArray *codes = 
	misc_separate_strings(const_str("string_language_codes"));

    if(index > 0)
	strcpy(buf, (gchar*)g_ptr_array_index(codes, index - 1));
    else
	strcpy(buf, "");

    if(strcmp(buf, opt_str("string_opt_language_code")) != 0 ||
       window.main == NULL)
    {
#ifdef ENABLE_NLS
	sprintf(buf2, "%s%slocale", pwd, G_DIR_SEPARATOR_S);
	g_free(pwd);
	if(g_file_test(buf2, G_FILE_TEST_EXISTS))
	{
	    bindtextdomain (GETTEXT_PACKAGE, buf2);
	    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	    textdomain (GETTEXT_PACKAGE);
	}
#endif

	g_setenv ("LANGUAGE", buf, TRUE);
	opt_set_str("string_opt_language_code", buf);

	{
	    extern int  _nl_msg_cat_cntr;
	    ++_nl_msg_cat_cntr;
	}

	if(window.main != NULL)
	{
	    window_destroy(&window.main, FALSE);
	    window_create(WINDOW_MAIN);
	    on_button_back_to_main_clicked(NULL, NULL);
	}
    }

    lg_commentary_load_commentary_file_from_option();

    free_gchar_array(&codes);
}

/** Get the index of the given code in the codes
    array. */
gint
language_get_code_index(const gchar *code)
{
    gint i, return_value = -1;
    GPtrArray *codes = 
	misc_separate_strings(const_str("string_language_codes"));
    gchar local_code[SMALL];

    strcpy(local_code, code);

    if(strcmp(code, "en") == 0)
	strcpy(local_code, "C");
    
    for(i=0;i<codes->len;i++)
	if(strcmp(local_code, (gchar*)g_ptr_array_index(codes, i)) == 0)
	{
	    return_value = i;
	    break;
	}

    free_gchar_array(&codes);

    return return_value;
}
