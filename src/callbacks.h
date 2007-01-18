/*
   callbacks.h

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

#ifndef CALLBACKS_H
#define CALLBACKS_H

#include "bygfoot.h"
#include "interface.h"
#include "support.h"

gboolean
on_button_quit_clicked                 (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_menu_new_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_open_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_save_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_save_as_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
void
on_menu_quit_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_about_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_button_load_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_save_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_back_to_main_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_transfers_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_new_week_clicked             (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_player_list2_button_press_event      (GtkWidget       *widget,
					 GdkEventButton  *event,
					 gpointer         user_data);

void
on_button_browse_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_browse_forward_clicked       (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_browse_back_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_menu_preferences_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_fixtures_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_tables_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_season_history_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_put_on_transfer_list_activate  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_remove_from_transfer_list_activate
                                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_fire_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_shoots_penalties_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_my_league_results_activate     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);


void
on_menu_browse_teams_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_treeview_right_button_press_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_menu_next_user_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_previous_user_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_custom_structure_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_player_list1_button_press_event     (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_menu_team_button_release_event        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_menu_manage_users_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data);


void
on_menu_user_show_last_match_activate  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_user_show_last_stats_activate  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_eventbox_style_button_press_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_eventbox_boost_button_press_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_button_cl_back_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_cl_forward_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
on_menu_show_finances_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_show_stadium_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_menu_check_button_press_event       (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);
void
on_menu_offer_new_contract_activate    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_show_info_activate      (GtkMenuItem     *menuitem,
				 gpointer         user_data);

void
on_player_menu_put_on_transfer_list_activate
                                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_player_menu_remove_from_transfer_list_activate
                                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_player_menu_offer_new_contract_activate
                                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_player_menu_fire_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_player_menu_shoots_penalties_activate
                                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_browse_players_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_player_menu_show_info_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
void
on_menu_rearrange_team_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_load_last_save_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_fixtures_week_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_user_show_history_activate     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
void
on_button_reset_players_clicked        (GtkButton       *button,
                                        gpointer         user_data);

void
on_menu_reset_players_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);


void
on_menu_league_stats_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_button_quit_button_press_event      (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_menu_help_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_contributors_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_user_show_coming_matches_activate
                                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_show_youth_academy_activate    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_set_investment_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_move_to_youth_academy_activate (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_player_menu_move_to_youth_academy_activate
                                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_youth_move_to_team_activate    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_youth_kick_out_of_academy_activate
                                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_mm_add_last_match_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_mm_manage_matches_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_save_window_geometry_activate  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_season_results_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
void
on_menu_betting_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
void
on_menu_show_job_exchange_activate     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
gboolean
on_hpaned2_button_release_event        (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);
void
on_training_camp_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
#endif
