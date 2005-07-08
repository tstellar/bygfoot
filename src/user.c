#include "cup.h"
#include "finance.h"
#include "fixture.h"
#include "free.h"
#include "game_gui.h"
#include "league.h"
#include "live_game.h"
#include "main.h"
#include "maths.h"
#include "misc.h"
#include "name.h"
#include "option.h"
#include "player.h"
#include "support.h"
#include "team.h"
#include "transfer.h"
#include "treeview.h"
#include "user.h"
#include "window.h"
#include "youth_academy.h"

/** Create a new user with default values. */
User
user_new(void)
{
    User new;

    new.name = g_string_new("NONAME");
    new.tm = NULL;
    new.team_id = -1;
    
    live_game_reset(&new.live_game, NULL, FALSE);

    new.events = g_array_new(FALSE, FALSE, sizeof(Event)); 
    new.history = g_array_new(FALSE, FALSE, sizeof(UserHistory)); 
    new.options.list = NULL;
    new.options.datalist = NULL;

    new.sponsor.name = g_string_new("");
    new.sponsor.contract = new.sponsor.benefit = -1;

    return new;
}

/** Move a user's team to top or bottom league
    at the beginning of a new game and set up the team.
    @param user The user we set up the team for. */
void
user_set_up_team_new_game(User *user)
{
    gchar buf[SMALL];
    gint rndom;

    if(user->scout == -1)
    {
	user_set_up_team(user);
	user_history_add(user, USER_HISTORY_START_GAME, user->tm->id, user->tm->clid, -1, "");
    }
    else
    {
	rndom = math_rndi(0, lig(user->scout).teams->len - 1);
	while(team_is_user(&g_array_index(lig(user->scout).teams, Team, rndom)) != -1)
	    rndom = math_rndi(0, lig(user->scout).teams->len - 1);
      
	sprintf(buf, "%s", g_array_index(lig(user->scout).teams, Team, rndom).name->str);
	g_string_printf(g_array_index(lig(user->scout).teams, Team, rndom).name, "%s",
			user->tm->name->str);
	g_string_printf(user->tm->name, "%s", buf);

	user->tm = &g_array_index(lig(user->scout).teams, Team, rndom);
	user->team_id = g_array_index(lig(user->scout).teams, Team, rndom).id;

	user_history_add(user, USER_HISTORY_START_GAME, user->tm->id, user->tm->clid, -1, "");

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
	    if(g_array_index(user->tm->players, Player, j).pos == i &&
	       g_array_index(user->tm->players, Player, j).recovery != 1)
	    {
		player_remove_from_team(user->tm, j);
		break;
	    }

    for(i=0;i<user->tm->players->len;i++)
	g_array_index(user->tm->players, Player, i).recovery = 0;

    user->scout = user->physio = QUALITY_AVERAGE;
    
    user->tm->style = 0;
    
    user_set_up_finances(user);
    user_set_up_counters(user);

    user->youth_academy = youth_academy_new(user->tm);

    user->counters[COUNT_USER_NEW_SPONSOR] = (sett_int("int_opt_disable_finances")) ? -5 : 1;
}


/** Set the counters of the user to their initial values. */
void
user_set_up_counters(User *user)
{
    gint i;

    for(i=0;i<COUNT_USER_END;i++)
	user->counters[i] = 0;

    user->counters[COUNT_USER_LOAN] =
	user->counters[COUNT_USER_POSITIVE] = -1;
}
/** Set up the user's finances when he's got a new team.
    @param user The user we set up the finances for. */
void
user_set_up_finances(User *user)
{
    gint i;

    for(i=0; i<MON_OUT_END;i++)
	user->money_out[0][i] = user->money_out[1][i] = 0;
    for(i=0; i<MON_IN_END;i++)
	user->money_in[0][i] = user->money_in[1][i] = 0;
    
    user->debt = 0;
    user->money = 
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
    gint i;

    if(regenerate_team)
    {
	for(i=0;i<usr(idx).tm->players->len;i++)
	    free_player(&g_array_index(usr(idx).tm->players, Player, i));

	g_array_free(usr(idx).tm->players, TRUE);
	usr(idx).tm->players = g_array_new(FALSE, FALSE, sizeof(Player));

	team_generate_players_stadium(usr(idx).tm);
	for(i=0;i<usr(idx).tm->players->len;i++)
	    g_array_index(usr(idx).tm->players, Player, i).team = usr(idx).tm;
    }

    free_user(&usr(idx));
    g_array_remove_index(users, idx);

    cur_user = 0;

    if(window.main != NULL)
	game_gui_show_main();
}

void
user_set_player_list_attributes(const User *user, PlayerListAttribute *attribute, gint list_number)
{
    gint i, cnt = 0;
    gchar prefix[SMALL];

    sprintf(prefix, "int_opt_user_pl%d_att", list_number);

    for(i=0;i<user->options.list->len;i++)
	if(g_str_has_prefix(g_array_index(user->options.list, Option, i).name->str, prefix))
	{
	    attribute->on_off[cnt] = g_array_index(user->options.list, Option, i).value;
	    cnt++;
	}
}

/** Find out whether there are user games at the specified date. */
gboolean
query_user_games_in_week_round(gint week_number, gint week_round_number)
{
    gint i;

    for(i=0;i<users->len;i++)
	if(query_user_game_in_week_round(i, week_number, week_round_number))
	    return TRUE;

    return FALSE;
}

/** Find out whether the specified user has a match at the specified date. */
gboolean
query_user_game_in_week_round(gint usr_idx, gint week_number, gint week_round_number)
{
    gint i, j;

    for(i=0;i<ligs->len;i++)
	for(j=0;j<lig(i).fixtures->len;j++)
	    if(fixture_user_team_involved(&g_array_index(lig(i).fixtures, Fixture, j)) == usr_idx &&
	       g_array_index(lig(i).fixtures, Fixture, j).week_number == week_number &&
	       g_array_index(lig(i).fixtures, Fixture, j).week_round_number == week_round_number)
		return TRUE;

    for(i=0;i<acps->len;i++)
	for(j=0;j<acp(i)->fixtures->len;j++)
	    if(fixture_user_team_involved(&g_array_index(acp(i)->fixtures, Fixture, j)) == usr_idx &&
	       g_array_index(acp(i)->fixtures, Fixture, j).week_number == week_number &&
	       g_array_index(acp(i)->fixtures, Fixture, j).week_round_number == week_round_number)
		return TRUE;

    return FALSE;
}


/** Get the user managing the team.
    @param tm The team.
    @return A pointer to the user. */
User*
user_from_team(const Team *tm)
{
    gint i;

    for(i=0;i<users->len;i++)
	if(usr(i).tm == tm)
	    return &usr(i);

    g_warning("User going with team %s not found.\n",
	      tm->name->str);

    main_exit_program(EXIT_POINTER_NOT_FOUND, NULL);

    return NULL;
}

/** Check the success counter of the user and offer a new job or
    fire him if the counter says so. */
void
user_job_offer(User *user)
{
    Team *new_team = NULL;

    if(math_rnd(0, 1) > const_float("float_user_success_counter_check") ||
       ABS(user->counters[COUNT_USER_SUCCESS]) < (gfloat)const_int("int_user_success_offer_limit") * 0.9)
	return;

    if(user->counters[COUNT_USER_SUCCESS] < -(gfloat)const_int("int_user_success_offer_limit") * 0.9 &&
       !user->counters[COUNT_USER_WARNING])
    {
	user_event_add(user, EVENT_TYPE_WARNING, -1, -1, NULL,
		       _("The owners of %s are not satisfied with the recent performance of the team. There are rumours they're looking for a new manager."), user->tm->name->str);
	user->counters[COUNT_USER_WARNING] = 1;
	return;
    }

    if(math_rnd(0, 1) < const_float("float_user_success_base_prob") * 
       log((gfloat)ABS(user->counters[COUNT_USER_SUCCESS]) * 
	   const_float("float_user_success_prob_factor")))
    {
	if(user->counters[COUNT_USER_SUCCESS] < 0)
	{
	    new_team = team_get_new(user->tm, TRUE);
	    user_event_add(user, EVENT_TYPE_FIRE_FAILURE, -1, -1, new_team, NULL);
	    user_history_add(user, USER_HISTORY_FIRE_FAILURE, user->tm->id,
			     new_team->id, new_team->clid, "");
	}
	else if(option_int("int_opt_user_show_job_offers", &user->options))
	{
	    new_team = team_get_new(user->tm, FALSE);
	    user_event_add(user, EVENT_TYPE_JOB_OFFER, -1, -1, new_team, NULL);
	}
    }
}

/** Update the counters of the users. */
void
user_weekly_update_counters(User *user)
{
    gint rank = team_get_league_rank(user->tm);
    gint teamslen = ((GArray*)(league_cup_get_teams(user->tm->clid)))->len;
    gint rank_bounds[2] = {(gint)rint(const_float("float_user_success_table_bound_upper") *
				      (gfloat)teamslen),
			   (gint)rint(const_float("float_user_success_table_bound_lower") *
				      (gfloat)teamslen)};
    gint *cnts = user->counters;
    gint increase_capacity;
    gfloat increase_safety;

    if(cnts[COUNT_USER_STADIUM_CAPACITY] > 0)
    {
	increase_capacity = math_rndi(const_int("int_stadium_improvement_base_seats") - 
				      const_int("int_stadium_improvement_seats_variance"),
				      const_int("int_stadium_improvement_base_seats") + 
				      const_int("int_stadium_improvement_seats_variance"));
	user->tm->stadium.capacity += MIN(cnts[COUNT_USER_STADIUM_CAPACITY],
					  increase_capacity);
	cnts[COUNT_USER_STADIUM_CAPACITY] = 
	    MAX(cnts[COUNT_USER_STADIUM_CAPACITY] - increase_capacity, 0);
    }

    if(cnts[COUNT_USER_STADIUM_SAFETY] > 0)
    {
	increase_safety = math_rnd(const_float("float_stadium_improvement_base_safety") - 
				   const_float("float_stadium_improvement_safety_variance"),
				   const_float("float_stadium_improvement_base_safety") + 
				   const_float("float_stadium_improvement_safety_variance"));
	user->tm->stadium.safety += MIN((gfloat)cnts[COUNT_USER_STADIUM_SAFETY] / 100,
					increase_safety);
	user->tm->stadium.safety = MIN(user->tm->stadium.safety, 1);

	cnts[COUNT_USER_STADIUM_SAFETY] = 
	    MAX(cnts[COUNT_USER_STADIUM_SAFETY] - (gint)rint(increase_safety * 100), 0);
    }
    
    if(query_league_matches_in_week(league_from_clid(user->tm->clid), week))
    {
	if(rank < rank_bounds[0])
	    user->counters[COUNT_USER_SUCCESS] += (rank_bounds[0] - rank);
	else if(rank > rank_bounds[1])
	    user->counters[COUNT_USER_SUCCESS] -= (rank - rank_bounds[1]);
	else
	{
	    if(user->counters[COUNT_USER_SUCCESS] > 0)
		user->counters[COUNT_USER_SUCCESS] -= const_int("int_user_success_mediocre_rank_change");
	    else
		user->counters[COUNT_USER_SUCCESS] += const_int("int_user_success_mediocre_rank_change");
	}
    }
}

/** Return a default new user event. */
Event
user_event_new(void)
{
    Event new;

    new.user = NULL;
    new.type = -1;
    new.value1 = new.value2 = -1;
    new.value_pointer = NULL;
    new.value_string = NULL;

    return new;
}

/** Add an event with the specified values to the event array of the user. */
void
user_event_add(User *user, gint type, gint value1, gint value2, 
	       gpointer value_pointer, gchar *format, ...)
{
    Event new = user_event_new();
    gchar text[SMALL];
    va_list args;
     
    new.user = user;
    new.type = type;
    new.value1 = value1;
    new.value2 = value2;
    new.value_pointer = value_pointer;
    
    if(format != NULL)
    {
	va_start (args, format);
	g_vsprintf(text, format, args);
	va_end (args);
	new.value_string = g_string_new(text);
    }
    else
	new.value_string = NULL;    

    g_array_append_val(user->events, new);
}

/** Remove an event from the user event array.
    @param user The user.
    @param idx The index of the event in the array. */
void
user_event_remove(User *user, gint idx)
{
    free_event(&g_array_index(user->events, Event, idx));
    g_array_remove_index(user->events, idx);
}

/** Show the next event for the current user. */
void
user_event_show_next(void)
{    
    Event *event = NULL;
    gchar buf[SMALL],
	buf2[SMALL], buf3[SMALL];

    if(current_user.events->len == 0)
	return;

    stat4 = STATUS_SHOW_EVENT;

    event = &g_array_index(current_user.events, Event, 0);

    switch(event->type)
    {
	default:
	    g_warning("user_event_show_next: unknown event type %d\n", event->type);
	    break;
	case EVENT_TYPE_PLAYER_LEFT:
	    game_gui_show_warning(_("%s has left your team because his contract expired."),
		    event->value_string->str);
	    break;
	case EVENT_TYPE_WARNING:
	    game_gui_show_warning(event->value_string->str);
	    break;
	case EVENT_TYPE_FIRE_FINANCE:
	    stat2 = STATUS_JOB_OFFER_FIRE_FINANCE;
	    statp = event->value_pointer;
	    game_gui_show_job_offer((Team*)event->value_pointer, STATUS_JOB_OFFER_FIRE_FINANCE);
	    break;
	case EVENT_TYPE_FIRE_FAILURE:
	    stat2 = STATUS_JOB_OFFER_FIRE_FAILURE;
	    statp = event->value_pointer;
	    game_gui_show_job_offer((Team*)event->value_pointer, STATUS_JOB_OFFER_FIRE_FAILURE);
	    break;
	case EVENT_TYPE_JOB_OFFER:
	    stat2 = STATUS_JOB_OFFER_SUCCESS;
	    statp = event->value_pointer;
	    game_gui_show_job_offer((Team*)event->value_pointer, STATUS_JOB_OFFER_SUCCESS);
	    break;
	case EVENT_TYPE_OVERDRAW:
	    if(event->value1 == 1)
		game_gui_show_warning(_("You have overdrawn your bank account. The team owners give you %d weeks to get above your drawing credit limit."), const_int("int_finance_overdraw_positive"));
	    else
		game_gui_show_warning(_("You have overdrawn your bank account once again. Bear in mind that after the fourth time you get fired.\nThe team owners give you %d weeks to get above your drawing credit limit."), const_int("int_finance_overdraw_positive"));
	    break;
	case EVENT_TYPE_TRANSFER_OFFER_USER:
	    game_gui_show_warning(_("Have a look at the transfer list, there's an offer for %s."),
		    event->value_string->str);
	    break;
	case EVENT_TYPE_TRANSFER_OFFER_CPU:
	    game_gui_show_warning(_("Your offer for %s has been accepted. If you still want to buy him, go to the transfer list and left click on the player."),
		    event->value_string->str);
	    break;
	case EVENT_TYPE_TRANSFER_OFFER_REJECTED_BETTER_OFFER:
	    misc_print_grouped_int(event->value1, buf2);
	    misc_print_grouped_int(event->value2, buf3);	    
	    game_gui_show_warning(_("The owners of %s have rejected your offer (%s / %s) for %s. There was a better offer for the player than yours."), ((Team*)event->value_pointer)->name->str, buf2, buf3, event->value_string->str);
	    break;
	case EVENT_TYPE_TRANSFER_OFFER_REJECTED_FEE_WAGE:
	    misc_print_grouped_int(event->value1, buf2);
	    misc_print_grouped_int(event->value2, buf3);	    
	    game_gui_show_warning(_("The owners of %s have rejected your offer (%s / %s) for %s. Neither the fee nor the wage you offered were acceptable, they say."), ((Team*)event->value_pointer)->name->str, buf2, buf3, event->value_string->str);
	    break;
	case EVENT_TYPE_TRANSFER_OFFER_REJECTED_FEE:
	    misc_print_grouped_int(event->value1, buf2);
	    misc_print_grouped_int(event->value2, buf3);	    
	    game_gui_show_warning(_("The owners of %s have rejected your offer (%s / %s) for %s. The team owners weren't satisfied with the fee you offered."), ((Team*)event->value_pointer)->name->str, buf2, buf3, event->value_string->str);
	    break;
	case EVENT_TYPE_TRANSFER_OFFER_REJECTED_WAGE:
	    misc_print_grouped_int(event->value1, buf2);
	    misc_print_grouped_int(event->value2, buf3);	    
	    /* A player from a team has rejected a transfer offer. */
	    game_gui_show_warning(_("%s of %s has rejected your offer (%s / %s). He wasn't satisfied with the wage you offered."),
		    event->value_string->str,
		    ((Team*)event->value_pointer)->name->str, buf2, buf3);
	    break;
	case EVENT_TYPE_TRANSFER_OFFER_MONEY:
	    /* Buy a player from a team. */
	    game_gui_show_warning(_("You didn't have enough money to buy %s from %s."),
		    event->value_string->str, ((Team*)event->value_pointer)->name->str);
	    break;
	case EVENT_TYPE_TRANSFER_OFFER_ROSTER:
	    /* Buy a player from a team. */
	    game_gui_show_warning(_("Your roster is full. You couldn't buy %s from %s."),
		    event->value_string->str, ((Team*)event->value_pointer)->name->str);
	    break;
	case EVENT_TYPE_PLAYER_CAREER_STOP:
	    sprintf(buf, _("%s's injury was so severe that he can't play football on a professional level anymore. He leaves your team."), player_of_id_team(event->user->tm, event->value1)->name->str);
	    if(event->user->tm->players->len < 12)
	    {
		strcat(buf, _(" Fortunately he's got a cousin who can help your team out."));
		player_replace_by_new(player_of_id_team(event->user->tm, event->value1), TRUE);
	    }
	    else
		player_remove_from_team(event->user->tm, player_id_index(event->user->tm, event->value1));
	    treeview_show_user_player_list();
	    game_gui_show_warning(buf);
	    break;
    }

    user_event_remove(&current_user, 0);
}

/** Change the team of a user. */
void
user_change_team(User *user, Team *tm)
{
    gint i;

    user->tm = tm;
    user->team_id = tm->id;

    user_set_up_team(user);

    user->counters[COUNT_USER_NEW_SPONSOR] = 0;
    g_string_free(user->sponsor.name, TRUE);
    user->sponsor = user_get_sponsor(&current_user);    

    for(i=user->events->len - 1; i >= 0; i--)
	user_event_remove(user, i);    
}

/** Check whether one of the user teams has an unfit player
    in the startup formation. */
gboolean
query_user_teams_have_unfit(void)
{
    gint i, j;

    for(i=0;i<users->len;i++)
    {
	if(query_user_game_in_week_round(i, week, week_round))
	{
	    for(j=0;j<11;j++)
		if(g_array_index(usr(i).tm->players, Player, j).health != 0 ||
		   player_is_banned(&g_array_index(usr(i).tm->players, Player, j)) > 0)
		    return TRUE;
	}
    }

    return FALSE;
}

/** Compare function for history sorting. */
gint
user_history_compare(gconstpointer a, gconstpointer b)
{
    gint return_value = 0;
    const UserHistory *his1 = (const UserHistory*)a,
	*his2 = (const UserHistory*)b;

    if(his1->season < his2->season)
	return_value = 1;
    else if(his1->season > his2->season)
	return_value = -1;
    else if(his1->week < his2->week)
	return_value = 1;
    else if(his1->week > his2->week)
	return_value = -1;

    return return_value;
}

/** Add an element to the user history filled with the given values. */
void
user_history_add(User *user, gint type, gint team_id,
		 gint value1, gint value2, gchar *string)
{
    gint i;
    UserHistory new_history;
    UserHistory *his = &new_history;
    gboolean replace = FALSE;


    if(type == USER_HISTORY_WIN_FINAL ||
       type == USER_HISTORY_LOSE_FINAL ||
       type == USER_HISTORY_REACH_CUP_ROUND)
    {
	for(i=0;i<user->history->len;i++)
	    if((g_array_index(user->history, UserHistory, i).type == USER_HISTORY_WIN_FINAL ||
		g_array_index(user->history, UserHistory, i).type == USER_HISTORY_LOSE_FINAL ||
		g_array_index(user->history, UserHistory, i).type == USER_HISTORY_REACH_CUP_ROUND) &&
	       g_array_index(user->history, UserHistory, i).season == season &&
	       g_array_index(user->history, UserHistory, i).team_id == team_id &&
	       g_array_index(user->history, UserHistory, i).value1 == value1)
	    {
		/** Same cup round. */
		if(g_array_index(user->history, UserHistory, i).value2 == value2 &&
		   type == USER_HISTORY_REACH_CUP_ROUND)
		    return;

		his = &g_array_index(user->history, UserHistory, i);
		replace = TRUE;
		break;
	    }
    }

    if(type == USER_HISTORY_WIN_FINAL)
    {
	if(query_cup_is_international(value1))
	    user->counters[COUNT_USER_SUCCESS] +=
		const_int("int_user_success_international_winner");
	else if(query_cup_is_national(value1))
	    user->counters[COUNT_USER_SUCCESS] +=
		const_int("int_user_success_national_winner");
    }
    else if(type == USER_HISTORY_LOSE_FINAL)
    {
	if(query_cup_is_international(value1))
	    user->counters[COUNT_USER_SUCCESS] +=
		const_int("int_user_success_international_final");
	else if(query_cup_is_national(value1))
	    user->counters[COUNT_USER_SUCCESS] +=
		const_int("int_user_success_national_winner");
		const_int("int_user_success_national_final");
    }
    else if(type == USER_HISTORY_REACH_CUP_ROUND)
    {
	if(value2 == cup_from_clid(value1)->rounds->len - 2)
	{
	    if(query_cup_is_international(value1))
		user->counters[COUNT_USER_SUCCESS] +=
		    const_int("int_user_success_international_semis");
	    else if(query_cup_is_national(value1))
		user->counters[COUNT_USER_SUCCESS] +=
		    const_int("int_user_success_national_semis");
	}
	else if(value2 == cup_from_clid(value1)->rounds->len - 3)
	{
	    if(query_cup_is_international(value1))
		user->counters[COUNT_USER_SUCCESS] +=
		    const_int("int_user_success_international_quarter");
	    else if(query_cup_is_national(value1))
		user->counters[COUNT_USER_SUCCESS] +=
		    const_int("int_user_success_national_quarter");
	}
    }
    else if(type == USER_HISTORY_PROMOTED)
	user->counters[COUNT_USER_SUCCESS] += 
	    const_int("int_user_success_promotion");
    else if(type == USER_HISTORY_RELEGATED)
	user->counters[COUNT_USER_SUCCESS] -= 
	    const_int("int_user_success_relegation");

    his->season = season;
    his->week = week;

    his->type = type;
    his->team_id = team_id;
    his->value1 = value1;
    his->value2 = value2;
    
    if(replace)
    {
	g_string_printf(his->value_string, "%s", string);
	g_array_sort(user->history, (GCompareFunc)user_history_compare);
    }
    else
    {
	his->value_string = g_string_new(string);
	g_array_prepend_val(user->history, *his);
    }
}

/** Write the text corresponding to the history event
    into the buffer. */
void
user_history_to_string(const UserHistory *history, gchar *buf)
{
    gchar buf2[SMALL];

    switch(history->type)
    {
	default:
	    g_warning("user_history_to_string: unknown history type %d.\n", history->type);
	    strcpy(buf, "FIXME!!!");
	case USER_HISTORY_START_GAME:
	    /* Buy a team in a league. */
	    sprintf(buf, _("You start the game with %s in the %s."),
		    team_of_id(history->team_id)->name->str,
		    league_cup_get_name_string(history->value1));
	    break;
    	case USER_HISTORY_FIRE_FINANCES:
	    /* Team fires, team in a league. */
	    sprintf(buf, _("%s fires you because of financial mismanagement.\nYou find a new job with %s in the %s."),
		    team_of_id(history->team_id)->name->str,
		    team_of_id(history->value1)->name->str,
		    league_cup_get_name_string(history->value2));
	    break;
    	case  USER_HISTORY_FIRE_FAILURE:
	    /* Team fires, team in a league. */
	    sprintf(buf, _("%s fires you because of unsuccessfulness.\nYou find a new job with %s in the %s."),
		    team_of_id(history->team_id)->name->str,
		    team_of_id(history->value1)->name->str,
		    league_cup_get_name_string(history->value2));
	    break;
    	case  USER_HISTORY_JOB_OFFER_ACCEPTED:
	    /* Team in a league. Leave team. */
	    sprintf(buf, _("%s offer you a job in the %s.\nYou accept the challenge and leave %s."),
		    team_of_id(history->value1)->name->str,
		    league_cup_get_name_string(history->value2),
		    team_of_id(history->team_id)->name->str);
	    break;
    	case  USER_HISTORY_END_SEASON:
	    /* League name. */
	    sprintf(buf, _("You finish the season in the %s on rank %d."),
		    league_cup_get_name_string(history->value1),
		    history->value2);
	    break;
    	case  USER_HISTORY_PROMOTED:
	    /* League name. */
	    sprintf(buf, _("You get promoted to the %s."),
		    league_cup_get_name_string(history->value1));
	    break;	    
    	case  USER_HISTORY_RELEGATED:
	    /* League name. */
	    sprintf(buf, _("You get relegated to the %s."),
		    league_cup_get_name_string(history->value1));
	    break;	    
    	case  USER_HISTORY_WIN_FINAL:
	    /* Cup name, team name. */
	    sprintf(buf, _("You win the %s final against %s."),
		    league_cup_get_name_string(history->value1),
		    history->value_string->str);
	    break;
    	case  USER_HISTORY_LOSE_FINAL:
	    /* Cup name, team name. */
	    sprintf(buf, _("You lose in the %s final against %s."),
		    league_cup_get_name_string(history->value1),
		    history->value_string->str);
	    break;
    	case USER_HISTORY_REACH_CUP_ROUND:	    
	    cup_get_round_name(cup_from_clid(history->value1), history->value2, buf2);
	    /* Cup round name (e.g. Last 32), number, cup name. */
	    sprintf(buf, _("You reach the %s (round %d) of the %s."), buf2,
		    history->value2 + 1,
		    league_cup_get_name_string(history->value1));
	    break;
    	case USER_HISTORY_CHAMPION:	    
	    /* League name. */
	    sprintf(buf, _("You are champion of the %s!"),
		    league_cup_get_name_string(history->value1));
	    break;
    }
}

/** Find out whether there's a user who
    didn't have his turn before a new week round begins. */
gboolean
query_user_no_turn(void)
{
    gint i;

    for(i=0;i<users->len;i++)
	if(!usr(i).counters[COUNT_USER_TOOK_TURN])
	    return TRUE;

    return FALSE;
}

/** Get a random sponsor for the user. */
UserSponsor
user_get_sponsor(const User *user)
{
    UserSponsor new;
    gint suc_factor = (current_user.counters[COUNT_USER_SUCCESS] < 0) ? -1 : 1;
    gchar *names[18] =
	/* Company name. */
	{_(" Systems"),
	 /* Company name. */
	 _(" Communications"),
	 /* Company name. */
	 _(" Holdings"),
	 /* Company name. */
	 _(" Industries"),
	 /* Company name. */
	 _(" Company"),
	 /* Company name. */
	 _(" Telecommunications"),
	 /* Company name. */
	 _(" Labs"),
	 /* Company name. */
	 _(" Technologies"),
	 /* Company name. */
	 _(" Chemicals"),
	 /* Company name. */
	 _(" Energy"),
	 /* Company name. */
	 _(" Bank"),
	 /* Company name. */
	 _(" Products"),
	 /* Company name. */
	 _(" Software"),
	 /* Company name. */
	 _(" Scientific"),
	 /* Company name. */
	 _(" Financial"),
	 /* Company name. */
	 _(" Petroleum"),
	 /* Company name. */
	 _(" Restaurants"),
	 /* Company name. */
	 _(" Data Systems")};
    gchar *short_names[7] =
	/* Company short name, leading to things like 'Marshall Data Systems Ltd.' */
	{_(" Ltd."),
	 /* Company short name. */
	 _(" Assoc."),
	 /* Company short name. */
	 _(" Co."),
	 /* Company short name. */
	 _(" Ent."),
	 /* Company short name. Copy the '&amp;'. */
	 _(" &amp; Co."),
	 /* Company short name. */
	 _(" Corp."),
	 /* Company short name. */
	 _(" Group")};
    gchar *name_add[3] =
	/* Company addition, leading to 'Marshall & Sons Petroleum Co.'. Copy the '&amp;'. */
	{_(" &amp; Sons"),
	 /* Company addition. Copy the '&amp;'. */
	 _(" &amp; Daughters"),
	/* Company addition. */
	 _(" Bros.")};
   
    new.name = g_string_new(name_get_random_last_name(name_get_list_from_sid(user->tm->names_file->str)));    
    if(math_rnd(0, 1) < 0.2)
	g_string_append(new.name, name_add[math_rndi(0, 2)]);    
    g_string_append(new.name, names[math_rndi(0, 17)]);
    if(math_rnd(0, 1) < 0.7)
	g_string_append(new.name, short_names[math_rndi(0, 6)]);

    new.contract = 4 * math_rndi(9, 36);
    new.benefit = (gint)rint((((const_float("float_sponsor_wage_percentage_upper") -
			       const_float("float_sponsor_wage_percentage_lower")) /
			      (2 * (gfloat)const_int("int_user_success_offer_limit"))) *
			     (gfloat)current_user.counters[COUNT_USER_SUCCESS]
			     + ((const_float("float_sponsor_wage_percentage_upper") -
				 const_float("float_sponsor_wage_percentage_lower")) / 2)) *
			     (1 + suc_factor * 
			      ((gfloat)new.contract / 52 - 1) * const_float("float_sponsor_contract_length_factor")) *
			     (finance_wage_unit(current_user.tm) * current_user.tm->players->len));
    new.benefit = math_round_integer(new.benefit, 2);

    return new;
}

/** Show a list of sponsors for the user to choose from. */
void
user_show_sponsors(void)
{
    gint i, num_offers;
    GArray *sponsors = g_array_new(FALSE, FALSE, sizeof(UserSponsor));
    UserSponsor sponsor;

    if(ABS(current_user.counters[COUNT_USER_SUCCESS]) >= 
       const_int("int_user_success_offer_limit"))
	num_offers = (current_user.counters[COUNT_USER_SUCCESS] < 0) ?
	    const_int("int_sponsor_offers_lower") :
	    const_int("int_sponsor_offers_upper");
    else
	num_offers = (gint)rint(((gfloat)(const_int("int_sponsor_offers_upper") -
					  const_int("int_sponsor_offers_lower")) /
				 (2 * (gfloat)const_int("int_user_success_offer_limit"))) *
				(gfloat)current_user.counters[COUNT_USER_SUCCESS]
				+ ((gfloat)(const_int("int_sponsor_offers_upper") -
					    const_int("int_sponsor_offers_lower")) / 2) + 1);

    num_offers += math_rndi(-1, 1);
    if(num_offers <= 0)
	num_offers = 1;

    for(i=0;i<num_offers;i++)
    {
	sponsor = user_get_sponsor(&current_user);
	g_array_append_val(sponsors, sponsor);
    }
    
    window_create(WINDOW_SPONSORS);

    treeview_show_sponsors(sponsors);

    for(i=0;i<num_offers;i++)
	g_string_free(g_array_index(sponsors, UserSponsor, i).name, TRUE);
    g_array_free(sponsors, TRUE);
}

/** The current sponsor offers to continue. */
void
user_show_sponsor_continue(void)
{
    GArray *sponsors = g_array_new(FALSE, FALSE, sizeof(UserSponsor));
    UserSponsor sponsor;
    gchar buf[SMALL];

    sponsor = user_get_sponsor(&current_user);

    g_string_printf(sponsor.name, current_user.sponsor.name->str);
    
    g_array_append_val(sponsors, sponsor);

    window_create(WINDOW_SPONSORS);

    sprintf(buf, _("Your current sponsor is satisfied with your results and would like to renew the contract. Currently they're paying you %d a week."), current_user.sponsor.benefit);
    gtk_label_set_text(GTK_LABEL(lookup_widget(window.sponsors, "label_sponsors")), buf);

    treeview_show_sponsors(sponsors);

    g_string_free(g_array_index(sponsors, UserSponsor, 0).name, TRUE);
    g_array_free(sponsors, TRUE);

    stat1 = STATUS_SPONSOR_CONTINUE;
}
