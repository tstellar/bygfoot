#include "callbacks.h"
#include "file.h"
#include "finance.h"
#include "free.h"
#include "game_gui.h"
#include "gui.h"
#include "interface.h"
#include "league.h"
#include "live_game.h"
#include "load_save.h"
#include "main.h"
#include "misc_callback_func.h"
#include "misc_interface.h"
#include "misc2_interface.h"
#include "option.h"
#include "option_gui.h"
#include "options_interface.h"
#include "support.h"
#include "transfer.h"
#include "treeview.h"
#include "treeview_helper.h"
#include "user.h"
#include "window.h"

/** Show the help/about window. 
    @param page Which notebook page to display. */
void
window_show_help(gint page)
{
    gchar buf[SMALL];
    gchar *help_file = file_find_support_file("bygfoot_help", TRUE);
    OptionList help_list;

    if(help_file == NULL)
    {
	game_gui_show_warning(_("Didn't find file 'bygfoot_help'."));
	return;
    }

    help_list.list = NULL;
    help_list.datalist = NULL;
    file_load_opt_file(help_file, &help_list);

    window_create(WINDOW_HELP);

    sprintf(buf, "<span %s>Bygfoot Football Manager %s</span>\n(c) 2005 Győző Both (gyboth@bygfoot.com)\nhttp://bygfoot.sourceforge.net", const_app("string_help_window_program_name_attribute"), VERS);
    gtk_label_set_markup(GTK_LABEL(lookup_widget(window.help, "label_about")), buf);

    treeview_show_contributors(&help_list);

    game_gui_set_help_labels();

    gtk_notebook_set_current_page(GTK_NOTEBOOK(lookup_widget(window.help, "notebook1")), page);

    g_free(help_file);
    free_option_list(&help_list, FALSE);
}

/**
   Show the country selection window. All files with prefix
   'country_' from $HOME/.bygfoot/definitions are appended to a combo box.
*/
void
window_show_startup(void)
{
    GtkWidget *window_startup =
	window_create(WINDOW_STARTUP);
    GtkWidget *combo_country =
	lookup_widget(window_startup, "combo_country");
    GPtrArray *country_files = NULL;
    GtkTreeModel *model = NULL;
    GtkCellRenderer *renderer = NULL;
    
    country_files = file_get_country_files();

    if(country_files->len == 0)
	main_exit_program(EXIT_NO_COUNTRY_FILES, "Didn't find any country definition files in the support directories.");

    model = treeview_create_country_list(country_files);
    gtk_combo_box_set_model(GTK_COMBO_BOX(combo_country), model);
    g_object_unref(model);

    gtk_combo_box_entry_set_text_column(GTK_COMBO_BOX_ENTRY(combo_country), 1);
    gtk_cell_layout_clear(GTK_CELL_LAYOUT(combo_country));

    renderer = gtk_cell_renderer_pixbuf_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo_country), renderer, FALSE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo_country), renderer, "pixbuf", 0, NULL);

    renderer = treeview_helper_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo_country), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo_country), renderer, "text", 1, NULL);

    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_country), 0);

    free_g_string_array(&country_files);
}

/** Show the file selection window. */
void
window_show_file_sel(void)
{
    gchar buf[SMALL];
    const gchar *home = g_get_home_dir();
    gchar *filename = NULL;

    window_create(WINDOW_FILE_CHOOSER);

    if(stat1 == STATUS_SAVE_GAME)
	gtk_file_chooser_set_action(GTK_FILE_CHOOSER(window.file_chooser),
				    GTK_FILE_CHOOSER_ACTION_SAVE);
    else
	gtk_file_chooser_set_action(GTK_FILE_CHOOSER(window.file_chooser),
				    GTK_FILE_CHOOSER_ACTION_OPEN);

    if(strlen(save_file->str) > 0)
	gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(window.file_chooser),
				      save_file->str);
    else
    {
	sprintf(buf, "%s/%s/saves/.", home, HOMEDIRNAME);
	gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(window.file_chooser), buf);
    }

    if(gtk_dialog_run(GTK_DIALOG(window.file_chooser)) == GTK_RESPONSE_OK)
    {
	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(window.file_chooser));

	if(stat1 == STATUS_LOAD_GAME)
	    load_save_load_game(filename);
	else if(stat1 == STATUS_LOAD_GAME_TEAM_SELECTION)
	    misc_callback_startup_load(filename);
	else if(stat1 == STATUS_SAVE_GAME)
	    load_save_save_game(filename);

	if(stat1 == STATUS_LOAD_GAME ||
	   stat1 == STATUS_LOAD_GAME_TEAM_SELECTION)
	{
	    cur_user = 0;
	    on_button_back_to_main_clicked(NULL, NULL);
	    setsav1;
	}

	g_free(filename);
    }

    window_destroy(&window.file_chooser, FALSE);
}

/**  Show the options window. */
void
window_show_options(void)
{
    window_create(WINDOW_OPTIONS);

    option_gui_set_up_window();
}

/** Show the player list context menu, triggered by 'event'. */
void
window_show_menu_player(GdkEvent *event)
{
    if(window.menu_player != NULL)
	window_destroy(&window.menu_player, FALSE);

    window.menu_player = create_menu_player();

    gtk_menu_popup(GTK_MENU(window.menu_player), NULL, NULL, NULL, NULL, 
		   ((GdkEventButton*)event)->button, gdk_event_get_time(event));

    if(week >= transfer_get_deadline())
    {
	gtk_widget_set_sensitive(lookup_widget(window.menu_player,
					       "player_menu_put_on_transfer_list"), FALSE);
	gtk_widget_set_sensitive(lookup_widget(window.menu_player,
					       "player_menu_remove_from_transfer_list"), FALSE);
    }
}

/** Show the digits window with the labels and values set 
    according to the arguments. */
void
window_show_digits(const gchar *text_main, const gchar* text1, gint value1, 
		   const gchar* text2, gint value2)
{
    GtkLabel *label_main, *label_1, *label_2;
    GtkSpinButton *spinbutton1, *spinbutton2;

    window_create(WINDOW_DIGITS);

    label_main = GTK_LABEL(lookup_widget(window.digits, "label_main"));
    label_1 = GTK_LABEL(lookup_widget(window.digits, "label_1"));
    label_2 = GTK_LABEL(lookup_widget(window.digits, "label_2"));

    spinbutton1 = GTK_SPIN_BUTTON(lookup_widget(window.digits, "spinbutton1"));
    spinbutton2 = GTK_SPIN_BUTTON(lookup_widget(window.digits, "spinbutton2"));

    if(stat0 == STATUS_GET_LOAN ||
       stat0 == STATUS_PAY_LOAN)
	gtk_spin_button_set_range(spinbutton1, (gdouble)1, (gdouble)value1);

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
}

/** Show the stadium window for the current user. */
void
window_show_stadium(void)
{
    gchar buf[SMALL];
    const Team *tm = current_user.tm;
    GtkLabel *label_capacity,
	*label_stadium_status, *label_average_attendance;
    GtkProgressBar *progressbar_safety,
	*progressbar_average_attendance;
    gfloat average_attendance_perc = 0;

    window_create(WINDOW_STADIUM);

    label_capacity = GTK_LABEL(lookup_widget(window.stadium, "label_capacity"));
    label_stadium_status = GTK_LABEL(lookup_widget(window.stadium, "label_stadium_status"));
    label_average_attendance = GTK_LABEL(lookup_widget(window.stadium, "label_average_attendance"));
    
    progressbar_safety = GTK_PROGRESS_BAR(lookup_widget(window.stadium, "progressbar_safety"));
    progressbar_average_attendance = GTK_PROGRESS_BAR(lookup_widget(window.stadium, "progressbar_average_attendance"));

    gui_label_set_text_from_int(label_capacity, tm->stadium.capacity, FALSE);
    gui_label_set_text_from_int(label_average_attendance, tm->stadium.average_attendance, FALSE);
    

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
    gtk_widget_show((GtkWidget*)window);

    return FALSE;
}

/** Show the window where the user can select between yes and no.
    @param text The text shown in the window. */
void
window_show_yesno(const gchar *text)
{
    window_create(WINDOW_YESNO);
    gtk_label_set_text(GTK_LABEL(lookup_widget(window.yesno, "label_yesno")), text);
}

/** Show the transfer dialog (yes/no/later).
    @param text The text to put into the label. */
void
window_show_transfer_dialog(const gchar *text)
{
    window_create(WINDOW_TRANSFER_DIALOG);
    gtk_label_set_text(GTK_LABEL(lookup_widget(window.transfer_dialog, "label_transfer_dialog")), text);
}

/** Set the spinbuttons in the live window
    to the correct values. */
void
window_live_set_spinbuttons(void)
{
    gtk_spin_button_set_value(
	GTK_SPIN_BUTTON(lookup_widget(window.live, "spinbutton_speed")),
	(gfloat)option_int("int_opt_user_live_game_speed", &usr(stat2).options));
    gtk_spin_button_set_value(
	GTK_SPIN_BUTTON(lookup_widget(window.live, "spinbutton_verbosity")),
	(gfloat)option_int("int_opt_user_live_game_verbosity", &usr(stat2).options));
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
    gint old_popups_active = popups_active;
    gchar buf[SMALL];
    GtkWidget *wind = NULL;
    
    sprintf(buf, "Bygfoot Football Manager %s", VERS);

    switch(window_type)
    {
	default:
	    g_warning("window_create: unknown window type %d\n", window_type);
	    main_exit_program(EXIT_INT_NOT_FOUND, NULL);
	    break;
	case WINDOW_MAIN:
	    if(window.main == NULL)
	    {
		window.main = create_main_window();
		wind = window.main;
		sprintf(buf, _("Welcome to Bygfoot %s"), VERS);
		game_gui_print_message(buf);
		sprintf(buf, "Bygfoot Football Manager %s", VERS);
	    }
	    else
		wind = window.main;
	    if(opt_int("int_opt_maximize_main_window"))
		gtk_window_maximize(GTK_WINDOW(wind));
	    break;
	case WINDOW_STARTUP:
	    if(window.startup != NULL)
		g_warning("window_create: called on already existing window\n");
	    else
	    {
		popups_active++;
		window.startup = create_window_startup();
	    }
	    wind = window.startup;
	    break;
	case WINDOW_LIVE:
	    if(window.live != NULL)
		g_warning("window_create: called on already existing window\n");
	    else
	    {
		popups_active++;
		window.live = create_window_live();
	    }
	    strcpy(buf, league_cup_get_name_string(((LiveGame*)statp)->fix->clid));
	    wind = window.live;
	    window_live_set_spinbuttons();
	    break;
	case WINDOW_WARNING:
	    if(window.warning != NULL)
		g_warning("window_create: called on already existing window\n");
	    else
		window.warning = create_window_warning();
	    wind = window.warning;
	    strcpy(buf, _("Erm..."));
	    break;
	case WINDOW_PROGRESS:
	    if(window.progress != NULL)
		g_warning("window_create: called on already existing window\n");
	    else
		window.progress = create_window_progress();
	    wind = window.progress;
	    strcpy(buf, "");
	    break;
	case WINDOW_DIGITS:
	    if(window.digits != NULL)
		g_warning("window_create: called on already existing window\n");
	    else
	    {
		popups_active++;
		window.digits = create_window_digits();
	    }
	    wind = window.digits;
	    strcpy(buf, _("Numbers..."));
	    break;
	case WINDOW_STADIUM:
	    if(window.stadium != NULL)
		g_warning("window_create: called on already existing window\n");
	    else
	    {
		popups_active++;
		window.stadium = create_window_stadium();
	    }
	    wind = window.stadium;
	    strcpy(buf, _("Your stadium"));
	    break;
	case WINDOW_JOB_OFFER:
	    if(window.job_offer != NULL)
		g_warning("window_create: called on already existing window\n");
	    else
	    {
		popups_active++;
		window.job_offer = create_window_job_offer();
	    }
	    wind = window.job_offer;
	    strcpy(buf, _("Job offer"));
	    break;
	case WINDOW_YESNO:
	    if(window.yesno != NULL)
		g_warning("window_create: called on already existing window\n");
	    else
	    {
		popups_active++;
		window.yesno = create_window_yesno();
	    }
	    wind = window.yesno;
	    strcpy(buf, "???");
	    break;
	case WINDOW_OPTIONS:
	    if(window.options != NULL)
		g_warning("window_create: called on already existing window\n");
	    else
	    {
		popups_active++;
		window.options = create_window_options();
	    }
	    wind = window.options;
	    strcpy(buf, _("Options"));
	    break;
	case WINDOW_FONT_SEL:
	    if(window.font_sel != NULL)
		g_warning("window_create: called on already existing window\n");
	    else
		window.font_sel = create_window_font_sel();
	    wind = window.font_sel;
	    strcpy(buf, _("Select font"));
	    break;
	case WINDOW_FILE_CHOOSER:
	    if(window.file_chooser != NULL)
		g_warning("window_create: called on already existing window\n");
	    else
		window.file_chooser = create_window_file_chooser();
	    wind = window.file_chooser;
	    break;
	case WINDOW_CONTRACT:
	    if(window.contract != NULL)
		g_warning("window_create: called on already existing window\n");
	    else
		window.contract = create_window_contract();
	    wind = window.contract;
	    strcpy(buf, _("Contract offer"));
	    break;
	case WINDOW_USER_MANAGEMENT:
	    if(window.user_management != NULL)
		g_warning("window_create: called on already existing window\n");
	    else
		window.user_management = create_window_user_management();
	    wind = window.user_management;
	    strcpy(buf, _("User management"));
	    break;
	case WINDOW_DEBUG:
	    if(window.wdebug != NULL)
		g_warning("window_create: called on already existing window\n");
	    else
		window.wdebug = create_window_debug();
	    wind = window.wdebug;
	    strcpy(buf, "Bygfoot debug window");
	    break;
	case WINDOW_HELP:
	    if(window.help != NULL)
		g_warning("window_create: called on already existing window\n");
	    else
		window.help = create_window_help();
	    wind = window.help;
	    break;
	case WINDOW_TRANSFER_DIALOG:
	    if(window.transfer_dialog != NULL)
		g_warning("window_create: called on already existing window\n");
	    else
		window.transfer_dialog = create_window_transfer_dialog();
	    wind = window.transfer_dialog;
	    strcpy(buf, _("Transfer offer"));
	    break;
    }

    if(window_type != WINDOW_FILE_CHOOSER)
	gtk_window_set_title(GTK_WINDOW(wind), buf);

    if(window_type != WINDOW_PROGRESS)
	g_timeout_add(20, (GSourceFunc)window_show, (gpointer)wind);
    else
	gtk_widget_show(wind);

    if(popups_active != old_popups_active &&
       window.main != NULL)
	gtk_widget_set_sensitive(window.main, FALSE);

    return wind;
}

/** Destroy a window widget and set the popups and
    main window sensitivity correctly. 
    @param window The window we destroy.
    @param count_popups Whether this window adds to the popup
    counter that determines when the main window gets (in)sensitive. */
void
window_destroy(GtkWidget **wind, gboolean count_popups)
{
    if(*wind == NULL)
	return;

    if(*wind != window.main && count_popups)
    {
	popups_active--;

	if(popups_active == 0 && window.main != NULL)
	    gtk_widget_set_sensitive(window.main, TRUE);
    }

    gtk_widget_destroy(*wind);

    *wind = NULL;
}
