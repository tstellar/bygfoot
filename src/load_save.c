#include "file.h"
#include "gui.h"
#include "load_save.h"
#include "option.h"
#include "support.h"
#include "variables.h"
#include "xml_loadsave_misc.h"
#include "xml_loadsave_cup.h"
#include "xml_loadsave_league.h"
#include "xml_loadsave_users.h"
#include "xml_loadsave_transfers.h"
#include "xml.h"

#define PROGRESS_MAX 6

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

    gui_show_progress(0, "Saving miscellaneous...");

    xml_loadsave_misc_write(prefix);

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	      GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	"Saving leagues...");

    for(i=0;i<ligs->len;i++)
	xml_loadsave_league_write(prefix, &lig(i));

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	    GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	"Saving cups...");

    for(i=0;i<cps->len;i++)
	xml_loadsave_cup_write(prefix, &cp(i));

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	    GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	"Saving users...");

    xml_loadsave_users_write(prefix);

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	    GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	"Saving transfer list...");

    xml_loadsave_transfers_write(prefix);

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	    GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	"Compressing savegame...");

    sprintf(buf, "%s___", prefix);
    file_compress_files(fullname, buf);

    gui_show_progress(1, "Done.");

    g_free(prefix);
    g_free(fullname);

    gui_show_progress(-1, "");
}

/** Load the game from the specified file. */
void
load_save_load_game(const gchar* filename)
{
    gchar buf[SMALL];
    gchar *basename = g_path_get_basename(filename),
	*dirname = g_path_get_dirname(filename);
    gchar *prefix = g_strndup(basename, strlen(basename) - strlen(const_str("string_save_suffix")));

    gui_show_progress(0, "Uncompressing savegame...");

    file_decompress(filename);

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	      GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	"Loading miscellaneous...");

    xml_loadsave_misc_read(dirname, prefix);

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	      GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	"Loading leagues...");

    xml_load_leagues(dirname, prefix);

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	      GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	"Loading cups...");

    xml_load_cups(dirname, prefix);

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	      GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	"Loading users...");

    xml_load_users(dirname, prefix);

    gui_show_progress(
	((PROGRESS_MAX * gtk_progress_bar_get_fraction(
	      GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar")))) + 1) / PROGRESS_MAX,
	"Loading transfer list...");

    xml_load_transfers(dirname, prefix);

    gui_show_progress(1, "Done.");

    sprintf(buf, "rm -rf %s/%s___*", dirname, prefix);
    file_my_system(buf);

    g_free(basename);
    g_free(dirname);
    g_free(prefix);

    g_string_printf(save_file, "%s", filename);

    gui_show_progress(-1, "");
}
