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

#include "fixture.h"
#include "league.h"
#include "live_game.h"
#include "option.h"
#include "support.h"
#include "treeview2.h"
#include "treeview_helper.h"
#include "user.h"

void
treeview2_create_mmatches(GtkListStore *ls)
{
    gint i;
    GtkTreeIter iter;
    gchar result[SMALL];

    for(i=0;i<current_user.mmatches->len;i++)
    {
	live_game_unit_result_to_buf(
	    &g_array_index(
		g_array_index(current_user.mmatches, MemMatch, i).lg.units,
		LiveGameUnit, 
		g_array_index(current_user.mmatches, MemMatch, i).lg.units->len - 1), result,
	    (g_array_index(current_user.mmatches, MemMatch, i).user_team == 1));
	gtk_list_store_append(ls, &iter);
	gtk_list_store_set(ls, &iter, 0, i + 1,
			   1, &g_array_index(current_user.mmatches, MemMatch, i),
			   2, &g_array_index(current_user.mmatches, MemMatch, i),
			   3, result, 
			   4, g_array_index(current_user.mmatches, MemMatch, i).
			   competition_name->str, 
			   5, g_array_index(current_user.mmatches, MemMatch, i).
			   country_name, -1);
	gtk_list_store_set(ls, &iter, 
			   TREEVIEW_MMATCH_COL_REPLAY, _("REPLAY"), 
			   TREEVIEW_MMATCH_COL_REMOVE, _("REMOVE"),
			   TREEVIEW_MMATCH_COL_EXPORT, _("EXPORT"),
			   -1);
    }

    gtk_list_store_append(ls, &iter);
    gtk_list_store_set(ls, &iter, 0, const_int("int_treeview_helper_int_empty"),
		       1, NULL, -1);
}

void
treeview2_set_up_mmatches(GtkTreeView *treeview)
{
    gint i;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    gchar *titles[6] =
	{"",
	 _("Your team"),
	 _("Opponent"),
	 _("Result"),
	 _("Competition"),
	 _("Country")};

    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(treeview),
				GTK_SELECTION_NONE);
    gtk_tree_view_set_headers_visible(treeview, TRUE);

    col = gtk_tree_view_column_new();
    gtk_tree_view_append_column(treeview, col);
    renderer = treeview_helper_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_set_cell_data_func(col, renderer,
					    treeview_helper_int_to_cell,
					    NULL, NULL);

    for(i=1;i<3;i++)
    {
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, titles[i]);
	gtk_tree_view_append_column(treeview, col);
	renderer = treeview_helper_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_set_cell_data_func(col, renderer,
						treeview_helper_mm_teams,
						NULL, NULL);
    }

    for(i=3;i<TREEVIEW_MMATCH_COL_END;i++)
    {
	col = gtk_tree_view_column_new();
	gtk_tree_view_append_column(treeview, col);
	renderer = treeview_helper_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, FALSE);

	if(i < TREEVIEW_MMATCH_COL_REPLAY)
	{
	    gtk_tree_view_column_set_title(col, titles[i]);
	    gtk_tree_view_column_add_attribute(col, renderer,
					       "text", i);
	}
	else
	{
	    gtk_tree_view_column_set_cell_data_func(col, renderer,
						    treeview_helper_mm_teams,
						    NULL, NULL);
	    g_object_set(renderer, "xalign", 0.5, NULL);
	}
    }
}

/** Show the list of memorable matches of a user. */
void
treeview2_show_mmatches(void)
{
    GtkTreeView *treeview = 
	GTK_TREE_VIEW(lookup_widget(window.mmatches, "treeview_mmatches"));
    GtkListStore *model = 
	gtk_list_store_new(9, G_TYPE_INT, G_TYPE_POINTER, G_TYPE_POINTER,
			   G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, 
			   G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    
    treeview_helper_clear(treeview);
    
    treeview2_set_up_mmatches(treeview);

    treeview2_create_mmatches(model);
    gtk_tree_view_set_model(treeview, GTK_TREE_MODEL(model));
    g_object_unref(model);
}

void
treeview2_create_season_results(GtkListStore *ls)
{
    gint i;
    GtkTreeIter iter;
    GPtrArray *results = fixture_get_season_results();

    for(i=0;i<results->len;i++)
    {	
	gtk_list_store_append(ls, &iter);
	gtk_list_store_set(
	    ls, &iter,
	    0, ((Fixture*)g_ptr_array_index(results, i))->week_number,
	    1, ((Fixture*)g_ptr_array_index(results, i))->week_round_number,
	    2, league_cup_get_name_string(((Fixture*)g_ptr_array_index(results, i))->clid),
	    3, g_ptr_array_index(results, i),
	    4, g_ptr_array_index(results, i),
	    5, g_ptr_array_index(results, i),
	    -1);
    }

    g_ptr_array_free(results, TRUE);
}

void
treeview2_set_up_season_results(GtkTreeView *treeview)
{
    gint i;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    gchar *titles[6] =
	/* Week */
	{_("We"),
	 /* Round */
	 _("Ro"),
	 _("Competition"),
	 _("Opponent"),
	 "",
	 _("Result")};

    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(treeview),
				GTK_SELECTION_NONE);
    gtk_tree_view_set_headers_visible(treeview, TRUE);

    for(i=0;i<3;i++)
    {
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, titles[i]);
	gtk_tree_view_append_column(treeview, col);
	renderer = treeview_helper_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer,
					   "text", i);
	if(i<2)
	{
	    gtk_tree_view_column_set_alignment(col, 0.5);
	    g_object_set(renderer, "xalign", 0.5, NULL);
	}
    }

    for(i=3;i<6;i++)
    {
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, titles[i]);
	gtk_tree_view_append_column(treeview, col);
	renderer = treeview_helper_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_set_cell_data_func(col, renderer,
						treeview_helper_season_results,
						NULL, NULL);
	if(i == 4)
	{
	    gtk_tree_view_column_set_alignment(col, 0.5);
	    g_object_set(renderer, "xalign", 0.5, NULL);
	}
    }
}

/** Show the user's matches for the complete season. */
void
treeview2_show_season_results(void)
{
    GtkTreeView *treeview = 
	GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right"));
    GtkListStore *model = 
	gtk_list_store_new(6, G_TYPE_INT, G_TYPE_INT, G_TYPE_STRING, 
			   G_TYPE_POINTER, G_TYPE_POINTER, G_TYPE_POINTER);
    
    treeview_helper_clear(treeview);
    
    treeview2_set_up_season_results(treeview);

    treeview2_create_season_results(model);
    gtk_tree_view_set_model(treeview, GTK_TREE_MODEL(model));
    g_object_unref(model);    
}
