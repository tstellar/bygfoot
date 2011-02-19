/*
   misc_interface.c

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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "misc_callbacks.h"
#include "misc_interface.h"
#include "support.h"
#include "file.h"

GtkWidget*
create_window_startup (void)
{
  GtkWidget *window_startup;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_misc.glade", TRUE));
  window_startup = GTK_WIDGET (gtk_builder_get_object (builder, "window_startup"));

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GTK_HOOKUP_OBJECT (window_startup, builder, "combo_country");
  GTK_HOOKUP_OBJECT (window_startup, builder, "treeview_startup");
  GTK_HOOKUP_OBJECT (window_startup, builder, "combobox_start_league");
  GTK_HOOKUP_OBJECT (window_startup, builder, "button_add_player");
  GTK_HOOKUP_OBJECT (window_startup, builder, "treeview_users");
  GTK_HOOKUP_OBJECT (window_startup, builder, "entry_player_name");
  GTK_HOOKUP_OBJECT (window_startup, builder, "team_selection_ok");
  GTK_HOOKUP_OBJECT (window_startup, builder, "radiobutton_team_def_load");
  GTK_HOOKUP_OBJECT (window_startup, builder, "radiobutton_team_def_names");
  GTK_HOOKUP_OBJECT (window_startup, builder, "checkbutton_randomise_teams");

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_startup;
}

GtkWidget*
create_window_font_sel (void)
{
  GtkWidget *window_font_sel;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_misc.glade", TRUE));
  window_font_sel = GTK_WIDGET (gtk_builder_get_object (builder, "window_font_sel"));

  /* Store pointers to all widgets, for use by lookup_widget(). */

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_font_sel;
}

GtkWidget*
create_window_live (void)
{
  GtkWidget *window_live;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_misc.glade", TRUE));
  window_live = GTK_WIDGET (gtk_builder_get_object (builder, "window_live"));

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GTK_HOOKUP_OBJECT (window_live, builder, "button_live_close");
  GTK_HOOKUP_OBJECT (window_live, builder, "button_pause");
  GTK_HOOKUP_OBJECT (window_live, builder, "button_resume");
  GTK_HOOKUP_OBJECT (window_live, builder, "eventbox_poss0");
  GTK_HOOKUP_OBJECT (window_live, builder, "eventbox_poss1");
  GTK_HOOKUP_OBJECT (window_live, builder, "hscale_area");
  GTK_HOOKUP_OBJECT (window_live, builder, "image_lg_boost");
  GTK_HOOKUP_OBJECT (window_live, builder, "image_lg_opp_boost");
  GTK_HOOKUP_OBJECT (window_live, builder, "image_lg_opp_style");
  GTK_HOOKUP_OBJECT (window_live, builder, "image_lg_style");
  GTK_HOOKUP_OBJECT (window_live, builder, "label_lg_avskill");
  GTK_HOOKUP_OBJECT (window_live, builder, "label_lg_formation");
  GTK_HOOKUP_OBJECT (window_live, builder, "progressbar_live");
  GTK_HOOKUP_OBJECT (window_live, builder, "scrolledwindow9");
  GTK_HOOKUP_OBJECT (window_live, builder, "spinbutton_speed");
  GTK_HOOKUP_OBJECT (window_live, builder, "spinbutton_verbosity");
  GTK_HOOKUP_OBJECT (window_live, builder, "treeview_commentary");
  GTK_HOOKUP_OBJECT (window_live, builder, "treeview_lg_opponent");
  GTK_HOOKUP_OBJECT (window_live, builder, "treeview_result");
  GTK_HOOKUP_OBJECT (window_live, builder, "treeview_stats");
  GTK_HOOKUP_OBJECT (window_live, builder, "eventbox_lg_style");
  GTK_HOOKUP_OBJECT (window_live, builder, "eventbox_lg_boost");

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_live;
}

GtkWidget*
create_window_stadium (void)
{
  GtkWidget *window_stadium;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_misc.glade", TRUE));
  window_stadium = GTK_WIDGET (gtk_builder_get_object (builder, "window_stadium"));

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GTK_HOOKUP_OBJECT (window_stadium, builder, "label_capacity");
  GTK_HOOKUP_OBJECT (window_stadium, builder, "label_stadium_status");
  GTK_HOOKUP_OBJECT (window_stadium, builder, "label_average_attendance");
  GTK_HOOKUP_OBJECT (window_stadium, builder, "label_stadium_name");
  GTK_HOOKUP_OBJECT (window_stadium, builder, "spin_ticket_price");
  GTK_HOOKUP_OBJECT (window_stadium, builder, "progressbar_safety");
  GTK_HOOKUP_OBJECT (window_stadium, builder, "progressbar_average_attendance");
  GTK_HOOKUP_OBJECT (window_stadium, builder, "spinbutton_capacity");
  GTK_HOOKUP_OBJECT (window_stadium, builder, "spinbutton_safety");
  GTK_HOOKUP_OBJECT (window_stadium, builder, "label_costs_capacity");
  GTK_HOOKUP_OBJECT (window_stadium, builder, "label_costs_safety");
  GTK_HOOKUP_OBJECT (window_stadium, builder, "label_duration_capacity");
  GTK_HOOKUP_OBJECT (window_stadium, builder, "label_duration_safety");
  GTK_HOOKUP_OBJECT (window_stadium, builder, "label_costs_capacity");
  GTK_HOOKUP_OBJECT (window_stadium, builder, "label_costs_safety");
  GTK_HOOKUP_OBJECT (window_stadium, builder, "label_duration_capacity");
  GTK_HOOKUP_OBJECT (window_stadium, builder, "label_duration_safety");

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_stadium;
}

GtkWidget*
create_window_file_chooser (void)
{
  GtkWidget *window_file_chooser;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_misc.glade", TRUE));
  window_file_chooser = GTK_WIDGET (gtk_builder_get_object (builder, "window_file_chooser"));

  /* Store pointers to all widgets, for use by lookup_widget(). */

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_file_chooser;
}

GtkWidget*
create_window_sponsors (void)
{
  GtkWidget *window_sponsors;
  GtkBuilder *builder;
  builder = load_ui(file_find_support_file("bygfoot_misc.glade", TRUE));
  window_sponsors = GTK_WIDGET (gtk_builder_get_object (builder, "window_sponsors"));

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GTK_HOOKUP_OBJECT (window_sponsors, builder, "treeview_sponsors");

  /* free memory used by GtkBuilder object */
  g_object_unref (G_OBJECT (builder));

  return window_sponsors;
}

