/*
   debug.c

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
#include "game_gui.h"
#include "league.h"
#include "live_game.h"
#include "option.h"
#include "strategy.h"
#include "support.h"
#include "team.h"
#include "user.h"
#include "variables.h"

/** Take some debug action depending on the text. Text is a prefix and a number. */
void
debug_action(const gchar *text)
{
#ifdef DEBUG
    printf("debug_action\n");
#endif

    gchar buf[SMALL];
    gint value = -1;
    gint i, j;

    printf("debact: %s\n", text);

    sscanf(text, "%[^-0-9]%d", buf, &value);

    if(g_str_has_prefix(text, "deb"))
    {
	option_set_int("int_debug", &constants, value);
	game_gui_print_message("Debug value set to %d.", value);
    }
    else if(g_str_has_prefix(text, "writer"))
    {
    	option_set_int("int_debug_writer", &constants, value);
    	game_gui_print_message("Debug writer value set to %d.", value);
    } 
    else if(g_str_has_prefix(text, "cap"))
    {
	current_user.tm->stadium.capacity += value;
	game_gui_print_message("Stadium capacity changed by %d. New: %d.", value,
		current_user.tm->stadium.capacity);
    }
    else if(g_str_has_prefix(text, "saf"))
    {
	current_user.tm->stadium.safety += ((gfloat)value / 100);
	current_user.tm->stadium.safety = 
	    CLAMP(current_user.tm->stadium.safety, 0, 1);
	game_gui_print_message("Stadium safety changed by %d. New: %.2f", value,
		current_user.tm->stadium.safety);
    }
    else if(g_str_has_prefix(text, "mon"))
    {
	current_user.money += value;
	game_gui_print_message("Money changed by %d. New: %d.", value,
		current_user.money);
    }
    else if(g_str_has_prefix(text, "suc"))
    {
	current_user.counters[COUNT_USER_SUCCESS] += value;
	game_gui_print_message("Success counter changed by %d. New: %d.", value,
		current_user.counters[COUNT_USER_SUCCESS]);
    }
    else if(g_str_has_prefix(text, "scout"))
    {
	current_user.scout = value;
	game_gui_print_message("Scout changed to %d.", value);
    }
    else if(g_str_has_prefix(text, "phys"))
    {
	current_user.physio = value;
	game_gui_print_message("Physio changed to %d.", value);
    }
    else if(g_str_has_prefix(text, "yc"))
    {
	current_user.youth_academy.coach = value;
	game_gui_print_message("Youth coach changed to %d.", value);
    }
    else if(g_str_has_prefix(text, "pospref"))
    {
	current_user.youth_academy.pos_pref = value;
	game_gui_print_message("Recruiting pref changed to %d.", value);
    }
    else if(g_str_has_prefix(text, "goto"))
    {
        if(debug < 50)
            option_set_int("int_debug", &constants, 50);

        if(option_int("int_opt_user_show_live_game", &current_user.options))
            option_set_int("int_opt_user_show_live_game", &current_user.options, 0);

        sett_set_int("int_opt_goto_mode", 1);
        if(value < 100)
            while(week < value)
            {
                on_button_new_week_clicked(NULL, NULL);
                game_gui_set_main_window_header();
                while (gtk_events_pending ())
                    gtk_main_iteration ();
            }        
        else
            while(season < value - 100)
            {
                on_button_new_week_clicked(NULL, NULL);
                game_gui_set_main_window_header();
                while (gtk_events_pending ())
                    gtk_main_iteration ();
            }
        sett_set_int("int_opt_goto_mode", 0);
    }
    else if(g_str_has_prefix(text, "testcom") ||
	    g_str_has_prefix(text, "tc"))
    {
	stat5 = -value - 1000;
	game_gui_print_message("Commentary type displayed: %d.", value);
    }
    else if(g_str_has_prefix(text, "printweeks"))
    {
        for(i = 0; i < cps->len; i++)
        {
            if(cp(i).add_week != 1000)
            {
                g_print("Cup: %s\n", cp(i).name);
                for(j = 0; j < cp(i).rounds->len; j++)
                    g_print("  Round %2d: Week %2d (w/o delay: %2d)\n",
                            j, cup_get_first_week_of_cup_round(&cp(i), j, TRUE),
                            cup_get_first_week_of_cup_round(&cp(i), j, FALSE));                
            }
        }
    }
    else if(g_str_has_prefix(text, "help"))
    {
	g_print("Debug options:\n"
	       "deb \t set debug value\n"
	       "writer \t set debug-writer value\n"
	       "cap \t change stadium capacity\n"
	       "saf \t change stadium safety\n"
	       "mon \t change money\n"
	       "suc \t change success counter\n"
	       "scout \t change scout\n"
	       "physio \t change physio\n"
	       "printweeks \t print the starting weeks of all cup rounds\n"
	       "youth coach \t change youth coach\n"
	       "pospref \t change recruiting pref\n"
	       "goto \t Press 'new week' automatically until\n"
	       "     \t the appropriate week is reached\n"
	       "     \t Supply 100+X to go to season X (e.g. 102)\n"
	       "testcom|tc \t Test a specific live game commentary.\n"
	       "           \t Find the numbers in live_game_struct.h (LiveGameEventType)\n"
	       "           \t Use 'goto' afterwards.\n"
	       "help \t display this help\n");
    }

    setsav0;
}

gboolean
debug_reset_counter(gpointer data)
{
#ifdef DEBUG
    printf("debug_reset_counter\n");
#endif

    counters[COUNT_SHOW_DEBUG] = 0;

    return FALSE;
}

void
debug_calibrate_betting_odds(gint skilldiffmax, gint matches_per_skilldiff)
{
#ifdef DEBUG
    printf("debug_calibrate_betting_odds\n");
#endif

    gint i, skilldiff, matches;
    Fixture *fix = &g_array_index(lig(0).fixtures, Fixture, 0);
    LiveGame live_game;

    fix->home_advantage = FALSE;
    
    for(skilldiff=0;skilldiff<=skilldiffmax;skilldiff++)
    {
	gint res[3] = {0, 0, 0};

	for(matches=0;matches<matches_per_skilldiff;matches++)
	{
	    fix->attendance = -1;
	    fix->result[0][0] = fix->result[1][0] = 0;
	    for(i=0;i<fix->teams[0]->players->len;i++)
	    {
		strategy_repair_player(&g_array_index(fix->teams[0]->players, Player, i));
		strategy_repair_player(&g_array_index(fix->teams[1]->players, Player, i));
		g_array_index(fix->teams[0]->players, Player, i).skill = 90;
		g_array_index(fix->teams[1]->players, Player, i).skill = 90 - skilldiff;
		g_array_index(fix->teams[0]->players, Player, i).fitness = 0.9;
		g_array_index(fix->teams[1]->players, Player, i).fitness = 0.9;
	    }

	    live_game_calculate_fixture(fix, &live_game);
	    if(fix->result[0][0] < fix->result[1][0])
		res[2]++;
	    else
		res[(fix->result[0][0] == fix->result[1][0])]++;
	}

	g_print("sd %3d res %3d %3d %3d prob %.2f %.2f %.2f\n", skilldiff,
		res[0], res[1], res[2], (gfloat)res[0] / (gfloat)matches, 
		(gfloat)res[1] / (gfloat)matches, (gfloat)res[2] / (gfloat)matches);
    }
}

/** Check whether the 4 forwards, boost on, style all-out-attack
    easter egg should be activated. */
gboolean
debug_egg_forwards_boost_style(void)
{
#ifdef DEBUG
    printf("debug_egg_forwards_boost_style\n");
#endif

    gint i, fwds = 0;
    
    if(current_user.tm->boost != 1 ||
       current_user.tm->style != 2 ||
       current_user.tm->players->len < 11)
	return FALSE;

    for(i=0;i<11;i++)
	if(g_array_index(current_user.tm->players, Player, i).cpos == 3 &&
	   g_array_index(current_user.tm->players, Player, i).cskill > 0)
	    fwds++;

    return (fwds > 3);
}

/** 
 * debug_writer writes debug-messages to an outpot
 * 
 * *file_name = the name of the file where the debug-funtion is called
 * *method_name = the name of the function where the debug-funtion is called
 * *text = the text of the debug message
 * 
 * debuglevel < int_debug -> print message to output
 */
void
debug_writer_out(const gchar *file_name,
				 const gchar *method_name,
			 	 const gchar *text,
			 	 gint debuglevel)
{
#ifdef DEBUG
    printf("debug_writer_out\n");
#endif

	
	gint writer = option_int("int_debug_writer", &constants);
	gint debugging = option_int("int_debug", &constants);

	if ((debuglevel < debugging) && (writer == 1))
	{
		printf("%s # %s # %s\n", file_name, method_name, text);
	}
	
	//Example
	//gchar message[SMALL];
    //sprintf(message, "Number of players in player list: %d", current_user.tm->players->len);
    //debug_writer_out("misc2_callbacks2.c",
	//		 "on_button_transfer_yes_clicked",
	//		 	 message,
	//		 	 3);
	
}
