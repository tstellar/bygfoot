#include "cup.h"
#include "file.h"
#include "free.h"
#include "gui.h"
#include "league.h"
#include "misc.h"
#include "support.h"
#include "table.h"
#include "transfer_struct.h"
#include "user.h"
#include "variables.h"
#include "xml.h"
#include "xml_loadsave_cup.h"
#include "xml_loadsave_fixtures.h"
#include "xml_loadsave_league.h"
#include "xml_loadsave_live_game.h"
#include "xml_loadsave_table.h"
#include "xml_loadsave_teams.h"
#include "xml_loadsave_transfers.h"
#include "xml_loadsave_users.h"

gint
xml_get_tag_from_name(const gchar *element_name)
{
    gchar buf[SMALL];

    misc_truncate_string(element_name, buf, -1);

    return (gint)g_ascii_strtod(buf, NULL);
}

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
	if(!g_str_has_suffix(((GString*)g_ptr_array_index(dir_contents, i))->str, "_table.xml") && 
	   !g_str_has_suffix(((GString*)g_ptr_array_index(dir_contents, i))->str, "_fixtures.xml") &&
	   !g_str_has_suffix(((GString*)g_ptr_array_index(dir_contents, i))->str, "_teams.xml") &&
	   !query_misc_string_contains(((GString*)g_ptr_array_index(dir_contents, i))->str, "_promcup"))
	    xml_load_league(dirname, ((GString*)g_ptr_array_index(dir_contents, i))->str,
			    dir_contents);
    
    free_g_string_array(&dir_contents);
}

void
xml_load_league(const gchar *dirname, const gchar *basename, const GPtrArray *dir_contents)
{
    gint i;
    gchar buf[SMALL];
    League new = league_new();
    gchar *prefix = g_strndup(basename, strlen(basename) - 4);

    sprintf(buf, "%s/%s", dirname, basename);
    xml_loadsave_league_read(buf, &new);

    g_array_append_val(ligs, new);

    sprintf(buf, "Loading league: %s",
	    new.name->str);
    gui_show_progress(
	gtk_progress_bar_get_fraction(
	    GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar"))), buf);

    sprintf(buf, "%s/%s_teams.xml", dirname, prefix);
    xml_loadsave_teams_read(buf, lig(ligs->len - 1).teams);

    sprintf(buf, "%s/%s_fixtures.xml", dirname, prefix);
    xml_loadsave_fixtures_read(buf, lig(ligs->len - 1).fixtures);

    sprintf(buf, "%s/%s_table.xml", dirname, prefix);
    xml_loadsave_table_read(buf, &lig(ligs->len - 1).table);

    if(strlen(lig(ligs->len - 1).prom_rel.prom_games_dest_sid->str) != 0)
    {
	lig(ligs->len - 1).prom_rel.prom_games_cup = cup_new();
	sprintf(buf, "%s_promcup", prefix);

	for(i=0;i<dir_contents->len;i++)
	    if(g_str_has_prefix(((GString*)g_ptr_array_index(dir_contents, i))->str, buf) &&
	       !query_misc_string_contains(((GString*)g_ptr_array_index(dir_contents, i))->str, "_table") &&
	       !g_str_has_suffix(((GString*)g_ptr_array_index(dir_contents, i))->str, "_fixtures.xml") &&
	       !g_str_has_suffix(((GString*)g_ptr_array_index(dir_contents, i))->str, "_teams.xml"))
	    {
		sprintf(buf, "%s/%s", dirname, ((GString*)g_ptr_array_index(dir_contents, i))->str);
		xml_loadsave_cup_read(buf, &lig(ligs->len - 1).prom_rel.prom_games_cup);
		break;
	    }
    }

    g_free(prefix);
}

void
xml_load_cups(const gchar *dirname, const gchar *basename)
{
    gint i;
    gchar buf[SMALL];
    GPtrArray *dir_contents = NULL;

    sprintf(buf, "%s___cup_", basename);
    dir_contents = file_dir_get_contents(dirname, buf, ".xml");

    free_cups_array(&cps, TRUE);
    
    for(i=0;i<dir_contents->len;i++)
    {
	if(!query_misc_string_contains(((GString*)g_ptr_array_index(dir_contents, i))->str, "_table") &&
	   !g_str_has_suffix(((GString*)g_ptr_array_index(dir_contents, i))->str, "_fixtures.xml") &&
	   !g_str_has_suffix(((GString*)g_ptr_array_index(dir_contents, i))->str, "_teams.xml"))
	    xml_load_cup(dirname, ((GString*)g_ptr_array_index(dir_contents, i))->str,
			 dir_contents);
    }
    
    free_g_string_array(&dir_contents);
}

void
xml_load_cup(const gchar *dirname, const gchar *basename, const GPtrArray *dir_contents)
{
    gint i;
    gchar buf[SMALL];
    Cup new = cup_new();
    Table new_table;
    gchar *prefix = g_strndup(basename, strlen(basename) - 4);

    sprintf(buf, "%s/%s", dirname, basename);
    xml_loadsave_cup_read(buf, &new);

    g_array_append_val(cps, new);

    sprintf(buf, "Loading cup: %s",
	    new.name->str);
    gui_show_progress(
	gtk_progress_bar_get_fraction(
	    GTK_PROGRESS_BAR(lookup_widget(window.progress, "progressbar"))), buf);

    if(new.type == CUP_TYPE_INTERNATIONAL)
    {
	sprintf(buf, "%s/%s_teams.xml", dirname, prefix);
	xml_loadsave_teams_read(buf, cp(cps->len - 1).teams);
    }

    sprintf(buf, "%s/%s_fixtures.xml", dirname, prefix);
    xml_loadsave_fixtures_read(buf, cp(cps->len - 1).fixtures);

    for(i=0;i<dir_contents->len;i++)
    {
	if(g_str_has_prefix(((GString*)g_ptr_array_index(dir_contents, i))->str,
			    prefix) &&
	   query_misc_string_contains(((GString*)g_ptr_array_index(dir_contents, i))->str,
				"_table"))
	{
	    new_table = table_new();
	    sprintf(buf, "%s/%s", dirname, 
		    ((GString*)g_ptr_array_index(dir_contents, i))->str);
	    xml_loadsave_table_read(buf, &new_table);
	    g_array_append_val(cp(cps->len - 1).tables, new_table);
	}
    }

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
