/*
   window.c

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

#include "callbacks.h"
#include "debug.h"
#include "file.h"
#include "finance.h"
#include "fixture.h"
#include "free.h"
#include "game_gui.h"
#include "gui.h"
#include "interface.h"
#include "language.h"
#include "league.h"
#include "live_game.h"
#include "load_save.h"
#include "main.h"
#include "maths.h"
#include "misc.h"
#include "misc_callback_func.h"
#include "misc_interface.h"
#include "misc2_interface.h"
#include "misc3_interface.h"
#include "option.h"
#include "option_gui.h"
#include "options_interface.h"
#include "support.h"
#include "training.h"
#include "training_interface.h"
#include "transfer.h"
#include "treeview.h"
#include "treeview2.h"
#include "treeview_helper.h"
#include "user.h"
#include "window.h"

/** Show the splash screen window. */
void
window_show_splash(void)
{
#ifdef DEBUG
    printf("window_show_splash\n");
#endif

    window_create(WINDOW_SPLASH);

    treeview_show_contributors(
	GTK_TREE_VIEW(lookup_widget(window.splash, "treeview_splash_contributors")));
    
    window_load_hint_number();
    window_splash_show_hint();
}

/** Show the hint determined by the hint counter in the splash window. */
void
window_splash_show_hint(void)
{
#ifdef DEBUG
    printf("window_splash_show_hint\n");
#endif

    gchar buf[SMALL];

    gtk_label_set_text(
	GTK_LABEL(lookup_widget(window.splash, "label_splash_hint")),
	g_array_index(hints.list, Option, counters[COUNT_HINT_NUMBER]).string_value);

    sprintf(buf, "(%d/%d)", counters[COUNT_HINT_NUMBER] + 1,
	    hints.list->len);

    gtk_label_set_text(
	GTK_LABEL(lookup_widget(window.splash, "label_splash_hintcounter")),
	buf);    
}

/** Load the index of the hint to show in the splash screen. */
void
window_load_hint_number(void)
{
#ifdef DEBUG
    printf("window_load_hint_number\n");
#endif

    gchar filename[SMALL];
    gchar dir[SMALL];
    FILE *fil;
    
    file_get_bygfoot_dir(dir);

    sprintf(filename, "%s%shint_num",
	    dir, G_DIR_SEPARATOR_S);

    fil = fopen(filename, "r");

    if(fil == NULL)
    {
	counters[COUNT_HINT_NUMBER] = 0;
	return;
    }

    fscanf(fil, "%d", &counters[COUNT_HINT_NUMBER]);

    fclose(fil);

    if(counters[COUNT_HINT_NUMBER] < 0 ||
       counters[COUNT_HINT_NUMBER] >= hints.list->len)
    {
	debug_print_message("Hint counter out of bounds: %d (bounds 0 and %d).\n",
		  counters[COUNT_HINT_NUMBER], hints.list->len - 1);
	counters[COUNT_HINT_NUMBER] = 0;
    }
}

/** Save the index of the current hint. */
void
window_save_hint_number(void)
{
#ifdef DEBUG
    printf("window_save_hint_number\n");
#endif

    gchar filename[SMALL];
    gchar dir[SMALL];
    FILE *fil;

    file_get_bygfoot_dir(dir);

    sprintf(filename, "%s%shint_num",
	    dir, G_DIR_SEPARATOR_S);

    fil = fopen(filename, "w");

    if(fil == NULL)
	return;

    fprintf(fil, "%d", counters[COUNT_HINT_NUMBER]);

    fclose(fil);
}

/** Show the window with the progress bar,
    sometimes with a nice picture. */
void
window_show_progress(gint pictype)
{
#ifdef DEBUG
    printf("window_show_progress\n");
#endif

    GPtrArray *pics = NULL;
    const gchar *picdir = NULL;
    gchar buf[SMALL];
    gchar *buf2 = NULL;
    GtkImage *image = NULL;

    if(sett_int("int_opt_goto_mode"))
        return;

    window_create(WINDOW_PROGRESS);

    if(pictype == PIC_TYPE_NONE ||
       !opt_int("int_opt_progressbar_pics"))
	return;

    image = GTK_IMAGE(lookup_widget(window.progress,
				    "image_match"));

    switch(pictype)
    {
	default:
	    debug_print_message("window_show_progress: unknown picture type %d",
		      pictype);
	    break;
	case PIC_TYPE_SAVE:
	    gtk_image_set_from_stock(image, GTK_STOCK_FLOPPY,
				     GTK_ICON_SIZE_DIALOG);
	    break;
	case PIC_TYPE_LOAD:
	    gtk_image_set_from_stock(image, GTK_STOCK_OPEN,
				     GTK_ICON_SIZE_DIALOG);
	    break;
	case PIC_TYPE_MATCHPIC:
	    if(debug_egg_forwards_boost_style())
	    {
		buf2 = file_find_support_file("soccerbabes.jpg", FALSE);
		if(buf2 != NULL)
		{
		    gtk_image_set_from_file(image, buf2);
		    g_free(buf2);
		}
	    }

	    if(buf2 == NULL)
	    {
		picdir = file_get_first_support_dir_suffix("pics");
		if(picdir == NULL)
		    return;
		
		pics = file_dir_get_contents(picdir, "match", "");
		
		if(pics->len != 0)
		{	
		    sprintf(buf, "%s%s%s", picdir, G_DIR_SEPARATOR_S,
			    (gchar*)g_ptr_array_index(pics, math_rndi(0, pics->len - 1)));
		    gtk_image_set_from_file(image, buf);
		}
		
		free_gchar_array(&pics);
	    }
	    break;
    }
}

/** Show the betting window. */
void
window_show_bets(void)
{
#ifdef DEBUG
    printf("window_show_bets\n");
#endif

    window_create(WINDOW_BETS);

    gtk_toggle_button_set_active(
	GTK_TOGGLE_BUTTON(lookup_widget(window.bets, "checkbutton_bet_all_leagues")),
	opt_user_int("int_opt_user_bet_show_all_leagues"));
    gtk_toggle_button_set_active(
	GTK_TOGGLE_BUTTON(lookup_widget(window.bets, "checkbutton_bet_cups")),
	opt_user_int("int_opt_user_bet_show_cups"));
    gtk_toggle_button_set_active(
	GTK_TOGGLE_BUTTON(lookup_widget(window.bets, "checkbutton_bet_user_recent")),
	opt_user_int("int_opt_user_bet_show_my_recent"));

    treeview2_show_bets();
}

/** Show the help/about window. 
    @param page Which notebook page to display. */
void
window_show_help(gint page)
{
#ifdef DEBUG
    printf("window_show_help\n");
#endif

    gchar buf[SMALL];

    window_create(WINDOW_HELP);

    sprintf(buf, "<span %s>Bygfoot Football Manager %s</span>\n(c) %s Győző Both (gyboth@bygfoot.com)\nhttp://bygfoot.sourceforge.net", const_app("string_help_window_program_name_attribute"), VERS, YEAR);
    gtk_label_set_markup(GTK_LABEL(lookup_widget(window.help, "label_about")), buf);

    treeview_show_contributors(
	GTK_TREE_VIEW(lookup_widget(window.help, "treeview_contributors")));

    game_gui_set_help_labels();

    gtk_notebook_set_current_page(GTK_NOTEBOOK(lookup_widget(window.help, "notebook1")), page);
}

static void
is_capital_sensitive (GtkCellLayout   *cell_layout,
		      GtkCellRenderer *cell,
		      GtkTreeModel    *tree_model,
		      GtkTreeIter     *iter,
		      gpointer         data)
{
  gboolean sensitive;

  sensitive = !gtk_tree_model_iter_has_child (tree_model, iter);

  g_object_set (cell, "sensitive", sensitive, NULL);
}

/**
   Show the country selection window. All files with prefix
   'country_' from $HOME/.bygfoot/definitions are appended to a combo box.
*/
void
window_show_startup(void)
{
#ifdef DEBUG
    printf("window_show_startup\n");
#endif

    GtkWidget *window_startup =
	window_create(WINDOW_STARTUP);
    GtkWidget *combo_country =
	lookup_widget(window_startup, "combo_country");
    GPtrArray *country_files = NULL;
    GtkTreeModel *model = NULL;
    GtkCellRenderer *renderer = NULL;
    gchar *last_country = file_load_text_from_saves("last_country");
    
    country_files = file_get_country_files();

    if(country_files->len == 0)
	main_exit_program(EXIT_NO_COUNTRY_FILES, 
			  "Didn't find any country definition files in the support directories.");

    gtk_cell_layout_clear(GTK_CELL_LAYOUT(combo_country));

    renderer = gtk_cell_renderer_pixbuf_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo_country), renderer, FALSE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo_country), renderer, "pixbuf", 0, NULL);

    renderer = treeview_helper_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo_country), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo_country), renderer, "text", 1, NULL);

    language_pick_country(country_files);
    model = treeview_create_country_list(country_files);
    gtk_combo_box_set_model(GTK_COMBO_BOX(combo_country), model);
    gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combo_country),
					renderer,
					is_capital_sensitive,
					NULL, NULL);

    g_object_unref(model);

    if(country.sid != NULL)
        misc_callback_show_team_list(combo_country, country.sid);        
    else if(last_country != NULL)
        misc_callback_show_team_list(combo_country, last_country);        
    else
        misc_callback_show_team_list(combo_country, (const gchar*)g_ptr_array_index(country_files, country_files->len - 1));

    if(last_country != NULL)
        g_free(last_country);

    //gtk_combo_box_set_active(GTK_COMBO_BOX(combo_country), country_index);

    free_gchar_array(&country_files);
}

/** Show the file selection window. */
void
window_show_file_sel(void)
{
#ifdef DEBUG
    printf("window_show_file_sel\n");
#endif

    gchar buf[SMALL];
    const gchar *home = g_get_home_dir();
    gchar *filename = NULL;
    GtkFileFilter *filter;
    gboolean mm_file_exists = FALSE;

    window_create(WINDOW_FILE_CHOOSER);

    if(stat5 != STATUS_LOAD_GAME &&
       stat5 != STATUS_LOAD_GAME_SPLASH)
	gtk_file_chooser_set_action(GTK_FILE_CHOOSER(window.file_chooser),
				    GTK_FILE_CHOOSER_ACTION_SAVE);
    else
	gtk_file_chooser_set_action(GTK_FILE_CHOOSER(window.file_chooser),
				    GTK_FILE_CHOOSER_ACTION_OPEN);

    filter = gtk_file_filter_new();
    if(stat5 == STATUS_SAVE_GAME || 
       stat5 == STATUS_LOAD_GAME ||
       stat5 == STATUS_LOAD_GAME_SPLASH)
    {
	gtk_file_filter_set_name(filter, _("Bygfoot Save Files"));
	gtk_file_filter_add_pattern(filter, "*.zip");
	gtk_file_filter_add_pattern(filter, "last_save");
    }
    else
    {
	gtk_file_filter_set_name(filter, _("Bygfoot Memorable Matches"));
	gtk_file_filter_add_pattern(filter, "*.bmm.zip");
    }
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(window.file_chooser), filter);

    filter = gtk_file_filter_new ();
    gtk_file_filter_set_name(filter, _("All Files"));
    gtk_file_filter_add_pattern(filter, "*");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(window.file_chooser), filter);

    if((stat5 == STATUS_SAVE_GAME || 
	stat5 == STATUS_LOAD_GAME ||
	stat5 == STATUS_LOAD_GAME_SPLASH) &&
       save_file != NULL)
	gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(window.file_chooser),
				      save_file);
    else if(users->len > 0 && current_user.mmatches_file != NULL)
	gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(window.file_chooser),
				      current_user.mmatches_file);
    else
    {
        if(os_is_unix)
            sprintf(buf, "%s%s%s%ssaves", home, G_DIR_SEPARATOR_S, 
		    HOMEDIRNAME, G_DIR_SEPARATOR_S);
        else
        {
            gchar *pwd = g_get_current_dir();
            sprintf(buf, "%s%ssaves", pwd, G_DIR_SEPARATOR_S);
            g_free(pwd);
	}

	gtk_file_chooser_set_current_folder(
	    GTK_FILE_CHOOSER(window.file_chooser), buf);
    }

    if(gtk_dialog_run(GTK_DIALOG(window.file_chooser)) == GTK_RESPONSE_OK)
    {
	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(window.file_chooser));

	if(stat5 == STATUS_LOAD_GAME)
	    load_save_load_game(filename, FALSE);
	else if(stat5 == STATUS_LOAD_GAME_SPLASH)
	    misc_callback_startup_load(filename);
	else if(stat5 == STATUS_SAVE_GAME)
	    load_save_save_game(filename);
	else if(stat5 == STATUS_SELECT_MM_FILE_LOAD)
	{
	    mm_file_exists = g_file_test(filename, G_FILE_TEST_EXISTS);
	    if((mm_file_exists && g_str_has_suffix(filename, ".bmm.zip")) ||
	       !mm_file_exists)
	    {
		if(mm_file_exists)
		    user_mm_load_file(filename, NULL);
		else
		    user_mm_set_filename(filename, NULL);
		window_show_mmatches();
	    }
	    else
		game_gui_show_warning(_("Not a valid Bygfoot Memorable Matches filename."));
	}
	else if(stat5 == STATUS_SELECT_MM_FILE_ADD)
	{
	    user_mm_set_filename(filename, NULL);
	    mm_file_exists = g_file_test(current_user.mmatches_file,
					 G_FILE_TEST_EXISTS);
	    user_mm_add_last_match(mm_file_exists, TRUE);
	}
	else if(stat5 == STATUS_SELECT_MM_FILE_IMPORT)
	{
	    user_mm_import_file(filename);
	    window_show_mmatches();
	}
	else if(stat5 == STATUS_SELECT_MM_FILE_EXPORT)
	    user_mm_export_file(filename);

	if(stat5 == STATUS_LOAD_GAME ||
	   stat5 == STATUS_LOAD_GAME_SPLASH)
	{
	    cur_user = 0;
	    on_button_back_to_main_clicked(NULL, NULL);
	    setsav1;
	}

	g_free(filename);
    }

    window_destroy(&window.file_chooser);
}

/** Show window with memorable matches list. */
void
window_show_mmatches(void)
{
#ifdef DEBUG
    printf("window_show_mmatches\n");
#endif

    if(window.mmatches == NULL)
	window_create(WINDOW_MMATCHES);
    treeview2_show_mmatches();

    gtk_entry_set_text(GTK_ENTRY(lookup_widget(window.mmatches, "entry_mm_file")),
		       current_user.mmatches_file);
}

/**  Show the options window. */
void
window_show_options(void)
{
#ifdef DEBUG
    printf("window_show_options\n");
#endif

    window_create(WINDOW_OPTIONS);

    option_gui_set_up_window();
}

/** Show the player list context menu, triggered by 'event'. */
void
window_show_menu_player(GdkEvent *event)
{
#ifdef DEBUG
    printf("window_show_menu_player\n");
#endif

    window.menu_player = create_menu_player();

    gtk_menu_popup(GTK_MENU(window.menu_player), NULL, NULL, NULL, NULL, 
		   ((GdkEventButton*)event)->button, gdk_event_get_time(event));
}

/** Show the youth academy context menu, triggered by 'event'. */
void
window_show_menu_youth(GdkEvent *event)
{
#ifdef DEBUG
    printf("window_show_menu_youth\n");
#endif

    window.menu_youth = create_menu_youth();

    gtk_menu_popup(GTK_MENU(window.menu_youth), NULL, NULL, NULL, NULL, 
		   ((GdkEventButton*)event)->button, gdk_event_get_time(event));
}

/** Show the digits window with the labels and values set 
    according to the arguments. */
void
window_show_digits(const gchar *text_main, const gchar* text1, gint value1, 
		   const gchar* text2, gint value2, gboolean show_alr)
{
#ifdef DEBUG
    printf("window_show_digits\n");
#endif

    GtkLabel *label_main, *label_1, *label_2;
    GtkSpinButton *spinbutton1, *spinbutton2;

    window_create(WINDOW_DIGITS);

    label_main = GTK_LABEL(lookup_widget(window.digits, "label_main"));
    label_1 = GTK_LABEL(lookup_widget(window.digits, "label_1"));
    label_2 = GTK_LABEL(lookup_widget(window.digits, "label_2"));

    spinbutton1 = GTK_SPIN_BUTTON(lookup_widget(window.digits, "spinbutton1"));
    spinbutton2 = GTK_SPIN_BUTTON(lookup_widget(window.digits, "spinbutton2"));

    if(stat1 == STATUS_GET_LOAN ||
       stat1 == STATUS_PAY_LOAN)
	gtk_spin_button_set_range(spinbutton1, (gdouble)1, (gdouble)value1);
    else if(stat1 == STATUS_SET_YA_PERCENTAGE)
	gtk_spin_button_set_range(spinbutton2, (gdouble)0,
				  (gdouble)const_int("int_youth_academy_max_percentage"));

    gtk_spin_button_set_value(spinbutton1, (gdouble)value1);
    gtk_spin_button_set_value(spinbutton2, (gdouble)value2);

    gtk_label_set_text(label_main, text_main);

    if(text1 != NULL)
	gtk_label_set_text(label_1, text1);
    else
	gtk_widget_hide(GTK_WIDGET(label_1)->parent);

    if(text2 != NULL)
	gtk_label_set_text(label_2, text2);
    else
	gtk_widget_hide(GTK_WIDGET(label_2)->parent);

    if(show_alr)
        gtk_widget_show(lookup_widget(window.digits, "button_digits_alr"));
}

/** Show the stadium window for the current user. */
void
window_show_stadium(void)
{
#ifdef DEBUG
    printf("window_show_stadium\n");
#endif

    gchar buf[SMALL];
    const Team *tm = current_user.tm;
    GtkLabel *label_capacity,
	*label_stadium_status, *label_average_attendance,
	*label_name;
    GtkProgressBar *progressbar_safety,
	*progressbar_average_attendance;
    gfloat average_attendance_perc = 0;
    GtkSpinButton *spin_ticket_price;

    window_create(WINDOW_STADIUM);

    label_capacity = GTK_LABEL(lookup_widget(window.stadium, "label_capacity"));
    label_stadium_status = GTK_LABEL(lookup_widget(window.stadium, "label_stadium_status"));
    label_average_attendance = GTK_LABEL(lookup_widget(window.stadium, "label_average_attendance"));
    label_name = GTK_LABEL(lookup_widget(window.stadium, "label_stadium_name"));
    
    spin_ticket_price = GTK_SPIN_BUTTON(lookup_widget(window.stadium, "spin_ticket_price"));
    gtk_spin_button_set_value(spin_ticket_price, tm->stadium.ticket_price);
    /* Set the lower bound to constant_ticket_price-(1/constant_ticket_attendance)
       +1 So that we can't divide by 0 or negative numbers*/
    gfloat lower_bound = const_int("int_team_stadium_ticket_price")-(1/const_float("float_team_stadium_price_attendance_factor"))+1;

    gtk_spin_button_set_range(spin_ticket_price, lower_bound, 100);

    progressbar_safety = GTK_PROGRESS_BAR(lookup_widget(window.stadium, "progressbar_safety"));
    progressbar_average_attendance = GTK_PROGRESS_BAR(lookup_widget(window.stadium, "progressbar_average_attendance"));

    gui_label_set_text_from_int(label_capacity, tm->stadium.capacity, FALSE);
    gui_label_set_text_from_int(label_average_attendance, tm->stadium.average_attendance, FALSE);
    
    if(tm->stadium.name != NULL)
	gtk_label_set_text(label_name, tm->stadium.name);
    else
	gtk_widget_hide(GTK_WIDGET(label_name));

    if(tm->stadium.games > 0)
	average_attendance_perc =
	    (gfloat)(tm->stadium.average_attendance * tm->stadium.games) /
	    (gfloat)tm->stadium.possible_attendance;

    gtk_progress_bar_set_fraction(progressbar_safety, tm->stadium.safety);
    gtk_progress_bar_set_fraction(progressbar_average_attendance, average_attendance_perc);

    sprintf(buf, "%d%%", (gint)rint(tm->stadium.safety * 100));
    gtk_progress_bar_set_text(progressbar_safety, buf);

    sprintf(buf, "%d%%", (gint)rint(average_attendance_perc * 100));
    gtk_progress_bar_set_text(progressbar_average_attendance, buf);

    if(current_user.counters[COUNT_USER_STADIUM_CAPACITY] + 
       current_user.counters[COUNT_USER_STADIUM_SAFETY] != 0)
    {
	sprintf(buf, _("Improvement in progress.\n%d seats and %d%% safety still to be done.\nExpected finish: %d weeks."),
		current_user.counters[COUNT_USER_STADIUM_CAPACITY],
		current_user.counters[COUNT_USER_STADIUM_SAFETY],
		MAX(finance_get_stadium_improvement_duration(
			(gfloat)current_user.counters[COUNT_USER_STADIUM_CAPACITY], TRUE),
		    finance_get_stadium_improvement_duration(
			(gfloat)current_user.counters[COUNT_USER_STADIUM_SAFETY] / 100, FALSE)));
	gtk_label_set_text(label_stadium_status, buf);
    }
    else
	gtk_label_set_text(label_stadium_status, _("No improvements currently in progress."));
}

gboolean
window_show(gpointer window)
{
#ifdef DEBUG
    printf("window_show\n");
#endif

    gtk_widget_show((GtkWidget*)window);

    return FALSE;
}

/** Show the window where the user can select between yes and no.
    @param text The text shown in the window. */
void
window_show_yesno(const gchar *text)
{
#ifdef DEBUG
    printf("window_show_yesno\n");
#endif

    window_create(WINDOW_YESNO);
    gtk_label_set_text(GTK_LABEL(lookup_widget(window.yesno, "label_yesno")), text);
}

/** Show the transfer dialog (yes/no/later).
    @param text The text to put into the label. */
void
window_show_transfer_dialog(const gchar *text)
{
#ifdef DEBUG
    printf("window_show_transfer_dialog\n");
#endif

    window_create(WINDOW_TRANSFER_DIALOG);
    gtk_label_set_text(GTK_LABEL(lookup_widget(window.transfer_dialog, "label_transfer_dialog")), text);
}

/** Set up the spinbuttons and boost/style images in the
    live window. */
void
window_live_set_up(void)
{
#ifdef DEBUG
    printf("window_live_set_up\n");
#endif

    GtkSpinButton *sb_speed = 
	GTK_SPIN_BUTTON(lookup_widget(window.live, "spinbutton_speed"));
    gfloat user_option = (gfloat)option_int("int_opt_user_live_game_speed", 
					    &usr(stat2).options);
    
    gtk_spin_button_set_range(
	sb_speed, 0, -rint((gfloat)(const_int("int_game_gui_live_game_speed_max") - 10) /
			   (gfloat)(const_int("int_game_gui_live_game_speed_grad"))));
    
    gtk_spin_button_set_value(sb_speed, user_option);
    
    gtk_spin_button_set_value(
	GTK_SPIN_BUTTON(lookup_widget(window.live, "spinbutton_verbosity")),
	(gfloat)option_int("int_opt_user_live_game_verbosity", &usr(stat2).options));

    game_gui_write_meters(usr(stat2).tm);
}

/** Save main window size and position into a file.*/
void
window_main_save_geometry(void)
{
#ifdef DEBUG
    printf("window_main_save_geometry\n");
#endif

    gchar filename[SMALL];
    gchar dir[SMALL];
    FILE *fil = NULL;
    gint width, height, pos_x, pos_y, paned_pos;
    
    file_get_bygfoot_dir(dir);

    sprintf(filename, "%s%swindow_settings",
	    dir, G_DIR_SEPARATOR_S);

    if(window.main != NULL && file_my_fopen(filename, "w", &fil, FALSE))
    {
	gtk_window_get_size(GTK_WINDOW(window.main), &width, &height);
	gtk_window_get_position(GTK_WINDOW(window.main), &pos_x, &pos_y);
	paned_pos = gtk_paned_get_position(
	    GTK_PANED(lookup_widget(window.main, "hpaned2")));

	fprintf(fil, "int_window_settings_width\t%d\n", width);
	fprintf(fil, "int_window_settings_height\t%d\n", height);
	fprintf(fil, "int_window_settings_pos_x\t%d\n", pos_x);
	fprintf(fil, "int_window_settings_pos_y\t%d\n", pos_y);
	fprintf(fil, "int_window_settings_paned_pos\t%d\n", paned_pos);
    
	fclose(fil);
    }
}

/** Set the main window geometry according to the file
    settings. */
void
window_main_load_geometry(void)
{
#ifdef DEBUG
    printf("window_main_load_geometry\n");
#endif

    gchar filename[SMALL];
    gchar dir[SMALL];
    OptionList optionlist;
    
    file_get_bygfoot_dir(dir);

    sprintf(filename, "%s%swindow_settings",
	    dir, G_DIR_SEPARATOR_S);

    if(g_file_test(filename, G_FILE_TEST_EXISTS))
    {
	optionlist.list = NULL;
	optionlist.datalist = NULL;
	file_load_opt_file(filename, &optionlist, FALSE);

	gtk_window_resize(GTK_WINDOW(window.main),
			  option_int("int_window_settings_width", &optionlist),
			  option_int("int_window_settings_height", &optionlist));
	gtk_window_move(GTK_WINDOW(window.main),
			option_int("int_window_settings_pos_x", &optionlist),
			option_int("int_window_settings_pos_y", &optionlist));
	gtk_paned_set_position(GTK_PANED(lookup_widget(window.main, "hpaned2")),
			       option_int("int_window_settings_paned_pos", &optionlist));

	free_option_list(&optionlist, FALSE);
    }
}

/** Create and show a window. Which one depends on the argument.
    @param window_type An integer telling us which window to
    create.
    @param count_popups Whether this window adds to the popup
    counter that determines when the main window gets (in)sensitive.
    @return The pointer to the new window.
    @see #Windows */
GtkWidget*
window_create(gint window_type)
{
    gchar buf[SMALL];
    GtkWidget *wind = NULL;
    
    sprintf(buf, "Bygfoot Football Manager %s", VERS);

    switch(window_type)
    {
	default:
	    main_exit_program(EXIT_INT_NOT_FOUND, 
			      "window_create: unknown window type %d\n", window_type);
	    break;
	case WINDOW_MAIN:
	    if(window.main == NULL)
	    {
		window.main = create_main_window();
		wind = window.main;
		window_main_load_geometry();
		window.paned_pos = 
		    gtk_paned_get_position(
			GTK_PANED(lookup_widget(window.main, "hpaned2")));
		game_gui_print_message(_("Welcome to Bygfoot %s"), VERS);
		sprintf(buf, "Bygfoot Football Manager %s", VERS);
	    }
	    else
		wind = window.main;
	    if(opt_int("int_opt_maximize_main_window"))
		gtk_window_maximize(GTK_WINDOW(wind));
	    break;
	case WINDOW_STARTUP:
	    if(window.startup != NULL)
		debug_print_message("window_create: called on already existing window\n");
	    else
		window.startup = create_window_startup();
	    wind = window.startup;
	    break;
	case WINDOW_LIVE:
	    if(window.live != NULL)
		debug_print_message("window_create: called on already existing window\n");
	    else
		window.live = create_window_live();
	    if(((LiveGame*)statp)->fix != NULL)
		strcpy(buf, league_cup_get_name_string(((LiveGame*)statp)->fix->clid));
	    wind = window.live;
	    window_live_set_up();
	    break;
	case WINDOW_WARNING:
	    if(window.warning != NULL)
		debug_print_message("window_create: called on already existing window\n");
	    else
		window.warning = create_window_warning();
	    wind = window.warning;
	    strcpy(buf, _("Erm..."));
	    break;
	case WINDOW_PROGRESS:
	    if(window.progress != NULL)
		debug_print_message("window_create: called on already existing window\n");
	    else
		window.progress = create_window_progress();
	    wind = window.progress;
	    strcpy(buf, "");
	    break;
	case WINDOW_DIGITS:
	    if(window.digits != NULL)
		debug_print_message("window_create: called on already existing window\n");
	    else
		window.digits = create_window_digits();

	    wind = window.digits;
	    strcpy(buf, _("Numbers..."));
	    break;
	case WINDOW_STADIUM:
	    if(window.stadium != NULL)
		debug_print_message("window_create: called on already existing window\n");
	    else
		window.stadium = create_window_stadium();
	    wind = window.stadium;
	    strcpy(buf, _("Your stadium"));
	    break;
	case WINDOW_JOB_OFFER:
	    if(window.job_offer != NULL)
		debug_print_message("window_create: called on already existing window\n");
	    else
		window.job_offer = create_window_job_offer();
	    wind = window.job_offer;
	    strcpy(buf, _("Job offer"));
	    break;
	case WINDOW_YESNO:
	    if(window.yesno != NULL)
		debug_print_message("window_create: called on already existing window\n");
	    else
		window.yesno = create_window_yesno();
	    wind = window.yesno;
	    strcpy(buf, "???");
	    break;
	case WINDOW_OPTIONS:
	    if(window.options != NULL)
		debug_print_message("window_create: called on already existing window\n");
	    else
		window.options = create_window_options();
	    wind = window.options;
	    strcpy(buf, _("Options"));
	    break;
	case WINDOW_FONT_SEL:
	    if(window.font_sel != NULL)
		debug_print_message("window_create: called on already existing window\n");
	    else
		window.font_sel = create_window_font_sel();
	    wind = window.font_sel;
	    strcpy(buf, _("Select font"));
	    break;
	case WINDOW_FILE_CHOOSER:
	    if(window.file_chooser != NULL)
		debug_print_message("window_create: called on already existing window\n");
	    else
		window.file_chooser = create_window_file_chooser();
	    wind = window.file_chooser;
	    break;
	case WINDOW_CONTRACT:
	    if(window.contract != NULL)
		debug_print_message("window_create: called on already existing window\n");
	    else
		window.contract = create_window_contract();
	    wind = window.contract;
	    strcpy(buf, _("Contract offer"));
	    break;
	case WINDOW_USER_MANAGEMENT:
	    if(window.user_management != NULL)
		debug_print_message("window_create: called on already existing window\n");
	    else
		window.user_management = create_window_user_management();
	    wind = window.user_management;
	    strcpy(buf, _("User management"));
	    break;
	case WINDOW_DEBUG:
	    if(window.wdebug != NULL)
		debug_print_message("window_create: called on already existing window\n");
	    else
		window.wdebug = create_window_debug();
	    wind = window.wdebug;
	    strcpy(buf, "Bygfoot debug window");
	    break;
	case WINDOW_HELP:
	    if(window.help != NULL)
		debug_print_message("window_create: called on already existing window\n");
	    else
		window.help = create_window_help();
	    wind = window.help;
	    break;
	case WINDOW_TRANSFER_DIALOG:
	    if(window.transfer_dialog != NULL)
		debug_print_message("window_create: called on already existing window\n");
	    else
		window.transfer_dialog = create_window_transfer_dialog();
	    wind = window.transfer_dialog;
	    strcpy(buf, _("Transfer offer"));
	    break;
	case WINDOW_SPONSORS:
	    if(window.sponsors != NULL)
		debug_print_message("window_create: called on already existing window\n");
	    else
		window.sponsors = create_window_sponsors();
	    wind = window.sponsors;
	    strcpy(buf, _("Sponsorship offers"));
	    break;
	case WINDOW_MMATCHES:
	    if(window.mmatches != NULL)
		debug_print_message("window_create: called on already existing window\n");
	    else
		window.mmatches = create_window_mmatches();
	    wind = window.mmatches;
	    strcpy(buf, _("Memorable matches"));
	    break;
	case WINDOW_BETS:
	    if(window.bets != NULL)
		debug_print_message("window_create: called on already existing window\n");
	    else
		window.bets = create_window_bets();
	    wind = window.bets;
	    strcpy(buf, _("Betting"));
	    break;
	case WINDOW_SPLASH:
	    if(window.splash != NULL)
		debug_print_message("window_create: called on already existing window\n");
	    else
		window.splash = create_window_splash();
	    wind = window.splash;
	    break;
    case WINDOW_TRAINING_CAMP:
        if(window.training_camp != NULL)
            debug_print_message("window_create: called on already existing window\n");
        else
            window.training_camp = create_window_training_camp();
        wind = window.training_camp;
        strcpy(buf, _("Training camp"));
        break;
    case WINDOW_ALR:
        if(window.alr != NULL)
            debug_print_message("window_create: called on already existing window\n");
        else
            window.alr = create_window_alr();
        wind = window.alr;
        strcpy(buf, _("Automatic loan repayment"));
        break;
    case WINDOW_NEWS:
        if(window.news != NULL)
            debug_print_message("window_create: called on already existing window\n");
        else
            window.news = create_window_news();
        wind = window.news;
        strcpy(buf, _("Bygfoot News"));
        break;
    case WINDOW_CONSTANTS:
        if(window.constants != NULL)
            debug_print_message("window_create: called on already existing window\n");
        else
            window.constants = create_window_constants();
        wind = window.constants;
        strcpy(buf, _("Bygfoot constants"));
        break;
    }

    if(window_type != WINDOW_FILE_CHOOSER)
	gtk_window_set_title(GTK_WINDOW(wind), buf);

    if(window_type != WINDOW_PROGRESS && window_type != WINDOW_SPLASH)
	g_timeout_add(20, (GSourceFunc)window_show, (gpointer)wind);
    else
	gtk_widget_show(wind);

    return wind;
}

/** Destroy a window widget and set the popups and
    main window sensitivity correctly. 
    @param window The window we destroy.
    @param count_popups Whether this window adds to the popup
    counter that determines when the main window gets (in)sensitive. */
void
window_destroy(GtkWidget **wind)
{
#ifdef DEBUG
    printf("window_destroy\n");
#endif

    if(*wind == NULL)
	return;

    if(*wind == window.splash)
    {
	counters[COUNT_HINT_NUMBER] = 
	    (counters[COUNT_HINT_NUMBER] + 1) % hints.list->len;

	window_save_hint_number();
    }

    gtk_widget_destroy(*wind);

    *wind = NULL;
}

/* Show the training camp window for the current user. */
void
window_show_training_camp(void)
{
#ifdef DEBUG
    printf("window_show_training_camp\n");
#endif

	GtkEntry *tfCosts;
	GtkWidget *rb_camp1;
    GtkWidget *rb_camp2;
    GtkWidget *rb_camp3;
    GtkLabel *l_costs;
    GtkLabel *l_recreation;
    GtkLabel *l_training;
    GtkLabel *l_camp_points;
    GtkHScale *hs_recreation;
    GtkHScale *hs_training;
    GtkHScale *hs_camp_points;
	gchar buf[SMALL];
	
    window_create(WINDOW_TRAINING_CAMP);
        
    // Initialize entry costs
    misc_print_grouped_int(
	math_round_integer(finance_wage_unit(current_user.tm) * const_float("float_training_camp_factor1"), -2),
	buf);
    tfCosts = GTK_ENTRY(lookup_widget(window.training_camp, "tf_costs"));
    gtk_entry_set_text (tfCosts, buf);
    
    // Initialize radio group camp
    rb_camp1 = GTK_WIDGET(lookup_widget(window.training_camp, "rb_camp1"));
    rb_camp2 = GTK_WIDGET(lookup_widget(window.training_camp, "rb_camp2"));
    rb_camp3 = GTK_WIDGET(lookup_widget(window.training_camp, "rb_camp3"));

    if(opt_user_int("int_opt_user_training_camp_hotel") == TRAINING_CAMP_HOTEL_GOOD)
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rb_camp1), TRUE);
    else if(opt_user_int("int_opt_user_training_camp_hotel") == TRAINING_CAMP_HOTEL_FIRST)
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rb_camp2), TRUE);
    else
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rb_camp3), TRUE);
    
    //Initialize labels
    l_costs = GTK_LABEL(lookup_widget(window.training_camp, "l_costs"));
    l_recreation = GTK_LABEL(lookup_widget(window.training_camp, "l_recreation"));
    l_training = GTK_LABEL(lookup_widget(window.training_camp, "l_training"));
    l_camp_points = GTK_LABEL(lookup_widget(window.training_camp, "l_camp_points"));
    gtk_misc_set_alignment (GTK_MISC(l_costs), 0.0, 0.5);
    gtk_misc_set_alignment (GTK_MISC(l_recreation), 0.0, 0.5);
    gtk_misc_set_alignment (GTK_MISC(l_training), 0.0, 0.5);
    gtk_misc_set_alignment (GTK_MISC(l_camp_points), 0.0, 0.5);
    
    //Initialize hScales
    hs_recreation = GTK_HSCALE(lookup_widget(window.training_camp, "hs_recreation"));
    hs_training = GTK_HSCALE(lookup_widget(window.training_camp, "hs_training"));
    hs_camp_points = GTK_HSCALE(lookup_widget(window.training_camp, "hs_camp_points"));
    gtk_range_set_range (GTK_RANGE(hs_recreation), CAMP_SCALE_MIN, CAMP_SCALE_MAX);
    gtk_range_set_range (GTK_RANGE(hs_training), CAMP_SCALE_MIN, CAMP_SCALE_MAX);
    gtk_range_set_range (GTK_RANGE(hs_camp_points), CAMP_SCALE_MIN, CAMP_SCALE_MAX);
    gtk_range_set_value (GTK_RANGE(hs_recreation), opt_user_int("int_opt_user_training_camp_recreation"));
    gtk_range_set_value (GTK_RANGE(hs_training), 
			 CAMP_SCALE_MAX - opt_user_int("int_opt_user_training_camp_recreation"));
    gtk_range_set_value (GTK_RANGE(hs_camp_points), CAMP_SCALE_MIN);
    gtk_scale_set_digits (GTK_SCALE(hs_recreation), 0);
    gtk_scale_set_digits (GTK_SCALE(hs_training), 0);
    gtk_scale_set_digits (GTK_SCALE(hs_camp_points), 0);
    gtk_widget_set_sensitive (GTK_WIDGET(hs_recreation), FALSE);
    gtk_widget_set_sensitive (GTK_WIDGET(hs_training), FALSE);
    gtk_widget_set_sensitive (GTK_WIDGET(hs_camp_points), FALSE);
    
	GdkColor color;
    //gtk_widget_modify_fg(GTK_WIDGET(hs_recreation), GTK_STATE_INSENSITIVE , NULL);
    //gtk_widget_modify_fg(GTK_WIDGET(hs_training), GTK_STATE_INSENSITIVE , NULL);
    //gtk_widget_modify_fg(GTK_WIDGET(hs_camp_points), GTK_STATE_INSENSITIVE , NULL);
    //gtk_widget_modify_bg(GTK_WIDGET(hs_recreation), GTK_STATE_NORMAL, NULL);
    gdk_color_parse("black", &color);
    gtk_widget_modify_fg(GTK_WIDGET(hs_recreation), GTK_STATE_INSENSITIVE , &color);
    gtk_widget_modify_fg(GTK_WIDGET(hs_training), GTK_STATE_INSENSITIVE , &color);
    gtk_widget_modify_fg(GTK_WIDGET(hs_camp_points), GTK_STATE_INSENSITIVE , &color);
    gtk_widget_modify_bg(GTK_WIDGET(hs_recreation), GTK_STATE_NORMAL, &color);
    gtk_widget_modify_bg(GTK_WIDGET(hs_training), GTK_STATE_NORMAL, &color);
    gtk_widget_modify_bg(GTK_WIDGET(hs_camp_points), GTK_STATE_NORMAL, &color);
}

/** Create and set up the debt repayment window. */
void
window_show_alr(void)
{
#ifdef DEBUG
    printf("window_show_alr\n");
#endif

    gchar weekly_installment[SMALL],
        debt[SMALL];

    window_create(WINDOW_ALR);

    misc_print_grouped_int(current_user.alr_weekly_installment, weekly_installment);
    misc_print_grouped_int(-current_user.debt, debt);

    gui_label_set_text_from_int(GTK_LABEL(lookup_widget(window.alr, "label_current_start_week")),
                                current_user.alr_start_week, FALSE);
    gtk_label_set_text(GTK_LABEL(lookup_widget(window.alr, "label_current_weekly_installment")), weekly_installment);
    gtk_label_set_text(GTK_LABEL(lookup_widget(window.alr, "label_alr_debt")), debt);

    gtk_spin_button_set_range(GTK_SPIN_BUTTON(lookup_widget(window.alr, "spinbutton_start_week")),
                              week + 1, MIN(week + current_user.counters[COUNT_USER_LOAN], fixture_get_last_scheduled_week()));
    gtk_spin_button_set_range(GTK_SPIN_BUTTON(lookup_widget(window.alr, "spinbutton_weekly_installment")),
                              0, -current_user.debt * powf(1 + current_user.debt_interest, const_int("int_finance_payback_weeks")) + 1);
}

/** Show the window where the user can edit constants determining
    gameplay behaviour. */
void
window_show_constants(void)
{
    window_create(WINDOW_CONSTANTS);
    treeview2_show_constants();
}
