#include "finance.h"
#include "game_gui.h"
#include "maths.h"
#include "misc2_callback_func.h"
#include "option.h"
#include "player.h"
#include "team.h"
#include "treeview.h"
#include "transfer.h"
#include "user.h"

/** Transfer a user player. */
void
misc2_callback_transfer_user_player(void)
{
    Team *new_team = transoff(stat1, 0).tm;
    gchar buf[SMALL];

    if(team_is_user(new_team) != -1)
    {
	if(new_team->players->len < const_int("int_team_max_players") &&
	   BUDGET(user_get_index(user_from_team(new_team))) > transoff(stat1, 0).fee)
	{
	    current_user.money += transoff(stat1, 0).fee;
	    current_user.money_in[1][MON_IN_TRANSFERS] += transoff(stat1, 0).fee;
	    user_from_team(new_team)->money -= transoff(stat1, 0).fee;
	    user_from_team(new_team)->money_out[1][MON_OUT_TRANSFERS] -= 
		transoff(stat1, 0).fee;
	    player_copy(player_of_id(trans(stat1).tm, trans(stat1).id),
			new_team, new_team->players->len);
	    player_of(new_team, new_team->players->len - 1)->contract = 
		(gfloat)math_rndi(const_int("int_transfer_contract_lower"),
				  const_int("int_transfer_contract_upper"));
	    g_array_remove_index(current_user.tm->players, 
				 player_id_index(current_user.tm, trans(stat1).id));
	    transfer_remove_player(stat1);
	}
	else
	{
	    sprintf(buf, _("%s couldn't afford to buy %s or his roster was full."),
		    user_from_team(new_team)->name->str, 
		    player_of_id(trans(stat1).tm, trans(stat1).id)->name->str);
	    game_gui_show_warning(buf);
	    sprintf(buf, _("You didn't have enough money to buy %s or your roster was full."),
		    player_of_id(trans(stat1).tm, trans(stat1).id)->name->str);
	    user_event_add(user_from_team(new_team), EVENT_TYPE_WARNING, -1, -1, NULL, buf);
	    g_array_remove_index(trans(stat1).offers, 0);
	}
    }
    else
    {		
	current_user.money += transoff(stat1, 0).fee;
	current_user.money_in[1][MON_IN_TRANSFERS] += transoff(stat1, 0).fee;
	player_remove_from_team(current_user.tm, 
				player_id_index(current_user.tm, trans(stat1).id));
	transfer_remove_player(stat1);
    }

    treeview_show_user_player_list(&current_user);
    game_gui_set_main_window_header();
}
