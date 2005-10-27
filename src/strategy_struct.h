/*
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

#ifndef STRATEGY_STRUCT_H
#define STRATEGY_STRUCT_H

/** Lineup types for a CPU team (ie. which players
    are preferred when putting together the first 11). */
enum StratLineupType
{
    STRAT_LINEUP_BEST = 1,
    STRAT_LINEUP_WEAKEST,
    STRAT_LINEUP_FITTEST,
    STRAT_LINEUP_UNFITTEST,
    STRAT_LINEUP_END
};

/** A struct describing the pre-match strategy settings 
    of a CPU team. */
typedef struct
{
    /** A condition describing when the strategy should be applied. */
    gchar *condition;
    /** Array of possible formations, sorted by preference. */
    GArray *formations;
    /** Boost, style values and lineup type. */
    gint boost, style, lineup;
    /** The fitness value below which a player gets substituted
	if there's a sub with better fitness. */
    gfloat min_fitness;
} StrategyPrematch;

typedef struct
{
    /** A condition describing when the action should be taken. */
    gchar *condition, *sub_condition;
    /** New boost and style values. */
    gint boost, style;
    /** Substitution specifiers (position and property).
	Property is taken from #StratLineupType. */
    gint sub_in_pos, sub_in_prop,
	sub_out_pos, sub_out_prop;
    /** An id to prevent actions from being applied again and
	again during a match. */
    gint id;
} StrategyMatchAction;

/** A CPU strategy. */
typedef struct
{
    /** String id and description of the strategy. */
    gchar *sid, *desc;
    /** How often this strategy gets picked, relative
	to the other strategies. */
    gint priority;
    /** Array with prematch settings. */
    GArray *prematch;
    /** Array with match settings. */
    GArray *match_action;
} Strategy;

#endif
