/*
   option_gui.c

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
#include "file.h"
#include "game_gui.h"
#include "language.h"
#include "option.h"
#include "option_gui.h"
#include "misc.h"
#include "support.h"
#include "training.h"
#include "treeview.h"
#include "user.h"
#include "variables.h"

/** An enumeration for all the options that are boolean,
    e.g. whether or not quit requires confirmation. */
enum BooleanOptions
{
    BOOL_OPT_CONF_QUIT = 0,
    BOOL_OPT_SAVE_OVERWRITE,
    BOOL_OPT_MAXIMIZE,
    BOOL_OPT_PREFER_MESS,
    BOOL_OPT_PROGRESSBAR_PICS,
    BOOL_OPT_SKIP,
    BOOL_OPT_AUTOSAVE,
    BOOL_OPT_AUTO_STORE_RESTORE_DEFAULT_TEAM,
    BOOL_OPT_SHOW_LIVE,
    BOOL_OPT_SHOW_TENDENCY,
    BOOL_OPT_PAUSE_INJURY,
    BOOL_OPT_PAUSE_RED,
    BOOL_OPT_PAUSE_BREAK,
    BOOL_OPT_AUTO_SUB,
    BOOL_OPT_CONF_UNFIT,
    BOOL_OPT_CONF_YOUTH,
    BOOL_OPT_SHOW_JOB,
    BOOL_OPT_SWAP,
    BOOL_OPT_SHOW_OVERALL,
    BOOL_OPT_SHOW_ALL_LEAGUES,
    BOOL_OPT_BET_SHOW_ALL_LEAGUES,
    BOOL_OPT_BET_SHOW_CUPS,
    BOOL_OPT_BET_SHOW_MY_RECENT,
    BOOL_OPT_NEWS_USER,
    BOOL_OPT_NEWS_CUP,
    BOOL_OPT_NEWS_LEAGUE,
    BOOL_OPT_NEWS_RECENT,
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
#ifdef DEBUG
    printf("option_gui_write_bool_widgets\n");
#endif

    bool_widgets[BOOL_OPT_CONF_QUIT] = 
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_conf_quit"));
    bool_options[BOOL_OPT_CONF_QUIT] = opt_intp("int_opt_confirm_quit");

    bool_widgets[BOOL_OPT_CONF_UNFIT] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_conf_unfit"));
    bool_options[BOOL_OPT_CONF_UNFIT] = opt_intp("int_opt_confirm_unfit");

    bool_widgets[BOOL_OPT_SAVE_OVERWRITE] = 
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_save_overwrite"));
    bool_options[BOOL_OPT_SAVE_OVERWRITE] = opt_intp("int_opt_save_will_overwrite");
    bool_widgets[BOOL_OPT_AUTO_STORE_RESTORE_DEFAULT_TEAM] = 
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_store_restore_default_team"));
    bool_options[BOOL_OPT_AUTO_STORE_RESTORE_DEFAULT_TEAM] = opt_user_intp("int_opt_user_store_restore_default_team");
    bool_widgets[BOOL_OPT_MAXIMIZE] = 
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_maximize"));
    bool_options[BOOL_OPT_MAXIMIZE] = opt_intp("int_opt_maximize_main_window");

    bool_widgets[BOOL_OPT_PREFER_MESS] = 
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_pref_mess"));
    bool_options[BOOL_OPT_PREFER_MESS] = opt_intp("int_opt_prefer_messages");

    bool_widgets[BOOL_OPT_PROGRESSBAR_PICS] = 
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, 
					"checkbutton_show_progress_pics"));
    bool_options[BOOL_OPT_PROGRESSBAR_PICS] = opt_intp("int_opt_progressbar_pics");

    bool_widgets[BOOL_OPT_SKIP] = 
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_skip"));
    bool_options[BOOL_OPT_SKIP] = opt_intp("int_opt_skip");

    bool_widgets[BOOL_OPT_AUTOSAVE] = 
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_autosave"));
    bool_options[BOOL_OPT_AUTOSAVE] = opt_intp("int_opt_autosave");

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

    bool_widgets[BOOL_OPT_CONF_YOUTH] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_conf_youth"));
    bool_options[BOOL_OPT_CONF_YOUTH] = opt_user_intp("int_opt_user_confirm_youth");

    bool_widgets[BOOL_OPT_SHOW_JOB] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_show_job"));
    bool_options[BOOL_OPT_SHOW_JOB] = opt_user_intp("int_opt_user_show_job_offers");

    bool_widgets[BOOL_OPT_SWAP] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_swap_adapts"));
    bool_options[BOOL_OPT_SWAP] = opt_user_intp("int_opt_user_swap_adapts");

    bool_widgets[BOOL_OPT_SHOW_OVERALL] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_show_overall"));
    bool_options[BOOL_OPT_SHOW_OVERALL] = opt_user_intp("int_opt_user_show_overall");

    bool_widgets[BOOL_OPT_SHOW_ALL_LEAGUES] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_show_all_leagues"));
    bool_options[BOOL_OPT_SHOW_ALL_LEAGUES] = opt_user_intp("int_opt_user_show_all_leagues");

    bool_widgets[BOOL_OPT_BET_SHOW_ALL_LEAGUES] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_bet_show_all_leagues"));
    bool_options[BOOL_OPT_BET_SHOW_ALL_LEAGUES] = opt_user_intp("int_opt_user_bet_show_all_leagues");

    bool_widgets[BOOL_OPT_BET_SHOW_CUPS] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_bet_show_cups"));
    bool_options[BOOL_OPT_BET_SHOW_CUPS] = opt_user_intp("int_opt_user_bet_show_cups");

    bool_widgets[BOOL_OPT_BET_SHOW_MY_RECENT] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_bet_show_only_recent"));
    bool_options[BOOL_OPT_BET_SHOW_MY_RECENT] = opt_user_intp("int_opt_user_bet_show_my_recent");

    bool_widgets[BOOL_OPT_NEWS_USER] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_news_user"));
    bool_options[BOOL_OPT_NEWS_USER] = opt_intp("int_opt_news_create_user");

    bool_widgets[BOOL_OPT_NEWS_CUP] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_news_cup"));
    bool_options[BOOL_OPT_NEWS_CUP] = opt_intp("int_opt_news_create_cup");

    bool_widgets[BOOL_OPT_NEWS_LEAGUE] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_news_league"));
    bool_options[BOOL_OPT_NEWS_LEAGUE] = opt_intp("int_opt_news_create_league");

    bool_widgets[BOOL_OPT_NEWS_RECENT] =
	GTK_TOGGLE_BUTTON(lookup_widget(window.options, "checkbutton_news_recent"));
    bool_options[BOOL_OPT_NEWS_RECENT] = opt_intp("int_opt_news_show_recent");

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
    SPIN_OPT_AUTOSAVE_FILES,
    SPIN_OPT_PRECISION,
    SPIN_OPT_REFRESH,
    SPIN_OPT_LIVE_SPEED,
    SPIN_OPT_LIVE_VERBOSITY,
    SPIN_OPT_CONTRACT,
    SPIN_OPT_BET_WAGER,
    SPIN_OPT_TRAINING_CAMP_RECREATION,
    SPIN_OPT_END
};

/** Write the pointers to the options and the corresponding widgets. */
void
option_gui_write_spin_widgets(gint **spin_options, GtkSpinButton **spin_widgets)
{
#ifdef DEBUG
    printf("option_gui_write_spin_widgets\n");
#endif

    gint tmp = 0;

    spin_widgets[SPIN_OPT_AUTOSAVE] =
	GTK_SPIN_BUTTON(lookup_widget(window.options, "spinbutton_autosave"));
    spin_options[SPIN_OPT_AUTOSAVE] = opt_intp("int_opt_autosave_interval");

    spin_widgets[SPIN_OPT_AUTOSAVE_FILES] =
	GTK_SPIN_BUTTON(lookup_widget(window.options, "spinbutton_autosave_files"));
    spin_options[SPIN_OPT_AUTOSAVE_FILES] = opt_intp("int_opt_autosave_files");

    spin_widgets[SPIN_OPT_PRECISION] =
	GTK_SPIN_BUTTON(lookup_widget(window.options, "spinbutton_precision"));
    spin_options[SPIN_OPT_PRECISION] = opt_intp("int_opt_player_precision");

    spin_widgets[SPIN_OPT_REFRESH] =
	GTK_SPIN_BUTTON(lookup_widget(window.options, "spinbutton_refresh"));
    spin_options[SPIN_OPT_REFRESH] = opt_intp("int_opt_live_game_player_list_refresh");

    spin_widgets[SPIN_OPT_LIVE_SPEED] =
	GTK_SPIN_BUTTON(lookup_widget(window.options, "spinbutton_live_speed"));
    spin_options[SPIN_OPT_LIVE_SPEED] = opt_user_intp("int_opt_user_live_game_speed");

    spin_widgets[SPIN_OPT_TRAINING_CAMP_RECREATION] =
	GTK_SPIN_BUTTON(lookup_widget(window.options, "spinbutton_recreation"));
    spin_options[SPIN_OPT_TRAINING_CAMP_RECREATION] = opt_user_intp("int_opt_user_training_camp_recreation");

    /** Note the spinbutton value so that it doesn't get lost
	when setting the range. */    
    tmp = gtk_spin_button_get_value_as_int(spin_widgets[SPIN_OPT_LIVE_SPEED]);
    gtk_spin_button_set_range(
	spin_widgets[SPIN_OPT_LIVE_SPEED], 0,
	-rint((gfloat)(const_int("int_game_gui_live_game_speed_max") - 10) /
	      (gfloat)(const_int("int_game_gui_live_game_speed_grad"))));
    gtk_spin_button_set_value(spin_widgets[SPIN_OPT_LIVE_SPEED],
			      (gdouble)tmp);

    spin_widgets[SPIN_OPT_LIVE_VERBOSITY] =
	GTK_SPIN_BUTTON(lookup_widget(window.options, "spinbutton_live_verbosity"));
    spin_options[SPIN_OPT_LIVE_VERBOSITY] = opt_user_intp("int_opt_user_live_game_verbosity");

    spin_widgets[SPIN_OPT_CONTRACT] =
	GTK_SPIN_BUTTON(lookup_widget(window.options, "spinbutton_contract"));
    spin_options[SPIN_OPT_CONTRACT] = opt_user_intp("int_opt_user_contract_limit");

    spin_widgets[SPIN_OPT_BET_WAGER] =
	GTK_SPIN_BUTTON(lookup_widget(window.options, "spinbutton_bet_wager"));
    spin_options[SPIN_OPT_BET_WAGER] = opt_user_intp("int_opt_user_bet_default_wager");

    tmp = gtk_spin_button_get_value_as_int(spin_widgets[SPIN_OPT_BET_WAGER]);
    gtk_spin_button_set_range(
	spin_widgets[SPIN_OPT_BET_WAGER], 1,
	(gdouble)const_int("int_bet_wager_max"));
    gtk_spin_button_set_value(spin_widgets[SPIN_OPT_BET_WAGER],
			      (gdouble)tmp);
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
option_gui_write_entry_widgets(gchar ***entry_options, GtkEntry **entry_widgets)
{
#ifdef DEBUG
    printf("option_gui_write_entry_widgets\n");
#endif

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
#ifdef DEBUG
    printf("option_gui_set_up_window\n");
#endif

    gint i;
    gchar buf[SMALL];
    GtkToggleButton *bool_widgets[BOOL_OPT_END];
    gint *bool_options[BOOL_OPT_END];

    GtkSpinButton *spin_widgets[SPIN_OPT_END];
    gint *spin_options[SPIN_OPT_END];

    GtkEntry *entry_widgets[ENTRY_OPT_END];
    gchar **entry_options[ENTRY_OPT_END];

    GtkToggleButton *news_popup_buttons[3] = 
        {GTK_TOGGLE_BUTTON(lookup_widget(window.options, "radiobutton_news_popup_no")),
         GTK_TOGGLE_BUTTON(lookup_widget(window.options, "radiobutton_news_popup_user")),
         GTK_TOGGLE_BUTTON(lookup_widget(window.options, "radiobutton_news_popup_always"))};

    treeview_show_language_combo();
    treeview_show_training_hotels_combo();

    option_gui_write_bool_widgets(bool_options, bool_widgets);
    option_gui_write_spin_widgets(spin_options, spin_widgets);
    option_gui_write_entry_widgets(entry_options, entry_widgets);

    for(i=0;i<BOOL_OPT_END;i++)
        gtk_toggle_button_set_active(bool_widgets[i], *(bool_options[i]));
	
    for(i=0;i<SPIN_OPT_END;i++)
	gtk_spin_button_set_value(spin_widgets[i], (gfloat)(*(spin_options[i])));

    for(i=0;i<ENTRY_OPT_END;i++)
	gtk_entry_set_text(entry_widgets[i], *(entry_options[i]));

    sprintf(buf, "%d", CAMP_SCALE_MAX - gtk_spin_button_get_value_as_int(
		GTK_SPIN_BUTTON(lookup_widget(window.options, "spinbutton_recreation"))));
    gtk_label_set_text(GTK_LABEL(lookup_widget(window.options, "label_training")), buf);

    gtk_toggle_button_set_active(news_popup_buttons[opt_int("int_opt_news_popup")], TRUE);
}

/** Read the widget states in the options window and set the
    options accordingly. */
void
option_gui_write_options(void)
{
#ifdef DEBUG
    printf("option_gui_write_options\n");
#endif

    gint i;
    gint language_index = 
	gtk_combo_box_get_active(GTK_COMBO_BOX(lookup_widget(window.options, "combobox_languages")));
    gint training_camp_hotel =
	gtk_combo_box_get_active(GTK_COMBO_BOX(lookup_widget(window.options, "combobox_hotel"))) + 1;
    GtkToggleButton *bool_widgets[BOOL_OPT_END];
    gint *bool_options[BOOL_OPT_END];
    GtkSpinButton *spin_widgets[SPIN_OPT_END];
    gint *spin_options[SPIN_OPT_END];
    GtkEntry *entry_widgets[ENTRY_OPT_END];
    gchar **entry_options[ENTRY_OPT_END];
    GtkToggleButton *news_popup_buttons[3] = 
        {GTK_TOGGLE_BUTTON(lookup_widget(window.options, "radiobutton_news_popup_no")),
         GTK_TOGGLE_BUTTON(lookup_widget(window.options, "radiobutton_news_popup_user")),
         GTK_TOGGLE_BUTTON(lookup_widget(window.options, "radiobutton_news_popup_always"))};

    language_set(language_index);
    opt_user_set_int("int_opt_user_training_camp_hotel", training_camp_hotel);

    option_gui_write_bool_widgets(bool_options, bool_widgets);
    option_gui_write_spin_widgets(spin_options, spin_widgets);
    option_gui_write_entry_widgets(entry_options, entry_widgets);    

    for(i=0;i<BOOL_OPT_END;i++)
	*(bool_options[i]) = gtk_toggle_button_get_active(bool_widgets[i]);

    for(i=0;i<SPIN_OPT_END;i++)
	*(spin_options[i]) = gtk_spin_button_get_value_as_int(spin_widgets[i]);

    for(i=0;i<ENTRY_OPT_END;i++)
    {
	misc_string_assign(entry_options[i], gtk_entry_get_text(entry_widgets[i]));

	if(i == ENTRY_OPT_CONSTANTS && 
	   strcmp(gtk_entry_get_text(entry_widgets[i]), opt_str("string_opt_constants_file")) != 0)
	    file_load_opt_file(gtk_entry_get_text(entry_widgets[i]), &constants, TRUE);
	else if(i == ENTRY_OPT_FONT_NAME &&
		strcmp(gtk_entry_get_text(entry_widgets[i]), opt_str("string_opt_font_name")) != 0)
	    on_button_back_to_main_clicked(NULL, NULL);
    }

    for(i = 0; i < 3; i++)
        if(gtk_toggle_button_get_active(news_popup_buttons[i]))
        {
            opt_set_int("int_opt_news_popup", i);
            break;
        }
                
    game_gui_write_check_items();
}
