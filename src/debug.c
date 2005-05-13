#include "debug.h"
#include "game_gui.h"
#include "option.h"
#include "user.h"
#include "variables.h"

/** Take some debug action depending on the text and the value. */
void
debug_action(const gchar *text, gint value)
{
    gchar message[SMALL];

    if(g_str_has_prefix(text, "deb"))
    {
	opt_set_int("int_opt_debug", value);
	sprintf(message, "Debug value set to %d.", value);
    }
    else if(g_str_has_prefix(text, "cap"))
    {
	current_user.tm->stadium.capacity += value;
	sprintf(message, "Stadium capacity changed by %d.", value);
    }
    else if(g_str_has_prefix(text, "saf"))
    {
	current_user.tm->stadium.safety += ((gfloat)value / 100);
	current_user.tm->stadium.safety = 
	    CLAMP(current_user.tm->stadium.safety, 0, 1);
	sprintf(message, "Stadium safety changed by %d.", value);
    }
    else if(g_str_has_prefix(text, "mon"))
    {
	current_user.money += value;
	sprintf(message, "Money changed by %d.", value);
    }

    game_gui_print_message(message);
}

gboolean
debug_reset_counter(gpointer data)
{
    counters[COUNT_SHOW_DEBUG] = 0;

    return FALSE;
}
