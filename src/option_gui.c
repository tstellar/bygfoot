#include "option.h"
#include "option_gui.h"
#include "support.h"
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
/*     BOOL_OPT_CONF_UNFIT, */
/*     BOOL_OPT_SHOW_LIVE, */
/*     BOOL_OPT_SHOW_TENDENCY, */
/*     BOOL_OPT_PAUSE_INJURY, */
/*     BOOL_OPT_PAUSE_RED, */
/*     BOOL_OPT_PAUSE_BREAK, */
/*     BOOL_OPT_AUTO_SUB, */
/*     BOOL_OPT_SHOW_JOB, */
/*     BOOL_OPT_REARRANGE, */
/*     BOOL_OPT_SWAP, */
/*     BOOL_OPT_SHOW_OVERALL, */
/*     BOOL_OPT_PL1_ATT_NAME, */
/*     BOOL_OPT_PL1_ATT_CPOS, */
/*     BOOL_OPT_PL1_ATT_POS, */
/*     BOOL_OPT_PL1_ATT_CSKILL, */
/*     BOOL_OPT_PL1_ATT_SKILL, */
/*     BOOL_OPT_PL1_ATT_FITNESS, */
/*     BOOL_OPT_PL1_ATT_GAMES, */
/*     BOOL_OPT_PL1_ATT_SHOTS, */
/*     BOOL_OPT_PL1_ATT_GOALS, */
/*     BOOL_OPT_PL1_ATT_STATUS, */
/*     BOOL_OPT_PL1_ATT_CARDS, */
/*     BOOL_OPT_PL1_ATT_AGE, */
/*     BOOL_OPT_PL1_ATT_ETAL, */
/*     BOOL_OPT_PL1_ATT_VALUE, */
/*     BOOL_OPT_PL1_ATT_WAGE, */
/*     BOOL_OPT_PL1_ATT_CONTRACT, */
/*     BOOL_OPT_PL1_ATT_TEAM, */
/*     BOOL_OPT_PL1_ATT_LEAGUE, */
/*     BOOL_OPT_PL2_ATT_NAME, */
/*     BOOL_OPT_PL2_ATT_CPOS, */
/*     BOOL_OPT_PL2_ATT_POS, */
/*     BOOL_OPT_PL2_ATT_CSKILL, */
/*     BOOL_OPT_PL2_ATT_SKILL, */
/*     BOOL_OPT_PL2_ATT_FITNESS, */
/*     BOOL_OPT_PL2_ATT_GAMES, */
/*     BOOL_OPT_PL2_ATT_SHOTS, */
/*     BOOL_OPT_PL2_ATT_GOALS, */
/*     BOOL_OPT_PL2_ATT_STATUS, */
/*     BOOL_OPT_PL2_ATT_CARDS, */
/*     BOOL_OPT_PL2_ATT_AGE, */
/*     BOOL_OPT_PL2_ATT_ETAL, */
/*     BOOL_OPT_PL2_ATT_VALUE, */
/*     BOOL_OPT_PL2_ATT_WAGE, */
/*     BOOL_OPT_PL2_ATT_CONTRACT, */
/*     BOOL_OPT_PL2_ATT_TEAM, */
/*     BOOL_OPT_PL2_ATT_LEAGUE, */
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
    bool_options[BOOL_OPT_SAVE_OVERWRITE] = opt_intp("int_opt_save_will_ovewrite");

    bool_widgets[BOOL_OPT_MAXIMIZE] = 
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_maximize"));
    bool_options[BOOL_OPT_MAXIMIZE] = opt_intp("int_opt_maximize_main_window");

    bool_widgets[BOOL_OPT_PREFER_MESS] = 
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_pref_mess"));
    bool_options[BOOL_OPT_PREFER_MESS] = opt_intp("int_opt_prefer_messages");

    bool_widgets[BOOL_OPT_AUTOSAVE] = 
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_autosave"));
    bool_options[BOOL_OPT_AUTOSAVE] = opt_intp("int_opt_autosave");

/*     bool_widgets[BOOL_OPT_CONF_UNFIT] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_CONF_UNFIT] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_SHOW_LIVE] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_SHOW_LIVE] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_SHOW_TENDENCY] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_SHOW_TENDENCY] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PAUSE_INJURY] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PAUSE_INJURY] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PAUSE_RED] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PAUSE_RED] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PAUSE_BREAK] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PAUSE_BREAK] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_AUTO_SUB] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_AUTO_SUB] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_SHOW_JOB] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_SHOW_JOB] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_REARRANGE] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_REARRANGE] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_SWAP] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_SWAP] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_SHOW_OVERALL] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_SHOW_OVERALL] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL1_ATT_NAME] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL1_ATT_NAME] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL1_ATT_CPOS] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL1_ATT_CPOS] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL1_ATT_POS] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL1_ATT_POS] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL1_ATT_CSKILL] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL1_ATT_CSKILL] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL1_ATT_SKILL] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL1_ATT_SKILL] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL1_ATT_FITNESS] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL1_ATT_FITNESS] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL1_ATT_GAMES] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL1_ATT_GAMES] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL1_ATT_SHOTS] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL1_ATT_SHOTS] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL1_ATT_GOALS] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL1_ATT_GOALS] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL1_ATT_STATUS] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL1_ATT_STATUS] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL1_ATT_CARDS] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL1_ATT_CARDS] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL1_ATT_AGE] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL1_ATT_AGE] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL1_ATT_ETAL] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL1_ATT_ETAL] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL1_ATT_VALUE] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL1_ATT_VALUE] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL1_ATT_WAGE] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL1_ATT_WAGE] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL1_ATT_CONTRACT] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL1_ATT_CONTRACT] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL1_ATT_TEAM] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL1_ATT_TEAM] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL1_ATT_LEAGUE] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL1_ATT_LEAGUE] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL2_ATT_NAME] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL2_ATT_NAME] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL2_ATT_CPOS] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL2_ATT_CPOS] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL2_ATT_POS] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL2_ATT_POS] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL2_ATT_CSKILL] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL2_ATT_CSKILL] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL2_ATT_SKILL] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL2_ATT_SKILL] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL2_ATT_FITNESS] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL2_ATT_FITNESS] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL2_ATT_GAMES] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL2_ATT_GAMES] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL2_ATT_SHOTS] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL2_ATT_SHOTS] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL2_ATT_GOALS] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL2_ATT_GOALS] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL2_ATT_STATUS] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL2_ATT_STATUS] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL2_ATT_CARDS] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL2_ATT_CARDS] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL2_ATT_AGE] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL2_ATT_AGE] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL2_ATT_ETAL] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL2_ATT_ETAL] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL2_ATT_VALUE] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL2_ATT_VALUE] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL2_ATT_WAGE] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL2_ATT_WAGE] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL2_ATT_CONTRACT] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL2_ATT_CONTRACT] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL2_ATT_TEAM] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL2_ATT_TEAM] = opt_user_intp("int_opt_user_"); */

/*     bool_widgets[BOOL_OPT_PL2_ATT_LEAGUE] = */
/* 	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_")); */
/*     bool_options[BOOL_OPT_PL2_ATT_LEAGUE] = opt_user_intp("int_opt_user_"); */
}

/** Enumeration for the spinbutton widgets and options. */
enum SpinOptions
{
    SPIN_OPT_AUTOSAVE = 0,
    SPIN_OPT_PRECISION,
/*     SPIN_OPT_REFRESH, */
/*     SPIN_OPT_LIVE_SPEED, */
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

/*     spin_widgets[SPIN_OPT_REFRESH] = */
/* 	GTK_SPIN_BUTTON(lookup_widget(window.options, "spinbutton_refresh")); */
/*     spin_options[SPIN_OPT_REFRESH] = opt_intp("int_opt_live_game_player_list_refresh"); */

/*     spin_widgets[SPIN_OPT_LIVE_SPEED] = */
/* 	GTK_SPIN_BUTTON(lookup_widget(window.options, "spinbutton_")); */
/*     spin_options[SPIN_OPT_LIVE_SPEED] = opt_user_intp("int_opt_user_live_game_speed"); */
}


/** Text entries. */
enum EntryOptions
{
    ENTRY_OPT_NAMES = 0,
    ENTRY_OPT_CONSTANTS,
    ENTRY_OPT_USER_CONF,
    ENTRY_OPT_END
};

/** Write the pointers to the options and the corresponding widgets. */
void
option_gui_write_entry_widgets(GString **entry_options, GtkEntry **entry_widgets)
{
    entry_widgets[ENTRY_OPT_NAMES] = 
	GTK_ENTRY(lookup_widget(window.options, "entry_names_file"));
    entry_options[ENTRY_OPT_NAMES] = opt_strp("string_opt_player_names_file");

    entry_widgets[ENTRY_OPT_CONSTANTS] = 
	GTK_ENTRY(lookup_widget(window.options, "entry_constants_file"));
    entry_options[ENTRY_OPT_CONSTANTS] = opt_strp("string_opt_constants_file");

    entry_widgets[ENTRY_OPT_USER_CONF] = 
	GTK_ENTRY(lookup_widget(window.options, "entry_user_file"));
    entry_options[ENTRY_OPT_USER_CONF] = opt_strp("string_opt_default_user_conf_file");
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
