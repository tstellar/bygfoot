#include "callbacks.h"
/*d*/
#include "game.h"
#include "live_game.h"
#include "maths.h"
#include "player.h"
#include "team.h"

#include "gui.h"
#include "main.h"
#include "treeview.h"
#include "variables.h"

gboolean
on_button_quit_clicked                 (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    main_exit_program(EXIT_OK, NULL);

    return FALSE;
}


void
on_menu_new_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_open_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_save_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_save_as_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_start_editor_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_start_update_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_quit_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_about_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_team_editor_help_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_button_load_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button_save_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button_back_to_main_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button_transfers_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button_fin_stad_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button_preview_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button_new_week_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
    /*d*/
    gint i, j, k;
    Fixture *fix;/*  = &g_array_index(lig(0).fixtures, Fixture, */
/* 				  math_rndi(0, lig(0).fixtures->len - 1)); */
    
    for(k=0;k<lig(0).fixtures->len;k++)
	if(g_array_index(lig(0).fixtures, Fixture, k).week_number == week &&
	   g_array_index(lig(0).fixtures, Fixture, k).week_round_number == 1)
	{
	    fix = &g_array_index(lig(0).fixtures, Fixture, k);
    
	    printf("\n");
	    for(j=0;j<2;j++)
	    {
		printf("%s %d %d %.1f\n", fix->teams[j]->name->str, fix->teams[j]->structure,
		       fix->teams[j]->style, team_average_cskill(fix->teams[j]));
	    }
	    
	    live_game_calculate_fixture(fix);
	    printf("%d : %d\n", math_sum_int_array(fix->result[0], 3),
		   math_sum_int_array(fix->result[1], 3));
	}

    week++;
}


void
on_button_help_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{

}

gboolean
on_player_list_button_press_event      (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{

  return FALSE;
}


gboolean
on_player_list2_button_press_event     (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{

  return FALSE;
}


void
on_structure_entry_activate            (GtkEntry        *entry,
                                        gpointer         user_data)
{

}


void
on_structure_button_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button_rearrange_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button_undo_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{

}



void
on_button_browse_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button_browse_forward_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button_browse_back_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_optionmenu_finstad_changed          (GtkOptionMenu   *optionmenu,
                                        gpointer         user_data)
{

}


void
on_button_fixtures_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button_tables_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_menu_preferences_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_notify_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_job_offers_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_live_game_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_overwrite_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_fixtures_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_league1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_league2_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_league3_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_league4_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_league5_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_league6_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_cup_1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_cup2_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_cup3_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_cup4_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_cup5_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_cup6_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_tables_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_players1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_season_history_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_team_history_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_player_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_show_info_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_show_history_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_put_on_transfer_list_activate  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_remove_from_transfer_list_activate (GtkMenuItem     *menuitem,
					    gpointer         user_data)
{

}


void
on_menu_fire_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_shoots_penalties_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_all_out_defend_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_defend_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_balanced_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_attack_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_all_out_attack_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_scout_best_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_scout_good_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_scout_average_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_scout_bad_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_physio_best_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_physio_good_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_physio_average_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_physio_bad_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_boost_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_my_league_results_activate     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_browse_teams_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    GtkWidget *treeview_right =
	lookup_widget(main_window, "treeview_right");

    treeview_show_team_list(GTK_TREE_VIEW(treeview_right), TRUE);

    status = STATUS_BROWSE_TEAMS;
}


gboolean
on_treeview_right_button_press_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{

  return FALSE;
}


void
on_menu_finances_stadium_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_increase_capacity_activate     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_increase_safety_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_get_loan_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_menu_pay_loan_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

