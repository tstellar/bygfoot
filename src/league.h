/*
   league.h

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

#ifndef LEAGUE_H
#define LEAGUE_H

#include "bygfoot.h"
#include "cup.h"
#include "league_struct.h"
#include "fixture_struct.h"

#define league_cup_get_teams(clid) (clid < ID_CUP_START) ? ((gpointer)(league_from_clid(clid)->teams)) : ((gpointer)(cup_from_clid(clid)->teams))
#define league_cup_get_fixtures(clid) (clid < ID_CUP_START) ? (league_from_clid(clid)->fixtures) : (cup_from_clid(clid)->fixtures)
#define league_cup_get_name_string(clid) (clid < ID_CUP_START) ? league_from_clid(clid)->name : cup_from_clid(clid)->name
#define league_cup_get_yellow_red(clid) (clid < ID_CUP_START) ? (league_from_clid(clid)->yellow_red) : (cup_from_clid(clid)->yellow_red)

#define query_league_has_prom_games(league) (league->prom_rel.prom_games_dest_sid != NULL)

/** A struct needed when managing promotions
    and relegations. */
typedef struct
{
    Team tm; /**< The team we move. */
    GArray *dest_idcs;   /**< Array of possible destination indices. */
    gint prom_rel_type; /**< Promotion or relegation */
    gboolean dest_assigned; /**< Whether a destination league for the team
			       is already assigned. */
} TeamMove;


League
league_new(gboolean new_id);

PromRelElement
prom_rel_element_new(void);

gint
league_cup_get_index_from_clid(gint clid);

League*
league_from_clid(gint clid);

gint
league_cup_get_next_clid(gint clid, gboolean count_inactive);

gint
league_cup_get_previous_clid(gint clid, gboolean count_inactive);

Fixture*
league_cup_get_next_fixture(gint clid, gint week_number, gint week_round_number);

Fixture*
league_cup_get_previous_fixture(gint clid, gint week_number, gint week_round_number);

gint
league_cup_average_capacity(gint clid);

void
league_get_team_movements(League *league, GArray *team_movements);

gint
league_index_from_sid(const gchar *sid);

void
league_remove_team_with_id(League *league, gint id);

void
league_season_start(League *league);

gboolean
query_league_rank_in_prom_games(const League *league, gint rank);

gboolean
query_league_prom_games_begin(const League *league);

gboolean
query_league_matches_in_week(const League *league, gint week_number);

void
league_get_team_movements_prom_rel(const League *league, GArray *team_movements);

void
league_get_team_movements_prom_games(const League *league, GArray *team_movements,
				     const GPtrArray *prom_games_teams, gboolean up);

void
league_team_movements_destinations(GArray *team_movements, const gint *league_size);

void
league_team_movements_prune(GArray *team_movements, const gint *league_size,
			    gint *league_cur_size);

void
league_team_movements_assign_dest(GArray *team_movements, gint idx,
				  const gint *league_size, gint *league_cur_size);

gboolean
query_league_team_movements_unassigned_single(const GArray *team_movements);

gboolean
query_league_team_movements_unassigned(const GArray *team_movements);

void
league_team_movements_print(const GArray *team_movements, 
			    const gint *league_size, const gint *league_cur_size);

gint
league_team_movements_compare_dest_idcs(gconstpointer a, gconstpointer b, 
					gpointer data);

gboolean
query_leagues_active_in_country(void);

#endif
