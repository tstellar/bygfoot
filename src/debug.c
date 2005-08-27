#include "callbacks.h"
#include "debug.h"
#include "game_gui.h"
#include "option.h"
#include "support.h"
#include "user.h"
#include "variables.h"

/** Take some debug action depending on the text and the value. */
void
debug_action(const gchar *text, gint value)
{
    if(g_str_has_prefix(text, "deb"))
    {
	opt_set_int("int_opt_debug", value);
	game_gui_print_message("Debug value set to %d.", value);
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
	while(week < value)
	    on_button_new_week_clicked(NULL, NULL);
    }
    else if(g_str_has_prefix(text, "testcom") ||
	    g_str_has_prefix(text, "tc"))
    {
	stat5 = -value;
	game_gui_print_message("Commentary type displayed: %d.", value);
    }
    else if(g_str_has_prefix(text, "help"))
    {
	printf("Debug options:\n"
	       "deb \t set debug value\n"
	       "cap \t change stadium capacity\n"
	       "saf \t change stadium safety\n"
	       "mon \t change money\n"
	       "suc \t change success counter\n"
	       "scout \t change scout\n"
	       "physio \t change physio\n"
	       "youth coach \t change youth coach\n"
	       "pospref \t change recruiting pref\n"
	       "goto \t Press 'new week' automatically until\n"
	       "     \t the appropriate week is reached\n"
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
    counters[COUNT_SHOW_DEBUG] = 0;

    return FALSE;
}
