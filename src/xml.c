#include "cup.h"
#include "file.h"
#include "free.h"
#include "gui.h"
#include "league.h"
#include "misc.h"
#include "option.h"
#include "support.h"
#include "table.h"
#include "transfer_struct.h"
#include "user.h"
#include "variables.h"
#include "xml.h"
#include "xml_loadsave_cup.h"
#include "xml_loadsave_fixtures.h"
#include "xml_loadsave_league.h"
#include "xml_loadsave_league_stat.h"
#include "xml_loadsave_live_game.h"
#include "xml_loadsave_table.h"
#include "xml_loadsave_teams.h"
#include "xml_loadsave_transfers.h"
#include "xml_loadsave_users.h"

void
xml_load_users(const gchar *dirname, const gchar *basename)
{
    gint i;
    gchar buf[SMALL];

    free_users(TRUE);

    xml_loadsave_users_read(dirname, basename);

    for(i=0;i<users->len;i++)
    {
	sprintf(buf, "%s/%s___user_%02d_options", dirname, basename, i);
	file_load_opt_file(buf, &usr(i).options);

	sprintf(buf, "%s/%s___user_%02d_live_game.xml", dirname, basename, i);
	xml_loadsave_live_game_read(buf, &usr(i).live_game);
    }
}

void
xml_load_leagues(const gchar *dirname, const gchar *basename)
{
    gint i;
    gchar buf[SMALL];
    GPtrArray *dir_contents = NULL;

    sprintf(buf, "%s___league_", basename);
    dir_contents = file_dir_get_contents(dirname, buf, ".xml");

    free_leagues_array(&ligs, TRUE);
    
    for(i=0;i<dir_contents->len;i++)
	if(!g_str_has_suffix((gchar*)g_ptr_array_index(dir_contents, i), "_table.xml") && 
	   !g_str_has_suffix((gchar*)g_ptr_array_index(dir_contents, i), "_fixtures.xml") &&
	   !g_str_has_suffix((gchar*)g_ptr_array_index(dir_contents, i), "_teams.xml") &&
	   !g_str_has_suffix((gchar*)g_ptr_array_index(dir_contents, i), "_stat.xml") &&
	   !g_strrstr((gchar*)g_ptr_array_index(dir_contents, i), "_promcup"))
	    xml_load_league(dirname, (gchar*)g_ptr_array_index(dir_contents, i),
			    dir_contents);
    
    free_gchar_array(&dir_contents);
}

void
xml_load_league(const gchar *dirname, const gchar *basename, const GPtrArray *dir_contents)
{
    gchar buf[SMALL];
    League new = league_new(FALSE);
    gchar *prefix = g_strndup(basename, strlen(basename) - 4);

    sprintf(buf, "%s/%s", dirname, basename);
    xml_loadsave_league_read(buf, &new);

    g_array_append_val(ligs, new);

    sprintf(buf, _("Loading league: %s"),
	    new.name->str);
    gui_show_progress(
	gtk_progress_bar_get_fraction(
	    GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar"))), buf);

    if(debug > 80)
	printf("%s\n", buf);

    sprintf(buf, "%s/%s_teams.xml", dirname, prefix);
    xml_loadsave_teams_read(buf, lig(ligs->len - 1).teams);

    sprintf(buf, "%s/%s_fixtures.xml", dirname, prefix);
    xml_loadsave_fixtures_read(buf, lig(ligs->len - 1).fixtures);

    sprintf(buf, "%s/%s_table.xml", dirname, prefix);
    xml_loadsave_table_read(buf, &lig(ligs->len - 1).table);

    sprintf(buf, "%s/%s_stat.xml", dirname, prefix);
    xml_loadsave_league_stat_read(buf, &lig(ligs->len - 1).stats);

    g_free(prefix);
}

void
xml_load_cups(const gchar *dirname, const gchar *basename)
{
    gint i;
    gchar buf[SMALL];
    GPtrArray *dir_contents = NULL;
    Cup new_cup;

    sprintf(buf, "%s___cup_", basename);
    dir_contents = file_dir_get_contents(dirname, buf, ".xml");

    free_cups_array(&cps, TRUE);
    
    for(i=0;i<dir_contents->len;i++)
    {
	if(!g_strrstr((gchar*)g_ptr_array_index(dir_contents, i), "_table") &&
	   !g_str_has_suffix((gchar*)g_ptr_array_index(dir_contents, i), "_fixtures.xml") &&
	   !g_str_has_suffix((gchar*)g_ptr_array_index(dir_contents, i), "_teams.xml"))
	{
	    new_cup = cup_new(FALSE);
	    g_array_append_val(cps, new_cup);
	    xml_load_cup(&g_array_index(cps, Cup, cps->len - 1), dirname,
			 (gchar*)g_ptr_array_index(dir_contents, i),
			 dir_contents);
	}
    }
    
    free_gchar_array(&dir_contents);
}

void
xml_load_cup(Cup *cup, const gchar *dirname, const gchar *basename, const GPtrArray *dir_contents)
{
    gchar buf[SMALL];
    gchar *prefix = g_strndup(basename, strlen(basename) - 4);

    sprintf(buf, "%s/%s", dirname, basename);
    xml_loadsave_cup_read(buf, cup);

    sprintf(buf, _("Loading cup: %s"),
	    cup->name->str);
    gui_show_progress(
	gtk_progress_bar_get_fraction(
	    GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar"))), buf);

    if(debug > 80)
	printf("%s\n", buf);

    sprintf(buf, "%s/%s_fixtures.xml", dirname, prefix);
    xml_loadsave_fixtures_read(buf, cup->fixtures);

    g_free(prefix);
}

void
xml_load_transfers(const gchar *dirname, const gchar *basename)
{
    gchar buf[SMALL];

    sprintf(buf, "%s/%s___transfer_list.xml", dirname, basename);

    free_g_array(&transfer_list);
    transfer_list = g_array_new(FALSE, FALSE, sizeof(Transfer));

    xml_loadsave_transfers_read(buf);
}
