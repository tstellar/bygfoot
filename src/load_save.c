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

#include "callbacks.h"
#include "file.h"
#include "gui.h"
#include "game_gui.h"
#include "language.h"
#include "load_save.h"
#include "misc.h"
#include "option.h"
#include "support.h"
#include "user.h"
#include "variables.h"
#include "window.h"
#include "xml_loadsave_misc.h"
#include "xml_loadsave_cup.h"
#include "xml_loadsave_league.h"
#include "xml_loadsave_leagues_cups.h"
#include "xml_loadsave_season_stats.h"
#include "xml_loadsave_transfers.h"
#include "xml_loadsave_users.h"
#include "xml.h"

#define PROGRESS_MAX 7

/** Save the game to the specified file. */
void
load_save_save_game(const gchar *filename)
{
    gchar buf[SMALL];
    gchar *prefix = (g_str_has_suffix(filename, const_str("string_fs_save_suffix"))) ?
	g_strndup(filename, strlen(filename) - strlen(const_str("string_fs_save_suffix"))) :
	g_strdup(filename);
    gchar *fullname = (g_str_has_suffix(filename, const_str("string_fs_save_suffix"))) ?
	g_strdup(filename) :
	g_strdup_printf("%s%s", filename, const_str("string_fs_save_suffix"));

    misc_string_assign(&save_file, fullname);

    if(g_file_test(fullname, G_FILE_TEST_EXISTS))
	/*todo: replace with g_remove*/
	file_remove_files(fullname);

    if(debug > 60)
	printf("load_save_save options\n");

    gui_show_progress(0, _("Saving options..."));

    sprintf(buf, "%s___options", prefix);
    file_save_opt_file(buf, &options);
    sprintf(buf, "%s___settings", prefix);
    file_save_opt_file(buf, &settings);

    if(debug > 60)
	printf("load_save_save leagues/cups \n");

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	      GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	_("Saving leagues and cups..."));

    xml_loadsave_leagues_cups_write(prefix);

    if(debug > 60)
	printf("load_save_save users \n");

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	      GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	_("Saving users..."));

    xml_loadsave_users_write(prefix);

    if(debug > 60)
	printf("load_save_save transfers \n");

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	      GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	_("Saving transfer list..."));

    xml_loadsave_transfers_write(prefix);

    if(debug > 60)
	printf("load_save_save stats \n");

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	      GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	_("Saving season stats..."));

    xml_loadsave_season_stats_write(prefix);

    if(debug > 60)
	printf("load_save_save misc \n");

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	      GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	_("Saving miscellaneous..."));

    xml_loadsave_misc_write(prefix);

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	      GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	_("Compressing savegame..."));

    sprintf(buf, "%s___", prefix);
    file_compress_files(fullname, buf);

    if(debug > 60)
	printf("load_save_save done \n");

    gui_show_progress(1, _("Done."));

    load_save_last_save_set(fullname);

    g_free(prefix);
    g_free(fullname);

    gui_show_progress(-1, "");
    setsav1;
}

/** Load the game from the specified file.
    @param create_main_window Whether to create and show the main window. */
gboolean
load_save_load_game(const gchar* filename, gboolean create_main_window)
{
    gchar buf[SMALL];
    gchar *fullname = (g_str_has_suffix(filename, const_str("string_fs_save_suffix"))) ?
	g_strdup(filename) :
	g_strdup_printf("%s%s", filename, const_str("string_fs_save_suffix"));
    gchar *basename = g_path_get_basename(fullname),
	*dirname = g_path_get_dirname(fullname);
    gchar *prefix = (g_str_has_suffix(basename, const_str("string_fs_save_suffix"))) ?
	g_strndup(basename, strlen(basename) - strlen(const_str("string_fs_save_suffix"))) :
	g_strdup(basename);

    if(g_str_has_suffix(filename, "last_save"))
    {
	g_free(basename);
	g_free(dirname);
	g_free(prefix);
	g_free(fullname);

	basename = load_save_last_save_get();

	if(basename != NULL)
	{
	    load_save_load_game(basename, create_main_window);
	    g_free(basename);
	    return TRUE;
	}
	else
	{
	    game_gui_show_warning(_("Last save file not found."));
	    return FALSE;
	}
    }

    if(window.main != NULL)
	gtk_widget_hide(window.main);

    gui_show_progress(0, _("Uncompressing savegame..."));

    file_decompress(fullname);

    if(debug > 60)
	printf("load_save_load options\n");

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	      GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	_("Loading options..."));

    sprintf(buf, "%s%s%s___options", dirname, G_DIR_SEPARATOR_S, prefix);
    file_load_opt_file(buf, &options);
    sprintf(buf, "%s%s%s___settings", dirname, G_DIR_SEPARATOR_S, prefix);
    file_load_opt_file(buf, &settings);
    language_set(language_get_code_index(opt_str("string_opt_language_code")) + 1);

    if(debug > 60)
	printf("load_save_load leagues \n");

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	      GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	_("Loading leagues and cups..."));

    xml_loadsave_leagues_cups_read(dirname, prefix);

    if(debug > 60)
	printf("load_save_load users \n");

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	      GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	_("Loading users..."));

    xml_load_users(dirname, prefix);

    if(debug > 60)
	printf("load_save_load transfers \n");

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	      GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	_("Loading transfer list..."));

    xml_load_transfers(dirname, prefix);

    if(debug > 60)
	printf("load_save_load stats \n");

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	      GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	_("Loading season stats..."));

    xml_loadsave_season_stats_read(dirname, prefix);

    if(debug > 60)
	printf("load_save_load misc \n");

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	      GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	_("Loading miscellaneous..."));

    xml_loadsave_misc_read(dirname, prefix);

    if(debug > 60)
	printf("load_save_load done \n");

    gui_show_progress(1, _("Done."));

    sprintf(buf, "%s%s%s___*", dirname, G_DIR_SEPARATOR_S, prefix);
    file_remove_files(buf);

    misc_string_assign(&save_file, fullname);

    load_save_last_save_set(fullname);

    gui_show_progress(-1, "");

    if(create_main_window)
    {
	window_create(WINDOW_MAIN);
	on_button_back_to_main_clicked(NULL, NULL);
    }
    else if(window.main != NULL)
    {
	gtk_widget_show(window.main);
	window_main_load_geometry();
    }

    g_free(basename);
    g_free(dirname);
    g_free(prefix);
    g_free(fullname);

    return TRUE;
}

/** Store the name of the last savegame in the users home dir. */
void
load_save_last_save_set(const gchar *filename)
{
    gchar buf[SMALL];
    const gchar *home = g_get_home_dir();
    FILE *fil = NULL;

    if(os_is_unix)
	sprintf(buf, "%s%s%s%ssaves%slast_save", home, G_DIR_SEPARATOR_S,
		HOMEDIRNAME, G_DIR_SEPARATOR_S, G_DIR_SEPARATOR_S);
    else
    {
	gchar *pwd = g_get_current_dir();
	sprintf(buf, "%s%ssaves%slast_save", pwd, G_DIR_SEPARATOR_S,
		G_DIR_SEPARATOR_S);
	g_free(pwd);
    }

    if(!file_my_fopen(buf, "w", &fil, FALSE))
	return;

    fprintf(fil, "%s", filename);

    fclose(fil);
}

/** Return the filename of the last savegame. */
gchar*
load_save_last_save_get(void)
{
    gchar buf[SMALL];
    const gchar *home = g_get_home_dir();
    FILE *fil = NULL;
    gint i = 0, c;

    if(os_is_unix)
	sprintf(buf, "%s%s%s%ssaves%slast_save", home, G_DIR_SEPARATOR_S,
		HOMEDIRNAME, G_DIR_SEPARATOR_S,  G_DIR_SEPARATOR_S);
    else
    {
	gchar *pwd = g_get_current_dir();
	sprintf(buf, "%s%ssaves%slast_save", pwd, G_DIR_SEPARATOR_S,
		G_DIR_SEPARATOR_S);
	g_free(pwd);
    }

    if(!file_my_fopen(buf, "r", &fil, FALSE))
	return NULL;

    while ((c = (gchar)fgetc(fil)) != EOF)
	buf[i++] = (gchar)c;
    buf[i] = 0;    

    fclose(fil);

    return g_strdup(buf);
}

/** Write an autosave. */
void
load_save_autosave(void)
{
    gchar buf[SMALL];
    const gchar *home = g_get_home_dir();
    FILE *fil = NULL;
    
    if(!opt_int("int_opt_autosave") ||
       !query_user_games_this_week_round())
	return;

    counters[COUNT_AUTOSAVE] = (counters[COUNT_AUTOSAVE] + 1) % opt_int("int_opt_autosave_interval");

    if(counters[COUNT_AUTOSAVE] != 0)
	return;

    if(os_is_unix)
	sprintf(buf, "%s%s%s%ssaves%sautosave%02d.zip", home, G_DIR_SEPARATOR_S,
		HOMEDIRNAME, G_DIR_SEPARATOR_S, G_DIR_SEPARATOR_S,
		counters[COUNT_AUTOSAVE_FILE]);
    else
    {
	gchar *pwd = g_get_current_dir();
	sprintf(buf, "%s%ssaves%sautosave%02d.zip", pwd, G_DIR_SEPARATOR_S,
		G_DIR_SEPARATOR_S, counters[COUNT_AUTOSAVE_FILE]);
	g_free(pwd);
    }

    if(!file_my_fopen(buf, "w", &fil, FALSE))
	return;

    fclose(fil);

    load_save_save_game(buf);

    counters[COUNT_AUTOSAVE_FILE] = (counters[COUNT_AUTOSAVE_FILE] + 1) % opt_int("int_opt_autosave_files");
}

/** Try to load a savegame given on the command line. */
gboolean
load_game_from_command_line(const gchar *filename)
{
    gchar *fullname = NULL,
	*support_file_name = NULL;

    if(strcmp(filename, "last_save") == 0)
	return load_save_load_game(filename, TRUE);

    fullname = (g_str_has_suffix(filename, const_str("string_fs_save_suffix"))) ?
	g_strdup(filename) :
	g_strdup_printf("%s%s", filename, const_str("string_fs_save_suffix"));

    if(g_file_test(fullname, G_FILE_TEST_EXISTS))
    {
	if(load_save_load_game(fullname, TRUE))
	{
	    g_free(fullname);
	    return TRUE;
	}
	else
	    return FALSE;
    }

    support_file_name = file_find_support_file(fullname, FALSE);

    if(g_file_test(support_file_name, G_FILE_TEST_EXISTS))
    {
	if(load_save_load_game(support_file_name, TRUE))
	{
	    g_free(fullname);
	    g_free(support_file_name);
	    return TRUE;
	}
	else
	    return FALSE;
    }
    
    g_warning("Could not find file %s.\n", fullname);
    g_free(fullname);

    return FALSE;
}
