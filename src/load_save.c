#include "callbacks.h"
#include "file.h"
#include "gui.h"
#include "game_gui.h"
#include "load_save.h"
#include "option.h"
#include "support.h"
#include "user.h"
#include "variables.h"
#include "xml_loadsave_misc.h"
#include "xml_loadsave_cup.h"
#include "xml_loadsave_league.h"
#include "xml_loadsave_season_stats.h"
#include "xml_loadsave_transfers.h"
#include "xml_loadsave_users.h"
#include "xml.h"

#define PROGRESS_MAX 8

/** Save the game to the specified file. */
void
load_save_save_game(const gchar *filename)
{
    gint i;
    gchar buf[SMALL];
    gchar *prefix = (g_str_has_suffix(filename, const_str("string_save_suffix"))) ?
	g_strndup(filename, strlen(filename) - strlen(const_str("string_save_suffix"))) :
	g_strdup(filename);
    gchar *fullname = (g_str_has_suffix(filename, const_str("string_save_suffix"))) ?
	g_strdup(filename) :
	g_strdup_printf("%s%s", filename, const_str("string_save_suffix"));

    g_string_printf(save_file, "%s", fullname);

    if(g_file_test(fullname, G_FILE_TEST_EXISTS))
    {
	/*todo: replace with g_remove*/
	sprintf(buf, "rm -rf %s", fullname);
	file_my_system(buf);
    }

    if(debug > 60)
	printf("load_save_save options/constants \n");

    gui_show_progress(0, _("Saving options/constants..."));

    sprintf(buf, "%s___options", prefix);
    file_save_opt_file(buf, &options);
    sprintf(buf, "%s___constants", prefix);
    file_save_opt_file(buf, &constants);

    if(debug > 60)
	printf("load_save_save leagues \n");

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	      GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	_("Saving leagues..."));

    for(i=0;i<ligs->len;i++)
	xml_loadsave_league_write(prefix, &lig(i));

    if(debug > 60)
	printf("load_save_save cups \n");

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	    GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	_("Saving cups..."));

    for(i=0;i<cps->len;i++)
	xml_loadsave_cup_write(prefix, &cp(i));

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

/** Load the game from the specified file. */
void
load_save_load_game(const gchar* filename)
{
    gchar buf[SMALL];
    gchar *fullname = (g_str_has_suffix(filename, const_str("string_save_suffix"))) ?
	g_strdup(filename) :
	g_strdup_printf("%s%s", filename, const_str("string_save_suffix"));
    gchar *basename = g_path_get_basename(fullname),
	*dirname = g_path_get_dirname(fullname);
    gchar *prefix = (g_str_has_suffix(basename, const_str("string_save_suffix"))) ?
	g_strndup(basename, strlen(basename) - strlen(const_str("string_save_suffix"))) :
	g_strdup(basename);

    gtk_widget_hide(window.main);

    if(strcmp(filename, "last_save") == 0)
    {
	g_free(basename);
	g_free(dirname);
	g_free(prefix);
	g_free(fullname);

	basename = load_save_last_save_get();

	if(basename != NULL)
	{
	    load_save_load_game(basename);
	    g_free(basename);
	}
	else
	    g_warning("load_save_load_game: last save file not found.\n");

	return;
    }

    gui_show_progress(0, _("Uncompressing savegame..."));

    file_decompress(fullname);

    if(debug > 60)
	printf("load_save_load options/constants \n");

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	      GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	_("Loading options/constants..."));

    sprintf(buf, "%s___options", prefix);
    file_load_opt_file(buf, &options);
    sprintf(buf, "%s___constants", prefix);
    file_load_opt_file(buf, &constants);

    if(debug > 60)
	printf("load_save_load leagues \n");

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	      GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	_("Loading leagues..."));

    xml_load_leagues(dirname, prefix);

    if(debug > 60)
	printf("load_save_load cups \n");

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	      GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	_("Loading cups..."));

    xml_load_cups(dirname, prefix);

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

    sprintf(buf, "rm -rf %s/%s___*", dirname, prefix);
    file_my_system(buf);

    g_string_printf(save_file, "%s", fullname);

    load_save_last_save_set(fullname);

    gui_show_progress(-1, "");

    gtk_widget_show(window.main);

    setsav1;

    cur_user = 0;
    on_button_back_to_main_clicked(NULL, NULL);

    g_free(basename);
    g_free(dirname);
    g_free(prefix);
    g_free(fullname);
}

/** Store the name of the last savegame in the users home dir. */
void
load_save_last_save_set(const gchar *filename)
{
    gchar buf[SMALL];
    const gchar *home = g_get_home_dir();
    FILE *fil = NULL;

    sprintf(buf, "%s/%s/saves/last_save", home, HOMEDIRNAME);

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

    sprintf(buf, "%s/%s/saves/last_save", home, HOMEDIRNAME);

    if(!file_my_fopen(buf, "r", &fil, FALSE))
	return NULL;

    fscanf(fil, "%s", buf);

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

    sprintf(buf, "%s/%s/saves/autosave%02d.zip", home, HOMEDIRNAME,
	    counters[COUNT_AUTOSAVE_FILE]);

    if(!file_my_fopen(buf, "w", &fil, FALSE))
	return;

    load_save_save_game(buf);

    counters[COUNT_AUTOSAVE_FILE] = (counters[COUNT_AUTOSAVE_FILE] + 1) % opt_int("int_opt_autosave_files");
}
