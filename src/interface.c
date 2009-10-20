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

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

GtkWidget*
create_main_window (void)
{
  GtkWidget *main_window;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot.ui", TRUE));
  main_window = GTK_WIDGET (gtk_builder_get_object (builder, "main_window"));

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "hpaned2")),"hpaned2");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "entry_message")),"entry_message");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "label_user")),"label_user");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "label_season")),"label_season");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "label_week")),"label_week");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "label_round")),"label_round");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "label_team")),"label_team");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "label_league")),"label_league");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "label_rank")),"label_rank");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "label_money")),"label_money");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_next_user")),"menu_next_user");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_previous_user")),"menu_previous_user");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "label29")),"label29");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "label_av_skills")),"label_av_skills");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_all_out_defend")),"menu_all_out_defend");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_defend")),"menu_defend");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_balanced")),"menu_balanced");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_attack")),"menu_attack");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_all_out_attack")),"menu_all_out_attack");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_scout_best")),"menu_scout_best");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_scout_good")),"menu_scout_good");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_scout_average")),"menu_scout_average");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_scout_bad")),"menu_scout_bad");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_physio_best")),"menu_physio_best");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_physio_good")),"menu_physio_good");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_physio_average")),"menu_physio_average");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_physio_bad")),"menu_physio_bad");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_boost_anti")),"menu_boost_anti");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_boost_off")),"menu_boost_off");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_boost_on")),"menu_boost_on");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_yc_best")),"menu_yc_best");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_yc_good")),"menu_yc_good");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_yc_average")),"menu_yc_average");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_yc_bad")),"menu_yc_bad");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_recruit0")),"menu_recruit0");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_recruit1")),"menu_recruit1");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_recruit2")),"menu_recruit2");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_recruit3")),"menu_recruit3");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_recruit4")),"menu_recruit4");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "image_style")),"image_style");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "image_boost")),"image_boost");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_job_offers")),"menu_job_offers");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_live_game")),"menu_live_game");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_overwrite")),"menu_overwrite");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "player_list1")),"player_list1");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "player_list2")),"player_list2");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "player_list1")),"player_list1");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "treeview_right")),"treeview_right");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "button_save")),"button_save");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_save")),"menu_save");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "notebook_player")),"notebook_player");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "button_cl_back")),"button_cl_back");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "button_cl_forward")),"button_cl_forward");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "button_browse_back")),"button_browse_back");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "button_browse_forward")),"button_browse_forward");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "hbox1")),"hbox1");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_file")),"menu_file");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_options")),"menu_options");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_figures")),"menu_figures");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_scout")),"menu_scout");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_physio")),"menu_physio");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_browse_teams")),"menu_browse_teams");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_youth_academy")),"menu_youth_academy");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_training_camp")),"menu_training_camp");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_show_job_exchange")),"menu_show_job_exchange");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_put_on_transfer_list")),"menu_put_on_transfer_list");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_remove_from_transfer_list")),"menu_remove_from_transfer_list");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_fire")),"menu_fire");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_move_to_youth_academy")),"menu_move_to_youth_academy");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_user")),"menu_user");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_finances_stadium")),"menu_finances_stadium");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_help")),"menu_help");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_browse_players")),"menu_browse_players");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_offer_new_contract")),"menu_offer_new_contract");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "button_reset_players")),"button_reset_players");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_reset_players")),"menu_reset_players");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menubar1")),"menubar1");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_player")),"menu_player");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "menu_youth")),"menu_youth");
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
