/*
   treeview2.c

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

#include "bet.h"
#include "fixture.h"
#include "job.h"
#include "league.h"
#include "live_game.h"
#include "misc.h"
#include "option.h"
#include "support.h"
#include "team.h"
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

void
treeview2_create_bets(GtkListStore *ls)
{
    gint i, j, k, rank;
    GtkTreeIter iter;
    gchar buf[SMALL], buf2[SMALL],
	team_names[2][SMALL];
    const BetMatch *bet = NULL;
    const BetUser *bet_user = NULL;
    const Fixture *fix = NULL;
    
    for(k=1;k>=0;k--)
    {
	if(bets[k]->len > 0)
	{
	    gtk_list_store_append(ls, &iter);
	    gtk_list_store_set(ls, &iter, 
			       0, (k == 1) ?
			       /* Copy the <span...> stuff, translate in between. */
			       _("<span background='lightgrey' size='large' weight='bold'>Current bets</span>") :
			       /* Copy the <span...> stuff, translate in between. */
			       _("<span background='lightgrey' size='large' weight='bold'>Recent bets</span>"),
			       1, NULL, 2, NULL, 3, NULL, 4, "", 5, "", -1);
	}

	for(i=0;i<bets[k]->len;i++)
	{
	    fix = fixture_from_id(g_array_index(bets[k], BetMatch, i).fix_id);
	    if(fix->clid == current_user.tm->clid ||
	       (fix->clid >= ID_CUP_START &&
		opt_user_int("int_opt_user_bet_show_cups")) ||
	       (fix->clid < ID_CUP_START &&
		opt_user_int("int_opt_user_bet_show_all_leagues")))
	    {
		bet = &g_array_index(bets[k], BetMatch, i);
		bet_user = bet_is_user(bet);
		strcpy(buf, "");

		if(k == 1 || (k == 0 && 
		   (bet_user != NULL ||
		    !opt_user_int("int_opt_user_bet_show_my_recent"))))
		{
		    if(i == 0 || fix->clid != 
		       fixture_from_id(g_array_index(bets[k], BetMatch, i - 1).fix_id)->clid)
		    {
			if(i > 0)
			{
			    gtk_list_store_append(ls, &iter);
			    gtk_list_store_set(ls, &iter, 0, "", 
					       1, NULL, 2, NULL, 3, NULL, 4, "", 5, "", -1);
			}
	    
			gtk_list_store_append(ls, &iter);
			gtk_list_store_set(ls, &iter, 
					   0, league_cup_get_name_string(fix->clid),
					   1, NULL, 2, NULL, 3, NULL, 4, "", 5, "", -1);
		    }
	
		    if(bet_user != NULL)
		    {
			misc_print_grouped_int(bet_user->wager, buf2);
	    
			if(bet_user->wager > 0)
			    strcpy(buf, buf2);
			else
			    sprintf(buf, "<span foreground='%s'>%s</span>",
				    const_app("string_treeview_finances_expenses_fg"), buf2);
		    }

		    for(j=0;j<2;j++)
			if(query_fixture_has_tables(fix))
			{
			    if(fix->clid < ID_CUP_START)
				rank = team_get_league_rank(fix->teams[j]);
			    else
				rank = team_get_cup_rank(fix->teams[j], 
							 cup_get_last_tables_round(fix->clid), TRUE);

			    sprintf(team_names[j], "%s [%d]",
				    fix->teams[j]->name, rank);
			}
			else if(fix->clid >= ID_CUP_START &&
				query_cup_is_national(fix->clid))
			    sprintf(team_names[j], "%s (%d)",
				    fix->teams[j]->name,
				    league_from_clid(fix->teams[j]->clid)->layer);
			else
			    strcpy(team_names[j], fix->teams[j]->name);

		    gtk_list_store_append(ls, &iter);
		    gtk_list_store_set(ls, &iter, 0, team_names[0],
				       1, bet, 2, bet, 3, bet,
				       4, team_names[1],
				       5, buf, -1);
		}
	    }
	}
    }
}

void
treeview2_set_up_bets(GtkTreeView *treeview)
{
    gint i;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    gchar *titles[6] =
	{_("Team1"),
	 /* Team 1 wins (betting window). */
	 _("Win1"),
	 _("Draw"),
	 /* Team 2 wins (betting window). */
	 _("Win2"),
	 _("Team2"),
	 /* How much the user wagers; how much he won or lost. */
	 _("Wager/\nWin/Loss")};

    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(treeview),
				GTK_SELECTION_SINGLE);
    gtk_tree_view_set_headers_visible(treeview, TRUE);

    col = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(col, titles[0]);
    gtk_tree_view_append_column(treeview, col);
    renderer = treeview_helper_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "markup", 0);

    gtk_tree_view_column_set_alignment(col, 1.0);
    g_object_set(renderer, "xalign", 1.0, NULL);

    for(i=1;i<4;i++)
    {
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, titles[i]);
	gtk_tree_view_append_column(treeview, col);
	renderer = treeview_helper_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_set_cell_data_func(col, renderer,
						treeview_helper_bet_odds,
						NULL, NULL);
    }

    col = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(col, titles[4]);
    gtk_tree_view_append_column(treeview, col);
    renderer = treeview_helper_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "text", 4);

    col = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(col, titles[5]);
    gtk_tree_view_append_column(treeview, col);
    renderer = treeview_helper_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "markup", 5);
    gtk_tree_view_column_set_alignment(col, 0.5);
    g_object_set(renderer, "xalign", 0.5, NULL);
}

/** Show the current and recent bets in the betting window. */
void
treeview2_show_bets(void)
{
    GtkTreeView *treeview = 
	GTK_TREE_VIEW(lookup_widget(window.bets, "treeview_bets"));
    GtkListStore *model = 
	gtk_list_store_new(6, G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_POINTER, 
			   G_TYPE_POINTER, G_TYPE_STRING, G_TYPE_STRING);
    
    treeview_helper_clear(treeview);
    
    treeview2_set_up_bets(treeview);

    treeview2_create_bets(model);
    gtk_tree_view_set_model(treeview, GTK_TREE_MODEL(model));
    g_object_unref(model);
}

void
treeview2_create_job_exchange(GtkListStore *ls)
{
    gint i;
    GtkTreeIter iter;
    gchar buf[SMALL];
    const Team *tm = NULL;

    for(i=0;i<jobs->len;i++)
    {
	tm = job_get_team(&g_array_index(jobs, Job, i));
	sprintf(buf, "%s (%d)", g_array_index(jobs, Job, i).league_name,
		g_array_index(jobs, Job, i).league_layer);

	gtk_list_store_append(ls, &iter);
	gtk_list_store_set(ls, &iter, 0, i + 1,
			   1, tm->name, 
			   2, buf,
			   3, &g_array_index(jobs, Job, i),
			   4, &g_array_index(jobs, Job, i),
			   5, &g_array_index(jobs, Job, i),
			   -1);
    }
}

void
treeview2_set_up_job_exchange(GtkTreeView *treeview)
{
    gint i;
    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    gchar *titles[6] =
	{"",
	 _("Team"),
	 _("League"),
	 _("Country"),
	 _("Av.skill"),
	 _("Talent %")};

    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(treeview),
				GTK_SELECTION_SINGLE);
    gtk_tree_view_set_headers_visible(treeview, TRUE);

    col = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(col, titles[0]);
    gtk_tree_view_append_column(treeview, col);
    renderer = treeview_helper_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_add_attribute(col, renderer,
				       "text", 0);

    for(i=1;i<3;i++)
    {
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, titles[i]);
	gtk_tree_view_append_column(treeview, col);
	renderer = treeview_helper_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer,
					   "text", i);
    }

    for(i=3;i<6;i++)
    {
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, titles[i]);
	gtk_tree_view_append_column(treeview, col);
	renderer = treeview_helper_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);	
	gtk_tree_view_column_set_cell_data_func(col, renderer,
						treeview_helper_job_exchange,
						NULL, NULL);	
	if(i > 3)
	{
	    gtk_tree_view_column_set_alignment(col, 0.5);
	    g_object_set(renderer, "xalign", 0.5, NULL);	    
	}
    }
}

/** Show the teams that offer a job. */
void
treeview2_show_job_exchange(void)
{
    GtkTreeView *treeview = 
	GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right"));
    GtkListStore *model = 
	gtk_list_store_new(6, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, 
			   G_TYPE_POINTER, G_TYPE_POINTER, G_TYPE_POINTER);
    
    treeview_helper_clear(treeview);
    
    treeview2_set_up_job_exchange(treeview);

    treeview2_create_job_exchange(model);
    gtk_tree_view_set_model(treeview, GTK_TREE_MODEL(model));
    g_object_unref(model);    
}
