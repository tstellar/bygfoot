#include "game.h"
#include "game_gui.h"
#include "misc_callback_func.h"
#include "start_end.h"
#include "support.h"
#include "team.h"
#include "treeview.h"
#include "user.h"
#include "variables.h"
#include "window.h"
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

    treeview_show_team_list(GTK_TREE_VIEW(treeview_startup), FALSE, FALSE);

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

/** Start a new game after users and teams are selected. */
void
misc_callback_start_game(void)
{
    gint i;

    stat0 = STATUS_MAIN;

    start_new_game();

    for(i=0;i<users->len;i++)	
	user_set_up_team_new_game(&usr(i));

    window_destroy(&window.startup, TRUE);
    window_destroy(&window.startup_users, TRUE);

    window_create(WINDOW_MAIN);
    
    game_gui_show_main();
}

/** Add a user to the users array. */
void
misc_callback_add_player(void)
{
    GtkToggleButton *team_selection_radio1 =
	GTK_TOGGLE_BUTTON(lookup_widget(window.startup, "team_selection_radio1"));
    GtkToggleButton *team_selection_radio2 =
	GTK_TOGGLE_BUTTON(lookup_widget(window.startup, "team_selection_radio2"));
    GtkTreeView *treeview =
	GTK_TREE_VIEW(lookup_widget(window.startup, "treeview_startup"));
    GtkEntry *entry_player_name = 
	GTK_ENTRY(lookup_widget(window.startup, "entry_player_name"));
    const gchar *player_name = gtk_entry_get_text(entry_player_name);
    User new_user = user_new();
    
    if(strlen(player_name) > 0)
	g_string_printf(new_user.name, "%s", player_name);
    
    gtk_entry_set_text(entry_player_name, "");

    if(gtk_toggle_button_get_active(team_selection_radio1))
	new_user.scout = 1;
    else if(gtk_toggle_button_get_active(team_selection_radio2))
	new_user.scout = 0;
    else
	new_user.scout = -1;

    new_user.tm = treeview_get_pointer(treeview, 2);

    g_array_append_val(users, new_user);

    treeview_show_users_startup();

    treeview_show_team_list(GTK_TREE_VIEW(lookup_widget(window.startup, "treeview_startup")),
			    FALSE, FALSE);

    if(users->len == 1)
    {
	gtk_widget_set_sensitive(lookup_widget(window.startup, "team_selection_ok"), TRUE);
	gtk_widget_set_sensitive(lookup_widget(window.startup, "combo_country"), FALSE);
    }
}

/** Remove a user from the users list. 
    @param event The mouse click event on the treeview. */
void
misc_callback_remove_user(GdkEventButton *event)
{
    GtkTreeView *treeview =
	GTK_TREE_VIEW(lookup_widget(window.startup_users, "treeview_users"));
    
    treeview_select_row(treeview, event);

    user_remove(treeview_get_index(treeview, 0) - 1, FALSE);
    
    treeview_show_users_startup();
    
    if(users->len == 0)
    {
	gtk_widget_set_sensitive(lookup_widget(window.startup, "team_selection_ok"), FALSE);
	gtk_widget_set_sensitive(lookup_widget(window.startup, "combo_country"), TRUE);
    }
}

/** Stop the live game so that users can make subs etc. */
void
misc_callback_pause_live_game(void)
{
    gtk_widget_hide(lookup_widget(window.live, "button_pause"));
    gtk_widget_show(lookup_widget(window.live, "button_resume"));

    game_gui_set_main_window_sensitivity(TRUE);
    game_save_team_states();

    if(stat2 == current_user)
    {
	treeview_show_user_player_list(&usr(current_user), 1);
	treeview_show_user_player_list(&usr(current_user), 2);
    }

    stat0 = STATUS_LIVE_GAME_PAUSE;
}
