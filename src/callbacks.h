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
on_button_preview_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_new_week_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_help_clicked                 (GtkButton       *button,
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
on_players_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_season_history_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_show_info_activate             (GtkMenuItem     *menuitem,
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
on_menu_show_user_list_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_custom_structure_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_player_list1_button_press_event     (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_menu_team_button_press_event        (GtkWidget       *widget,
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
on_menu_live_game_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_offer_new_contract_activate    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_player_menu_show_info_activate      (GtkMenuItem     *menuitem,
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
#endif

void
on_menu_browse_players_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
