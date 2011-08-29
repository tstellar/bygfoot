/*
  bet_struct.h

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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "bygfoot.h"
#include "finance.h"
#include "game_gui.h"
#include "option.h"
#include "maths.h"
#include "misc.h"
#include "support.h"
#include "training.h"
#include "training_callbacks.h"
#include "training_interface.h"
#include "training_struct.h"
#include "treeview.h"
#include "user.h"
#include "variables.h"
#include "window.h"

G_MODULE_EXPORT void
on_b_cancel_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef DEBUG
    printf("on_b_cancel_clicked\n");
#endif

    window_destroy(&window.training_camp);
}

G_MODULE_EXPORT void
on_b_ok_clicked                        (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef DEBUG
    printf("on_b_ok_clicked\n");
#endif

    GtkWidget *rb_camp1;
    GtkWidget *rb_camp2;
    GtkHScale *hs_recreation;
    GtkHScale *hs_training;
    gdouble value_training;
    gdouble value_recreation;
    gint number_camp;
    gboolean save;
    Team *current_team = current_user.tm;

    save = gtk_toggle_button_get_active(
	GTK_TOGGLE_BUTTON(lookup_widget(window.training_camp, "checkbutton_save")));

    //Get active radio
    rb_camp1 = GTK_WIDGET(lookup_widget(window.training_camp, "rb_camp1"));
    rb_camp2 = GTK_WIDGET(lookup_widget(window.training_camp, "rb_camp2"));

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rb_camp1)))
	number_camp = TRAINING_CAMP_HOTEL_GOOD;
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rb_camp2)))
	number_camp = TRAINING_CAMP_HOTEL_FIRST;
    else
	number_camp = TRAINING_CAMP_HOTEL_PREMIUM;
	
    //Get values for training and recreation	
    hs_recreation = GTK_HSCALE(lookup_widget(window.training_camp, "hs_recreation"));
    hs_training = GTK_HSCALE(lookup_widget(window.training_camp, "hs_training"));
    value_training = gtk_range_get_value(GTK_RANGE(hs_training));
    value_recreation = gtk_range_get_value(GTK_RANGE(hs_recreation));
    
    //Calculate training camp
    calculateTrainingCamp(current_team, value_training, value_recreation, number_camp);

    //Set new av-values -> GUI
    game_gui_write_av_skills(current_team);
	
    //Set new player values in GUI
    treeview_show_user_player_list();
	
    //Calculate costs of the training camp
    calculateCostsTrainingCamp(number_camp);
	
    //Set new av-values -> GUI
    game_gui_write_money();

    if(save)
    {
	opt_user_set_int("int_opt_user_training_camp_hotel", number_camp);
	opt_user_set_int("int_opt_user_training_camp_recreation", value_recreation);
    }
	
    window_destroy(&window.training_camp);

    current_user.counters[COUNT_USER_TRAININGS_WEEK]++;
    current_user.counters[COUNT_USER_TRAININGS_LEFT_SEASON]--;
    game_gui_print_message(_("%d training camps left this season."),
			   current_user.counters[COUNT_USER_TRAININGS_LEFT_SEASON]);
}

G_MODULE_EXPORT void
on_rb_camp3_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef DEBUG
    printf("on_rb_camp3_clicked\n");
#endif

    GtkEntry *tfCosts;
    gchar buf[SMALL];
	
    misc_print_grouped_int(
	math_round_integer(finance_wage_unit(current_user.tm) * const_float("float_training_camp_factor3"), -2), 
	buf);
    tfCosts = GTK_ENTRY(lookup_widget(window.training_camp, "tf_costs"));
    gtk_entry_set_text (tfCosts, buf);
}


G_MODULE_EXPORT void
on_rb_camp2_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef DEBUG
    printf("on_rb_camp2_clicked\n");
#endif

    GtkEntry *tfCosts;
    gchar buf[SMALL];
	
    misc_print_grouped_int(
	math_round_integer(finance_wage_unit(current_user.tm) * const_float("float_training_camp_factor2"), -2), 
	buf);
    tfCosts = GTK_ENTRY(lookup_widget(window.training_camp, "tf_costs"));
    gtk_entry_set_text (tfCosts, buf);
}


G_MODULE_EXPORT void
on_rb_camp1_clicked                    (GtkButton       *button,
					gpointer         user_data)
{
#ifdef DEBUG
    printf("on_rb_camp1_clicked\n");
#endif

    GtkEntry *tfCosts;
    gchar buf[SMALL];
	
    misc_print_grouped_int(
	math_round_integer(finance_wage_unit(current_user.tm) * const_float("float_training_camp_factor1"), -2), 
	buf);
    tfCosts = GTK_ENTRY(lookup_widget(window.training_camp, "tf_costs"));
    gtk_entry_set_text (tfCosts, buf);
}

G_MODULE_EXPORT void
on_b_dec_recreation_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef DEBUG
    printf("on_b_dec_recreation_clicked\n");
#endif

    GtkHScale *hs_camp_points;
    GtkHScale *hs_recreation;
    gdouble value_camp_points;
    gdouble value_recreation;
		
    hs_camp_points = GTK_HSCALE(lookup_widget(window.training_camp, "hs_camp_points"));
    hs_recreation = GTK_HSCALE(lookup_widget(window.training_camp, "hs_recreation"));
    value_camp_points = gtk_range_get_value(GTK_RANGE(hs_camp_points));
    value_recreation = gtk_range_get_value(GTK_RANGE(hs_recreation));
	
    if (value_recreation > CAMP_SCALE_MIN)
    {
	value_recreation --;
	value_camp_points ++;
	gtk_range_set_value(GTK_RANGE(hs_recreation), value_recreation);
	gtk_range_set_value(GTK_RANGE(hs_camp_points), value_camp_points);
    }
}


G_MODULE_EXPORT void
on_b_inc_recreation_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef DEBUG
    printf("on_b_inc_recreation_clicked\n");
#endif

    GtkHScale *hs_camp_points;
    GtkHScale *hs_recreation;
    gdouble value_camp_points;
    gdouble value_recreation;
		
    hs_camp_points = GTK_HSCALE(lookup_widget(window.training_camp, "hs_camp_points"));
    hs_recreation = GTK_HSCALE(lookup_widget(window.training_camp, "hs_recreation"));
    value_camp_points = gtk_range_get_value(GTK_RANGE(hs_camp_points));
    value_recreation = gtk_range_get_value(GTK_RANGE(hs_recreation));
	
    if (value_camp_points > CAMP_SCALE_MIN)
    {
	value_recreation ++;
	value_camp_points --;
	gtk_range_set_value(GTK_RANGE(hs_recreation), value_recreation);
	gtk_range_set_value(GTK_RANGE(hs_camp_points), value_camp_points);
    }
}


G_MODULE_EXPORT void
on_b_dec_training_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef DEBUG
    printf("on_b_dec_training_clicked\n");
#endif

    GtkHScale *hs_camp_points;
    GtkHScale *hs_training;
    gdouble value_camp_points;
    gdouble value_training;
		
    hs_camp_points = GTK_HSCALE(lookup_widget(window.training_camp, "hs_camp_points"));
    hs_training = GTK_HSCALE(lookup_widget(window.training_camp, "hs_training"));
    value_camp_points = gtk_range_get_value(GTK_RANGE(hs_camp_points));
    value_training = gtk_range_get_value(GTK_RANGE(hs_training));
	
    if (value_training > CAMP_SCALE_MIN)
    {
	value_training --;
	value_camp_points ++;
	gtk_range_set_value(GTK_RANGE(hs_training), value_training);
	gtk_range_set_value(GTK_RANGE(hs_camp_points), value_camp_points);
    }
}


G_MODULE_EXPORT void
on_b_inc_training_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
#ifdef DEBUG
    printf("on_b_inc_training_clicked\n");
#endif

    GtkHScale *hs_camp_points;
    GtkHScale *hs_training;
    gdouble value_camp_points;
    gdouble value_training;
		
    hs_camp_points = GTK_HSCALE(lookup_widget(window.training_camp, "hs_camp_points"));
    hs_training = GTK_HSCALE(lookup_widget(window.training_camp, "hs_training"));
    value_camp_points = gtk_range_get_value(GTK_RANGE(hs_camp_points));
    value_training = gtk_range_get_value(GTK_RANGE(hs_training));
	
    if (value_camp_points > CAMP_SCALE_MIN)
    {
	value_training ++;
	value_camp_points --;
	gtk_range_set_value(GTK_RANGE(hs_training), value_training);
	gtk_range_set_value(GTK_RANGE(hs_camp_points), value_camp_points);
    }
}


G_MODULE_EXPORT gboolean
on_window_training_camp_delete_event   (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
#ifdef DEBUG
    printf("on_window_training_camp_delete_event\n");
#endif

    on_b_cancel_clicked(NULL, NULL);

    return TRUE;
}
