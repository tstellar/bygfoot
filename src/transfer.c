#include "finance.h"
#include "free.h"
#include "league.h"
#include "maths.h"
#include "option.h"
#include "player.h"
#include "team.h"
#include "transfer.h"
#include "user.h"

/** Weekly update of the transfer list. */
void
transfer_update(void)
{
    gint i;

    if(week > transfer_get_deadline())
	return;

    if(week == transfer_get_deadline())
    {
	for(i=transfer_list->len - 1;i>=0;i--)
	    transfer_remove_player(i);
	return;
    }

    transfer_evaluate_offers();

    for(i=transfer_list->len - 1;i>=0;i--)
    {
	trans(i).time--;
	if(trans(i).time == 0)
	    transfer_remove_player(i);
    }

    transfer_add_new_players();
    transfer_add_offers();
}

/** Add offers for the users' players on the list. */
void
transfer_add_offers(void)
{
    
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

/** Move players to the users' teams if their offer
    was accepted or remove the players from the list if not. */
void
transfer_evaluate_offers(void)
{
    gint i, j;
    gint idx = -1;

    for(i=transfer_list->len - 1;i>=0;i--)
    {
	if(trans(i).offers->len > 1)
	    g_array_sort(trans(i).offers, transfer_offer_compare_func);

	for(j=0;j<trans(i).offers->len;j++)
	    if(team_is_user(transoff(i, j).tm) != -1 &&
	       transoff(i, j).fee > BUDGET(user_get_index(user_from_team(transoff(i, j).tm))))
		user_event_add(user_from_team(transoff(i, j).tm),
			       EVENT_TYPE_TRANSFER_OFFER_MONEY, -1, -1,
			       trans(i).pl->team, trans(i).pl->name->str);
	    else
	    {
		idx = j;
		break;
	    }
	
	for(j=idx + 1;j<trans(i).offers->len;j++)
	    user_event_add(user_from_team(transoff(i, j).tm),
			   EVENT_TYPE_TRANSFER_OFFER_REJECTED, -1, -1,
			   trans(i).pl->team, trans(i).pl->name->str);

	if(trans(i).offers->len != 0)
	{
	    printf("%s val %d wa %d fee %d wag %d\n",
		   trans(i).pl->name->str, trans(i).pl->value,
		   trans(i).pl->wage, transoff(i, idx).fee,
		   transoff(i, idx).wage);

	    if(trans(i).pl->value > transoff(i, idx).fee ||
	       trans(i).pl->wage > transoff(i, idx).wage)
		user_event_add(user_from_team(transoff(i, idx).tm),
			       EVENT_TYPE_TRANSFER_OFFER_REJECTED, -1, -1,
			       trans(i).pl->team, trans(i).pl->name->str);
	    else if(transoff(i, idx).fee > BUDGET(user_get_index(user_from_team(transoff(i, idx).tm))))
		user_event_add(user_from_team(transoff(i, idx).tm),
			       EVENT_TYPE_TRANSFER_OFFER_MONEY, -1, -1,
			       trans(i).pl->team, trans(i).pl->name->str);
	    else if(transoff(i, idx).tm->players->len >= const_int("int_team_max_players"))
		user_event_add(user_from_team(transoff(i, idx).tm),
			       EVENT_TYPE_TRANSFER_OFFER_ROSTER, -1, -1,
			       trans(i).pl->team, trans(i).pl->name->str);
	    else
	    {
		user_event_add(user_from_team(transoff(i, idx).tm),
			       EVENT_TYPE_TRANSFER_OFFER_ACCEPTED, -1, -1,
			       trans(i).pl->team, trans(i).pl->name->str);
		player_move(trans(i).pl->team, player_id_index(trans(i).pl->team, trans(i).pl->id),
			    transoff(i, idx).tm, transoff(i, idx).tm->players->len);
		player_replace_by_new(trans(i).pl, FALSE);
		transfer_remove_player(i);
	    }
	}
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
				(math_rnd(0, 1) < const_float("float_transfer_cup_percentage"))),
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
	  query_player_is_on_transfer_list(pl))
    {
	tm = (Team*)g_ptr_array_index(teams, math_rndi(0, teams->len - 1));
	pl = player_of(tm, math_rndi(0, tm->players->len - 1));
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
    gfloat deviance = 0;

    new.pl = pl;
    new.time = time;
    new.offers = g_array_new(FALSE, FALSE, sizeof(TransferOffer));

    for(i=0;i<4;i++)
    {
	deviance = math_rnd(-(i + 1) * const_float("float_transfer_scout_deviance"),
			    (i + 1) * const_float("float_transfer_scout_deviance"));
	new.fee[i] = (gint)rint((gfloat)pl->value * (1 + deviance));
	new.wage[i] = (gint)rint((gfloat)pl->wage * (1 + deviance));
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

/** Find out whether a player is on the transfer list. */
gboolean
query_player_is_on_transfer_list(const Player *pl)
{
    gint i;

    for(i=0;i<transfer_list->len;i++)
	if(trans(i).pl == pl)
	    return TRUE;

    return FALSE;
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
