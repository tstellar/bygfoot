#include "file.h"
#include "option.h"
#include "option_gui.h"
#include "support.h"
#include "user.h"
#include "variables.h"

/** An enumeration for all the options that are boolean,
    e.g. whether or not quit requires confirmation. */
enum BooleanOptions
{
    BOOL_OPT_CONF_NEW_WEEK = 0,
    BOOL_OPT_CONF_QUIT,
    BOOL_OPT_SAVE_OVERWRITE,
    BOOL_OPT_MAXIMIZE,
    BOOL_OPT_PREFER_MESS,
    BOOL_OPT_AUTOSAVE,
    BOOL_OPT_CONF_UNFIT,
    BOOL_OPT_SHOW_LIVE,
    BOOL_OPT_SHOW_TENDENCY,
    BOOL_OPT_PAUSE_INJURY,
    BOOL_OPT_PAUSE_RED,
    BOOL_OPT_PAUSE_BREAK,
    BOOL_OPT_AUTO_SUB,
    BOOL_OPT_SHOW_JOB,
    BOOL_OPT_REARRANGE,
    BOOL_OPT_SWAP,
    BOOL_OPT_SHOW_OVERALL,
    BOOL_OPT_PL1_ATT_NAME,
    BOOL_OPT_PL1_ATT_CPOS,
    BOOL_OPT_PL1_ATT_POS,
    BOOL_OPT_PL1_ATT_CSKILL,
    BOOL_OPT_PL1_ATT_SKILL,
    BOOL_OPT_PL1_ATT_FITNESS,
    BOOL_OPT_PL1_ATT_GAMES,
    BOOL_OPT_PL1_ATT_SHOTS,
    BOOL_OPT_PL1_ATT_GOALS,
    BOOL_OPT_PL1_ATT_STATUS,
    BOOL_OPT_PL1_ATT_CARDS,
    BOOL_OPT_PL1_ATT_AGE,
    BOOL_OPT_PL1_ATT_ETAL,
    BOOL_OPT_PL1_ATT_VALUE,
    BOOL_OPT_PL1_ATT_WAGE,
    BOOL_OPT_PL1_ATT_CONTRACT,
    BOOL_OPT_PL1_ATT_TEAM,
    BOOL_OPT_PL1_ATT_LEAGUE,
    BOOL_OPT_PL2_ATT_NAME,
    BOOL_OPT_PL2_ATT_CPOS,
    BOOL_OPT_PL2_ATT_POS,
    BOOL_OPT_PL2_ATT_CSKILL,
    BOOL_OPT_PL2_ATT_SKILL,
    BOOL_OPT_PL2_ATT_FITNESS,
    BOOL_OPT_PL2_ATT_GAMES,
    BOOL_OPT_PL2_ATT_SHOTS,
    BOOL_OPT_PL2_ATT_GOALS,
    BOOL_OPT_PL2_ATT_STATUS,
    BOOL_OPT_PL2_ATT_CARDS,
    BOOL_OPT_PL2_ATT_AGE,
    BOOL_OPT_PL2_ATT_ETAL,
    BOOL_OPT_PL2_ATT_VALUE,
    BOOL_OPT_PL2_ATT_WAGE,
    BOOL_OPT_PL2_ATT_CONTRACT,
    BOOL_OPT_PL2_ATT_TEAM,
    BOOL_OPT_PL2_ATT_LEAGUE,
    BOOL_OPT_END
};


/** Write the pointers to the options and the corresponding widgets. */
void
option_gui_write_bool_widgets(gint **bool_options, GtkToggleButton **bool_widgets)
{
    bool_widgets[BOOL_OPT_CONF_NEW_WEEK] = 
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_conf_new_week_round"));
    bool_options[BOOL_OPT_CONF_NEW_WEEK] = opt_intp("int_opt_confirm_new_week_round");

    bool_widgets[BOOL_OPT_CONF_QUIT] = 
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_conf_quit"));
    bool_options[BOOL_OPT_CONF_QUIT] = opt_intp("int_opt_confirm_quit");

    bool_widgets[BOOL_OPT_SAVE_OVERWRITE] = 
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_save_overwrite"));
    bool_options[BOOL_OPT_SAVE_OVERWRITE] = opt_intp("int_opt_save_will_overwrite");

    bool_widgets[BOOL_OPT_MAXIMIZE] = 
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_maximize"));
    bool_options[BOOL_OPT_MAXIMIZE] = opt_intp("int_opt_maximize_main_window");

    bool_widgets[BOOL_OPT_PREFER_MESS] = 
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_pref_mess"));
    bool_options[BOOL_OPT_PREFER_MESS] = opt_intp("int_opt_prefer_messages");

    bool_widgets[BOOL_OPT_AUTOSAVE] = 
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_autosave"));
    bool_options[BOOL_OPT_AUTOSAVE] = opt_intp("int_opt_autosave");

    bool_widgets[BOOL_OPT_CONF_UNFIT] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_conf_unfit"));
    bool_options[BOOL_OPT_CONF_UNFIT] = opt_user_intp("int_opt_user_confirm_unfit");

    bool_widgets[BOOL_OPT_SHOW_LIVE] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_show_live"));
    bool_options[BOOL_OPT_SHOW_LIVE] = opt_user_intp("int_opt_user_show_live_game");

    bool_widgets[BOOL_OPT_SHOW_TENDENCY] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_show_tendency"));
    bool_options[BOOL_OPT_SHOW_TENDENCY] = opt_user_intp("int_opt_user_show_tendency_bar");

    bool_widgets[BOOL_OPT_PAUSE_INJURY] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_pause_injury"));
    bool_options[BOOL_OPT_PAUSE_INJURY] = opt_user_intp("int_opt_user_pause_injury");

    bool_widgets[BOOL_OPT_PAUSE_RED] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_pause_red"));
    bool_options[BOOL_OPT_PAUSE_RED] = opt_user_intp("int_opt_user_pause_red");

    bool_widgets[BOOL_OPT_PAUSE_BREAK] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_pause_break"));
    bool_options[BOOL_OPT_PAUSE_BREAK] = opt_user_intp("int_opt_user_pause_break");

    bool_widgets[BOOL_OPT_AUTO_SUB] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_auto_sub"));
    bool_options[BOOL_OPT_AUTO_SUB] = opt_user_intp("int_opt_user_auto_sub");

    bool_widgets[BOOL_OPT_SHOW_JOB] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_show_job"));
    bool_options[BOOL_OPT_SHOW_JOB] = opt_user_intp("int_opt_user_show_job_offers");

    bool_widgets[BOOL_OPT_REARRANGE] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_rearrange_adapts"));
    bool_options[BOOL_OPT_REARRANGE] = opt_user_intp("int_opt_user_reaarrange_adapts");

    bool_widgets[BOOL_OPT_SWAP] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_swap_adapts"));
    bool_options[BOOL_OPT_SWAP] = opt_user_intp("int_opt_user_swap_adapts");

    bool_widgets[BOOL_OPT_SHOW_OVERALL] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_show_overall"));
    bool_options[BOOL_OPT_SHOW_OVERALL] = opt_user_intp("int_opt_user_show_overall");

    bool_widgets[BOOL_OPT_PL1_ATT_NAME] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton1"));
    bool_options[BOOL_OPT_PL1_ATT_NAME] = opt_user_intp("int_opt_user_pl1_att_name");

    bool_widgets[BOOL_OPT_PL1_ATT_CPOS] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton2"));
    bool_options[BOOL_OPT_PL1_ATT_CPOS] = opt_user_intp("int_opt_user_pl1_att_cpos");

    bool_widgets[BOOL_OPT_PL1_ATT_POS] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton3"));
    bool_options[BOOL_OPT_PL1_ATT_POS] = opt_user_intp("int_opt_user_pl1_att_pos");

    bool_widgets[BOOL_OPT_PL1_ATT_CSKILL] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton4"));
    bool_options[BOOL_OPT_PL1_ATT_CSKILL] = opt_user_intp("int_opt_user_pl1_att_cskill");

    bool_widgets[BOOL_OPT_PL1_ATT_SKILL] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton5"));
    bool_options[BOOL_OPT_PL1_ATT_SKILL] = opt_user_intp("int_opt_user_pl1_att_skill");

    bool_widgets[BOOL_OPT_PL1_ATT_FITNESS] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton6"));
    bool_options[BOOL_OPT_PL1_ATT_FITNESS] = opt_user_intp("int_opt_user_pl1_att_fitness");

    bool_widgets[BOOL_OPT_PL1_ATT_GAMES] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton7"));
    bool_options[BOOL_OPT_PL1_ATT_GAMES] = opt_user_intp("int_opt_user_pl1_att_games");

    bool_widgets[BOOL_OPT_PL1_ATT_SHOTS] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton8"));
    bool_options[BOOL_OPT_PL1_ATT_SHOTS] = opt_user_intp("int_opt_user_pl1_att_shots");

    bool_widgets[BOOL_OPT_PL1_ATT_GOALS] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton9"));
    bool_options[BOOL_OPT_PL1_ATT_GOALS] = opt_user_intp("int_opt_user_pl1_att_goals");

    bool_widgets[BOOL_OPT_PL1_ATT_STATUS] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton10"));
    bool_options[BOOL_OPT_PL1_ATT_STATUS] = opt_user_intp("int_opt_user_pl1_att_status");

    bool_widgets[BOOL_OPT_PL1_ATT_CARDS] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton11"));
    bool_options[BOOL_OPT_PL1_ATT_CARDS] = opt_user_intp("int_opt_user_pl1_att_cards");

    bool_widgets[BOOL_OPT_PL1_ATT_AGE] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton12"));
    bool_options[BOOL_OPT_PL1_ATT_AGE] = opt_user_intp("int_opt_user_pl1_att_age");

    bool_widgets[BOOL_OPT_PL1_ATT_ETAL] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton13"));
    bool_options[BOOL_OPT_PL1_ATT_ETAL] = opt_user_intp("int_opt_user_pl1_att_etal");

    bool_widgets[BOOL_OPT_PL1_ATT_VALUE] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton14"));
    bool_options[BOOL_OPT_PL1_ATT_VALUE] = opt_user_intp("int_opt_user_pl1_att_value");

    bool_widgets[BOOL_OPT_PL1_ATT_WAGE] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton15"));
    bool_options[BOOL_OPT_PL1_ATT_WAGE] = opt_user_intp("int_opt_user_pl1_att_wage");

    bool_widgets[BOOL_OPT_PL1_ATT_CONTRACT] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton16"));
    bool_options[BOOL_OPT_PL1_ATT_CONTRACT] = opt_user_intp("int_opt_user_pl1_att_contract");

    bool_widgets[BOOL_OPT_PL1_ATT_TEAM] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton17"));
    bool_options[BOOL_OPT_PL1_ATT_TEAM] = opt_user_intp("int_opt_user_pl1_att_team");

    bool_widgets[BOOL_OPT_PL1_ATT_LEAGUE] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton18"));
    bool_options[BOOL_OPT_PL1_ATT_LEAGUE] = opt_user_intp("int_opt_user_pl1_att_league_cup");

    bool_widgets[BOOL_OPT_PL2_ATT_NAME] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton19"));
    bool_options[BOOL_OPT_PL2_ATT_NAME] = opt_user_intp("int_opt_user_pl2_att_name");

    bool_widgets[BOOL_OPT_PL2_ATT_CPOS] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton20"));
    bool_options[BOOL_OPT_PL2_ATT_CPOS] = opt_user_intp("int_opt_user_pl2_att_cpos");

    bool_widgets[BOOL_OPT_PL2_ATT_POS] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton21"));
    bool_options[BOOL_OPT_PL2_ATT_POS] = opt_user_intp("int_opt_user_pl2_att_pos");

    bool_widgets[BOOL_OPT_PL2_ATT_CSKILL] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton22"));
    bool_options[BOOL_OPT_PL2_ATT_CSKILL] = opt_user_intp("int_opt_user_pl2_att_cskill");

    bool_widgets[BOOL_OPT_PL2_ATT_SKILL] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton23"));
    bool_options[BOOL_OPT_PL2_ATT_SKILL] = opt_user_intp("int_opt_user_pl2_att_skill");

    bool_widgets[BOOL_OPT_PL2_ATT_FITNESS] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton24"));
    bool_options[BOOL_OPT_PL2_ATT_FITNESS] = opt_user_intp("int_opt_user_pl2_att_fitness");

    bool_widgets[BOOL_OPT_PL2_ATT_GAMES] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton25"));
    bool_options[BOOL_OPT_PL2_ATT_GAMES] = opt_user_intp("int_opt_user_pl2_att_games");

    bool_widgets[BOOL_OPT_PL2_ATT_SHOTS] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton26"));
    bool_options[BOOL_OPT_PL2_ATT_SHOTS] = opt_user_intp("int_opt_user_pl2_att_shots");

    bool_widgets[BOOL_OPT_PL2_ATT_GOALS] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton27"));
    bool_options[BOOL_OPT_PL2_ATT_GOALS] = opt_user_intp("int_opt_user_pl2_att_goals");

    bool_widgets[BOOL_OPT_PL2_ATT_STATUS] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton28"));
    bool_options[BOOL_OPT_PL2_ATT_STATUS] = opt_user_intp("int_opt_user_pl2_att_status");

    bool_widgets[BOOL_OPT_PL2_ATT_CARDS] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton29"));
    bool_options[BOOL_OPT_PL2_ATT_CARDS] = opt_user_intp("int_opt_user_pl2_att_cards");

    bool_widgets[BOOL_OPT_PL2_ATT_AGE] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton30"));
    bool_options[BOOL_OPT_PL2_ATT_AGE] = opt_user_intp("int_opt_user_pl2_att_age");

    bool_widgets[BOOL_OPT_PL2_ATT_ETAL] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton31"));
    bool_options[BOOL_OPT_PL2_ATT_ETAL] = opt_user_intp("int_opt_user_pl2_att_etal");

    bool_widgets[BOOL_OPT_PL2_ATT_VALUE] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton32"));
    bool_options[BOOL_OPT_PL2_ATT_VALUE] = opt_user_intp("int_opt_user_pl2_att_value");

    bool_widgets[BOOL_OPT_PL2_ATT_WAGE] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton33"));
    bool_options[BOOL_OPT_PL2_ATT_WAGE] = opt_user_intp("int_opt_user_pl2_att_wage");

    bool_widgets[BOOL_OPT_PL2_ATT_CONTRACT] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton34"));
    bool_options[BOOL_OPT_PL2_ATT_CONTRACT] = opt_user_intp("int_opt_user_pl2_att_contract");

    bool_widgets[BOOL_OPT_PL2_ATT_TEAM] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton35"));
    bool_options[BOOL_OPT_PL2_ATT_TEAM] = opt_user_intp("int_opt_user_pl2_att_team");

    bool_widgets[BOOL_OPT_PL2_ATT_LEAGUE] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton36"));
    bool_options[BOOL_OPT_PL2_ATT_LEAGUE] = opt_user_intp("int_opt_user_pl2_att_league_cup");
}

/** Enumeration for the spinbutton widgets and options. */
enum SpinOptions
{
    SPIN_OPT_AUTOSAVE = 0,
    SPIN_OPT_PRECISION,
    SPIN_OPT_REFRESH,
    SPIN_OPT_LIVE_SPEED,
    SPIN_OPT_END
};

/** Write the pointers to the options and the corresponding widgets. */
void
option_gui_write_spin_widgets(gint **spin_options, GtkSpinButton **spin_widgets)
{
    spin_widgets[SPIN_OPT_AUTOSAVE] =
	GTK_SPIN_BUTTON(lookup_widget(window.options, "spinbutton_autosave"));
    spin_options[SPIN_OPT_AUTOSAVE] = opt_intp("int_opt_autosave_interval");

    spin_widgets[SPIN_OPT_PRECISION] =
	GTK_SPIN_BUTTON(lookup_widget(window.options, "spinbutton_precision"));
    spin_options[SPIN_OPT_PRECISION] = opt_intp("int_opt_player_precision");

    spin_widgets[SPIN_OPT_REFRESH] =
	GTK_SPIN_BUTTON(lookup_widget(window.options, "spinbutton_refresh"));
    spin_options[SPIN_OPT_REFRESH] = opt_intp("int_opt_live_game_player_list_refresh");

    spin_widgets[SPIN_OPT_LIVE_SPEED] =
	GTK_SPIN_BUTTON(lookup_widget(window.options, "spinbutton_live_speed"));
    spin_options[SPIN_OPT_LIVE_SPEED] = opt_user_intp("int_opt_user_live_game_speed");
}


/** Text entries. */
enum EntryOptions
{
    ENTRY_OPT_CONSTANTS = 0,
    ENTRY_OPT_FONT_NAME,
    ENTRY_OPT_END
};

/** Write the pointers to the options and the corresponding widgets. */
void
option_gui_write_entry_widgets(GString **entry_options, GtkEntry **entry_widgets)
{
    entry_widgets[ENTRY_OPT_CONSTANTS] = 
	GTK_ENTRY(lookup_widget(window.options, "entry_constants_file"));
    entry_options[ENTRY_OPT_CONSTANTS] = opt_strp("string_opt_constants_file");

    entry_widgets[ENTRY_OPT_FONT_NAME] = 
	GTK_ENTRY(lookup_widget(window.options, "entry_font_name"));
    entry_options[ENTRY_OPT_FONT_NAME] = opt_strp("string_opt_font_name");
}

/** Write the widget states in the options window
    according to the option values. */
void
option_gui_set_up_window(void)
{
    gint i;
    GtkToggleButton *bool_widgets[BOOL_OPT_END];
    gint *bool_options[BOOL_OPT_END];

    GtkSpinButton *spin_widgets[SPIN_OPT_END];
    gint *spin_options[SPIN_OPT_END];

    GtkEntry *entry_widgets[ENTRY_OPT_END];
    GString *entry_options[ENTRY_OPT_END];

    option_gui_write_bool_widgets(bool_options, bool_widgets);
    option_gui_write_spin_widgets(spin_options, spin_widgets);
    option_gui_write_entry_widgets(entry_options, entry_widgets);

    for(i=0;i<BOOL_OPT_END;i++)
	gtk_toggle_button_set_active(bool_widgets[i], *(bool_options[i]));

    for(i=0;i<SPIN_OPT_END;i++)
	gtk_spin_button_set_value(spin_widgets[i], (gfloat)(*(spin_options[i])));

    for(i=0;i<ENTRY_OPT_END;i++)
	gtk_entry_set_text(entry_widgets[i], (entry_options[i])->str);
}

/** Read the widget states in the options window and set the
    options accordingly. */
void
option_gui_write_options(void)
{
    gint i;
    GtkToggleButton *bool_widgets[BOOL_OPT_END];
    gint *bool_options[BOOL_OPT_END];

    GtkSpinButton *spin_widgets[SPIN_OPT_END];
    gint *spin_options[SPIN_OPT_END];

    GtkEntry *entry_widgets[ENTRY_OPT_END];
    GString *entry_options[ENTRY_OPT_END];

    option_gui_write_bool_widgets(bool_options, bool_widgets);
    option_gui_write_spin_widgets(spin_options, spin_widgets);
    option_gui_write_entry_widgets(entry_options, entry_widgets);

    for(i=0;i<BOOL_OPT_END;i++)
	*(bool_options[i]) = gtk_toggle_button_get_active(bool_widgets[i]);

    for(i=0;i<SPIN_OPT_END;i++)
	*(spin_options[i]) = gtk_spin_button_get_value_as_int(spin_widgets[i]);

    for(i=0;i<ENTRY_OPT_END;i++)
    {
	if(i == ENTRY_OPT_CONSTANTS && 
	   strcmp(gtk_entry_get_text(entry_widgets[i]), opt_str("string_opt_constants_file" )) != 0)
	    file_load_constants_file(gtk_entry_get_text(entry_widgets[i]));

	g_string_printf(entry_options[i], "%s", gtk_entry_get_text(entry_widgets[i]));
    }
}
