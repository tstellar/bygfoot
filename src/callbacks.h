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
on_start_editor_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_start_update_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_quit_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_about_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_team_editor_help_activate      (GtkMenuItem     *menuitem,
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
on_button_fin_stad_clicked             (GtkButton       *button,
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

void
on_structure_entry_activate            (GtkEntry        *entry,
                                        gpointer         user_data);

void
on_structure_button_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_rearrange_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_undo_clicked                 (GtkButton       *button,
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
on_optionmenu_finstad_changed          (GtkOptionMenu   *optionmenu,
                                        gpointer         user_data);
void
on_button_fixtures_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
on_button_tables_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_menu_preferences_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_notify_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_job_offers_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_live_game_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_overwrite_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_fixtures_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_tables_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_players1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_season_history_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_player_activate                (GtkMenuItem     *menuitem,
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
on_menu_all_out_defend_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_defend_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_balanced_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_attack_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_all_out_attack_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_scout_best_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_scout_good_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_scout_average_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_scout_bad_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_physio_best_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_physio_good_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_physio_average_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_physio_bad_activate            (GtkMenuItem     *menuitem,
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
on_menu_finances_stadium_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_increase_capacity_activate     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_increase_safety_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_get_loan_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_menu_pay_loan_activate              (GtkMenuItem     *menuitem,
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

#endif
