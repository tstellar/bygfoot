/*
   game_gui.c

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
#include "gui.h"
#include "job.h"
#include "league.h"
#include "live_game.h"
#include "maths.h"
#include "misc.h"
#include "option.h"
#include "treeview.h"
#include "support.h"
#include "team.h"
#include "user.h"
#include "variables.h"
#include "window.h"

/** Show the live game in the live game window.
    @param unit The current unit we show. */
void
game_gui_live_game_show_unit(const LiveGameUnit *unit)
{
#ifdef DEBUG
    printf("game_gui_live_game_show_unit\n");
#endif

    gchar buf[SMALL];
    gfloat sleep_factor = (unit->time == 3) ? 
	const_float("float_game_gui_live_game_speed_penalties_factor") : 1;
    gfloat fraction = (gfloat)live_game_unit_get_minute(unit) / 90;
    GtkProgressBar *progress_bar =
	GTK_PROGRESS_BAR(lookup_widget(window.live, "progressbar_live"));
    GtkHScale *hscale = 
	GTK_HSCALE(lookup_widget(window.live, "hscale_area"));
    GtkWidget *button_pause = lookup_widget(window.live, "button_pause"),
	*button_resume = lookup_widget(window.live, "button_resume"),
	*button_live_close = lookup_widget(window.live, "button_live_close"),
	*eventbox_poss[2] = {lookup_widget(window.live, "eventbox_poss0"),
			     lookup_widget(window.live, "eventbox_poss1")};
    GdkColor color;

    if(unit->event.type == LIVE_GAME_EVENT_START_MATCH)
	treeview_live_game_show_initial_commentary(unit);
    else if(option_int("int_opt_user_live_game_verbosity", 
		       &usr(stat2).options) > unit->event.verbosity)
	treeview_live_game_show_commentary(unit);

    treeview_live_game_show_result(unit);

    gdk_color_parse (const_app("string_live_game_possession_color"), &color);
    gtk_widget_modify_bg(eventbox_poss[unit->possession], GTK_STATE_NORMAL, &color);
    gtk_widget_modify_bg(eventbox_poss[!unit->possession], GTK_STATE_NORMAL, NULL);

    if(option_int("int_opt_user_show_tendency_bar",
		  &usr(stat2).options))
	game_gui_live_game_set_hscale(unit, hscale);
    else
	gtk_widget_hide(GTK_WIDGET(hscale));

    sprintf(buf, "%d.", live_game_unit_get_minute(unit));
    gtk_progress_bar_set_fraction(progress_bar, (fraction > 1) ? 1 : fraction);
    gtk_progress_bar_set_text(progress_bar, buf);
    g_usleep((gint)rint(sleep_factor * 
			(gfloat)(const_int("int_game_gui_live_game_speed_max") +
				 (option_int("int_opt_user_live_game_speed", &usr(stat2).options) * 
				  const_int("int_game_gui_live_game_speed_grad")))));

    while(gtk_events_pending())
	gtk_main_iteration();

    if(unit->event.type == LIVE_GAME_EVENT_START_MATCH)
    {
	gtk_widget_set_sensitive(button_live_close, (stat1 == STATUS_SHOW_LAST_MATCH));
	gtk_widget_set_sensitive(button_pause, TRUE);
	gtk_widget_set_sensitive(button_resume, FALSE);
	gtk_widget_grab_focus(button_pause);
    }
    else if(unit->event.type == LIVE_GAME_EVENT_END_MATCH)
    {
	gtk_widget_set_sensitive(button_live_close, TRUE);
	gtk_widget_set_sensitive(button_pause, FALSE);
	gtk_widget_set_sensitive(button_resume, FALSE);
	gui_set_sensitive_lg_meters(FALSE);
	gtk_widget_grab_focus(button_live_close);

	game_gui_set_main_window_sensitivity(FALSE);
    }
    else if(unit->event.type == LIVE_GAME_EVENT_PENALTIES)
    {
	gtk_widget_set_sensitive(button_pause, (stat1 == STATUS_SHOW_LAST_MATCH));
	gtk_widget_set_sensitive(button_resume, FALSE);
    }
}

/** Set the area scale position and color in the live game window.
    @param unit The current unit.
    @param hscale The scale widget. */
void
game_gui_live_game_set_hscale(const LiveGameUnit *unit, GtkHScale *hscale)
{
#ifdef DEBUG
    printf("game_gui_live_game_set_hscale\n");
#endif

    GdkColor color;

    gtk_widget_modify_bg(GTK_WIDGET(hscale), GTK_STATE_NORMAL, NULL);

    if(unit->area == LIVE_GAME_UNIT_AREA_MIDFIELD)
    {
	gdk_color_parse(const_app("string_game_gui_live_game_scale_color_midfield"), &color);
	gtk_range_set_value(GTK_RANGE(hscale),
			    const_float("float_game_gui_live_game_scale_range") / 2);
    }
    else if(unit->event.type == LIVE_GAME_EVENT_GOAL ||
	    unit->event.type == LIVE_GAME_EVENT_OWN_GOAL)
    {
	gdk_color_parse(const_app("string_game_gui_live_game_scale_color_goal"), &color);

	gtk_range_set_value(GTK_RANGE(hscale), 
			    const_float("float_game_gui_live_game_scale_range") *
			    (unit->possession == 0));
    }
    else if(unit->event.type == LIVE_GAME_EVENT_SCORING_CHANCE ||
	    unit->event.type == LIVE_GAME_EVENT_PENALTY ||
	    unit->event.type == LIVE_GAME_EVENT_FREE_KICK)
    {
	gdk_color_parse(const_app("string_game_gui_live_game_scale_color_chance"), &color);

	gtk_range_set_value(GTK_RANGE(hscale), 
			    const_float("float_game_gui_live_game_scale_range") / 2 +
			    (const_float("float_game_gui_live_game_scale_range") * 
			     const_float("float_game_gui_live_game_scale_chance") *
			     ((unit->possession == 0) ? 1 : -1)));
    }
    else if(unit->event.type == LIVE_GAME_EVENT_POST ||
	    unit->event.type == LIVE_GAME_EVENT_MISS ||
	    unit->event.type == LIVE_GAME_EVENT_SAVE ||
	    unit->event.type == LIVE_GAME_EVENT_CROSS_BAR)
	gdk_color_parse(const_app("string_game_gui_live_game_scale_color_miss"), &color);
    else if(unit->area == LIVE_GAME_UNIT_AREA_ATTACK)
    {
	gdk_color_parse(const_app("string_game_gui_live_game_scale_color_attack"), &color);	
	gtk_range_set_value(GTK_RANGE(hscale),
			    const_float("float_game_gui_live_game_scale_range") / 2 +
			    (const_float("float_game_gui_live_game_scale_range") * 
			     const_float("float_game_gui_live_game_scale_attack") *
			     ((unit->possession == 0) ? 1 : -1)));
    }
    else if(unit->area == LIVE_GAME_UNIT_AREA_DEFEND)
    {
	gdk_color_parse(const_app("string_game_gui_live_game_scale_color_defend"), &color);
	gtk_range_set_value(GTK_RANGE(hscale),
			    const_float("float_game_gui_live_game_scale_range") / 2 +
			    (const_float("float_game_gui_live_game_scale_range") * 
			     const_float("float_game_gui_live_game_scale_attack") *
			     ((unit->possession == 0) ? -1 : 1)));
    }
    else
	debug_print_message("game_gui_live_game_set_hscale: don't know what to do!\n");

    gtk_widget_modify_bg(GTK_WIDGET(hscale), GTK_STATE_NORMAL, &color);
}

/** Show the player list of the opposing team in the live game
    window. */
void
game_gui_live_game_show_opponent(void)
{
#ifdef DEBUG
    printf("game_gui_live_game_show_opponent\n");
#endif

    GtkImage *image_style = 
	GTK_IMAGE(lookup_widget(window.live, "image_lg_opp_style")),
	*image_boost = 
	GTK_IMAGE(lookup_widget(window.live, "image_lg_opp_boost"));
    GtkLabel *label_form = 
	GTK_LABEL(lookup_widget(window.live, "label_lg_formation")),
	*label_avskill = 
	GTK_LABEL(lookup_widget(window.live, "label_lg_avskill"));
    GtkTreeView *treeview = 
	GTK_TREE_VIEW(lookup_widget(window.live, "treeview_lg_opponent"));
    gint idx = (team_is_user(((LiveGame*)statp)->fix->teams[0]) == -1);
    const Team *tm = ((LiveGame*)statp)->fix->teams[!idx];
    gint scout = user_from_team(((LiveGame*)statp)->fix->teams[idx])->scout;
    gfloat avskills[2] = {team_get_average_skill(tm, TRUE),
			  team_get_average_skill(current_user.tm, TRUE)};
    gchar buf[SMALL];

    treeview_show_player_list_team(treeview, tm, scout);
    game_gui_write_meter_images(tm, image_style, image_boost);    
    gui_label_set_text_from_int(label_form, tm->structure, FALSE);

    sprintf(buf, "%.1f (%+.1f)", avskills[0], 
	    avskills[0] - avskills[1]);
    gtk_label_set_text(label_avskill, buf);
}

/** Look up the widgets in the main window. */
void
game_gui_get_radio_items(GtkWidget **style, GtkWidget **scout,
			 GtkWidget **physio, GtkWidget **boost,
			 GtkWidget **yc, GtkWidget **ya_pos_pref)
{
#ifdef DEBUG
    printf("game_gui_get_radio_items\n");
#endif

    style[0] = lookup_widget(window.main, "menu_all_out_defend");
    style[1] = lookup_widget(window.main, "menu_defend");
    style[2] = lookup_widget(window.main, "menu_balanced");
    style[3] = lookup_widget(window.main, "menu_attack");
    style[4] = lookup_widget(window.main, "menu_all_out_attack");

    scout[0] = lookup_widget(window.main, "menu_scout_best");
    scout[1] = lookup_widget(window.main, "menu_scout_good");
    scout[2] = lookup_widget(window.main, "menu_scout_average");
    scout[3] = lookup_widget(window.main, "menu_scout_bad");

    physio[0] = lookup_widget(window.main, "menu_physio_best");
    physio[1] = lookup_widget(window.main, "menu_physio_good");
    physio[2] = lookup_widget(window.main, "menu_physio_average");
    physio[3] = lookup_widget(window.main, "menu_physio_bad");

    boost[0] = lookup_widget(window.main, "menu_boost_anti");
    boost[1] = lookup_widget(window.main, "menu_boost_off");
    boost[2] = lookup_widget(window.main, "menu_boost_on");

    yc[0] = lookup_widget(window.main, "menu_yc_best");
    yc[1] = lookup_widget(window.main, "menu_yc_good");
    yc[2] = lookup_widget(window.main, "menu_yc_average");
    yc[3] = lookup_widget(window.main, "menu_yc_bad");

    ya_pos_pref[0] = lookup_widget(window.main, "menu_recruit0");
    ya_pos_pref[1] = lookup_widget(window.main, "menu_recruit1");
    ya_pos_pref[2] = lookup_widget(window.main, "menu_recruit2");
    ya_pos_pref[3] = lookup_widget(window.main, "menu_recruit3");
    ya_pos_pref[4] = lookup_widget(window.main, "menu_recruit4");
}

/** Set information like season, user, week etc. into the appropriate labels. */
void
game_gui_set_main_window_header(void)
{
#ifdef DEBUG
    printf("game_gui_set_main_window_header\n");
#endif

    gint i, rank;
    gchar buf[SMALL];
    GtkLabel *label_user= GTK_LABEL(lookup_widget(window.main, "label_user")),
	*label_season= GTK_LABEL(lookup_widget(window.main, "label_season")),
	*label_week= GTK_LABEL(lookup_widget(window.main, "label_week")),
	*label_round= GTK_LABEL(lookup_widget(window.main, "label_round")),
	*label_team= GTK_LABEL(lookup_widget(window.main, "label_team")),
	*label_league= GTK_LABEL(lookup_widget(window.main, "label_league")),
	*label_rank= GTK_LABEL(lookup_widget(window.main, "label_rank")),
	*label_money= GTK_LABEL(lookup_widget(window.main, "label_money"));
    GtkWidget *menu_users[2] = {lookup_widget(window.main, "menu_next_user"),
				lookup_widget(window.main, "menu_previous_user")};
    const Fixture *fix = team_get_fixture(current_user.tm, FALSE);

    gtk_label_set_text(label_user, current_user.name);
    gui_label_set_text_from_int(label_season, season, FALSE);
    gui_label_set_text_from_int(label_week, week, FALSE);
    gui_label_set_text_from_int(label_round, week_round, FALSE);

    if(!sett_int("int_opt_disable_finances"))
    {
	misc_print_grouped_int(current_user.money, buf);
	gtk_label_set_text(label_money, buf);
    }
    else
    {
	gtk_widget_hide(GTK_WIDGET(label_money));
	gtk_widget_hide(GTK_WIDGET(lookup_widget(window.main, "label34")));
    }

    gtk_label_set_text(label_team, current_user.tm->name);

    if(fix == NULL)
    {
        rank = team_get_league_rank(current_user.tm, -1);
        gtk_label_set_text(label_league, league_cup_get_name_string(current_user.tm->clid));
    }
    else
    {
        rank = team_get_league_rank(current_user.tm, fix->clid);
        gtk_label_set_text(label_league, league_cup_get_name_string(fix->clid));        
    }

    if(rank != 0)
	gui_label_set_text_from_int(label_rank, rank, FALSE);
    else
    {
	gtk_widget_hide(GTK_WIDGET(label_rank));
	gtk_widget_hide(lookup_widget(window.main, "label29"));
    }

    for(i=0;i<2;i++)
	gtk_widget_set_sensitive(menu_users[i], (users->len > 1));

    game_gui_write_av_skills(NULL);

    game_gui_write_radio_items();

    game_gui_write_meters(current_user.tm);

    game_gui_write_check_items();
}

/** Set the average skills of the current team
    into the appropriate labels. */
void
game_gui_write_av_skills(const Team *tm)
{
#ifdef DEBUG
    printf("game_gui_write_av_skills\n");
#endif

    gchar buf[SMALL];
    GtkLabel *label_av_skills= GTK_LABEL(lookup_widget(window.main, "label_av_skills"));
    const Team *tm_local = (tm == NULL) ? current_user.tm : tm;

    sprintf(buf, "%.1f  %.1f", 
	    team_get_average_skill(tm_local, TRUE),
	    team_get_average_skill(tm_local, FALSE));
    gtk_label_set_text(label_av_skills, buf);
}

/** Set the images for the style and boost meters to the appropriate values
    from the team settings. */
void
game_gui_write_meter_images(const Team *tm, GtkImage *style, GtkImage *boost)
{
#ifdef DEBUG
    printf("game_gui_write_meter_images\n");
#endif

    gint i;

    gchar *image_style_files[5] = 
	{file_find_support_file(const_app("string_game_gui_style_all_out_defend_icon"), TRUE),
	 file_find_support_file(const_app("string_game_gui_style_defend_icon"), TRUE),
	 file_find_support_file(const_app("string_game_gui_style_balanced_icon"), TRUE),
	 file_find_support_file(const_app("string_game_gui_style_attack_icon"), TRUE),
	 file_find_support_file(const_app("string_game_gui_style_all_out_attack_icon"), TRUE)};
    gchar *image_boost_files[3] =
	{file_find_support_file(const_app("string_game_gui_boost_anti_icon"), TRUE),
	 file_find_support_file(const_app("string_game_gui_boost_off_icon"), TRUE),
	 file_find_support_file(const_app("string_game_gui_boost_on_icon"), TRUE)};

    gtk_image_set_from_file(style, image_style_files[tm->style + 2]);
    gtk_image_set_from_file(boost, image_boost_files[tm->boost + 1]);    

    for(i=0;i<5;i++)
	g_free(image_style_files[i]);
    for(i=0;i<3;i++)
	g_free(image_boost_files[i]);
}

/** Set the images for the style and boost meters in the main
    window and the live game window. */
void
game_gui_write_meters(const Team *tm)
{
#ifdef DEBUG
    printf("game_gui_write_meters\n");
#endif

    GtkImage *image_style_main = GTK_IMAGE(lookup_widget(window.main, "image_style")),
	*image_boost_main = GTK_IMAGE(lookup_widget(window.main, "image_boost"));
    GtkImage *image_style_live = NULL,
	*image_boost_live = NULL;

    game_gui_write_meter_images(tm, image_style_main, image_boost_main);

    if(window.live != NULL)
    {
	image_style_live = GTK_IMAGE(lookup_widget(window.live, "image_lg_style"));
	image_boost_live = GTK_IMAGE(lookup_widget(window.live, "image_lg_boost"));

	game_gui_write_meter_images(tm, image_style_live, image_boost_live);
    }
}

/** Activate the appropriate radio items for
    playing style etc. according to the user settings. */
void
game_gui_write_radio_items(void)
{
#ifdef DEBUG
    printf("game_gui_write_radio_items\n");
#endif

    GtkWidget *style[5], *scout[4], *physio[4], 
	*boost[3], *yc[4], *ya_pos_pref[5];

    game_gui_get_radio_items(style, scout, physio, boost, yc, ya_pos_pref);

    gtk_check_menu_item_set_active(
	GTK_CHECK_MENU_ITEM(style[current_user.tm->style + 2]), TRUE);
    gtk_check_menu_item_set_active(
	GTK_CHECK_MENU_ITEM(scout[current_user.scout % 10]), TRUE);
    gtk_check_menu_item_set_active(
	GTK_CHECK_MENU_ITEM(physio[current_user.physio % 10]), TRUE);
    gtk_check_menu_item_set_active(
	GTK_CHECK_MENU_ITEM(boost[current_user.tm->boost + 1]), TRUE);
    gtk_check_menu_item_set_active(
	GTK_CHECK_MENU_ITEM(yc[current_user.youth_academy.coach % 10]), TRUE);
    gtk_check_menu_item_set_active(
	GTK_CHECK_MENU_ITEM(ya_pos_pref[current_user.youth_academy.pos_pref]), TRUE);
}

/** Set playing style etc. variables according to
    the items.
    @param widget The widget that received a click. */
void
game_gui_read_radio_items(GtkWidget *widget)
{
#ifdef DEBUG
    printf("game_gui_read_radio_items\n");
#endif

    gint i;
    GtkWidget *boost[3], *yc[4], *ya_pos_pref[5];
    GtkWidget *style[5], *scout[4], *physio[4];
    gint old_scout = current_user.scout,
	old_physio = current_user.physio,
	old_yc = current_user.youth_academy.coach;

    game_gui_get_radio_items(style, scout, physio, boost, yc, ya_pos_pref);

    for(i=0;i<3;i++)
	if(widget == boost[i])
	{
	    current_user.tm->boost = i - 1;

	    if(current_user.tm->boost == 1 && sett_int("int_opt_disable_boost_on"))
	    {
		current_user.tm->boost = 0;
		game_gui_print_message(_("Boost ON is disabled in this country definition."));
	    }
	}

    for(i=0;i<5;i++)
	if(widget == style[i])
	    current_user.tm->style = i - 2;
    
    if(!sett_int("int_opt_disable_transfers"))
    {
	for(i=0;i<4;i++)
	    if(widget == scout[i])
		current_user.scout = 100 + i * 10 + old_scout % 10;
    }

    if(!sett_int("int_opt_disable_transfers"))
    {
	for(i=0;i<4;i++)
	    if(widget == physio[i])
		current_user.physio = 100 + i * 10 + old_physio % 10;
    }

    if(!sett_int("int_opt_disable_ya"))
    {
	for(i=0;i<4;i++)
	    if(widget == yc[i])
		current_user.youth_academy.coach = 100 + i * 10 + old_yc % 10;
    }
    
    if(!sett_int("int_opt_disable_ya"))
    {
	for(i=0;i<5;i++)
	    if(widget == ya_pos_pref[i])
		current_user.youth_academy.pos_pref = i;
    }

    if(math_get_place(current_user.scout, 2) == old_scout % 10)
	current_user.scout = old_scout % 10;

    if(math_get_place(current_user.physio, 2) == old_physio % 10)
	current_user.physio = old_physio % 10;

    if(math_get_place(current_user.youth_academy.coach, 2) == old_yc % 10)
	current_user.youth_academy.coach = old_yc % 10;

    if(old_scout != current_user.scout ||
       old_physio != current_user.physio ||
       old_yc != current_user.youth_academy.coach)
	game_gui_print_message(_("Next week you'll fire him and hire a new one."));

    game_gui_write_meters(current_user.tm);
    game_gui_write_radio_items();

    treeview_show_next_opponent();
}

/** Show the main menu. */
void
game_gui_show_main(void)
{
#ifdef DEBUG
    printf("game_gui_show_main\n");
#endif

    game_gui_set_main_window_header();
    treeview_show_user_player_list();

    current_user.counters[COUNT_USER_TOOK_TURN] = 1;

    if((opt_int("int_opt_news_popup") == 2 ||
        (opt_int("int_opt_news_popup") == 1 &&
         counters[COUNT_NEW_NEWS] == 1)) &&
       counters[COUNT_NEWS_SHOWN] == 0 &&
       counters[COUNT_NEW_NEWS] != 0)
        on_menu_news_activate(NULL, NULL);
    if(current_user.counters[COUNT_USER_SHOW_RES] && stat0 != STATUS_LIVE_GAME_PAUSE)
    {
	on_menu_user_show_last_stats_activate(NULL, NULL);
	current_user.counters[COUNT_USER_SHOW_RES] = 0;

	/** Check the success counter and offer a job 
	    (or fire player) if necessary. */
	user_job_offer(&current_user);
    }
    else
	treeview_show_next_opponent();

    if(current_user.counters[COUNT_USER_NEW_SPONSOR] == 1)
	user_show_sponsors();
    else if(current_user.counters[COUNT_USER_NEW_SPONSOR] == -1)
	user_show_sponsor_continue();
}

/** Print a message in the message area. */
void
game_gui_print_message(gchar *format, ...)
{
#ifdef DEBUG
    printf("game_gui_print_message\n");
#endif

    gchar text[SMALL];
    va_list args;
     
    va_start (args, format);
    g_vsprintf(text, format, args);
    va_end (args);

    gtk_entry_set_text(GTK_ENTRY(lookup_widget(window.main, "entry_message")), text);
    
    if(timeout_id != -1)
	g_source_remove(timeout_id);

    timeout_id = g_timeout_add(const_int("int_game_gui_message_duration") * 1000,
			       (GSourceFunc)game_gui_clear_entry_message, NULL);
}

/** Source function for the delay printing function. */
gboolean
game_gui_print_message_source(gpointer data)
{
#ifdef DEBUG
    printf("game_gui_print_message_source\n");
#endif

    game_gui_print_message((gchar*)data, NULL);

    g_free(data);

    return FALSE;
}

/** Print a message after some seconds of delay. */
void
game_gui_print_message_with_delay(const gchar *format, ...)
{
#ifdef DEBUG
    printf("game_gui_print_message_with_delay\n");
#endif

    gchar text[SMALL];
    va_list args;
     
    va_start (args, format);
    g_vsprintf(text, format, args);
    va_end (args);

    if(timeout_id != -1)
	g_source_remove(timeout_id);

    g_timeout_add(const_int("int_game_gui_message_delay") * 1000,
		  (GSourceFunc)game_gui_print_message_source,
		  (gpointer)g_strdup(text));
}

/** Function that gets called from time to time. */
gboolean
game_gui_clear_entry_message(gpointer data)
{
#ifdef DEBUG
    printf("game_gui_clear_entry_message\n");
#endif

    if(window.main != NULL)
	gtk_entry_set_text(GTK_ENTRY(lookup_widget(window.main, "entry_message")), "");

    return FALSE;
}

/** Which parts of the main window get affected
    by a live game pause. */
enum MainWindowInensitiveItems
{
    INSENSITIVE_ITEM_TOOLBAR = 0,
    INSENSITIVE_ITEM_MENU_FILE,
    INSENSITIVE_ITEM_MENU_OPTIONS,
    INSENSITIVE_ITEM_MENU_FIGURES,
    INSENSITIVE_ITEM_MENU_SCOUT,
    INSENSITIVE_ITEM_MENU_PHYSIO,
    INSENSITIVE_ITEM_MENU_BROWSE_TEAMS,
    INSENSITIVE_ITEM_MENU_YOUTH_ACADEMY,
    INSENSITIVE_ITEM_MENU_TRAINING_CAMP,
    INSENSITIVE_ITEM_MENU_SHOW_JOB_EXCHANGE,
    INSENSITIVE_ITEM_MENU_PUT_ON_TRANSFER_LIST,
    INSENSITIVE_ITEM_MENU_REMOVE_FROM_TRANSFER_LIST,
    INSENSITIVE_ITEM_MENU_FIRE,
    INSENSITIVE_ITEM_MENU_MOVE_TO_YOUTH_ACADEMY,
    INSENSITIVE_ITEM_MENU_USER,
    INSENSITIVE_ITEM_MENU_FINANCES_STADIUM,
    INSENSITIVE_ITEM_MENU_HELP,
    INSENSITIVE_ITEM_MENU_BROWSE_PLAYERS,
    INSENSITIVE_ITEM_MENU_OFFER_CONTRACT,
    INSENSITIVE_ITEM_END
};

enum MainWindowShowItems
{
    SHOW_ITEM_RESET_BUTTON = 0,
    SHOW_ITEM_RESET_MENU,
    SHOW_ITEM_END
};

/** Set appropriate parts of the main window insensitive when
    the live game is paused or resumed.
    @param value Whether we set sensitive or insensitive. */
void
game_gui_set_main_window_sensitivity(gboolean value)
{
#ifdef DEBUG
    printf("game_gui_set_main_window_sensitivity\n");
#endif

    gint i;
    GtkWidget *insensitive_items[INSENSITIVE_ITEM_END];
    GtkWidget *show_items[SHOW_ITEM_END];
    
    insensitive_items[INSENSITIVE_ITEM_TOOLBAR] = 
	lookup_widget(window.main, "hbox1");
    insensitive_items[INSENSITIVE_ITEM_MENU_FILE] = 
	lookup_widget(window.main, "menu_file");
    insensitive_items[INSENSITIVE_ITEM_MENU_OPTIONS] = 
	lookup_widget(window.main, "menu_options");
    insensitive_items[INSENSITIVE_ITEM_MENU_FIGURES] = 
	lookup_widget(window.main, "menu_figures");
    insensitive_items[INSENSITIVE_ITEM_MENU_SCOUT] = 
	lookup_widget(window.main, "menu_scout");
    insensitive_items[INSENSITIVE_ITEM_MENU_PHYSIO] = 
	lookup_widget(window.main, "menu_physio");
    insensitive_items[INSENSITIVE_ITEM_MENU_BROWSE_TEAMS] = 
	lookup_widget(window.main, "menu_browse_teams");
    insensitive_items[INSENSITIVE_ITEM_MENU_YOUTH_ACADEMY] = 
	lookup_widget(window.main, "menu_youth_academy");
	insensitive_items[INSENSITIVE_ITEM_MENU_TRAINING_CAMP] = 
	lookup_widget(window.main, "menu_training_camp");
    insensitive_items[INSENSITIVE_ITEM_MENU_SHOW_JOB_EXCHANGE] = 
	lookup_widget(window.main, "menu_show_job_exchange");
    insensitive_items[INSENSITIVE_ITEM_MENU_PUT_ON_TRANSFER_LIST] = 
	lookup_widget(window.main, "menu_put_on_transfer_list");
    insensitive_items[INSENSITIVE_ITEM_MENU_REMOVE_FROM_TRANSFER_LIST] = 
	lookup_widget(window.main, "menu_remove_from_transfer_list");
    insensitive_items[INSENSITIVE_ITEM_MENU_FIRE] = 
	lookup_widget(window.main, "menu_fire");
    insensitive_items[INSENSITIVE_ITEM_MENU_MOVE_TO_YOUTH_ACADEMY] = 
	lookup_widget(window.main, "menu_move_to_youth_academy");
    insensitive_items[INSENSITIVE_ITEM_MENU_USER] = 
	lookup_widget(window.main, "menu_user");
    insensitive_items[INSENSITIVE_ITEM_MENU_FINANCES_STADIUM] = 
	lookup_widget(window.main, "menu_finances_stadium");
    insensitive_items[INSENSITIVE_ITEM_MENU_HELP] = 
	lookup_widget(window.main, "menu_help");
    insensitive_items[INSENSITIVE_ITEM_MENU_BROWSE_PLAYERS] = 
	lookup_widget(window.main, "menu_browse_players");
    insensitive_items[INSENSITIVE_ITEM_MENU_OFFER_CONTRACT] = 
	lookup_widget(window.main, "menu_offer_new_contract");

    show_items[SHOW_ITEM_RESET_BUTTON] =
	lookup_widget(window.main, "button_reset_players");
    show_items[SHOW_ITEM_RESET_MENU] =
	lookup_widget(window.main, "menu_reset_players");

    for(i=0;i<INSENSITIVE_ITEM_END;i++)
	gtk_widget_set_sensitive(insensitive_items[i], !value);

    if(value)
	for(i=0;i<SHOW_ITEM_END;i++)
	    gtk_widget_show(show_items[i]);
    else
	for(i=0;i<SHOW_ITEM_END;i++)
	    gtk_widget_hide(show_items[i]);

    gtk_window_set_modal(GTK_WINDOW(window.live), !value);
}

/** Show a window with a warning.
    @param text The text to show in the window. */
void
game_gui_show_warning(const gchar *format, ...)
{
#ifdef DEBUG
    printf("game_gui_show_warning\n");
#endif

    gchar text[SMALL];
    va_list args;
     
    va_start (args, format);
    g_vsprintf(text, format, args);
    va_end (args);

    if(opt_int("int_opt_prefer_messages") &&
       window.main != NULL)
	game_gui_print_message(text, NULL);
    else
    {
	window_create(WINDOW_WARNING);
	gtk_label_set_text(GTK_LABEL(lookup_widget(window.warning, "label_warning")), text);
    }
}

/** Show the job offer window.
    @param team The team offering the job or NULL if we're looking
    at a job offer from the job exchange.
    @param job The job pointer or NULL (depends on whether we're looking
    at a job offer from the job exchange).
    @param type The offer type (eg. whether the user's been fired). */
void
game_gui_show_job_offer(Team *team, Job *job, gint type)
{
#ifdef DEBUG
    printf("game_gui_show_job_offer\n");
#endif

    gchar buf[SMALL], buf2[SMALL];    
    GtkLabel *label_text, *label_text2, *label_name, 
	*label_league, *label_rank, *label_money,
	*label_cap, *label_saf, *label_average_skill;
    Team *tm = (type != STATUS_JOB_EXCHANGE_SHOW_TEAM) ?
	team : job_get_team(job);

    statp = (type != STATUS_JOB_EXCHANGE_SHOW_TEAM) ?
	(gpointer)team : (gpointer)job;
    stat2 = type;

    if(type == STATUS_JOB_OFFER_FIRE_FINANCE ||
       type == STATUS_JOB_OFFER_FIRE_FAILURE)
	user_history_add(&current_user, (type == STATUS_JOB_OFFER_FIRE_FINANCE) ?
			 USER_HISTORY_FIRE_FINANCE : USER_HISTORY_FIRE_FAILURE,
			 current_user.tm->name, team->name, 
			 league_cup_get_name_string(team->clid), NULL);

    window_create(WINDOW_JOB_OFFER);

    label_text = GTK_LABEL(lookup_widget(window.job_offer, "label_text"));
    label_text2 = GTK_LABEL(lookup_widget(window.job_offer, "label_text2"));
    label_name = GTK_LABEL(lookup_widget(window.job_offer, "label_name"));
    label_league = GTK_LABEL(lookup_widget(window.job_offer, "label_league"));
    label_rank = GTK_LABEL(lookup_widget(window.job_offer, "label_rank"));
    label_money = GTK_LABEL(lookup_widget(window.job_offer, "label_money"));
    label_cap = GTK_LABEL(lookup_widget(window.job_offer, "label_cap"));
    label_saf = GTK_LABEL(lookup_widget(window.job_offer, "label_saf"));
    label_average_skill = 
	GTK_LABEL(lookup_widget(window.job_offer, "label_average_skill"));

    if(type == STATUS_JOB_OFFER_FIRE_FINANCE)
	sprintf(buf, _("The team owners have fired you because of financial mismanagement. Luckily, the owners of %s have heard of your dismissal and offer you a job. Here's some information on %s:"),
		tm->name, tm->name);
    else if(type == STATUS_JOB_OFFER_FIRE_FAILURE)
	sprintf(buf, _("The team owners have fired you because of unsuccessfulness. Luckily, the owners of %s have heard of your dismissal and offer you a job. Here's some information on %s:"),
		tm->name, tm->name);
    else if(type == STATUS_JOB_OFFER_SUCCESS)
	sprintf(buf, _("The owners of %s are deeply impressed by your success with %s and would like to hire you. Here's some information on %s:"),
		tm->name, current_user.tm->name, tm->name);
    else if(type == STATUS_JOB_EXCHANGE_SHOW_TEAM)
	strcpy(buf, _("Click on OK to apply for the job. Click on CANCEL to close the window."));
    
    strcpy(buf2, (type != STATUS_JOB_EXCHANGE_SHOW_TEAM) ?
	   _("Accept?") : _("Apply for the job?"));

    if(type != STATUS_JOB_OFFER_SUCCESS &&
       type != STATUS_JOB_EXCHANGE_SHOW_TEAM)
	strcat(buf2, _(" (NOTE: If you don't, the game is over for you.)"));

    gtk_label_set_text(label_text, buf);
    gtk_label_set_text(label_text2, buf2);
    gtk_label_set_text(label_name, tm->name);
    gtk_label_set_text(label_league, 
		       (type != STATUS_JOB_EXCHANGE_SHOW_TEAM) ?
		       league_cup_get_name_string(tm->clid) : job->league_name);

    if(job == NULL ||
       job->type == JOB_TYPE_NATIONAL)
	gui_label_set_text_from_int(label_rank, 
				    team_get_league_rank(tm, -1), FALSE);

    misc_print_grouped_int(
	math_round_integer(tm->stadium.capacity * 
			   math_rndi(const_int("int_initial_money_lower"),
				     const_int("int_initial_money_upper")), 2),
	buf);

    gtk_label_set_text(label_money, buf);
    misc_print_grouped_int(tm->stadium.capacity, buf);
    gtk_label_set_text(label_cap, buf);
    gui_label_set_text_from_int(label_saf,
				(gint)rint(tm->stadium.safety * 100), FALSE);

    sprintf(buf, "%.1f", team_get_average_skill(tm, FALSE));
    gtk_label_set_text(label_average_skill, buf);

    treeview_show_player_list_team(
	GTK_TREE_VIEW(lookup_widget(window.job_offer, "treeview_players")),
	tm, 
	(type != STATUS_JOB_OFFER_SUCCESS && type != STATUS_JOB_EXCHANGE_SHOW_TEAM) ? 
	2 : current_user.scout);
}

/** Write the checkbuttons in the menus. */
void
game_gui_write_check_items(void)
{
#ifdef DEBUG
    printf("game_gui_write_check_items\n");
#endif

    GtkCheckMenuItem *menu_job_offers = 
	GTK_CHECK_MENU_ITEM(lookup_widget(window.main, "menu_job_offers")),
	*menu_live_game = 
	GTK_CHECK_MENU_ITEM(lookup_widget(window.main, "menu_live_game")),
	*menu_overwrite = 
	GTK_CHECK_MENU_ITEM(lookup_widget(window.main, "menu_overwrite"));

    gtk_check_menu_item_set_active(menu_job_offers,
				   opt_user_int("int_opt_user_show_job_offers"));
    gtk_check_menu_item_set_active(menu_live_game,
				   opt_user_int("int_opt_user_show_live_game"));
    gtk_check_menu_item_set_active(menu_overwrite,
				   opt_int("int_opt_save_will_overwrite"));
}

/** Change the options according to the check menu widgets. */
void
game_gui_read_check_items(GtkWidget *widget)
{
#ifdef DEBUG
    printf("game_gui_read_check_items\n");
#endif

    GtkWidget *menu_job_offers = 
	lookup_widget(window.main, "menu_job_offers"),
	*menu_live_game = lookup_widget(window.main, "menu_live_game"),
	*menu_overwrite = lookup_widget(window.main, "menu_overwrite");

    if(widget == menu_job_offers)
    {
	opt_user_set_int("int_opt_user_show_job_offers", 
			 !opt_user_int("int_opt_user_show_job_offers"));
	game_gui_print_message(_("Job offers set to %s."),
			       team_attribute_to_char(
				   TEAM_ATTRIBUTE_BOOST, 
				   opt_user_int("int_opt_user_show_job_offers")));
    }
    else if(widget == menu_live_game)
    {
	opt_user_set_int("int_opt_user_show_live_game", 
			 !opt_user_int("int_opt_user_show_live_game"));
	game_gui_print_message(_("Live game set to %s."),
			       team_attribute_to_char(
				   TEAM_ATTRIBUTE_BOOST, 
				   opt_user_int("int_opt_user_show_live_game")));
    }
    else if(widget == menu_overwrite)
    {
	opt_set_int("int_opt_save_will_overwrite", 
		    !opt_int("int_opt_save_will_overwrite"));
	game_gui_print_message(_("Overwrite set to %s."),
			       team_attribute_to_char(
				   TEAM_ATTRIBUTE_BOOST, 
				   opt_int("int_opt_save_will_overwrite")));
    }
    else
	debug_print_message("game_gui_read_check_items: unknown widget.");
}

/** Set the appropriate text into the labels in the help window. 
    @param help_list The stuff loaded from the bygfoot_help file. */
void
game_gui_set_help_labels(void)
{
#ifdef DEBUG
    printf("game_gui_set_help_labels\n");
#endif

    GtkLabel *label_help_text1 = 
	GTK_LABEL(lookup_widget(window.help, "label_help_text1")),
	*label_help_text2 = GTK_LABEL(lookup_widget(window.help, "label_help_text2"));
    GString *text = g_string_new("");

    gtk_label_set_text(label_help_text1, 
		       _("Bygfoot is a very intuitive and simple game, so there isn't a full-grown documentation. However, if you have trouble, there are a few places to go.\n"));

    g_string_append_printf(text, _("At the Bygfoot forums you can report bugs, ask for help and discuss the game:\n"));
    g_string_append_printf(text, "http://bygfoot.sourceforge.net/forum\n");
    g_string_append_printf(text, _("\nYou can also write an email to the authors:\n"));
    g_string_append_printf(text, "gunnar@bygfoot.com, gyboth@bygfoot.com");

    gtk_label_set_text(label_help_text2, text->str);
    g_string_free(text, TRUE);
}

/** Set the money of the current team into the label. */
void
game_gui_write_money(void)
{
#ifdef DEBUG
    printf("game_gui_write_money\n");
#endif

    gchar buf[SMALL];
    GtkLabel *label_money= GTK_LABEL(lookup_widget(window.main, "label_money"));
    
    misc_print_grouped_int(current_user.money, buf);
	gtk_label_set_text(label_money, buf);
}
