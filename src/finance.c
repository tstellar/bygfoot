#include "finance.h"
#include "maths.h"
#include "option.h"
#include "player.h"
#include "user.h"

/** Weekly update of finances. */
void
finance_update_user_weekly(User *user)
{
    gint i;
    Team *tm = user->tm;
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

    for(i=0;i<tm->players->len;i++)
    {
	user->money_out[1][MON_OUT_WAGE] -= player_of(tm, i)->wage;

	if(player_of(tm, i)->health > 0)
	    user->money_out[1][MON_OUT_PHYSIO] -= 
		(gint)(finance_wage_unit(tm) * physio_factor[user->physio % 10]);
    }

    user->money_out[1][MON_OUT_SCOUT] -= (gint)(finance_wage_unit(tm) * scout_factor[user->scout % 10]);

    user->debt = (gint)rint((gfloat)user->debt * const_float("float_finance_interest"));

    for(i=0;i<MON_OUT_END;i++)
	user->money += user->money_out[1][i];
}

/** Return a base value for team finances.
    Scout wages etc. depend on this value. */
gfloat
finance_wage_unit(const Team *tm)
{
    gint i;
    gfloat sum = 0;

    for(i=0;i<tm->players->len;i++)
	sum += player_of(tm, i)->wage;

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
	sum += player_of(tm, i)->value;
    
/*     printf("pl %.0f stad %.0f\n", sum, */
/* 	   (gfloat)tm->stadium.capacity * powf(tm->stadium.safety, */
/* 					       const_float("float_finance_credit_stadium_safety_exponent"))); */

    sum = (sum * const_float("float_finance_credit_player_value_weight")) +
	((gfloat)tm->stadium.capacity * powf(tm->stadium.safety,
					     const_float("float_finance_credit_stadium_safety_exponent"))
	 * (1 - const_float("float_finance_credit_player_value_weight")));

/*     printf(" sum %.0f\n", sum); */

    sum *= (loan) ? const_float("float_finance_credit_factor_loan") :
	const_float("float_finance_credit_factor_drawing");

    return (gint)rint(sum);
}
