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
YouthAcademy
youth_academy_new(Team *tm)
{
    gint i;
    gint num_of_youths = math_rndi(const_int("int_youth_academy_youths_lower"),
				   const_int("int_youth_academy_youths_upper"));
    YouthAcademy new_academy;

    new_academy.tm = tm;
    new_academy.coach = 
	new_academy.av_coach = QUALITY_AVERAGE;
    new_academy.percentage = 
	new_academy.av_percentage = const_int("int_youth_academy_default_percentage");

    new_academy.counter_youth = math_rnd(const_float("float_youth_academy_youth_counter_lower"),
					 const_float("float_youth_academy_youth_counter_upper"));
    new_academy.players = g_array_new(FALSE, FALSE, sizeof(Player));

    for(i=0;i<num_of_youths;i++)
	youth_academy_add_new_player(&new_academy);

    return new_academy;
}

/** Add a new player to the academy based on the
    average skill value of the user team, the average percentage
    the user paid for the academy for a period of time and the
    average youth couch quality. */
void
youth_academy_add_new_player(YouthAcademy *youth_academy)
{
    gint i;
    gfloat av_skill = team_get_average_skill(youth_academy->tm, FALSE);
    gfloat percentage_coach_skill_factor;
    Player new;

    new.name = name_get(youth_academy->tm->names_file->str);

    new.id = player_id_new;
    new.pos = math_rndi(PLAYER_POS_GOALIE, PLAYER_POS_FORWARD);
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

    /* Argument for the skill factor function, depending on average coach and
	percentage values (weighted). */
    percentage_coach_skill_factor = (4 - youth_academy->av_coach) * 
	(gfloat)const_int("int_youth_academy_max_percentage") * 
	const_float("float_youth_academy_coach_weight") * 0.25 + youth_academy->av_percentage;

    /* Applying the skill factor funtion leading to a factor between
       float_youth_academy_skill_factor_lower and _upper */
    percentage_coach_skill_factor = 
	((const_float("float_youth_academy_skill_factor_upper") -
	  const_float("float_youth_academy_skill_factor_lower")) /
	 ((gfloat)const_int("int_youth_academy_max_percentage") * 
	  (1 + const_float("float_youth_academy_coach_weight")))) *
	percentage_coach_skill_factor + const_float("float_youth_academy_skill_factor_lower");

    new.skill = math_gauss_dist(
	percentage_coach_skill_factor * av_skill * (1 - const_float("float_youth_academy_skill_variance")),
	percentage_coach_skill_factor * av_skill * (1 + const_float("float_youth_academy_skill_variance")));

    new.talent = player_new_talent(new.skill);
    player_estimate_talent(&new);

    /* Reduce skill depending on age. */
    new.skill *= powf(const_float("float_youth_academy_skill_reduce_factor"),
		      new.peak_age - new.age);
    new.skill = CLAMP(new.skill, 0, const_float("float_player_max_skill"));
    new.cskill = new.skill;

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
    
    g_array_append_val(youth_academy->players, new);
}

/** Update the users' youth academies. */
void
youth_academy_update_weekly(void)
{
    gint i, j;
    YouthAcademy *ya = NULL;

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
			       _("Youth %s will be too old for the youth academy soon. Move him to your team or kick him out of the academy. Otherwise he'll probably look for another team to play in."), g_array_index(ya->players, Player, j).name->str);
	    else if(g_array_index(ya->players, Player, j).age > const_float("float_player_age_lower"))
	    {
		user_event_add(&usr(i), EVENT_TYPE_WARNING, -1, -1, NULL, 
			       _("Youth %s thought he's old enough for a real contract and left your youth academy."),
			       g_array_index(ya->players, Player, j).name->str);
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
