#ifndef JSON_SERIALIZE_H
#define JSON_SERIALIZE_H

#include "bygfoot.h"
#include "bet_struct.h"
#include "cup_struct.h"
#include "league_struct.h"
#include "job_struct.h"
#include "transfer_struct.h"
#include "json_compat.h"

json_object *
bygfoot_json_serialize_bygfoot(const Bygfoot *bygfoot);

json_object *
bygfoot_json_serialize_country_list(GPtrArray *country_list);

json_object *
bygfoot_json_serialize_countries(const GPtrArray *countries);

json_object *
bygfoot_json_serialize_country(const Country *country);

/** @name League */
/* @{ */
json_object *
bygfoot_json_serialize_league_array(const GArray *league_array);

json_object *
bygfoot_json_serialize_league(const League *league);

json_object *
bygfoot_json_serialize_prom_rel(PromRel prom_rel);

json_object *
bygfoot_json_serialize_prom_rel_elements(const GArray *garry);

json_object *
bygfoot_json_serialize_prom_rel_element(const PromRelElement *element);

json_object*
bygfoot_json_serialize_prom_games_array(const GArray *garry);

json_object*
bygfoot_json_serialize_prom_games(const PromGames *prom_games);

json_object *
bygfoot_json_serialize_two_match_weeks(GArray * const *two_match_weeks);

json_object *
bygfoot_json_serialize_joined_leagues(const GArray *garray);

json_object *
bygfoot_json_serialize_joined_league(const JoinedLeague *league);

json_object *
bygfoot_json_serialize_new_tables(const GArray *garray);

json_object *
bygfoot_json_serialize_new_table(const NewTable *table);

json_object *
bygfoot_json_serialize_week_breaks(const GArray *garry);

json_object *
bygfoot_json_serialize_week_break(const WeekBreak *week_break);
/* @} */

/** @name User */
/* @{ */
json_object *
bygfoot_json_serialize_users(const GArray *users);

json_object *
bygfoot_json_serialize_user(const User *user);

json_object *
bygfoot_json_serialize_user_bets(GArray * const *bets);

json_object *
bygfoot_json_serialize_user_sponsor(UserSponsor sponsor);

json_object *
bygfoot_json_serialize_user_histories(const GArray *histories);

json_object *
bygfoot_json_serialize_user_history(const UserHistory *history);

json_object *
bygfoot_json_serialize_user_money_in(const gint (*money_in)[5]);

json_object *
bygfoot_json_serialize_user_money_out(const gint (*money_out)[13]);
/* @} */

/** @name Bet */
/* @{ */
json_object *
bygfoot_json_serialize_bet_user(const BetUser *bet_user);

json_object *
bygfoot_json_serialize_bets(GArray **bets);

json_object *
bygfoot_json_serialize_bet_match(const BetMatch *bet_match);

/* @} */

/** @name Team */
/* @{ */
json_object *
bygfoot_json_serialize_teams(const GArray *teams);

json_object *
bygfoot_json_serialize_team_ptrs(GPtrArray *team_ptrs,
                                 GHashTable *fields);
json_object *
bygfoot_json_serialize_team_ptr(const Team *team);

json_object *
bygfoot_json_serialize_team(const Team *team, GHashTable *fields);

json_object *
bygfoot_json_serialize_youth_academy(YouthAcademy youth_academy);
/* @} */


/** @name Player */
/* @{ */
json_object *
bygfoot_json_serialize_players(const GArray *player);

json_object *
bygfoot_json_serialize_player(const Player *player);

json_object *
bygfoot_json_serialize_player_ptr(const Player *player);

json_object *
bygfoot_json_serialize_cards(const GArray *cards);

json_object *
bygfoot_json_serialize_player_card(const PlayerCard *card);

json_object *
bygfoot_json_serialize_games_goals_array(const GArray *games_goals);

json_object *
bygfoot_json_serialize_games_goals(const PlayerGamesGoals *games_goals);

/* @} */

/** @name Cup */
/* @{ */
json_object *
bygfoot_json_serialize_cups(const GArray *garry);

json_object *
bygfoot_json_serialize_cup(const Cup *cup);

json_object *
bygfoot_json_serialize_cup_ptrs(GPtrArray *cups);

json_object *
bygfoot_json_serialize_cup_ptr(const Cup *cup);

json_object *
bygfoot_json_serialize_cup_choose_teams(const GArray *garray);

json_object *
bygfoot_json_serialize_cup_choose_team(const CupChooseTeam *choose_team);

json_object *
bygfoot_json_serialize_cup_round_waits(const GArray *garray);

json_object *
bygfoot_json_serialize_cup_round_wait(const CupRoundWait *wait);
/* @} */

/** @name Transfers */
/* @{ */

json_object *
bygfoot_json_serialize_transfers(const GArray *transfers);

json_object *
bygfoot_json_serialize_transfer(const Transfer *transfer);

json_object *
bygfoot_json_serialize_transfer_offers(const GArray *offers);

json_object *
bygfoot_json_serialize_transfer_offer(const TransferOffer *offer);
/* @} */

/** @name Fixtures */
/* @{ */
json_object*
bygfoot_json_serialize_fixture_ptr(const Fixture *fixture);
/* @} */

/** @name Live Game */
/* @{ */
json_object *
bygfoot_json_serialize_live_game(LiveGame live_game);

json_object *
bygfoot_json_serialize_live_game_team_values(const gfloat (*team_values)[4]);

json_object *
bygfoot_json_serialize_live_game_units(const GArray *units);

json_object *
bygfoot_json_serialize_live_game_unit(const LiveGameUnit *unit);

json_object *
bygfoot_json_serialize_live_game_event(LiveGameEvent event);

json_object *
bygfoot_json_serialize_live_game_stats(LiveGameStats stats);

json_object *
bygfoot_json_serialize_live_game_stats_values(gint (*values)[9]);

json_object *
bygfoot_json_serialize_live_game_stats_players(GPtrArray * (*players)[5]);

json_object *
bygfoot_json_serialize_live_game_team_state_array(LiveGameTeamState *team_states);

json_object *
bygfoot_json_serialize_live_game_team_state(const LiveGameTeamState *team_state);

json_object *
bygfoot_json_serialize_live_game_action_ids(GArray **action_ids);
/* @} */

/** @name Stats */
/* @{ */
json_object*
bygfoot_json_serialize_season_stats(const GArray *stats);

json_object *
bygfoot_json_serialize_season_stat(const SeasonStat *stat);

json_object *
bygfoot_json_serialize_champ_stats(const GArray *stats);

json_object *
bygfoot_json_serialize_champ_stat(const ChampStat *stat);

json_object *
bygfoot_json_serialize_league_stats(const GArray *stats);

json_object *
bygfoot_json_serialize_league_stat(const LeagueStat *stat);

json_object *
bygfoot_json_serialize_stats(const GArray *stats);

json_object *
bygfoot_json_serialize_stat(const Stat *stat);
/* @} */

/** @name Job */
/* @{ */
json_object *
bygfoot_json_serialize_jobs(const GArray *jobs);

json_object *
bygfoot_json_serialize_job(const Job *job);
/* @} */

/** @name Table */
/* @{ */
json_object *
bygfoot_json_serialize_tables(const GArray *garray);

json_object *
bygfoot_json_serialize_table(const Table *table);

json_object *
bygfoot_json_serialize_table_elements(const GArray *garray);

json_object *
bygfoot_json_serialize_table_element(const TableElement *element);
/* @} */

/** @name Fixture */
/* @{ */
json_object *
bygfoot_json_serialize_fixtures(const GArray *garray);

json_object *
bygfoot_json_serialize_fixture(const Fixture *fixture);

json_object *
bygfoot_json_serialize_fixture_teams(Team * const *teams);

json_object *
bygfoot_json_serialize_fixture_result(const gint (*result)[3]);
/* @} */
#endif
