#include "misc_callback_func.h"
#include "start_end.h"
#include "support.h"
#include "treeview.h"
#include "variables.h"
#include "xml_country.h"


/* show the teams from the leagues in the country in
   the startup window */
void
misc_callback_show_team_list(GtkWidget *widget, const gchar *country_file)
{
    gint i,j;
    GtkWidget *treeview_startup =
	lookup_widget(widget, "treeview_startup");

    xml_country_read(country_file);

    treeview_show_team_list(GTK_TREE_VIEW(treeview_startup), FALSE);

/*d*/
/*     for(i=0;i<cps->len;i++) */
/*     { */
/* 	printf("** %d **\n", i); */
/* 	printf("name %s short_name %s symbol %s id %s\n", cp(i).name->str, cp(i).short_name->str, cp(i).symbol->str, cp(i).sid->str); */
/* 	printf("nid %d type %d last %d gap %d skilldiff %d cap %d\n", cp(i).id, cp(i).type, cp(i).last_week, cp(i).week_gap, cp(i).skill_diff, cp(i).average_capacity); */

/* 	for(j=0;j<cp(i).rounds->len;j++) */
/* 	    printf("%d homeaway %d repl %d neutr %d groups %d adv %d best %d\n", j, */
/* 		   g_array_index(cp(i).rounds, CupRound, j).home_away, */
/* 		   g_array_index(cp(i).rounds, CupRound, j).replay, */
/* 		   g_array_index(cp(i).rounds, CupRound, j).neutral, */
/* 		   g_array_index(cp(i).rounds, CupRound, j).round_robin_number_of_groups, */
/* 		   g_array_index(cp(i).rounds, CupRound, j).round_robin_number_of_advance, */
/* 		   g_array_index(cp(i).rounds, CupRound, j).round_robin_number_of_best_advance); */

/* 	printf("\n"); */
/* 	for(j=0;j<cp(i).choose_teams->len;j++) */
/* 	    printf("%d id %s num %d start %d end %d rand %d\n", j, */
/* 		   g_array_index(cp(i).choose_teams, CupChooseTeam, j).sid->str, */
/* 		   g_array_index(cp(i).choose_teams, CupChooseTeam, j).number_of_teams, */
/* 		   g_array_index(cp(i).choose_teams, CupChooseTeam, j).start_idx, */
/* 		   g_array_index(cp(i).choose_teams, CupChooseTeam, j).end_idx, */
/* 		   g_array_index(cp(i).choose_teams, CupChooseTeam, j).randomly); */

/* 	printf("\n"); */
/*     } */
}

/** Start a new game after the user's selected a team.
    @param widget A widget from the startup window that enables us
    to get the row in the team list the user has selected. */
void
misc_callback_start_game(GtkWidget *widget)
{
    GtkTreeView *treeview =
	GTK_TREE_VIEW(lookup_widget(widget, "treeview_startup"));
    
    my_team_clid = treeview_get_index(treeview, 0);
    my_team_id = treeview_get_index(treeview, 1);

    start_new_game();
}
