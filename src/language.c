/*
   language.c

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
#include "news.h"
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
#ifdef DEBUG
    printf("language_set\n");
#endif

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
	sprintf(buf2, "%s%slocale", pwd, G_DIR_SEPARATOR_S);
#ifdef ENABLE_NLS
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
	    extern int _nl_msg_cat_cntr;
	    ++_nl_msg_cat_cntr;
	}

	if(window.main != NULL)
	{
	    window_destroy(&window.main);
	    window_create(WINDOW_MAIN);
	    on_button_back_to_main_clicked(NULL, NULL);
	}
    }

    lg_commentary_load_commentary_file_from_option();
    news_load_news_file_from_option();
    g_free(pwd);
    free_gchar_array(&codes);
}

/** Get the index of the given code in the codes
    array. */
gint
language_get_code_index(const gchar *code)
{
#ifdef DEBUG
    printf("language_get_code_index\n");
#endif

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

/** Put the country matching the local language to the
    beginning of the array if possible. */
void
language_pick_country(GPtrArray *country_files)
{
#ifdef DEBUG
    printf("language_pick_country\n");
#endif

    gint i, j;
    GPtrArray *codes =
	misc_separate_strings(const_str("string_language_codes"));
    GPtrArray *defs =
	misc_separate_strings(const_str("string_language_defs"));
    gpointer prefdef = NULL;
    const gchar *lang = g_getenv("LANG");

    if(lang == NULL)
	lang = g_getenv("LC_ALL");

    if(lang != NULL)
	for(i=0;i<codes->len;i++)
	{
	    if(((g_str_has_prefix(lang, "en") && strcmp((gchar*)g_ptr_array_index(codes, i), "C") == 0) ||
		g_str_has_prefix(lang, (gchar*)g_ptr_array_index(codes, i))) &&
	       strcmp((gchar*)g_ptr_array_index(defs, i), "NONE") != 0)
            {
                for(j=0;j<country_files->len;j++)
		    if(g_str_has_suffix((gchar*)g_ptr_array_index(country_files, j),
                                        (gchar*)g_ptr_array_index(defs, i)))
		    {
			prefdef = g_ptr_array_index(country_files, j);
                        g_ptr_array_remove_index(country_files, j);
                        g_ptr_array_add(country_files, prefdef);
			break;
		    }
                break;
            }
	}

    free_gchar_array(&codes);
    free_gchar_array(&defs);
}

/** Find out which language to use (e.g. for live game commentary).
    Write the code (en, de etc.) into the buffer. */
void
language_get_code(gchar *buf)
{
#ifdef DEBUG
    printf("language_get_code\n");
#endif

    gchar *cur_locale = NULL;
#ifdef G_OS_UNIX
     cur_locale = setlocale(LC_MESSAGES, NULL);
#else
    cur_locale = g_win32_getlocale ();
#endif

    if(strcmp(opt_str("string_opt_language_code"), "C") == 0)
	strcpy(buf, "en");
    else if(strcmp(opt_str("string_opt_language_code"), "") == 0 &&
	    cur_locale != NULL)
    {
	strncpy(buf, cur_locale, 2);
	buf[2] = '\0';
    }
    else
	strcpy(buf, opt_str("string_opt_language_code"));
}
