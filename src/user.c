#include "league.h"
#include "maths.h"
#include "player.h"
#include "support.h"
#include "team.h"
#include "user.h"

/** These determine how the initial money of the user
    depends on the size of the stadium. */
#define CONSTANT_INITIAL_MONEY_LOWER 80
#define CONSTANT_INITIAL_MONEY_UPPER 110

/** Move the user's team to top or bottom league
    at the beginning of a new game and set up the user's team. */
void
user_set_up_my_team_new_game(GtkWidget *widget)
{
    GtkWidget *radiobutton1 =
	lookup_widget(widget, "team_selection_radio1");
    GtkWidget *radiobutton2 =
	lookup_widget(widget, "team_selection_radio2");
    gchar buf[SMALL];
    gint rndom, max, lig_idx = -1;

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton2)))
	user_set_up_my_team();
    else
    {
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton1)) &&
	   lig(0).id != my_team->clid)
	    lig_idx = 0;
	else if(lig(ligs->len - 1).id != my_team->clid)
	    lig_idx = ligs->len - 1;

	max = lig(lig_idx).teams->len - 1;
	rndom = math_rndi(0, max);
	sprintf(buf, "%s", g_array_index(lig(lig_idx).teams, Team, rndom).name->str);
	g_string_printf(g_array_index(lig(lig_idx).teams, Team, rndom).name, "%s",
			my_team->name->str);
	g_string_printf(my_team->name, "%s", buf);

	my_team = &g_array_index(lig(lig_idx).teams, Team, rndom);

	user_set_up_my_team();
    }
}

/** Set up finances, reomve some players etc. for a new user team. */
void
user_set_up_my_team(void)
{
    gint i, j;

    for(i=PLAYER_POS_DEFENDER; i<=PLAYER_POS_FORWARD; i++)
	for(j=my_team->players->len - 1; j > 10; j--)
	    if(g_array_index(my_team->players, Player, j).pos == i)
	    {
		player_remove_from_team(my_team, j);
		break;
	    }

    scout = physio = QUALITY_AVERAGE;
    
    user_set_up_finances();
}

/** Set up the user's finances when he's got a new team.*/
void
user_set_up_finances(void)
{
    gint i;

    for(i=0; i<FIN_END;i++)
	finances[i] = 0;

    finances[FIN_MONEY] = 
	math_round_integer(my_team->stadium.capacity * 
			   math_rndi(CONSTANT_INITIAL_MONEY_LOWER,
				     CONSTANT_INITIAL_MONEY_UPPER), 2);
}
