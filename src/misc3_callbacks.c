/*
   misc3_callbacks.c

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
#include "misc3_callbacks.h"
#include "misc3_interface.h"
#include "option.h"
#include "support.h"
#include "treeview2.h"
#include "treeview_helper.h"
#include "user.h"
#include "variables.h"
#include "window.h"

gboolean
on_window_bets_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    on_button_bet_close_clicked(NULL, NULL);
    
    return TRUE;
}

void
on_button_bet_close_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
    window_destroy(&window.bets, FALSE);
}

gboolean
on_checkbutton_bet_all_leagues_button_press_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    opt_user_set_int("int_opt_user_bet_show_all_leagues",
		     !opt_user_int("int_opt_user_bet_show_all_leagues"));

    treeview2_show_bets();

    return FALSE;
}


gboolean
on_checkbutton_bet_cups_button_press_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    opt_user_set_int("int_opt_user_bet_show_cups",
		     !opt_user_int("int_opt_user_bet_show_cups"));

    treeview2_show_bets();

    return FALSE;
}


gboolean
on_treeview_bets_button_press_event    (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    GtkTreePath *path = NULL;
    GtkTreeViewColumn *col = NULL;
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(widget));
    GtkTreeIter iter;
    GtkSpinButton *spin_wager = NULL;
    const BetMatch *bet = NULL;
    gint col_num = -1;
    gchar buf[SMALL];
    
    if(!gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(widget),
				      event->x, event->y,
				      &path, &col, NULL, NULL))
	return TRUE;

    if(!gtk_tree_model_get_iter(model, &iter, path))
    {
	gtk_tree_path_free(path);
	return TRUE;
    }

    gtk_tree_path_free(path);
    col_num = treeview_helper_get_col_number_column(col);

    if(col_num == 0 || col_num > 3)
	return TRUE;

    gtk_tree_model_get(model, &iter, col_num, &bet, -1);

    if(bet == NULL || fixture_from_id(bet->fix_id)->attendance != -1)
	return TRUE;

    if(bet_is_user(bet))
    {
	bet_remove(bet->fix_id);
	treeview2_show_bets();
	return FALSE;
    }

    sprintf(buf, _("You bet on outcome %d with an odd of %.2f. How much do you wager?"),
	    col_num - 1, bet->odds[col_num - 1]);

    stat1 = STATUS_PLACE_BET;
    stat2 = bet->fix_id;
    stat3 = col_num - 1;

    /* 'Wager' is the amount of money the user placed on a bet. */
    window_show_digits(buf, _("Wager"), 0, NULL, -1);
    spin_wager = GTK_SPIN_BUTTON(lookup_widget(window.digits, "spinbutton1"));
    gtk_spin_button_set_range(spin_wager, 0,
			      (gdouble)const_int("int_bet_wager_max"));
    gtk_spin_button_set_value(spin_wager,
			      (gdouble)opt_user_int("int_opt_user_bet_default_wager"));

    return TRUE;
}


gboolean
on_checkbutton_bet_user_recent_button_press_event
                                        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
    opt_user_set_int("int_opt_user_bet_show_my_recent",
		     !opt_user_int("int_opt_user_bet_show_my_recent"));

    treeview2_show_bets();

    return FALSE;
}
