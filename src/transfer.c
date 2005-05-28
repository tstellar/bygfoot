#include "cup.h"
#include "finance.h"
#include "free.h"
#include "game_gui.h"
#include "maths.h"
#include "option.h"
#include "player.h"
#include "support.h"
#include "team.h"
#include "transfer.h"
#include "treeview.h"
#include "user.h"

/** Weekly update of the transfer list. */
void
transfer_update(void)
{
    gint i, j;

    if(week > transfer_get_deadline())
	return;

    if(week == transfer_get_deadline())
    {
	for(i=transfer_list->len - 1;i>=0;i--)
	    transfer_remove_player(i);
	return;
    }

    for(i=transfer_list->len - 1;i>=0;i--)
    {
	for(j=trans(i).offers->len - 1;j>=0;j--)
	    if(trans(i).locked || team_is_user(transoff(i, j).tm) == -1)
		g_array_remove_index(trans(i).offers, j);

	trans(i).locked = FALSE;
    }

    transfer_add_new_players();
    transfer_add_cpu_offers();
    transfer_evaluate_offers();

    for(i=transfer_list->len - 1;i>=0;i--)
    {
	if(team_is_user(trans(i).tm) == -1 ||
	   trans(i).offers->len == 0)
	{
	    trans(i).time--;
	    if(trans(i).time == 0)
		transfer_remove_player(i);
	}
    }
}

/** Add offers for the users' players on the list.
    We also sort the offers from other users. */
void
transfer_add_cpu_offers(void)
{
    gint i;
    gfloat scout_factor_bounds[4][2] =
	{{const_float("float_transfer_offer_fee_best_lower"),
	  const_float("float_transfer_offer_fee_best_upper")},
	 {const_float("float_transfer_offer_fee_good_lower"),
	  const_float("float_transfer_offer_fee_good_upper")},
	 {-const_float("float_transfer_offer_fee_good_upper"),
	  -const_float("float_transfer_offer_fee_good_lower")},
	 {-const_float("float_transfer_offer_fee_best_upper"),
	  -const_float("float_transfer_offer_fee_best_lower")}};

    for(i=0;i<transfer_list->len;i++)
	if(team_is_user(trans(i).tm) != -1 &&
	   trans(i).offers->len == 0 &&
	   math_rnd(0, 1) < const_float("float_transfer_offer_prob_max") -
	   (user_from_team(trans(i).tm)->scout % 10 * const_float("float_transfer_offer_prob_reduce")))
	    transfer_add_offer(i, transfer_team_get_new(),
			       (gint)rint((gfloat)player_of_id_team(trans(i).tm, trans(i).id)->value *
					  (1 + math_rnd(
					      scout_factor_bounds[user_from_team(trans(i).tm)->scout % 10][0],
					      scout_factor_bounds[user_from_team(trans(i).tm)->scout % 10][1]))),
			       -1);
}

/** Return a random team as a potential buyer for a user player. */
Team*
transfer_team_get_new(void)
{
    gint rndom;
    GPtrArray *league_teams = team_get_sorted(team_compare_func, TEAM_COMPARE_UNSORTED, FALSE),
	*cup_teams = team_get_sorted(team_compare_func, TEAM_COMPARE_UNSORTED, TRUE);
    Team *return_value = NULL;

    while(return_value == NULL || team_is_user(return_value) != -1)
    {
	rndom = math_rndi(0, league_teams->len + cup_teams->len - 1);
	if(rndom < league_teams->len)
	    return_value = (Team*)g_ptr_array_index(league_teams, rndom);
	else
	    return_value = (Team*)g_ptr_array_index(cup_teams, rndom - league_teams->len);
    }

    return return_value;
}

/** Compare function for transfer offers. */
gint
transfer_offer_compare_func(gconstpointer a, gconstpointer b)
{
    const TransferOffer *tr1 = (const TransferOffer*)a,
	*tr2 = (const TransferOffer*)b;    
    gint return_value;

    if(tr1->fee > tr2->fee)
	return_value = -1;
    else if(tr2->fee > tr1->fee)
	return_value = 1;
    else if(tr1->wage > tr2->wage)
	return_value = -1;
    else if(tr2->wage > tr1->wage)
	return_value = 1;
    else
	return_value = 0;

    return return_value;
}

/** Send notification events about rejected transfer offers for
    user players. */
void
transfer_offers_notify(Transfer *tr)
{
    gint i;
    gboolean valid_offer = FALSE;
    TransferOffer *off = NULL;

    g_array_sort(tr->offers, transfer_offer_compare_func);
    off = &g_array_index(tr->offers, TransferOffer, 0);

    for(i=tr->offers->len - 1; i >= 1; i--)
    {
	if(team_is_user(g_array_index(tr->offers, TransferOffer, i).tm) != -1)
	    user_event_add(user_from_team(g_array_index(tr->offers, TransferOffer, i).tm),
			   EVENT_TYPE_TRANSFER_OFFER_REJECTED_BETTER_OFFER, 
			   g_array_index(tr->offers, TransferOffer, i).fee,
			   g_array_index(tr->offers, TransferOffer, i).wage,
			   tr->tm, player_of_id_team(tr->tm, tr->id)->name->str);

	g_array_remove_index(tr->offers, i);
    }

    if(team_is_user(tr->tm) != -1)
    {
	user_event_add(user_from_team(tr->tm), EVENT_TYPE_TRANSFER_OFFER_USER,
		       -1, -1, NULL, player_of_id_team(tr->tm, tr->id)->name->str);
	valid_offer = TRUE;
	tr->locked = TRUE;
    }
    else if(player_of_id_team(tr->tm, tr->id)->value > off->fee &&
	    player_of_id_team(tr->tm, tr->id)->wage > off->wage)
	user_event_add(user_from_team(off->tm),
		       EVENT_TYPE_TRANSFER_OFFER_REJECTED_FEE_WAGE, 
		       off->fee, off->wage,
		       tr->tm, player_of_id_team(tr->tm, tr->id)->name->str);
    else if(player_of_id_team(tr->tm, tr->id)->value > off->fee)
	user_event_add(user_from_team(off->tm),
		       EVENT_TYPE_TRANSFER_OFFER_REJECTED_FEE,
		       off->fee, off->wage,
		       tr->tm, player_of_id_team(tr->tm, tr->id)->name->str);
    else if(player_of_id_team(tr->tm, tr->id)->wage > off->wage)
	user_event_add(user_from_team(off->tm),
		       EVENT_TYPE_TRANSFER_OFFER_REJECTED_WAGE,
		       off->fee, off->wage,
		       tr->tm, player_of_id_team(tr->tm, tr->id)->name->str);
    else if(off->tm->players->len >= const_int("int_team_max_players"))
	user_event_add(user_from_team(off->tm),
		       EVENT_TYPE_TRANSFER_OFFER_ROSTER, -1, -1,
		       tr->tm, player_of_id_team(tr->tm, tr->id)->name->str);
    else
    {
	user_event_add(user_from_team(off->tm),
		       EVENT_TYPE_TRANSFER_OFFER_CPU, -1, -1,
		       NULL, player_of_id_team(tr->tm, tr->id)->name->str);
	valid_offer = TRUE;
	tr->locked = TRUE;
    }

    if(!valid_offer)
	g_array_remove_index(tr->offers, 0);
}

/** Move players to the users' teams if their offer
    was accepted or remove the players from the list if not. */
void
transfer_evaluate_offers(void)
{
    gint i, j;

    for(i=transfer_list->len - 1;i>=0;i--)
	if(trans(i).offers->len > 0)
	{
	    for(j=trans(i).offers->len - 1;j >= 0; j--)
		if(team_is_user(transoff(i, j).tm) != -1 &&
		   transoff(i, j).fee > BUDGET(team_is_user(transoff(i, j).tm)))
		{
		    user_event_add(user_from_team(transoff(i, j).tm),
				   EVENT_TYPE_TRANSFER_OFFER_MONEY, -1, -1,
				   trans(i).tm, player_of_id_team(trans(i).tm, trans(i).id)->name->str);
		    g_array_remove_index(trans(i).offers, j);
		}

	    if(trans(i).offers->len > 0)
		transfer_offers_notify(&trans(i));
	}
}

/** Add new players to the transfer list. */
void
transfer_add_new_players(void)
{
    gint i;
    gint number_of_new = math_rndi(const_int("int_transfer_new_players_lower"),
				   const_int("int_transfer_new_players_upper"));
    number_of_new = MIN(number_of_new,
			const_int("int_transfer_max_players") - transfer_list->len);
    
    for(i=0;i<number_of_new;i++)
	transfer_add_player(transfer_player_get_new(
				(math_rnd(0, 1) < const_float("float_transfer_cup_percentage") &&
				 cup_count_international() > 0)),
			    math_rndi(const_int("int_transfer_time_lower"),
				      const_int("int_transfer_time_upper")));
}

/** Select a random player for the transfer list. */
Player*
transfer_player_get_new(gboolean cup)
{
    GPtrArray *teams = team_get_sorted(team_compare_func, TEAM_COMPARE_UNSORTED, cup);
    Player *pl = NULL;
    Team *tm = NULL;

    while(pl == NULL || team_is_user(pl->team) != -1 ||
	  query_transfer_player_is_on_list(pl))
    {
	tm = (Team*)g_ptr_array_index(teams, math_rndi(0, teams->len - 1));
	pl = player_of_idx_team(tm, math_rndi(0, tm->players->len - 1));

	if(pl->skill > const_float("float_player_max_skill") *
	   const_float("float_transfer_good_player_skill_bound") &&
	   math_rnd(0, 1) > const_float("float_transfer_good_player_prob"))
	    pl = NULL;
    }
    
    return pl;
}

/** Add the player to the transfer list.
    @param time The number of weeks he'll be on the list. */
void
transfer_add_player(Player *pl, gint time)
{
    gint i;
    Transfer new;
    gfloat deviance_value, deviance_wage;

    new.tm = pl->team;
    new.id = pl->id;
    new.time = time;
    new.offers = g_array_new(FALSE, FALSE, sizeof(TransferOffer));
    new.locked = FALSE;

    for(i=0;i<4;i++)
    {
	deviance_value = math_rnd(-(i + 1) * const_float("float_transfer_scout_deviance_value"),
				  (i + 1) * const_float("float_transfer_scout_deviance_value"));
	deviance_wage = math_rnd(-(i + 1) * const_float("float_transfer_scout_deviance_wage"),
				 (i + 1) * const_float("float_transfer_scout_deviance_wage"));
	new.fee[i] = (gint)rint((gfloat)pl->value * (1 + deviance_value));
	new.wage[i] = (gint)rint((gfloat)pl->wage * (1 + deviance_wage));
    }

    g_array_append_val(transfer_list, new);
}

/** Return the transfer deadline. */
gint
transfer_get_deadline(void)
{
    gint i;
    gint length = 0;

    for(i=0;i<ligs->len;i++)
	length = MAX(length, (lig(i).teams->len - 1) * 2);

    return (gint)rint((gfloat)length * const_float("float_transfer_deadline_percentage"));
}

/** Remove a player from the transfer list. */
void
transfer_remove_player(gint idx)
{
    free_g_array(&trans(idx).offers);
    g_array_remove_index(transfer_list, idx);
}

/** Remove a player given by a pointer from the list. */
void
transfer_remove_player_ptr(const Player *pl)
{
    gint i;

    for(i=0;i<transfer_list->len;i++)
	if(player_of_id_team(trans(i).tm, trans(i).id) == pl)
	{
	    transfer_remove_player(i);
	    break;
	}
}

/** Find out whether a player is on the transfer list. */
gboolean
query_transfer_player_is_on_list(const Player *pl)
{
    gint i;

    for(i=0;i<transfer_list->len;i++)
	if(trans(i).tm == pl->team &&
	   trans(i).id == pl->id)
	    return TRUE;

    return FALSE;
}

/** Check whether there is an offer by the current
    user for the player. */
gboolean
query_transfer_current_team_offer(const Player *pl)
{
    gint i, j;

    for(i=0;i<transfer_list->len;i++)
	if(trans(i).tm == pl->team &&
	   trans(i).id == pl->id)
	{
	    for(j=0;j<trans(i).offers->len;j++)
		if(transoff(i, j).tm == current_user.tm)
		    return TRUE;
	}

    return FALSE;
}

/** Find out whether a player is on the list
    and there are offers to buy him. */
const TransferOffer*
transfer_player_has_offer(const Player *pl)
{
    gint i;

    for(i=0;i<transfer_list->len;i++)
	if(trans(i).tm == pl->team &&
	   trans(i).id == pl->id &&
	   trans(i).locked)
	    return &transoff(i, 0);

    return NULL;
}

/** Add an offer to the player with specified index on the transfer list.
    @param return TRUE if the offer updated an earlier offer, FALSE, otherwise. */
gboolean
transfer_add_offer(gint idx, Team *tm, gint fee, gint wage)
{
    gint i;
    TransferOffer new;

    new.tm = tm;
    new.fee = fee;
    new.wage = wage;

    for(i=0;i<trans(idx).offers->len;i++)
	if(transoff(idx, i).tm == tm)
	{
	    transoff(idx, i).fee = fee;
	    transoff(idx, i).wage = wage;
	    return TRUE;
	}

    g_array_append_val(trans(idx).offers, new);

    return FALSE;
}

/** Return the index of the transfer containing the player going with
    the team and the id. */
gint
transfer_get_index(const Team *tm, gint id)
{
    gint i;

    for(i=0;i<transfer_list->len;i++)
	if(trans(i).tm == tm && trans(i).id == id)
	    return i;

    g_warning("transfer_get_index: didn't find transfer.\n");

    return -1;
}

/** Remove any offers from the team for the given transfer player. */
gboolean
transfer_remove_offer(gint idx, const Team *tm)
{
    gint i;

    for(i=0;i<trans(idx).offers->len;i++)
	if(transoff(idx, i).tm == tm)
	{
	    g_array_remove_index(trans(idx).offers, i);
	    return TRUE;
	}    

    return FALSE;
}

/** Add or remove a user player from the transfer list. */
void
transfer_add_remove_user_player(Player *pl)
{
    gchar buf[SMALL];

    if(!query_transfer_player_is_on_list(pl))
    {
	transfer_add_player(pl,
			    (gint)rint(((gfloat)const_int("int_transfer_time_lower") +
					(gfloat)const_int("int_transfer_time_upper")) / 2));
	sprintf(buf, _("%s has been added to the transfer list for %d weeks."),
		pl->name->str, 
		(gint)rint(((gfloat)const_int("int_transfer_time_lower") +
			    (gfloat)const_int("int_transfer_time_upper")) / 2));
	game_gui_print_message(buf);
    }
    else
	transfer_remove_player_ptr(pl);

    treeview_show_transfer_list(GTK_TREE_VIEW(lookup_widget(window.main, "treeview_right")));
}
