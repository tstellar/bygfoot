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
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "player_info")),"player_info");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "player_list1")),"player_list1");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "player_info")),"player_info");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "treeview_right")),"treeview_right");
  GLADE_HOOKUP_OBJECT (main_window, GTK_WIDGET (gtk_builder_get_object (builder, "player_info")),"player_info");
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

  return main_window;
}

GtkWidget*
create_menu_player (void)
{
  GtkWidget *menu_player;
  GtkWidget *player_menu_show_info;
  GtkWidget *player_menu_put_on_transfer_list;
  GtkWidget *player_menu_remove_from_transfer_list;
  GtkWidget *player_menu_offer_new_contract;
  GtkWidget *player_menu_fire;
  GtkWidget *player_menu_shoots_penalties;
  GtkWidget *player_menu_move_to_youth_academy;
  GtkWidget *player_menu_edit_name;

  menu_player = gtk_menu_new ();

  player_menu_show_info = gtk_menu_item_new_with_mnemonic (_("Show info"));
  gtk_widget_show (player_menu_show_info);
  gtk_container_add (GTK_CONTAINER (menu_player), player_menu_show_info);

  player_menu_put_on_transfer_list = gtk_menu_item_new_with_mnemonic (_("Put on transfer list"));
  gtk_widget_show (player_menu_put_on_transfer_list);
  gtk_container_add (GTK_CONTAINER (menu_player), player_menu_put_on_transfer_list);

  player_menu_remove_from_transfer_list = gtk_menu_item_new_with_mnemonic (_("Remove from transfer list"));
  gtk_widget_show (player_menu_remove_from_transfer_list);
  gtk_container_add (GTK_CONTAINER (menu_player), player_menu_remove_from_transfer_list);

  player_menu_offer_new_contract = gtk_menu_item_new_with_mnemonic (_("Offer new contract"));
  gtk_widget_show (player_menu_offer_new_contract);
  gtk_container_add (GTK_CONTAINER (menu_player), player_menu_offer_new_contract);

  player_menu_fire = gtk_menu_item_new_with_mnemonic (_("Fire"));
  gtk_widget_show (player_menu_fire);
  gtk_container_add (GTK_CONTAINER (menu_player), player_menu_fire);

  player_menu_shoots_penalties = gtk_menu_item_new_with_mnemonic (_("Shoots penalties"));
  gtk_widget_show (player_menu_shoots_penalties);
  gtk_container_add (GTK_CONTAINER (menu_player), player_menu_shoots_penalties);

  player_menu_move_to_youth_academy = gtk_menu_item_new_with_mnemonic (_("Move to youth academy"));
  gtk_widget_show (player_menu_move_to_youth_academy);
  gtk_container_add (GTK_CONTAINER (menu_player), player_menu_move_to_youth_academy);

  player_menu_edit_name = gtk_menu_item_new_with_mnemonic (_("Edit name"));
  gtk_widget_show (player_menu_edit_name);
  gtk_container_add (GTK_CONTAINER (menu_player), player_menu_edit_name);

  g_signal_connect ((gpointer) player_menu_show_info, "activate",
                    G_CALLBACK (on_player_menu_show_info_activate),
                    NULL);
  g_signal_connect ((gpointer) player_menu_put_on_transfer_list, "activate",
                    G_CALLBACK (on_player_menu_put_on_transfer_list_activate),
                    NULL);
  g_signal_connect ((gpointer) player_menu_remove_from_transfer_list, "activate",
                    G_CALLBACK (on_player_menu_remove_from_transfer_list_activate),
                    NULL);
  g_signal_connect ((gpointer) player_menu_offer_new_contract, "activate",
                    G_CALLBACK (on_player_menu_offer_new_contract_activate),
                    NULL);
  g_signal_connect ((gpointer) player_menu_fire, "activate",
                    G_CALLBACK (on_player_menu_fire_activate),
                    NULL);
  g_signal_connect ((gpointer) player_menu_shoots_penalties, "activate",
                    G_CALLBACK (on_player_menu_shoots_penalties_activate),
                    NULL);
  g_signal_connect ((gpointer) player_menu_move_to_youth_academy, "activate",
                    G_CALLBACK (on_player_menu_move_to_youth_academy_activate),
                    NULL);
  g_signal_connect ((gpointer) player_menu_edit_name, "activate",
                    G_CALLBACK (on_player_menu_edit_name_activate),
                    NULL);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (menu_player, menu_player, "menu_player");
  GLADE_HOOKUP_OBJECT (menu_player, player_menu_show_info, "player_menu_show_info");
  GLADE_HOOKUP_OBJECT (menu_player, player_menu_put_on_transfer_list, "player_menu_put_on_transfer_list");
  GLADE_HOOKUP_OBJECT (menu_player, player_menu_remove_from_transfer_list, "player_menu_remove_from_transfer_list");
  GLADE_HOOKUP_OBJECT (menu_player, player_menu_offer_new_contract, "player_menu_offer_new_contract");
  GLADE_HOOKUP_OBJECT (menu_player, player_menu_fire, "player_menu_fire");
  GLADE_HOOKUP_OBJECT (menu_player, player_menu_shoots_penalties, "player_menu_shoots_penalties");
  GLADE_HOOKUP_OBJECT (menu_player, player_menu_move_to_youth_academy, "player_menu_move_to_youth_academy");
  GLADE_HOOKUP_OBJECT (menu_player, player_menu_edit_name, "player_menu_edit_name");

  return menu_player;
}

GtkWidget*
create_menu_youth (void)
{
  GtkWidget *menu_youth;
  GtkWidget *menu_youth_move_to_team;
  GtkWidget *image2149;
  GtkWidget *menu_youth_kick_out_of_academy;
  GtkWidget *image2150;

  menu_youth = gtk_menu_new ();

  menu_youth_move_to_team = gtk_image_menu_item_new_with_mnemonic (_("Move to team"));
  gtk_widget_show (menu_youth_move_to_team);
  gtk_container_add (GTK_CONTAINER (menu_youth), menu_youth_move_to_team);

  image2149 = create_pixmap (menu_youth, "menu_player_move_to_team.png");
  gtk_widget_show (image2149);
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_youth_move_to_team), image2149);

  menu_youth_kick_out_of_academy = gtk_image_menu_item_new_with_mnemonic (_("Kick out of academy"));
  gtk_widget_show (menu_youth_kick_out_of_academy);
  gtk_container_add (GTK_CONTAINER (menu_youth), menu_youth_kick_out_of_academy);

  image2150 = gtk_image_new_from_stock ("gtk-delete", GTK_ICON_SIZE_MENU);
  gtk_widget_show (image2150);
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_youth_kick_out_of_academy), image2150);

  g_signal_connect ((gpointer) menu_youth_move_to_team, "activate",
                    G_CALLBACK (on_menu_youth_move_to_team_activate),
                    NULL);
  g_signal_connect ((gpointer) menu_youth_kick_out_of_academy, "activate",
                    G_CALLBACK (on_menu_youth_kick_out_of_academy_activate),
                    NULL);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (menu_youth, menu_youth, "menu_youth");
  GLADE_HOOKUP_OBJECT (menu_youth, menu_youth_move_to_team, "menu_youth_move_to_team");
  GLADE_HOOKUP_OBJECT (menu_youth, image2149, "image2149");
  GLADE_HOOKUP_OBJECT (menu_youth, menu_youth_kick_out_of_academy, "menu_youth_kick_out_of_academy");
  GLADE_HOOKUP_OBJECT (menu_youth, image2150, "image2150");

  return menu_youth;
}
