#include "fixture.h"
#include "free.h"
#include "league.h"
#include "maths.h"
#include "option.h"
#include "player.h"
#include "support.h"
#include "team.h"
#include "user.h"

/** Create a new user with default values. */
User
user_new(void)
{
    gint i;
    User new;

    new.name = g_string_new("NONAME");
    new.font_name = g_string_new("0");
    new.tm = NULL;

    new.live_game.units = NULL;
    new.live_game.fix = NULL;

    for(i=0;i<LIVE_GAME_STAT_ARRAY_END;i++)
	new.live_game.stats.players[i][0] = 
	    new.live_game.stats.players[i][1] = NULL;

    new.options = g_array_new(FALSE, FALSE, sizeof(Option));

    return new;
}

/** Move a user's team to top or bottom league
    at the beginning of a new game and set up the team.
    @param user The user we set up the team for. */
void
user_set_up_team_new_game(User *user)
{
    gchar buf[SMALL];
    gint rndom, max, lig_idx = -1;

    if(user->scout == 0)
	user_set_up_team(user);
    else
    {
	if(user->scout == 1 &&
	   lig(0).id != user->tm->clid)
	    lig_idx = 0;
	else if(lig(ligs->len - 1).id != user->tm->clid)
	    lig_idx = ligs->len - 1;

	max = lig(lig_idx).teams->len - 1;
	rndom = math_rndi(0, max);
	sprintf(buf, "%s", g_array_index(lig(lig_idx).teams, Team, rndom).name->str);
	g_string_printf(g_array_index(lig(lig_idx).teams, Team, rndom).name, "%s",
			user->tm->name->str);
	g_string_printf(user->tm->name, "%s", buf);

	user->tm = &g_array_index(lig(lig_idx).teams, Team, rndom);

	user_set_up_team(user);
    }
}

/** Set up finances, remove some players etc. for a new user team.
    @param user The user whose team we set up. */
void
user_set_up_team(User *user)
{
    gint i, j;

    for(i=PLAYER_POS_DEFENDER; i<=PLAYER_POS_FORWARD; i++)
	for(j=user->tm->players->len - 1; j > 10; j--)
	    if(g_array_index(user->tm->players, Player, j).pos == i)
	    {
		player_remove_from_team(user->tm, j);
		break;
	    }

    user->scout = user->physio = QUALITY_AVERAGE;
    
    user_set_up_finances(user);
}

/** Set up the user's finances when he's got a new team.
    @param user The user we set up the finances for. */
void
user_set_up_finances(User *user)
{
    gint i;

    for(i=0; i<FIN_END;i++)
	user->finances[i] = 0;

    user->finances[FIN_MONEY] = 
	math_round_integer(user->tm->stadium.capacity * 
			   math_rndi(const_int("int_initial_money_lower"),
				     const_int("int_initial_money_upper")), 2);
}

/** Remove a user from the game.
    @param idx The index of the user in the #users array.
    @param regenerate_team Whether the user's team has to be
    regenerated. */
void
user_remove(gint idx, gboolean regenerate_team)
{
    free_user(&usr(idx));
    g_array_remove_index(users, idx);
}

void
user_set_player_list_attributes(const User *user, PlayerListAttribute *attribute, gint list_number)
{
    gint i, cnt = 0;
    gchar prefix[SMALL];

    sprintf(prefix, "int_opt_user_pl%d_att", list_number);

    for(i=0;i<user->options->len;i++)
	if(g_str_has_prefix(g_array_index(user->options, Option, i).name->str, prefix))
	{
	    attribute->on_off[cnt] = g_array_index(user->options, Option, i).value;
	    cnt++;
	}
}

/** Find out whether there are user games at the specified date. */
gboolean
user_games_in_week_round(gint week_number, gint week_round_number)
{
    gint i, j;

    if(week_round_number == 1)
    {
	for(i=0;i<ligs->len;i++)
	    for(j=0;j<lig(i).fixtures->len;j++)
		if(fixture_user_team_involved(&g_array_index(lig(i).fixtures, Fixture, j)) != -1 &&
		   g_array_index(lig(i).fixtures, Fixture, j).week_number == week_number &&
		   g_array_index(lig(i).fixtures, Fixture, j).week_round_number == week_round_number)
		    return TRUE;
    }
    else
    {
	for(i=0;i<cps->len;i++)
	    for(j=0;j<cp(i).fixtures->len;j++)
		if(fixture_user_team_involved(&g_array_index(cp(i).fixtures, Fixture, j)) != -1 &&
		   g_array_index(lig(i).fixtures, Fixture, j).week_number == week_number &&
		   g_array_index(lig(i).fixtures, Fixture, j).week_round_number == week_round_number)
		    return TRUE;
    }

    return FALSE;
}
