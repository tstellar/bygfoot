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

#ifndef TRAINING_
#define TRAINING_

#include "team.h"
#include "training_struct.h"
#include "user.h"

void
calculateTrainingCamp(Team *current_team,
					  gdouble value_training,
                      gdouble value_recreation,
                      gint number_camp);
                      
gfloat
calculate_skill_points(gint value_training,
                       gint number_camp,
                       gfloat age,
                       gfloat fitness,
                       gfloat lsu);
                                
gfloat
calculate_fitness_points(gint value_training,
						 gint value_recreation,
                         gint number_camp,
                         gfloat age);

gint
calculate_recovery_points(gint value_recreation,
                          gint number_camp,
                          gfloat age);
                          
gint
random_int(gint min, gint max);

void
calculateCostsTrainingCamp(gint number_camp);

#endif /*TRAINING_*/
