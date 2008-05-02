/*
  bet_struct.h

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

#include "enums.h"
#include "finance.h"
#include "glib.h"
#include "option.h"
#include "training.h"
#include "player.h" 

void
calculateTrainingCamp(Team *current_team,
		      gdouble value_training,
                      gdouble value_recreation,
                      gint number_camp)
{
    gint count, i;
    Player *player;
    gfloat skill_points=0.0;
    gfloat fitness_points=0.0;
    gint recovery_points=0;
	
    for (count=0; count<current_team->players->len; count++)
    {
	player = &g_array_index(current_team->players, Player, count);
		
	if (((value_training>0) || (value_recreation>0)) && (player->health == 0))
	{
	    skill_points=calculate_skill_points((gint) value_training,
						number_camp,
						player->age,
						player->fitness,
						player->lsu);
												 
	    fitness_points=calculate_fitness_points((gint) value_training,
						    (gint) value_recreation,
						    number_camp,
						    player->age);
            recovery_points=0;             							
	}
	else
	{
	    recovery_points = calculate_recovery_points((gint) value_recreation,
							number_camp,
							player->age);
	    skill_points=0.0;
	    fitness_points=0.0;
	}	
		
	//Set new values	
	if ((player->health > 0) && (player->recovery > 0))
	{
	    //Health & recovery
	    player->recovery -= recovery_points;
	}
	else
	{
	    //Skill
	    if ((player->skill+skill_points) <= (player->talent-2))
	    {	
		player->skill += skill_points;
		player->lsu = 0;
				
		// Functions which are called during a skill-update by player_update_skill() in player.c
		player->cskill = player_get_cskill(player, player->cpos, TRUE);
		player->value = player_assign_value(player);
    
		for(i=0; i<QUALITY_END; i++)
		    if(player->skill > player->etal[i])
		    {
			player_estimate_talent(player);
			break;
		    }
	    }
				
	    //Fitness
	    player->fitness -= fitness_points;
	    if (player->fitness > 1)
		player->fitness = 1;
	    else if (player->fitness < 0)
		player->fitness = 0;
	}
    }
}

gfloat
calculate_skill_points(gint value_training,
                       gint number_camp,
                       gfloat age,
                       gfloat fitness,
                       gfloat lsu)
{
    gint training_points=0;
    gfloat skill_points=0.0;
    gint random=0;
	
    //Calculat training points -> age
    if (age < 20)
	training_points += 4;
    else if (age < 25)
	training_points += 3;
    else if (age < 30)
	training_points += 2;
    else 
	training_points += 1;	
		
    //Calculat training points -> fittness
    if (fitness > 0.75)
	training_points += 4;
    else if (fitness > 0.50)
	training_points += 3;
    else if (fitness > 0.25)
	training_points += 2;
    else 
	training_points += 1;	
		
    //Calculat training points -> lsu
    if (lsu > 15)
	training_points += 4;
    else if (lsu > 10)
	training_points += 3;
    else if (lsu > 5)
	training_points += 2;
    else 
	training_points += 1;	
		
    //Calculate skill points
    random = random_int(1, 3);
    skill_points = training_points - random; 
	
    if (skill_points == 0)
	return skill_points;
    else
    {
	skill_points = skill_points * value_training * number_camp / 100.0;
	return skill_points;
    }
}

gfloat
calculate_fitness_points(gint value_training,
			 gint value_recreation,
                         gint number_camp,
                         gfloat age)
{
    gint training_points=0;
    gfloat fitness_points=0.0;
    gint random=0;
	
    if (value_recreation > 8)
    {
	if (value_recreation == 9)
	{
	    //Calculate fitness points
	    random = random_int(1, 2);
	    fitness_points = ((0.02 * number_camp) + (random / 100.0)) * -1;
	}
	else
	{
	    //Calculate fitness points
	    random = random_int(1, 2);
	    fitness_points = ((0.04 * number_camp) + (random / 100.0)) * -1;
	}
    }
    else
    {
	//Calculat fitness points -> age
	if (age < 20)
	    training_points += 1;
	else if (age < 25)
	    training_points += 2;
	else if (age < 30)
	    training_points += 3;
	else 
	    training_points += 4;	
		
	//Calculat fitness points -> value training
	switch (value_training)
	{
	    case 2: training_points += 1; break;
	    case 3: training_points += 1; break;
	    case 4: training_points += 2; break;
	    case 5: training_points += 2; break;
	    case 6: training_points += 3; break;
	    case 7: training_points += 3; break;
	    case 8: training_points += 4; break;
	    case 9: training_points += 4; break;
	    case 10: training_points += 5; break;
	}
	
	//Calculate fitness points
	random = random_int(1, 2);
	fitness_points = (((gfloat) training_points + random) / number_camp) / 100;
    }
	
    return fitness_points;
	
}                          

gint
calculate_recovery_points(gint value_recreation,
                          gint number_camp,
                          gfloat age)
{
    gint training_points=0;
    gint recovery_points=0;
    gint random=0;
	
    //Calculat recovery points -> age
    if (age < 20)
	training_points += 4;
    else if (age < 25)
	training_points += 3;
    else if (age < 30)
	training_points += 2;
    else 
	training_points += 1;	
	
    //Calculat recovery points -> recreation
    switch (value_recreation)
    {
	case 10: training_points += 4; break;
	case 9: training_points += 3; break;
	case 8: training_points += 2; break;
	case 7: training_points += 1; break;
    }
	
    //Calculate recreation points
    random = random_int(1, 2);
    recovery_points = (training_points + random) * number_camp;
	
    if (recovery_points > 17)
	return 1;
    else
	return 0;	
}

gint
random_int(gint min, gint max)
{
    gint random=0;
    GRand *grand = NULL;
	
    grand = g_rand_new ();
    random = (gint) g_rand_int_range(grand, min, max+1); //random() works until max -1
    return random;
}

void
calculateCostsTrainingCamp(gint number_camp)
{
    gint *money_out = current_user.money_out[0];
    gint *money = &current_user.money;
    gfloat factor = finance_wage_unit(current_user.tm);
	
    gint costs[3] = {factor * const_float("float_training_camp_factor1"),
		     factor * const_float("float_training_camp_factor2"),
		     factor * const_float("float_training_camp_factor2")};

    money_out[MON_OUT_TRAINING_CAMP] -= costs[number_camp - 1];
    *money -= costs[number_camp - 1];
}
