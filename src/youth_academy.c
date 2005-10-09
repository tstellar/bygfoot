#include "free.h"
#include "maths.h"
#include "name.h"
#include "option.h"
#include "player.h"
#include "team.h"
#include "user.h"
#include "youth_academy.h"

/** Set up a youth academy taking the average skill and talent in
    the team into account. */
void
youth_academy_new(User *user)
{
    gint i;
    gint num_of_youths = math_rndi(const_int("int_youth_academy_youths_lower"),
				   const_int("int_youth_academy_youths_upper"));

    user->youth_academy.tm = user->tm;
    user->youth_academy.coach = 
	user->youth_academy.av_coach = QUALITY_AVERAGE;
    user->youth_academy.pos_pref = PLAYER_POS_ANY;
    user->youth_academy.percentage = 
	user->youth_academy.av_percentage = 
	const_int("int_youth_academy_default_percentage");

    user->youth_academy.counter_youth = 
	math_rnd(const_float("float_youth_academy_youth_counter_lower"),
		 const_float("float_youth_academy_youth_counter_upper"));

    free_player_array(&user->youth_academy.players);
    user->youth_academy.players = g_array_new(FALSE, FALSE, sizeof(Player));

    for(i=0;i<num_of_youths;i++)
	youth_academy_add_new_player(&user->youth_academy);
}

/** Add a new player to the academy based on the
    average skill value of the user team, the average percentage
    the user paid for the academy for a period of time and the
    average youth coach quality. */
void
youth_academy_add_new_player(YouthAcademy *youth_academy)
{
    gint i;    
    gfloat pos_probs[4] = 
	{const_float("float_youth_academy_pos_goalie"),
	 const_float("float_youth_academy_pos_defender"),
	 const_float("float_youth_academy_pos_midfielder"),
	 const_float("float_youth_academy_pos_forward")};
    gfloat rndom;
    gfloat av_talent = team_get_average_talent(youth_academy->tm);
    gfloat percentage_coach_talent_factor;
    Player new;

    new.name = name_get(youth_academy->tm->names_file);

    new.id = player_id_new;

    for(i=0;i<4;i++)
    {
	if(i == youth_academy->pos_pref)
	    pos_probs[i] += (const_float("float_youth_academy_coach_search_addition") *
			     (gfloat)(QUALITY_END - youth_academy->coach));
	if(i > 0)
	    pos_probs[i] += pos_probs[i - 1];
    }

    rndom = math_rnd(0, pos_probs[3]);
    for(i=0;i<4;i++)
	if(rndom <= pos_probs[i])
	{
	    new.pos = i;
	    break;
	}

    new.cpos = new.pos;
    new.age = math_rnd(const_float("float_youth_academy_age_lower"),
		       const_float("float_youth_academy_age_upper"));
    new.peak_age =
	math_rnd(const_float("float_player_peak_age_lower") +
		 (new.pos == PLAYER_POS_GOALIE) * 
		 const_float("float_player_peak_age_goalie_addition"),
		 const_float("float_player_peak_age_upper") +
		 (new.pos == PLAYER_POS_GOALIE) * 
		 const_float("float_player_peak_age_goalie_addition"));

    /* Argument for the talent factor function, depending on average coach and
	percentage values (weighted). */
    percentage_coach_talent_factor = (4 - youth_academy->av_coach) * 
	(gfloat)const_int("int_youth_academy_max_percentage") * 
	const_float("float_youth_academy_coach_weight") * 0.25 + 
	youth_academy->av_percentage;

    /* Applying the talent factor funtion leading to a factor between
       float_youth_academy_talent_factor_lower and _upper */
    percentage_coach_talent_factor = 
	((const_float("float_youth_academy_talent_factor_upper") -
	  const_float("float_youth_academy_talent_factor_lower")) /
	 ((gfloat)const_int("int_youth_academy_max_percentage") * 
	  (1 + const_float("float_youth_academy_coach_weight")))) *
	percentage_coach_talent_factor + 
	const_float("float_youth_academy_talent_factor_lower");

    new.talent = math_gauss_dist(
	percentage_coach_talent_factor * av_talent * 
	(1 - const_float("float_youth_academy_talent_variance")),
	percentage_coach_talent_factor * av_talent * 
	(1 + const_float("float_youth_academy_talent_variance")));

    new.talent = CLAMP(new.talent, 0, const_float("float_player_max_skill"));
    new.skill = player_skill_from_talent(&new);
    new.cskill = new.skill;
    player_estimate_talent(&new);

    new.fitness = math_rnd(const_float("float_player_fitness_lower"),
			   const_float("float_player_fitness_upper"));
    new.health = new.recovery = 0;
    new.value = player_assign_value(&new);
    new.wage = player_assign_wage(&new);
    new.contract = math_rnd(const_float("float_player_contract_lower"),
			    const_float("float_player_contract_upper"));
    new.lsu = math_rnd(const_float("float_player_lsu_lower"),
		       const_float("float_player_lsu_upper"));
    new.cards = g_array_new(FALSE, FALSE, sizeof(PlayerCard));
    new.games_goals = g_array_new(FALSE, FALSE, sizeof(PlayerGamesGoals));

    for(i=0;i<PLAYER_VALUE_END;i++)
	new.career[i] = 0;

    new.team = youth_academy->tm;
    new.participation = FALSE;
    new.offers = 0;

    new.streak = PLAYER_STREAK_NONE;
    new.streak_count = new.streak_prob = 0;
    
    g_array_append_val(youth_academy->players, new);
}

/** Update the users' youth academies. */
void
youth_academy_update_weekly(void)
{
    gint i, j;
    YouthAcademy *ya = NULL;

    if(sett_int("int_opt_disable_ya"))
	return;

    for(i=0;i<users->len;i++)
    {
	ya = &usr(i).youth_academy;

	for(j=ya->players->len - 1;j>=0;j--)
	{
	    player_update_weekly(&g_array_index(ya->players, Player, j));
	    
	    if(g_array_index(ya->players, Player, i).fitness < 0.9)
		player_update_fitness(&g_array_index(ya->players, Player, j));
	    else
	    {
		g_array_index(ya->players, Player, j).fitness += math_rnd(-0.05, 0.05);
		g_array_index(ya->players, Player, j).fitness = 
		    MIN(g_array_index(ya->players, Player, j).fitness, 1);
	    }

	    if(g_array_index(ya->players, Player, j).age + 0.1 >
	       const_float("float_player_age_lower") &&
	       g_array_index(ya->players, Player, j).age + 0.08 <=
	       const_float("float_player_age_lower"))
		user_event_add(&usr(i), EVENT_TYPE_WARNING, -1, -1, NULL, 
			       _("Youth %s will be too old for the youth academy soon. Move him to your team or kick him out of the academy. Otherwise he'll probably look for another team to play in."), g_array_index(ya->players, Player, j).name);
	    else if(g_array_index(ya->players, Player, j).age > const_float("float_player_age_lower"))
	    {
		user_event_add(&usr(i), EVENT_TYPE_WARNING, -1, -1, NULL, 
			       _("Youth %s thought he's old enough for a real contract and left your youth academy."),
			       g_array_index(ya->players, Player, j).name);
		free_player(&g_array_index(ya->players, Player, j));
		g_array_remove_index(ya->players, j);
	    }
	}

	ya->av_coach = 
	    (ya->av_coach * const_float("float_youth_academy_average_weight") + 
	     (gfloat)ya->coach) / 
	    (const_float("float_youth_academy_average_weight") + 1);

	ya->av_percentage = 
	    (ya->av_percentage * const_float("float_youth_academy_average_weight") +
	     (gfloat)ya->percentage) /
	    (const_float("float_youth_academy_average_weight") + 1);

	if(ya->percentage > 0)
	{
	    ya->counter_youth -= 
		((1 / (gfloat)const_int("int_youth_academy_max_percentage")) *
		 ya->av_percentage + const_float("float_youth_academy_counter_decrease_min"));

	    if(ya->counter_youth <= 0)
	    {
		ya->counter_youth = math_rnd(const_float("float_youth_academy_youth_counter_lower"),
					     const_float("float_youth_academy_youth_counter_upper"));
		if(ya->players->len < const_int("int_youth_academy_max_youths"))
		{
		    youth_academy_add_new_player(ya);
		    user_event_add(&usr(i), EVENT_TYPE_WARNING, -1, -1, NULL, 
				   _("A new youth registered at your youth academy."));
		}
		else
		    user_event_add(&usr(i), EVENT_TYPE_WARNING, -1, -1, NULL, 
				   _("A new youth wanted to registered at your youth academy but there was no room for him."));
	    }
	}
    }
}
