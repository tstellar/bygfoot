#include "callbacks.h"
#include "finance.h"
#include "game.h"
#include "game_gui.h"
#include "gui.h"
#include "load_save.h"
#include "maths.h"
#include "misc_callback_func.h"
#include "option.h"
#include "start_end.h"
#include "support.h"
#include "treeview.h"
#include "user.h"
#include "variables.h"
#include "window.h"
#include "xml_country.h"
#include "xml_name.h"

/* show the teams from the leagues in the country in
   the startup window */
void
misc_callback_show_team_list(GtkWidget *widget, const gchar *country_file)
{
    GtkWidget *treeview_startup =
	lookup_widget(widget, "treeview_startup");

    xml_country_read(country_file);

    treeview_show_team_list(GTK_TREE_VIEW(treeview_startup), FALSE, FALSE);
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
    GtkTreeView *treeview_users =
	GTK_TREE_VIEW(lookup_widget(window.startup, "treeview_users"));
    GtkTreeView *treeview_startup =
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

    new_user.tm = treeview_get_pointer(treeview_startup, 2);

    g_array_append_val(users, new_user);

    treeview_show_users(treeview_users);

    treeview_show_team_list(treeview_startup, FALSE, FALSE);

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
    GtkTreeView *treeview_users =
	GTK_TREE_VIEW(lookup_widget(window.startup, "treeview_users"));
    GtkTreeView *treeview_startup =
	GTK_TREE_VIEW(lookup_widget(window.startup, "treeview_startup"));
    
    if(!treeview_select_row(treeview_users, event))
	return;

    user_remove(treeview_get_index(treeview_users, 0) - 1, FALSE);
    
    treeview_show_users(treeview_users);
    treeview_show_team_list(treeview_startup, FALSE, FALSE);
    
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
    gtk_widget_set_sensitive(lookup_widget(window.live, "button_pause"), FALSE);
    gtk_widget_set_sensitive(lookup_widget(window.live, "button_resume"), TRUE);

    game_gui_set_main_window_sensitivity(TRUE);
    game_save_team_states();

    if(stat2 == cur_user)
	treeview_show_user_player_list();

    stat0 = STATUS_LIVE_GAME_PAUSE;
}

/** Update the cost and expected duration labels in the stadium
    window when the user's clicked on the spinbuttons. 
    @param capacity Whether to update the capacity or safety labels. */
void
misc_callback_update_stadium_window(gboolean capacity)
{
    GtkLabel *label_costs_capacity = 
	GTK_LABEL(lookup_widget(window.stadium, "label_costs_capacity")),
	*label_costs_safety =
	GTK_LABEL(lookup_widget(window.stadium, "label_costs_safety")),
	*label_duration_capacity =
	GTK_LABEL(lookup_widget(window.stadium, "label_duration_capacity")),
	*label_duration_safety =
	GTK_LABEL(lookup_widget(window.stadium, "label_duration_safety"));
    GtkSpinButton *spinbutton_capacity =
	GTK_SPIN_BUTTON(lookup_widget(window.stadium, "spinbutton_capacity")),
	*spinbutton_safety =
	GTK_SPIN_BUTTON(lookup_widget(window.stadium, "spinbutton_safety"));
    gfloat value_capacity = gtk_spin_button_get_value(spinbutton_capacity),
	value_safety = gtk_spin_button_get_value(spinbutton_safety) / 100;
    gint cost, duration;
      
    if(capacity)
    {
	cost = finance_get_stadium_improvement_cost(value_capacity, TRUE);
	duration = finance_get_stadium_improvement_duration(value_capacity, TRUE);
	gui_label_set_text_from_int(label_costs_capacity, cost, FALSE);
	gui_label_set_text_from_int(label_duration_capacity, duration, FALSE);
    }
    else
    {
	cost = finance_get_stadium_improvement_cost(value_safety, FALSE);
	duration = finance_get_stadium_improvement_duration(value_safety, FALSE);
	gui_label_set_text_from_int(label_costs_safety, cost, FALSE);
	gui_label_set_text_from_int(label_duration_safety, duration, FALSE);
    }
}

/** Handle a click on the OK button of the stadium window. */
void
misc_callback_improve_stadium(void)
{
    GtkSpinButton *spinbutton_capacity =
	GTK_SPIN_BUTTON(lookup_widget(window.stadium, "spinbutton_capacity")),
	*spinbutton_safety =
	GTK_SPIN_BUTTON(lookup_widget(window.stadium, "spinbutton_safety"));
    gint value_capacity = gtk_spin_button_get_value_as_int(spinbutton_capacity),
	value_safety = gtk_spin_button_get_value_as_int(spinbutton_safety);
    gint cost_capacity, cost_safety;

    if(value_safety + current_user.counters[COUNT_USER_STADIUM_SAFETY]
       > 101 - current_user.tm->stadium.safety * 100)
    {
	game_gui_show_warning("Safety improvement too high, reset to highest possible value.");
	value_safety = (gint)rint(100 - current_user.tm->stadium.safety * 100) -
	    current_user.counters[COUNT_USER_STADIUM_SAFETY];
    }

    cost_capacity = finance_get_stadium_improvement_cost(value_capacity, TRUE);
    cost_safety = finance_get_stadium_improvement_cost((gfloat)value_safety / 100, FALSE);

    if(cost_safety + cost_capacity > BUDGET(cur_user))
    {
	game_gui_show_warning(_("You don't have the money."));
	return;
    }

    current_user.money -= (cost_capacity + cost_safety);
    current_user.money_out[1][MON_OUT_STADIUM_IMPROVEMENT] -= (cost_safety + cost_capacity);

    current_user.counters[COUNT_USER_STADIUM_CAPACITY] += value_capacity;
    current_user.counters[COUNT_USER_STADIUM_SAFETY] += value_safety;

    window_destroy(&window.stadium, TRUE);
    game_gui_set_main_window_header();

    setsav0;
}

/** Load a savegame directly from the startup window. */
void
misc_callback_startup_load(const gchar *filename)
{
    window_destroy(&window.startup, TRUE);
    window_create(WINDOW_MAIN);
    xml_name_read(opt_str("string_opt_player_names_file"), 1000);
    load_save_load_game(filename);
    cur_user = 0;
    on_button_back_to_main_clicked(NULL, NULL);
}
