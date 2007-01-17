/*
   bet.c

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

#include "bet.h"
#include "finance.h"
#include "fixture.h"
#include "game_gui.h"
#include "league.h"
#include "main.h"
#include "maths.h"
#include "misc.h"
#include "option.h"
#include "player.h"
#include "support.h"
#include "treeview2.h"
#include "user.h"

/** Round a betting odd to a multiple of 0.05. */
gfloat
bet_round_odd(gfloat odd)
{
    gint local_odd = (gint)rint(odd * 100);

    if(local_odd % 5 == 0)
	return odd;

    if(local_odd % 5 >= 3)
	return (gfloat)(local_odd + (5 - local_odd % 5)) / 100;

    return (gfloat)(local_odd - local_odd % 5) / 100;
}

/** Find the bet containing the fixture. */
BetMatch*
bet_from_fixture(gint fix_id)
{
    gint i, j;

    for(i=1;i>=0;i--)
	for(j=0;j<bets[i]->len;j++)
	    if(g_array_index(bets[i], BetMatch, j).fix_id == fix_id)
		return &g_array_index(bets[i], BetMatch, j);

    main_exit_program(EXIT_BET_ERROR, 
		      "bet_from_fixture: bet going with fixture %d not found",
		      fix_id);

    return NULL;
}

/** Manage the bets made by the users. */
void
bet_update_user_bets(void)
{
    gint i, j, outcome;
    const BetMatch *bet = NULL;
    const Fixture *fix = NULL;

    for(i=0;i<users->len;i++)
    {
	g_array_free(usr(i).bets[0], TRUE);
	usr(i).bets[0] = g_array_new(FALSE, FALSE, sizeof(BetUser));
	
	for(j=0;j<usr(i).bets[1]->len;j++)
	{
	    bet = bet_from_fixture(g_array_index(usr(i).bets[1], BetUser, j).fix_id);
	    fix = fixture_from_id(g_array_index(usr(i).bets[1], BetUser, j).fix_id, TRUE);

	    if(fix->result[0][0] < fix->result[1][0])
		outcome = 2;
	    else
		outcome = (fix->result[0][0] == fix->result[1][0]);

	    if(outcome == g_array_index(usr(i).bets[1], BetUser, j).outcome)
	    {
		usr(i).money += 
		    (gint)rint((gfloat)g_array_index(usr(i).bets[1], BetUser, j).wager *
			       bet->odds[outcome]);
		usr(i).money_in[1][MON_IN_BETS] += 
		    (gint)rint((gfloat)g_array_index(usr(i).bets[1], BetUser, j).wager *
			       bet->odds[outcome]);
		g_array_index(usr(i).bets[1], BetUser, j).wager = 
		    (gint)rint((gfloat)g_array_index(usr(i).bets[1], BetUser, j).wager *
			       bet->odds[outcome]);
	    }
	    else
	    {
		usr(i).money -= 
		    g_array_index(usr(i).bets[1], BetUser, j).wager;
		usr(i).money_out[1][MON_OUT_BETS] -= 
		    g_array_index(usr(i).bets[1], BetUser, j).wager;
		g_array_index(usr(i).bets[1], BetUser, j).wager *= -1;
	    }

	    g_array_append_val(usr(i).bets[0], 
			       g_array_index(usr(i).bets[1], BetUser, j));
	}

	g_array_free(usr(i).bets[1], TRUE);
	usr(i).bets[1] = g_array_new(FALSE, FALSE, sizeof(BetUser));
    }
}

/** Calculate the odds for the bet. */
void
bet_get_odds(BetMatch *bet)
{
    const Fixture *fix = fixture_from_id(bet->fix_id, TRUE);
    gfloat home_advantage = (fix->home_advantage) ?
	(const_float("float_game_home_advantage_lower") +
	 const_float("float_game_home_advantage_upper")) / 2 : 0;
    gfloat av_skill[2] = {0, 0}, skilldiff;
    gint i, j, better_idx;

    for(i=0;i<2;i++)
    {
	for(j=0;j<11;j++)
	    av_skill[i] += 
		player_get_game_skill(player_of_idx_team(fix->teams[i], j),
				      FALSE, TRUE);

	av_skill[i] /= 11;
    }

    av_skill[0] *= (1 + home_advantage);

    skilldiff = ABS(av_skill[0] - av_skill[1]);

    better_idx = 2 * (av_skill[0] < av_skill[1]);

    bet->odds[better_idx] = 
	0.33 + skilldiff * const_float("float_bet_better_factor");
    bet->odds[2 - better_idx] = 
	0.33 + skilldiff * const_float("float_bet_worse_factor");

    for(i=0;i<2;i++)
	bet->odds[i * 2] = CLAMP(bet->odds[i * 2],
				 const_float("float_bet_lower_limit"),
				 1 - const_float("float_bet_lower_limit"));
    
    bet->odds[1] = (1 - bet->odds[0] - bet->odds[2]);
    bet->odds[1] = CLAMP(bet->odds[1],
			 const_float("float_bet_lower_limit"),
			 1 - const_float("float_bet_lower_limit"));

    for(i=0;i<3;i++)
	bet->odds[i] = 
	    bet_round_odd((1 / bet->odds[i]) * 
			  (1 - const_float("float_bet_commission_decrease")));
}

/** Write the bets for the current week round. */
void
bet_update(void)
{
    gint i;
    GPtrArray *fixtures = fixture_get_week_list(week, week_round);
    BetMatch new_bet;

    bet_update_user_bets();

    g_array_free(bets[0], TRUE);
    bets[0] = g_array_new(FALSE, FALSE, sizeof(BetMatch));

    for(i=0;i<bets[1]->len;i++)
	g_array_append_val(bets[0],
			   g_array_index(bets[1], BetMatch, i));

    g_array_free(bets[1], TRUE);
    bets[1] = g_array_new(FALSE, FALSE, sizeof(BetMatch));

    for(i=0;i<fixtures->len;i++)
	if(fixture_user_team_involved((Fixture*)g_ptr_array_index(fixtures, i)) == -1)
	{
	    new_bet.fix_id = ((Fixture*)g_ptr_array_index(fixtures, i))->id;
	    bet_get_odds(&new_bet);
	    g_array_append_val(bets[1], new_bet);
	}

    g_ptr_array_free(fixtures, TRUE);
}

/** Return the user bet if the user betted on the bet or NULL. */
BetUser*
bet_is_user(const BetMatch *bet)
{
    gint i, j;

    for(i=1;i>=0;i--)
	for(j=0;j<current_user.bets[i]->len;j++)
	    if(bet->fix_id == g_array_index(current_user.bets[i], BetUser, j).fix_id)
		return &g_array_index(current_user.bets[i], BetUser, j);

    return NULL;
}

/** Place a new bet. 
    @return TRUE on success, FALSE otherwise. */
gboolean
bet_place(gint fix_id, gint outcome, gint wager)
{
    gint max_wager = (gint)rint(finance_wage_unit(current_user.tm) * 
				const_float("float_bet_wager_limit_factor"));
    BetUser new_bet;
    gchar buf[SMALL];

    if(wager <= 0)
	return TRUE;

    if(wager > BUDGET(cur_user))
    {
	game_gui_show_warning(_("You don't have the money."));
	return FALSE;
    }

    if(wager > max_wager)
    {
	misc_print_grouped_int(max_wager, buf);
	game_gui_show_warning(_("The betting office doesn't allow you to wager more than %s."), buf);
	gtk_spin_button_set_value(
	    GTK_SPIN_BUTTON(lookup_widget(window.digits, "spinbutton1")),
	    (gdouble)max_wager);
	return FALSE;
    }

    new_bet.fix_id = fix_id;
    new_bet.outcome = outcome;
    new_bet.wager = wager;

    g_array_append_val(current_user.bets[1], new_bet);

    if(window.bets != NULL)
	treeview2_show_bets();

    return TRUE;
}

/** Remove the bet on the given fixture. */
void
bet_remove(gint fix_id)
{
    gint i;

    for(i=0;i<current_user.bets[1]->len;i++)
	if(g_array_index(current_user.bets[1], BetUser, i).fix_id == fix_id)
	{
	    g_array_remove_index(current_user.bets[1], i);
	    return;
	}
}
