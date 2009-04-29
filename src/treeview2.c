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
#ifdef DEBUG
    printf("treeview2_create_mmatches\n");
#endif

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
#ifdef DEBUG
    printf("treeview2_set_up_mmatches\n");
#endif

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
#ifdef DEBUG
    printf("treeview2_show_mmatches\n");
#endif

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
#ifdef DEBUG
    printf("treeview2_create_season_results\n");
#endif

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
#ifdef DEBUG
    printf("treeview2_set_up_season_results\n");
#endif

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
#ifdef DEBUG
    printf("treeview2_show_season_results\n");
#endif

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
#ifdef DEBUG
    printf("treeview2_create_bets\n");
#endif

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
	    fix = fixture_from_id(g_array_index(bets[k], BetMatch, i).fix_id, TRUE);
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
		       fixture_from_id(
			   g_array_index(bets[k], BetMatch, i - 1).fix_id, TRUE)->clid)
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
				rank = team_get_league_rank(fix->teams[j], fix->clid);
			    else
				rank = team_get_cup_rank(fix->teams[j], 
							 cup_get_last_tables_round(fix->clid), TRUE);

			    sprintf(team_names[j], "%s [%d]",
				    fix->teams[j]->name, rank);
			}
			else if(fix->clid >= ID_CUP_START &&
				query_league_cup_has_property(fix->clid, "national"))
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
#ifdef DEBUG
    printf("treeview2_set_up_bets\n");
#endif

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
#ifdef DEBUG
    printf("treeview2_show_bets\n");
#endif

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
#ifdef DEBUG
    printf("treeview2_create_job_exchange\n");
#endif

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
#ifdef DEBUG
    printf("treeview2_set_up_job_exchange\n");
#endif

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
#ifdef DEBUG
    printf("treeview2_show_job_exchange\n");
#endif

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

void
treeview2_create_news(GtkListStore *ls)
{
#ifdef DEBUG
    printf("treeview2_create_news\n");
#endif

    gint i;
    GtkTreeIter iter;
    gchar buf[SMALL], buf2[SMALL];
    gboolean second_column;

    if(newspaper.articles->len == 0)
    {
	gtk_list_store_append(ls, &iter);
	sprintf(buf, "\n<span %s>%s</span>", 
		const_app("string_news_window_title_attribute"),
		_("No news available."));
	gtk_list_store_set(ls, &iter, 0, buf, 1, NULL, -1);
	return;
    }
    
    for(i = newspaper.articles->len - 1; i >= 0; i--)
    {
        second_column = (i == newspaper.articles->len - 1 ||
                         (i < newspaper.articles->len - 1 &&
                          g_array_index(newspaper.articles, NewsPaperArticle, i).clid !=
                          g_array_index(newspaper.articles, NewsPaperArticle, i + 1).clid));

        if(!opt_int("int_opt_news_show_recent") ||
           g_array_index(newspaper.articles, NewsPaperArticle, i).week_number == week - 1)
        {
            if(i == newspaper.articles->len - 1 ||
               (i < newspaper.articles->len - 1 &&
                (g_array_index(newspaper.articles, NewsPaperArticle, i).week_number !=
                 g_array_index(newspaper.articles, NewsPaperArticle, i + 1).week_number ||
                 g_array_index(newspaper.articles, NewsPaperArticle, i).week_round_number !=
                 g_array_index(newspaper.articles, NewsPaperArticle, i + 1).week_round_number)))
            {
                gtk_list_store_append(ls, &iter);
                gtk_list_store_set(ls, &iter, 0, "", 1, NULL, 2, NULL, -1);

                gtk_list_store_append(ls, &iter);
                sprintf(buf2, _("Week %d Round %d"),
                        g_array_index(newspaper.articles, NewsPaperArticle, i).week_number,
                        g_array_index(newspaper.articles, NewsPaperArticle, i).week_round_number);
                sprintf(buf, "<span %s>%s</span>\n\n",
                        const_app("string_news_window_week_number_attribute"),
                        buf2);

                second_column = TRUE;
            }
            else
            {
                gtk_list_store_append(ls, &iter);
                strcpy(buf, "");
            }

            if(second_column)
                gtk_list_store_set(ls, &iter, 
                                   0, buf, 
                                   1, &g_array_index(newspaper.articles, NewsPaperArticle, i),
                                   2, &g_array_index(newspaper.articles, NewsPaperArticle, i), -1);
            else
                gtk_list_store_set(ls, &iter, 
                                   0, buf, 
                                   1, &g_array_index(newspaper.articles, NewsPaperArticle, i),
                                   2, NULL, -1);
        }
    }
}

void
treeview2_set_up_news(GtkTreeView *treeview)
{
#ifdef DEBUG
    printf("treeview2_set_up_news\n");
#endif

    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    gint i;

    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(treeview),
				GTK_SELECTION_NONE);
    gtk_tree_view_set_headers_visible(treeview, FALSE);
    gtk_tree_view_set_rules_hint(treeview, FALSE);

    for(i = 0; i < 3; i++)
    {
        col = gtk_tree_view_column_new();
        gtk_tree_view_append_column(treeview, col);
        renderer = treeview_helper_cell_renderer_text_new();
        gtk_tree_view_column_pack_start(col, renderer, TRUE);	

        if(i == 0)
            gtk_tree_view_column_add_attribute(col, renderer,
                                               "markup", 0);
        else if(i == 1)
            gtk_tree_view_column_set_cell_data_func(col, renderer,
                                                    treeview_helper_news,
                                                    NULL, NULL);	        
        else
            gtk_tree_view_column_set_cell_data_func(col, renderer,
                                                    treeview_helper_news_additional,
                                                    NULL, NULL);	        

        g_object_set(renderer, "wrap-mode", PANGO_WRAP_WORD, NULL);
        g_object_set(renderer, "wrap-width", 400, NULL);
        g_object_set(renderer, "yalign", 0.0, NULL);
    }
}

/** Show the news in the news treeview. */
void
treeview2_show_news(void)
{
#ifdef DEBUG
    printf("treeview2_show_news\n");
#endif

    GtkTreeView *treeview = 
	GTK_TREE_VIEW(lookup_widget(window.news, "treeview_news"));
    GtkListStore *model = 
	gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_POINTER);
    
    treeview_helper_clear(treeview);
    
    treeview2_set_up_news(treeview);

    treeview2_create_news(model);
    gtk_tree_view_set_model(treeview, GTK_TREE_MODEL(model));
    g_object_unref(model);        
}

/** Types of constants we use in the game, 
    enum needed for displaying purposes. */
enum
{
    CONSTANTS_TYPE_INT = 0,
    CONSTANTS_TYPE_FLOAT,
    CONSTANTS_TYPE_STRING,
    CONSTANTS_TYPE_APP,
    CONSTANTS_TYPE_END
};

GtkTreeModel*
treeview2_create_constants(const GPtrArray *list, gint type)
{
#ifdef DEBUG
    printf("treeview2_create_constants\n");
#endif

    gint i;
    GtkTreeIter iter;
    GtkListStore *ls;

    switch(type)
    {
    default:
        debug_print_message("treeview2_create_constants: unknown constants type\n");
        return NULL;
    case CONSTANTS_TYPE_INT:
        ls = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);
        break;
    case CONSTANTS_TYPE_FLOAT:
        ls = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_FLOAT);
        break;
    case CONSTANTS_TYPE_STRING:
    case CONSTANTS_TYPE_APP:
        ls = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
        break;
    }	

    for(i = 0; i < list->len; i++)
    {
	gtk_list_store_append(ls, &iter);
        
        if(type == CONSTANTS_TYPE_INT)
            gtk_list_store_set(ls, &iter, 
                               0, ((Option*)g_ptr_array_index(list, i))->name, 
                               1, ((Option*)g_ptr_array_index(list, i))->value,
                               -1);        
        else if(type == CONSTANTS_TYPE_FLOAT)
            gtk_list_store_set(ls, &iter, 
                               0, ((Option*)g_ptr_array_index(list, i))->name, 
                               1, (gfloat)((Option*)g_ptr_array_index(list, i))->value / OPTION_FLOAT_DIVISOR,
                               -1);        
        else
            gtk_list_store_set(ls, &iter, 
                               0, ((Option*)g_ptr_array_index(list, i))->name, 
                               1, ((Option*)g_ptr_array_index(list, i))->string_value,
                               -1);        
    }

    return GTK_TREE_MODEL(ls);
}

void
treeview2_set_up_constants(GtkTreeView *treeview, gint type)
{
#ifdef DEBUG
    printf("treeview2_set_up_constants\n");
#endif

    GtkTreeViewColumn   *col;
    GtkCellRenderer     *renderer;
    gchar *titles[2] =
	{_("Name"),
	 _("Value")};
    gint i;

    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(treeview),
				GTK_SELECTION_SINGLE);
    gtk_tree_view_set_headers_visible(treeview, TRUE);
    gtk_tree_view_set_rules_hint(treeview, TRUE);
    gtk_tree_view_set_search_column(treeview, 0);
    gtk_tree_view_set_search_equal_func(treeview,
					treeview_helper_search_equal_strings,
					NULL, NULL);

    for(i = 0; i < 2; i++)
    {
        col = gtk_tree_view_column_new();
        gtk_tree_view_column_set_title(col, titles[i]);
        gtk_tree_view_append_column(treeview, col);
        renderer = treeview_helper_cell_renderer_text_new();
        gtk_tree_view_column_pack_start(col, renderer, TRUE);	
        gtk_tree_view_column_add_attribute(col, renderer,
                                           "text", i);
        
        if(i == 1)
        {
            g_object_set(renderer, "editable", TRUE, NULL);
            g_signal_connect (renderer,
                              "edited",
                              (type == CONSTANTS_TYPE_APP) ?
                              G_CALLBACK (treeview_helper_constants_app_editing_done) :
                              G_CALLBACK (treeview_helper_constants_editing_done),
                              treeview);
        }
    }
}

/** Show the news in the news treeview. */
void
treeview2_show_constants(void)
{
#ifdef DEBUG
    printf("treeview2_show_constants\n");
#endif

    gint i;
    GtkTreeView *treeview[4] = 
        {GTK_TREE_VIEW(lookup_widget(window.constants, "treeview_constants_integer")),
         GTK_TREE_VIEW(lookup_widget(window.constants, "treeview_constants_float")),
         GTK_TREE_VIEW(lookup_widget(window.constants, "treeview_constants_string")),
         GTK_TREE_VIEW(lookup_widget(window.constants, "treeview_constants_app"))};
    GtkTreeModel *model;
    GPtrArray *list[4] =
        {g_ptr_array_new(),
         g_ptr_array_new(),
         g_ptr_array_new(),
         g_ptr_array_new()};
    
    for(i = 0; i < constants.list->len; i++)
        if(g_str_has_prefix(g_array_index(constants.list, Option, i).name, "int_"))
            g_ptr_array_add(list[CONSTANTS_TYPE_INT], &g_array_index(constants.list, Option, i));
        else if(g_str_has_prefix(g_array_index(constants.list, Option, i).name, "float_"))
            g_ptr_array_add(list[CONSTANTS_TYPE_FLOAT], &g_array_index(constants.list, Option, i));
        else
            g_ptr_array_add(list[CONSTANTS_TYPE_STRING], &g_array_index(constants.list, Option, i));

    for(i = 0; i < constants_app.list->len; i++)
        g_ptr_array_add(list[CONSTANTS_TYPE_APP], &g_array_index(constants_app.list, Option, i));

    for(i = 0; i < 4; i++)
    {
        treeview_helper_clear(treeview[i]);
        treeview2_set_up_constants(treeview[i], i);
        model = treeview2_create_constants(list[i], i);
        gtk_tree_view_set_model(treeview[i], model);
        g_object_unref(model);        

        g_ptr_array_free(list[i], TRUE);
    }        
}
