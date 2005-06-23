#include "callbacks.h"
#include "finance.h"
#include "game_gui.h"
#include "maths.h"
#include "option.h"
#include "player.h"
#include "team.h"
#include "user.h"

/** Weekly update of finances. */
void
finance_update_user_weekly(User *user)
{
    gint i;
    Team *tm = user->tm;
    Team *new_team = NULL;
    gfloat physio_factor[4] =
	{const_float("float_finance_physio_factor1"),
	 const_float("float_finance_physio_factor2"),
	 const_float("float_finance_physio_factor3"),
	 const_float("float_finance_physio_factor4")};
    gfloat scout_factor[4] =
	{const_float("float_finance_scout_factor1"),
	 const_float("float_finance_scout_factor2"),
	 const_float("float_finance_scout_factor3"),
	 const_float("float_finance_scout_factor4")};

    if(opt_int("int_opt_disable_finances"))
	return;

    if(week % 2 == 1)
    {
	for(i=0;i<MON_OUT_END;i++)
	{
	    user->money_out[0][i] = user->money_out[1][i];
	    user->money_out[1][i] = 0;
	}
	for(i=0;i<MON_IN_END;i++)
	{
	    user->money_in[0][i] = user->money_in[1][i];
	    user->money_in[1][i] = 0;
	}
    }

    user->money += user->sponsor.benefit;
    user->money_in[1][MON_IN_SPONSOR] += user->sponsor.benefit;
    user->sponsor.contract = MAX(user->sponsor.contract - 1, 0);

    if(user->counters[COUNT_USER_NEW_SPONSOR] > 1)
	user->counters[COUNT_USER_NEW_SPONSOR]--;
    else if(user->sponsor.contract == 4 && 
	    math_rnd(0, 1) < ((1 - const_float("float_sponsor_continue_prob")) / 
			      (2 * (gfloat)const_int("int_user_success_offer_limit")) *
			      user->counters[COUNT_USER_SUCCESS]) +
	    (1 - (1 - const_float("float_sponsor_continue_prob")) / 2))
	user->counters[COUNT_USER_NEW_SPONSOR] = -1;
    else if(user->sponsor.contract == 0)
	user->counters[COUNT_USER_NEW_SPONSOR] = 1;

    if(query_team_plays(tm, week - 1, 1))
	for(i=0;i<tm->players->len;i++)
	{
	    user->money_out[1][MON_OUT_WAGE] -= g_array_index(tm->players, Player, i).wage;
	    user->money -= g_array_index(tm->players, Player, i).wage;
	    
	    if(player_of_idx_team(tm, i)->health > 0)
	    {
		user->money -= (gint)(finance_wage_unit(tm) * physio_factor[user->physio % 10]);
		user->money_out[1][MON_OUT_PHYSIO] -= 
		    (gint)(finance_wage_unit(tm) * physio_factor[user->physio % 10]);
	    }
	}
    
    user->money_out[1][MON_OUT_SCOUT] -= (gint)(finance_wage_unit(tm) * scout_factor[user->scout % 10]);
    user->money -=  (gint)(finance_wage_unit(tm) * scout_factor[user->scout % 10]);

    user->debt = (gint)rint((gfloat)user->debt * (1 + const_float("float_finance_interest")));

    if(user->money < -finance_team_drawing_credit_loan(user->tm, FALSE) &&
       user->counters[COUNT_USER_POSITIVE] == -1 && debug < 50)
    {
	user->counters[COUNT_USER_OVERDRAWN]++;
	if(user->counters[COUNT_USER_OVERDRAWN] <=
	   const_int("int_finance_overdraw_limit"))
	{
	    user_event_add(user, EVENT_TYPE_OVERDRAW, 
			   user->counters[COUNT_USER_OVERDRAWN], -1, NULL, NULL);
	    user->counters[COUNT_USER_POSITIVE] =
		const_int("int_finance_overdraw_positive");
	}
    }
    else if(user->counters[COUNT_USER_POSITIVE] > -1)
	user->counters[COUNT_USER_POSITIVE]--;	    

    if(user->counters[COUNT_USER_LOAN] > -1)
	user->counters[COUNT_USER_LOAN]--;

    if(user->counters[COUNT_USER_LOAN] == 0 && debug < 50)
	user_event_add(user, EVENT_TYPE_WARNING, -1, -1, NULL, 
		       _("You have to pay back your loan this week."));

    if(user->counters[COUNT_USER_POSITIVE] == 0 && debug < 50)
	user_event_add(user, EVENT_TYPE_WARNING, -1, -1, NULL, 
		       _("Your bank account has to be above your drawing credit limit next week."));

    if(((user->counters[COUNT_USER_LOAN] == -1 && user->debt != 0) ||
       (user->counters[COUNT_USER_POSITIVE] == -1 &&
	user->money < -finance_team_drawing_credit_loan(user->tm, FALSE))) && debug < 50)
    {
	new_team = team_get_new(tm, TRUE);
	user_event_add(user, EVENT_TYPE_FIRE_FINANCE, -1, -1, new_team, NULL);
	user_history_add(user, USER_HISTORY_FIRE_FINANCES, tm->id,
			 new_team->id, new_team->clid, "");
    }
}

/** Return a base value for team finances.
    Scout wages etc. depend on this value. */
gfloat
finance_wage_unit(const Team *tm)
{
    gint i;
    gfloat sum = 0;

    for(i=0;i<tm->players->len;i++)
	sum += player_of_idx_team(tm, i)->wage;

    return sum / (gfloat)tm->players->len;
}

/** Calculate the drawing credit or maximum loan of a team
    based on player values and stadium data. */
gint
finance_team_drawing_credit_loan(const Team *tm, gboolean loan)
{
    gint i;
    gfloat sum = 0;

    for(i=0;i<tm->players->len;i++)
	sum += player_of_idx_team(tm, i)->value;
    
    sum = (sum * const_float("float_finance_credit_player_value_weight")) +
	((gfloat)tm->stadium.capacity * 
	 powf(tm->stadium.safety, const_float("float_finance_credit_stadium_safety_exponent")) *
	 const_float("float_finance_credit_stadium_weight"));

    sum *= (loan) ? const_float("float_finance_credit_factor_loan") :
	const_float("float_finance_credit_factor_drawing");

    return (gint)rint(sum);
}

/** Contract a loan for the current user.
    @param value The amount of money. */
void
finance_get_loan(gint value)
{
    gchar buf[SMALL];

    current_user.money += value;
    current_user.debt -= value;

    current_user.counters[COUNT_USER_LOAN] = (current_user.counters[COUNT_USER_LOAN] == -1) ?
	const_int("int_finance_payback_weeks") : 
	current_user.counters[COUNT_USER_LOAN];    

    sprintf(buf, _("You have %d weeks to pay back your loan."),
	    current_user.counters[COUNT_USER_LOAN]); 

    on_menu_show_finances_activate(NULL, NULL);
    game_gui_print_message(buf);
}


/** Pay back some loan for the current user.
    @param value The amount of money paid back. */
void
finance_pay_loan(gint value)
{
    gchar buf[SMALL];
    gint add = (gint)rint((gfloat)value / (gfloat)(-current_user.debt) * 
			  (gfloat)const_int("int_finance_payback_weeks"));    

    current_user.money -= value;
    current_user.debt += value;

    if(current_user.debt == 0)
    {
	current_user.counters[COUNT_USER_LOAN] = -1;
	strcpy(buf, _("You are free from debt."));
    }
    else
    {
	current_user.counters[COUNT_USER_LOAN] = 
	    MIN(current_user.counters[COUNT_USER_LOAN] + add,
		const_int("int_finance_payback_weeks"));
	sprintf(buf, _("You have %d weeks to pay back the rest of your loan."),
		current_user.counters[COUNT_USER_LOAN]);
    }

    on_menu_show_finances_activate(NULL, NULL);
    game_gui_print_message(buf);
}

/** Return the cost of a stadium improvement.
    @param value The improvement value, either number of new seats
    or safety increase.
    @param capacity Whether seats are built or capacity increased. */
gint
finance_get_stadium_improvement_cost(gfloat value, gboolean capacity)
{
    gfloat return_value;

    if(capacity)
    {
	return_value = finance_wage_unit(current_user.tm) * 
	    (value / (gfloat)const_int("int_stadium_improvement_base_seats"))*
	    const_float("float_stadium_improvement_wage_unit_factor_seats");

	if(value - (gfloat)const_int("int_stadium_improvement_base_seats") >= 
	   const_int("int_stadium_improvement_max_discount_seats"))
	    return_value *= (1 - const_float("float_stadium_improvement_max_discount"));
	else
	    return_value *= (1 - const_float("float_stadium_improvement_max_discount") *
			     ((value - (gfloat)const_int("int_stadium_improvement_base_seats")) / 
			      (gfloat)const_int("int_stadium_improvement_max_discount_seats")));
    }
    else
    {
	return_value = finance_wage_unit(current_user.tm) * 
	    (value / const_float("float_stadium_improvement_base_safety"))*
	    const_float("float_stadium_improvement_wage_unit_factor_safety");

	if(value - const_float("float_stadium_improvement_base_safety") >=
	   const_float("float_stadium_improvement_max_discount_safety"))
	    return_value *= (1 - const_float("float_stadium_improvement_max_discount"));
	else
	    return_value *= (1 - const_float("float_stadium_improvement_max_discount") *
			     ((value - const_float("float_stadium_improvement_base_safety")) / 
			      const_float("float_stadium_improvement_max_discount_safety")));
    }
	
    return (gint)rint(return_value);
}

/** Return the expected duration of a stadium improvement.
    @param value The improvement value, either number of new seats
    or safety increase.
    @param capacity Whether seats are built or capacity increased. */
gint
finance_get_stadium_improvement_duration(gfloat value, gboolean capacity)
{
    gint return_value;

    if(capacity)
	return_value = (gint)((value - 1) /
			      (gfloat)const_int("int_stadium_improvement_base_seats")) + 1;
    else
	return_value = 
	    (gint)(((gint)rint(value * 100) - 1) /
		   (gint)rint(const_float("float_stadium_improvement_base_safety") * 100)) + 1;

    return return_value;
}
