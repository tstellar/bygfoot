#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "variables.h"
#include "file.h"

GtkWidget*
create_main_window (void)
{
  GtkWidget *main_window;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot.glade", TRUE));
  main_window = GTK_WIDGET (gtk_builder_get_object (builder, "main_window"));

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GTK_HOOKUP_OBJECT (main_window, builder, "hpaned2");
  GTK_HOOKUP_OBJECT (main_window, builder, "entry_message");
  GTK_HOOKUP_OBJECT (main_window, builder, "label_user");
  GTK_HOOKUP_OBJECT (main_window, builder, "label_season");
  GTK_HOOKUP_OBJECT (main_window, builder, "label_week");
  GTK_HOOKUP_OBJECT (main_window, builder, "label_round");
  GTK_HOOKUP_OBJECT (main_window, builder, "label_team");
  GTK_HOOKUP_OBJECT (main_window, builder, "label_league");
  GTK_HOOKUP_OBJECT (main_window, builder, "label_rank");
  GTK_HOOKUP_OBJECT (main_window, builder, "label_money");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_next_user");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_previous_user");
  GTK_HOOKUP_OBJECT (main_window, builder, "label29");
  GTK_HOOKUP_OBJECT (main_window, builder, "label34");
  GTK_HOOKUP_OBJECT (main_window, builder, "label_av_skills");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_all_out_defend");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_defend");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_balanced");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_attack");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_all_out_attack");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_scout_best");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_scout_good");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_scout_average");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_scout_bad");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_physio_best");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_physio_good");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_physio_average");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_physio_bad");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_boost_anti");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_boost_off");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_boost_on");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_yc_best");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_yc_good");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_yc_average");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_yc_bad");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_recruit0");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_recruit1");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_recruit2");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_recruit3");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_recruit4");
  GTK_HOOKUP_OBJECT (main_window, builder, "image_style");
  GTK_HOOKUP_OBJECT (main_window, builder, "image_boost");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_job_offers");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_live_game");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_overwrite");
  GTK_HOOKUP_OBJECT (main_window, builder, "player_list1");
  GTK_HOOKUP_OBJECT (main_window, builder, "player_list2");
  GTK_HOOKUP_OBJECT (main_window, builder, "player_list1");
  GTK_HOOKUP_OBJECT (main_window, builder, "treeview_right");
  GTK_HOOKUP_OBJECT (main_window, builder, "button_save");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_save");
  GTK_HOOKUP_OBJECT (main_window, builder, "notebook_player");
  GTK_HOOKUP_OBJECT (main_window, builder, "button_cl_back");
  GTK_HOOKUP_OBJECT (main_window, builder, "button_cl_forward");
  GTK_HOOKUP_OBJECT (main_window, builder, "button_browse_back");
  GTK_HOOKUP_OBJECT (main_window, builder, "button_browse_forward");
  GTK_HOOKUP_OBJECT (main_window, builder, "hbox1");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_file");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_options");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_figures");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_scout");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_physio");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_browse_teams");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_youth_academy");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_training_camp");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_show_job_exchange");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_put_on_transfer_list");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_remove_from_transfer_list");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_fire");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_move_to_youth_academy");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_user");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_finances_stadium");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_help");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_browse_players");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_offer_new_contract");
  GTK_HOOKUP_OBJECT (main_window, builder, "button_reset_players");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_reset_players");
  GTK_HOOKUP_OBJECT (main_window, builder, "menubar1");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_player");
  GTK_HOOKUP_OBJECT (main_window, builder, "menu_youth");
  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return main_window;
}

GtkWidget*
create_menu_player (void)
{
  GtkWidget *menu_player;
  menu_player = lookup_widget(window.main, "menu_player"); 
  return menu_player;
}

GtkWidget*
create_menu_youth (void)
{
  GtkWidget *menu_youth;
  menu_youth = lookup_widget(window.main, "menu_youth"); 
  return menu_youth;
}
