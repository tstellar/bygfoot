/*
   option_struct.h

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

#ifndef OPTION_STRUCT_H
#define OPTION_STRUCT_H

/** A struct representing an option or a constant. */
typedef struct
{
    gchar *name, *string_value;
    gint value;
} Option;

/** An array of name-value pairs with an associated keyed data list for
    quick access.  */
typedef struct
{
    GArray *list;
    GData *datalist;
    /* Some extra options for faster look up.
     * FIXME: We could possibly have separate options types, since we have
     * different options instances used in this program.
     */
    gfloat float_player_streak_influence_skill;
    gfloat float_player_fitness_exponent;
    gfloat float_player_boost_skill_effect;
    gfloat float_player_team_weight_forward_attack;
    gfloat float_player_team_weight_forward_midfield;
    gfloat float_player_team_weight_forward_defense;
    gfloat float_player_team_weight_midfielder_attack;
    gfloat float_player_team_weight_midfielder_midfield;
    gfloat float_player_team_weight_midfielder_defense;
    gfloat float_player_team_weight_defender_attack;
    gfloat float_player_team_weight_defender_midfield;
    gfloat float_player_team_weight_defender_defense;
    gfloat float_player_fitness_decrease_add;
    gfloat float_player_streak_influence_fitness_decrease;
    gfloat float_player_boost_fitness_effect;
    gfloat float_player_fitness_decrease_factor_goalie;
    gfloat float_player_lsu_update_limit;
    gfloat float_player_streak_prob_zero;
    gfloat float_player_fitness_increase_variance;
    gfloat float_player_fitness_decrease_younger_factor;
    gfloat float_player_max_skill;
    gfloat float_player_skill_update_younger_add;
    gfloat float_player_skill_update_younger_factor;
    gfloat float_player_streak_prob_max;
    gfloat float_player_streak_influence_fitness_increase;
    gfloat float_player_fitness_increase_add;
    gfloat float_player_streak_length_lower;
    gfloat float_player_streak_length_upper;
    gfloat float_player_etal_scout_factor;
    gfloat float_player_fitness_increase_younger_factor;


} OptionList;

#endif
