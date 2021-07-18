/** \file json_serialize.c
 *
 *
 * This contains functions for converting Bygfoot objects to json.  The general
 * strategy is to only serialize values that are unchanged during a Bygfoot
 * game.
 *
 * Object members that can change as the result of game play should be
 * queried with functions directly.
 *
 * Parent: Full, Child: Full : Fields == NULL, recursive = TRUE
 * Parent: Full, Child: ID: Fields == NULL, recursive = FALSE
 * Parent: ID: Fields == IDs
 * Child field in filter: IDs
 * Child field in filter, recursive = TRUE: FULL
 *
 */

#include <json-c/json_object.h>
#include <glib.h>
#include "variables.h"
#include "json_serialize.h"
#include "league_struct.h"

static json_object *
serialize_string(const gchar * string)
{
    if (!string)
        return NULL;

    return json_object_new_string(string);
}

static void
serialize_gchar_ptr_array_callback(gpointer string, gpointer user_data)
{
    struct json_object *obj = (struct json_object*)user_data;
    json_object_array_add(obj, serialize_string(string));
}

static struct json_object*
serialize_gchar_ptr_array(GPtrArray* ptr_array)
{
    struct json_object *array_obj = json_object_new_array_ext(ptr_array->len);
    int i;

    g_ptr_array_foreach(ptr_array, serialize_gchar_ptr_array_callback, array_obj);

    return array_obj;
}

static json_object *
serialize_gchar_array(gchar *const *array, gint len)
{
    json_object *array_obj = json_object_new_array_ext(len);
    gint i;

    for (i = 0; i < len; i++) {
        json_object_array_add(array_obj, serialize_string(array[i]));
    }
    return array_obj;
}

static struct json_object *
serialize_int_array(const gint *array, gint len)
{
    json_object *array_obj = json_object_new_array_ext(len);
    gint i;

    for (i = 0; i < len; i++) {
        json_object_array_add(array_obj, json_object_new_int64(array[i]));
    }
    return array_obj;
}

static json_object *
serialize_float_array(const float *array, gint len)
{
    json_object *array_obj = json_object_new_array_ext(len);
    gint i;

    for (i = 0; i < len; i++) {
        json_object_array_add(array_obj, json_object_new_double(array[i]));
    }
    return array_obj;
}

static struct json_object *
serialize_int_garray(GArray *array)
{
    return serialize_int_array((const gint*)array->data, array->len);
}

static GHashTable*
fields_to_hash_table(gchar **fields)
{
    gchar *field;
    GHashTable *hash_table;

    if (!fields)
        return NULL;
    hash_table = g_hash_table_new(g_str_hash, g_str_equal);
    for (field = *fields; field; field = *fields++) {
        g_hash_table_add(hash_table, field);
    }
    return hash_table;
}
struct json_object *
bygfoot_json_serialize_country_list(GPtrArray *country_list)
{
    return serialize_gchar_ptr_array(country_list);
}



#define SERIALIZE_OBJECT_FIELD_FILTER(json_object, object, field, \
                                      serialize_func, field_list) \
    if (!field_list || g_hash_table_contains(field_list, #field)) { \
      json_object_object_add(json_object, #field, serialize_func(object->field)); \
    }

#define SERIALIZE_OBJECT_FIELD_STRUCT(json_object, object, field, \
                                      serialize_func, field_list) \
    if (!field_list || g_hash_table_contains(field_list, #field)) { \
      json_object_object_add(json_object, #field, serialize_func(object->field, field_list)); \
    }

#define SERIALIZE_OBJECT_FIELD(json_object, object, field, serialize_func) \
  SERIALIZE_OBJECT_FIELD_FILTER(json_object, object, field, serialize_func, NULL)

#define SERIALIZE_GARRAY_FUNC_DEF(func, item_type, item_serialize_func) \
    json_object * \
    func(const GArray *garray) \
    { \
        json_object *array = json_object_new_array_ext(garray->len); \
        gint i; \
        for (i = 0; i < garray->len; i++) { \
            const item_type *item = &g_array_index(garray, item_type, i); \
            json_object_array_add(array, item_serialize_func(item)); \
        } \
        return array; \
    }


struct json_object *
bygfoot_json_serialize_bygfoot(const Bygfoot *bygfoot)
{
    struct json_object *bygfoot_obj = json_object_new_object();

    #define SERIALIZE_BYGFOOT_FIELD(field, serialize_func) \
            json_object_object_add(bygfoot_obj, #field, serialize_func(field));

    json_object_object_add(bygfoot_obj, "country", bygfoot_json_serialize_country(&country));
    SERIALIZE_BYGFOOT_FIELD(country_list, bygfoot_json_serialize_countries);
    SERIALIZE_BYGFOOT_FIELD(users, bygfoot_json_serialize_users);
    SERIALIZE_BYGFOOT_FIELD(season, json_object_new_int64);
    SERIALIZE_BYGFOOT_FIELD(week, json_object_new_int64);
    SERIALIZE_BYGFOOT_FIELD(week_round, json_object_new_int64);
    SERIALIZE_BYGFOOT_FIELD(transfer_list, bygfoot_json_serialize_transfers);
    SERIALIZE_BYGFOOT_FIELD(season_stats, bygfoot_json_serialize_season_stats);
    SERIALIZE_BYGFOOT_FIELD(bets, bygfoot_json_serialize_bets);
    SERIALIZE_BYGFOOT_FIELD(current_interest, json_object_new_double);
    SERIALIZE_BYGFOOT_FIELD(jobs, bygfoot_json_serialize_jobs);
    SERIALIZE_BYGFOOT_FIELD(cur_user, json_object_new_int64);
    
    return bygfoot_obj;
}

json_object *
bygfoot_json_serialize_users(const GArray *users)
{
    json_object *users_array = json_object_new_array_ext(users->len);
    gint i;

    for (i = 0 ; i < users->len; i++) {
        const User *user = &g_array_index(users, User, i);
        json_object_array_add(users_array, bygfoot_json_serialize_user(user));
    }
    return users_array;
}

struct json_object*
bygfoot_json_serialize_user(const User *user)
{
    struct json_object *user_obj = json_object_new_object();
    
    #define SERIALIZE_USER_FIELD(field, serialize_func) \
            json_object_object_add(user_obj, #field, serialize_func(user->field));

    SERIALIZE_USER_FIELD(name, serialize_string);
    SERIALIZE_USER_FIELD(tm, bygfoot_json_serialize_team_ptr);
    /* options: I don't think we need to save these, because they appear to
     * be loaded from a file. */
    /* events: I don't think we need to save these, because they appear to
     * be temporary */
    SERIALIZE_USER_FIELD(history, bygfoot_json_serialize_user_histories);
    json_object_object_add(user_obj, "counters", serialize_int_array(user->counters, COUNT_USER_END));
    SERIALIZE_USER_FIELD(money, json_object_new_int64);
    SERIALIZE_USER_FIELD(debt, json_object_new_int64);
    SERIALIZE_USER_FIELD(money_in, bygfoot_json_serialize_user_money_in);
    SERIALIZE_USER_FIELD(money_out, bygfoot_json_serialize_user_money_out);
    SERIALIZE_USER_FIELD(debt_interest, json_object_new_double);
    SERIALIZE_USER_FIELD(alr_start_week, json_object_new_int64);
    SERIALIZE_USER_FIELD(alr_weekly_installment, json_object_new_int64);
    SERIALIZE_USER_FIELD(scout, json_object_new_int64);
    SERIALIZE_USER_FIELD(physio, json_object_new_int64);
    SERIALIZE_USER_FIELD(live_game, bygfoot_json_serialize_live_game);
    SERIALIZE_USER_FIELD(sponsor, bygfoot_json_serialize_user_sponsor);
    SERIALIZE_USER_FIELD(youth_academy, bygfoot_json_serialize_youth_academy);
    SERIALIZE_USER_FIELD(mmatches_file, serialize_string);
    SERIALIZE_USER_FIELD(bets, bygfoot_json_serialize_user_bets);
    SERIALIZE_USER_FIELD(default_team, serialize_int_garray);
    SERIALIZE_USER_FIELD(default_structure, json_object_new_int64);
    SERIALIZE_USER_FIELD(default_style, json_object_new_int64);
    SERIALIZE_USER_FIELD(default_style, json_object_new_int64);
    SERIALIZE_USER_FIELD(default_boost, json_object_new_int64);

    return user_obj;
}

json_object *
bygfoot_json_serialize_user_bets(GArray * const *bets)
{
    json_object *bets_array = json_object_new_array_ext(2);
    gint i;

    for (i = 0; i < 2; i++) {
        const GArray *bets_user = bets[i];
        json_object *bets_user_array = json_object_new_array_ext(bets_user->len);
        json_object_array_add(bets_array, bets_user_array);
        int j;
        for (j = 0; j < bets_user->len; j++) {
            const BetUser *bet_user = &g_array_index(bets_user, BetUser, j);
            json_object_array_add(bets_user_array, bygfoot_json_serialize_bet_user(bet_user));
        }
    }
    return bets_array;
}

json_object *
bygfoot_json_serialize_user_sponsor(UserSponsor sponsor)
{
    json_object *sponsor_obj = json_object_new_object();

    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(sponsor_obj, (&sponsor), field, serialize_func);

    json_object_object_add(sponsor_obj, "name",
                           json_object_new_string_len(sponsor.name->str, sponsor.name->len));
    SERIALIZE(benefit, json_object_new_int64);
    SERIALIZE(contract, json_object_new_int64);
    #undef SERIALIZE

    return sponsor_obj;
}
    
json_object *
bygfoot_json_serialize_user_histories(const GArray *histories)
{
    json_object *history_array = json_object_new_array_ext(histories->len);
    gint i;

    for (i = 0; i < histories->len; i++) {
        const UserHistory *user_history = &g_array_index(histories, UserHistory, i);
        json_object_array_add(history_array,
                              bygfoot_json_serialize_user_history(user_history));
    }
    return history_array;
}

json_object *
bygfoot_json_serialize_user_history(const UserHistory *history)
{
    json_object *history_obj = json_object_new_object();

    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(history_obj, history, field, serialize_func);

    SERIALIZE(season, json_object_new_int64);
    SERIALIZE(week, json_object_new_int64);
    SERIALIZE(type, json_object_new_int64);
    SERIALIZE(team_name, serialize_string);
    json_object_object_add(history_obj, "string", serialize_gchar_array(history->string, 3));
    #undef SERIALIZE

    return history_obj;
}

json_object *
bygfoot_json_serialize_user_money_in(const gint (*money_in)[5])
{
    json_object *money_in_array = json_object_new_array_ext(2);
    gint i;

    for (i = 0; i < 2; i++) {
        json_object_array_add(money_in_array,
                              serialize_int_array(money_in[i], MON_IN_END));
    }
    return money_in_array;
}

json_object *
bygfoot_json_serialize_user_money_out(const gint (*money_out)[13])
{
    json_object *money_out_array = json_object_new_array_ext(2);
    gint i;

    for (i = 0; i < 2; i++) {
        json_object_array_add(money_out_array,
                              serialize_int_array(money_out[i], MON_OUT_END));
    }
    return money_out_array;
}

json_object *
bygfoot_json_serialize_bet_user(const BetUser *bet_user)
{
    json_object *bet_user_obj = json_object_new_object();

    #define SERIALIZE_FIELD(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(bet_user_obj, bet_user, field, serialize_func);

    SERIALIZE_FIELD(fix_id, json_object_new_int64);
    SERIALIZE_FIELD(outcome, json_object_new_int64);
    SERIALIZE_FIELD(wager, json_object_new_int64);
    #undef SERIALIZE_FIELD
}

json_object *
bygfoot_json_serialize_bets(GArray **bets)
{
    json_object *bets_array = json_object_new_array_ext(2);
    gint i;

    for (i = 0; i < 2; i++) {
        GArray *bet_match_array = bets[i];
        json_object *bet_match_array_obj = json_object_new_array_ext(bet_match_array->len);
        json_object_array_add(bets_array, bet_match_array_obj);
        gint j;
        for (j = 0; j < bet_match_array->len; j++) {
            const BetMatch *bet_match = &g_array_index(bet_match_array, BetMatch, j);
            json_object_array_add(bet_match_array_obj,
                                  bygfoot_json_serialize_bet_match(bet_match));
        }
    }
    return bets_array;
}

json_object *
bygfoot_json_serialize_bet_match(const BetMatch *bet_match)
{
    json_object *bet_match_obj = json_object_new_object();

    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(bet_match_obj, bet_match, field, serialize_func);

    SERIALIZE(fix_id, json_object_new_int64);
    json_object_object_add(bet_match_obj, "odds", serialize_float_array(bet_match->odds, 3));
    #undef SERIALIZE

    return bet_match_obj;
}

struct json_object *
bygfoot_json_serialize_countries(const GPtrArray *countries)
{
    struct json_object *countries_obj = json_object_new_array_ext(countries->len);
    gint i;

    for (i = 0; i < countries->len; i++) {
        const Country *country = g_ptr_array_index(countries, i);
        json_object_array_add(countries_obj, bygfoot_json_serialize_country(country));
    }
    return countries_obj;
}

struct json_object *
bygfoot_json_serialize_country(const Country *country)
{
    struct json_object *country_obj = json_object_new_object();

    #define SERIALIZE_COUNTRY_FIELD(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(country_obj, country, field, serialize_func)
    SERIALIZE_COUNTRY_FIELD(name, serialize_string);
    SERIALIZE_COUNTRY_FIELD(symbol, serialize_string);
    SERIALIZE_COUNTRY_FIELD(sid, serialize_string);
    SERIALIZE_COUNTRY_FIELD(rating, json_object_new_int64);
    SERIALIZE_COUNTRY_FIELD(reserve_promotion_rules, json_object_new_int64);
    SERIALIZE_COUNTRY_FIELD(leagues, bygfoot_json_serialize_league_array);
    SERIALIZE_COUNTRY_FIELD(cups, bygfoot_json_serialize_cups);
    SERIALIZE_COUNTRY_FIELD(allcups, bygfoot_json_serialize_cup_ptrs);

    #undef SERIALIZE_COUNTRY_FIELD

    return country_obj;
}

struct json_object *
bygfoot_json_serialize_league_array(const GArray *league_array)
{
    struct json_object *league_array_obj =
            json_object_new_array_ext(league_array->len);
    gint i;
    for (i = 0; i < league_array->len; i++) {
        const League *league = &g_array_index(league_array, League, i);
        json_object_array_add(league_array_obj,
                              bygfoot_json_serialize_league(league));
    }
    return league_array_obj;
}

struct json_object *
bygfoot_json_serialize_league(const League *league)
{
    struct json_object *league_obj = json_object_new_object();

    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(league_obj, league, field, serialize_func);
    #define SERIALIZE_STRUCT(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD_STRUCT(league_obj, league, field, serialize_func, NULL);

    SERIALIZE(name, serialize_string);
    SERIALIZE(short_name, serialize_string);
    SERIALIZE(sid, serialize_string);
    SERIALIZE(symbol, serialize_string);
    SERIALIZE(names_file, serialize_string);
    SERIALIZE(prom_rel, bygfoot_json_serialize_prom_rel);
    SERIALIZE(id, json_object_new_int64);
    SERIALIZE(layer, json_object_new_int64);
    SERIALIZE(first_week, json_object_new_int64);
    SERIALIZE(week_gap, json_object_new_int64);
    SERIALIZE(two_match_weeks, bygfoot_json_serialize_two_match_weeks);
    SERIALIZE(round_robins, json_object_new_int64);
    SERIALIZE(rr_breaks, serialize_int_garray);
    SERIALIZE(yellow_red, json_object_new_int64);
    SERIALIZE(average_talent, json_object_new_double);
    SERIALIZE(teams, bygfoot_json_serialize_teams);
    SERIALIZE(joined_leagues, bygfoot_json_serialize_joined_leagues);
    SERIALIZE(tables, bygfoot_json_serialize_tables);
    SERIALIZE(new_tables, bygfoot_json_serialize_new_tables);
    SERIALIZE(fixtures, bygfoot_json_serialize_fixtures);
    SERIALIZE(properties, serialize_gchar_ptr_array);
    SERIALIZE(week_breaks, bygfoot_json_serialize_week_breaks);
    json_object_object_add(league_obj, "stats", bygfoot_json_serialize_league_stat(&league->stats));
    SERIALIZE(skip_weeks_with, serialize_gchar_ptr_array);
    #undef SERIALIZE

    return league_obj;
}

json_object *
bygfoot_json_serialize_prom_rel(PromRel prom_rel)
{
    json_object *prom_rel_obj = json_object_new_object();

    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(prom_rel_obj, (&prom_rel), field, serialize_func);

    SERIALIZE(elements, bygfoot_json_serialize_prom_rel_elements);
    SERIALIZE(prom_games, bygfoot_json_serialize_prom_games_array);
    #undef SERIALIZE

    return prom_rel_obj;
}

SERIALIZE_GARRAY_FUNC_DEF(bygfoot_json_serialize_prom_rel_elements,
                          PromRelElement,
                          bygfoot_json_serialize_prom_rel_element);

json_object *
bygfoot_json_serialize_prom_rel_element(const PromRelElement* element)
{
    json_object *element_obj = json_object_new_object();
    
    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(element_obj, element, field, serialize_func);

    json_object_object_add(element_obj, "ranks", serialize_int_array(element->ranks, 2));
    SERIALIZE(from_table, json_object_new_int64);
    SERIALIZE(dest_sid, serialize_string);
    SERIALIZE(type, json_object_new_int64);
    SERIALIZE(num_teams, json_object_new_int64);
    #undef SERIALIZE

    return element_obj;
}

SERIALIZE_GARRAY_FUNC_DEF(bygfoot_json_serialize_prom_games_array,
                          PromGames,
                          bygfoot_json_serialize_prom_games);

json_object*
bygfoot_json_serialize_prom_games(const PromGames *prom_games)
{
    json_object *prom_games_obj = json_object_new_object();

    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(prom_games_obj, prom_games, field, serialize_func);

    json_object_object_add(prom_games_obj, "ranks", serialize_int_array(prom_games->ranks, 2));
    SERIALIZE(dest_sid, serialize_string);
    SERIALIZE(loser_sid, serialize_string);
    SERIALIZE(number_of_advance, json_object_new_int64);
    SERIALIZE(cup_sid, serialize_string);
    #undef SERIALIZE

    return prom_games_obj;
}
    
json_object *
bygfoot_json_serialize_two_match_weeks(GArray * const *two_match_weeks)
{
    json_object *two_match_weeks_array = json_object_new_array_ext(2);
    gint i;

    for (i = 0; i < 2; i++) {
        json_object_array_add(two_match_weeks_array,
                              serialize_int_garray(two_match_weeks[i]));
    }
    return two_match_weeks_array;
}

SERIALIZE_GARRAY_FUNC_DEF(bygfoot_json_serialize_joined_leagues,
                          JoinedLeague,
                          bygfoot_json_serialize_joined_league);

json_object *
bygfoot_json_serialize_joined_league(const JoinedLeague *league)
{
    json_object *league_obj = json_object_new_object(); 

    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(league_obj, league, field, serialize_func);

    SERIALIZE(sid, serialize_string);
    SERIALIZE(rr, json_object_new_int64);
    #undef SERIALIZE

    return league_obj;
}

SERIALIZE_GARRAY_FUNC_DEF(bygfoot_json_serialize_new_tables,
                          NewTable,
                          bygfoot_json_serialize_new_table);

json_object *
bygfoot_json_serialize_new_table(const NewTable *table)
{
    json_object *table_obj = json_object_new_object(); 

    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(table_obj, table, field, serialize_func);

    SERIALIZE(add_week, json_object_new_int64);
    SERIALIZE(name, serialize_string);
    #undef SERIALIZE

    return table_obj;
}

SERIALIZE_GARRAY_FUNC_DEF(bygfoot_json_serialize_week_breaks,
                          WeekBreak,
                          bygfoot_json_serialize_week_break);

json_object *
bygfoot_json_serialize_week_break(const WeekBreak *week_break)
{
    json_object *week_break_obj = json_object_new_object();

    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(week_break_obj, week_break, field, serialize_func);

    SERIALIZE(week_number, json_object_new_int64);
    SERIALIZE(length, json_object_new_int64);
    #undef SERIALIZE

    return week_break_obj;
}

struct json_object *
bygfoot_json_serialize_stadium(Stadium stadium, GHashTable *fields)
{
    struct json_object *stadium_obj = json_object_new_object();
    Stadium *stadium_ptr = &stadium;

    #define SERIALIZE_STADIUM_FIELD(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD_FILTER(stadium_obj, stadium_ptr, field, serialize_func, fields);

    SERIALIZE_STADIUM_FIELD(name, serialize_string);
    SERIALIZE_STADIUM_FIELD(capacity, json_object_new_int64);
    SERIALIZE_STADIUM_FIELD(average_attendance, json_object_new_int64);
    SERIALIZE_STADIUM_FIELD(possible_attendance, json_object_new_int64);
    SERIALIZE_STADIUM_FIELD(safety, json_object_new_double);
    SERIALIZE_STADIUM_FIELD(ticket_price, json_object_new_double);

    return stadium_obj;
}

json_object *
bygfoot_json_serialize_teams(const GPtrArray *teams)
{
    json_object *teams_array = json_object_new_array_ext(teams->len);
    gint i;

    for (i = 0; i < teams->len; i++) {
        Team *team = g_ptr_array_index(teams, i);
        json_object_array_add(teams_array, bygfoot_json_serialize_team(team, NULL));
    }
    return teams_array;
}

struct json_object *
bygfoot_json_serialize_team(const Team *team, GHashTable *fields)
{

    struct json_object *team_obj = json_object_new_object();

    #define SERIALIZE_TEAM_FIELD(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD_FILTER(team_obj, team, field, serialize_func, fields);
    #define SERIALIZE_TEAM_FIELD_STRUCT(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD_STRUCT(team_obj, team, field, serialize_func, fields);

    SERIALIZE_TEAM_FIELD(name, serialize_string);
    SERIALIZE_TEAM_FIELD(symbol, serialize_string);
    SERIALIZE_TEAM_FIELD(names_file, serialize_string);
    SERIALIZE_TEAM_FIELD(def_file, serialize_string);
    SERIALIZE_TEAM_FIELD(strategy_sid, serialize_string);
    SERIALIZE_TEAM_FIELD(clid, json_object_new_int64);
    SERIALIZE_TEAM_FIELD(id, json_object_new_int64);
    SERIALIZE_TEAM_FIELD(structure, json_object_new_int64);
    SERIALIZE_TEAM_FIELD(style, json_object_new_int64);
    SERIALIZE_TEAM_FIELD(boost, json_object_new_int64);
    SERIALIZE_TEAM_FIELD(average_talent, json_object_new_double);
    SERIALIZE_TEAM_FIELD(luck, json_object_new_double);
    SERIALIZE_TEAM_FIELD_STRUCT(stadium, bygfoot_json_serialize_stadium);
    SERIALIZE_TEAM_FIELD(players, bygfoot_json_serialize_players);
    SERIALIZE_TEAM_FIELD(first_team_sid, serialize_string);
    SERIALIZE_TEAM_FIELD(first_team_id, json_object_new_int64);
    SERIALIZE_TEAM_FIELD(reserve_level, json_object_new_int64);

    return team_obj;
}

struct json_object *
bygfoot_json_serialize_team_ptrs(GPtrArray *team_ptrs, GHashTable *fields)
{
    struct json_object *teams_array;
    gint i;

    if (!team_ptrs)
        return NULL;

    teams_array = json_object_new_array_ext(team_ptrs->len);

    for (i = 0; i < team_ptrs->len; i++) {
        const Team *team = g_ptr_array_index(team_ptrs, i);
        json_object_array_add(teams_array, bygfoot_json_serialize_team_ptr(team));
    }
    return teams_array;
}


struct json_object *
bygfoot_json_serialize_team_ptr(const Team *team)
{
    gchar *fields[] = {
        "name",
        "id",
	NULL
    };
    GHashTable *hash_table = fields_to_hash_table(fields);
    return bygfoot_json_serialize_team(team, hash_table);
}

json_object *
bygfoot_json_serialize_youth_academy(const YouthAcademy youth_academy)
{
    json_object *youth_academy_obj = json_object_new_object();

    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(youth_academy_obj, (&youth_academy), field, serialize_func)

    SERIALIZE(tm, bygfoot_json_serialize_team_ptr);
    SERIALIZE(pos_pref, json_object_new_int64);
    SERIALIZE(coach, json_object_new_int64);
    SERIALIZE(percentage, json_object_new_int64);
    SERIALIZE(av_coach, json_object_new_double);
    SERIALIZE(av_percentage, json_object_new_double);
    SERIALIZE(counter_youth, json_object_new_double);
    SERIALIZE(players, bygfoot_json_serialize_players);

    #undef SERIALIZE

    return youth_academy_obj;
}

json_object *
bygfoot_json_serialize_players(const GArray *players)
{
    json_object *players_array = json_object_new_array_ext(players->len);
    gint i;

    for (i = 0; i < players->len; i++) {
        const Player *player = &g_array_index(players, Player, i);
        json_object_array_add(players_array, bygfoot_json_serialize_player(player));
    }
    return players_array;
}

json_object *
bygfoot_json_serialize_player(const Player *player)
{
    json_object *player_obj = json_object_new_object();
    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(player_obj, player, field, serialize_func);

    SERIALIZE(name, serialize_string);
    SERIALIZE(pos, json_object_new_int64);
    SERIALIZE(cpos, json_object_new_int64);
    SERIALIZE(health, json_object_new_int64);
    SERIALIZE(recovery, json_object_new_int64);
    SERIALIZE(id, json_object_new_int64);
    SERIALIZE(value, json_object_new_int64);
    SERIALIZE(wage, json_object_new_int64);
    SERIALIZE(offers, json_object_new_int64);
    SERIALIZE(streak, json_object_new_int64);
    SERIALIZE(card_status, json_object_new_int64);
    SERIALIZE(skill, json_object_new_double);
    SERIALIZE(cskill, json_object_new_double);
    SERIALIZE(talent, json_object_new_double);
    json_object_object_add(player_obj, "etal", serialize_float_array(player->etal, QUALITY_END));
    SERIALIZE(fitness, json_object_new_double);
    SERIALIZE(lsu, json_object_new_double);
    SERIALIZE(age, json_object_new_double);
    SERIALIZE(peak_age, json_object_new_double);
    SERIALIZE(peak_region, json_object_new_double);
    SERIALIZE(contract, json_object_new_double);
    SERIALIZE(streak_prob, json_object_new_double);
    SERIALIZE(streak_count, json_object_new_double);
    SERIALIZE(participation, json_object_new_boolean);
    SERIALIZE(games_goals, bygfoot_json_serialize_games_goals_array);
    SERIALIZE(cards, bygfoot_json_serialize_cards);
    json_object_object_add(player_obj, "career", serialize_int_array(player->career, PLAYER_VALUE_END));
    SERIALIZE(team, bygfoot_json_serialize_team_ptr);
    #undef SERIALIZE

    return player_obj;
}

json_object *
bygfoot_json_serialize_player_ptr(const Player *player)
{
    json_object *player_obj = json_object_new_object();
    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(player_obj, player, field, serialize_func);

    SERIALIZE(name, serialize_string);
    SERIALIZE(id, json_object_new_int64);
    #undef SERIALIZE

    return player_obj;
}

json_object *
bygfoot_json_serialize_cards(const GArray *cards)
{
    json_object *cards_array = json_object_new_array_ext(cards->len);
    gint i;

    for (i = 0; i < cards->len; i++) {
        const PlayerCard *card = &g_array_index(cards, PlayerCard, i);
        json_object_array_add(cards_array, bygfoot_json_serialize_player_card(card));
    }
    return cards_array;
}

json_object *
bygfoot_json_serialize_player_card(const PlayerCard *card)
{
    json_object *card_obj = json_object_new_object();

    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(card_obj, card, field, serialize_func);

    SERIALIZE(clid, json_object_new_int64);
    SERIALIZE(yellow, json_object_new_int64);
    SERIALIZE(red, json_object_new_int64);
    #undef SERIALIZE

    return card_obj;
}

json_object *
bygfoot_json_serialize_games_goals_array(const GArray *games_goals)
{
    json_object *games_goals_array = json_object_new_array_ext(games_goals->len);
    gint i;
    for (i = 0; i < games_goals->len; i++) {
        const PlayerGamesGoals *stats = &g_array_index(games_goals,PlayerGamesGoals, i);
        json_object_array_add(games_goals_array, bygfoot_json_serialize_games_goals(stats));
    }
    return games_goals_array;
}

json_object *
bygfoot_json_serialize_games_goals(const PlayerGamesGoals *games_goals)
{
    json_object *games_goals_obj = json_object_new_object();
    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(games_goals_obj, games_goals, field, serialize_func);

    SERIALIZE(clid, json_object_new_int64);
    SERIALIZE(games, json_object_new_int64);
    SERIALIZE(goals, json_object_new_int64);
    SERIALIZE(shots, json_object_new_int64);
    #undef SERIALIZE

    return games_goals_obj;
}

struct json_object *
bygfoot_json_serialize_cup_round(const CupRound *round,
                                 GHashTable *fields)
{
    struct json_object *cup_round_obj = json_object_new_object();

    #define SERIALIZE_CUP_ROUND_FIELD(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD_FILTER(cup_round_obj, round, field, serialize_func, fields);
    #define SERIALIZE_CUP_ROUND_FIELD_STRUCT(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD_STRUCT(cup_round_obj, round, field, serialize_func, fields);

    SERIALIZE_CUP_ROUND_FIELD(name, serialize_string);
    SERIALIZE_CUP_ROUND_FIELD(home_away, json_object_new_boolean);
    SERIALIZE_CUP_ROUND_FIELD(replay, json_object_new_int64);
    SERIALIZE_CUP_ROUND_FIELD(neutral, json_object_new_boolean);
    SERIALIZE_CUP_ROUND_FIELD(randomise_teams, json_object_new_boolean);
    SERIALIZE_CUP_ROUND_FIELD(round_robin_number_of_groups, json_object_new_int64);
    SERIALIZE_CUP_ROUND_FIELD(round_robin_number_of_advance, json_object_new_int64);
    SERIALIZE_CUP_ROUND_FIELD(round_robin_number_of_best_advance, json_object_new_int64);
    SERIALIZE_CUP_ROUND_FIELD(round_robins, json_object_new_int64);
    SERIALIZE_CUP_ROUND_FIELD(rr_breaks, serialize_int_garray);
    SERIALIZE_CUP_ROUND_FIELD(new_teams, json_object_new_int64);
    SERIALIZE_CUP_ROUND_FIELD(byes, json_object_new_int64);
    SERIALIZE_CUP_ROUND_FIELD(delay, json_object_new_int64);
    SERIALIZE_CUP_ROUND_FIELD(two_match_weeks, bygfoot_json_serialize_two_match_weeks);
    SERIALIZE_CUP_ROUND_FIELD_STRUCT(team_ptrs, bygfoot_json_serialize_team_ptrs);
    SERIALIZE_CUP_ROUND_FIELD(choose_teams, bygfoot_json_serialize_cup_choose_teams);
    SERIALIZE_CUP_ROUND_FIELD(tables, bygfoot_json_serialize_tables);
    SERIALIZE_CUP_ROUND_FIELD(waits, bygfoot_json_serialize_cup_round_waits);

    return cup_round_obj;
}

struct json_object *
bygfoot_json_serialize_cup_rounds(const GArray *rounds,
                                  GHashTable *fields)
{
    struct json_object *rounds_array_obj =
            json_object_new_array_ext(rounds->len);
    gint i;
    for (i = 0; i < rounds->len; i++) {
        const CupRound *round = &g_array_index(rounds, CupRound, i);
        json_object_array_add(rounds_array_obj,
                              bygfoot_json_serialize_cup_round(round, fields));
    }
    return rounds_array_obj;
}

SERIALIZE_GARRAY_FUNC_DEF(bygfoot_json_serialize_cups,
                          Cup,
                          bygfoot_json_serialize_cup);

struct json_object *
bygfoot_json_serialize_cup(const Cup *cup)
{
    struct json_object *cup_obj = json_object_new_object();

    #define SERIALIZE_CUP_FIELD(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(cup_obj, cup, field, serialize_func);
    #define SERIALIZE_CUP_FIELD_STRUCT(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD_STRUCT(cup_obj, cup, field, serialize_func, NULL);

    SERIALIZE_CUP_FIELD(name, serialize_string);
    SERIALIZE_CUP_FIELD(short_name, serialize_string);
    SERIALIZE_CUP_FIELD(symbol, serialize_string);
    SERIALIZE_CUP_FIELD(sid, serialize_string);
    SERIALIZE_CUP_FIELD(id, json_object_new_int64);
    SERIALIZE_CUP_FIELD(group, json_object_new_int64);
    SERIALIZE_CUP_FIELD(last_week, json_object_new_int64);
    SERIALIZE_CUP_FIELD(week_gap, json_object_new_int64);
    SERIALIZE_CUP_FIELD(add_week, json_object_new_int64);
    SERIALIZE_CUP_FIELD(yellow_red, json_object_new_int64);
    SERIALIZE_CUP_FIELD(talent_diff, json_object_new_double);
    SERIALIZE_CUP_FIELD(next_fixture_update_week, json_object_new_int64);
    SERIALIZE_CUP_FIELD(next_fixture_update_week_round, json_object_new_int64);
    SERIALIZE_CUP_FIELD(properties, serialize_gchar_ptr_array);
    SERIALIZE_CUP_FIELD_STRUCT(rounds, bygfoot_json_serialize_cup_rounds);
    SERIALIZE_CUP_FIELD_STRUCT(bye, bygfoot_json_serialize_team_ptrs);
    SERIALIZE_CUP_FIELD_STRUCT(teams, bygfoot_json_serialize_team_ptrs);
    SERIALIZE_CUP_FIELD(fixtures, bygfoot_json_serialize_fixtures);
    SERIALIZE_CUP_FIELD(week_breaks, bygfoot_json_serialize_week_breaks);
    SERIALIZE_CUP_FIELD(skip_weeks_with, serialize_gchar_ptr_array);
    SERIALIZE_CUP_FIELD(history, bygfoot_json_serialize_cup_history);
    return cup_obj;
}

json_object *
bygfoot_json_serialize_cup_ptrs(GPtrArray *cups)
{
    struct json_object *cups_array = json_object_new_array_ext(cups->len);
    gint i;

    for (i = 0; i < cups->len; i++) {
        const Cup *cup = g_ptr_array_index(cups, i);
        json_object_array_add(cups_array, bygfoot_json_serialize_cup_ptr(cup));
    }
    return cups_array;
}

json_object *
bygfoot_json_serialize_cup_ptr(const Cup *cup)
{
    struct json_object *cup_obj = json_object_new_object();

    json_object_object_add(cup_obj, "sid", serialize_string(cup->sid));
    json_object_object_add(cup_obj, "id", json_object_new_int64(cup->id));
    return cup_obj;
}

SERIALIZE_GARRAY_FUNC_DEF(bygfoot_json_serialize_cup_choose_teams,
                          CupChooseTeam,
                          bygfoot_json_serialize_cup_choose_team);

json_object *
bygfoot_json_serialize_cup_choose_team(const CupChooseTeam *choose_team)
{
    json_object *choose_team_array = json_object_new_array();
    const CupChooseTeam *iter;

    for (iter = choose_team; iter; iter = iter->next) {
        json_object *choose_team_obj = json_object_new_object();
        json_object_array_add(choose_team_array, choose_team_obj);
        #define SERIALIZE(field, serialize_func) \
                SERIALIZE_OBJECT_FIELD(choose_team_obj, iter, field, serialize_func);

        SERIALIZE(sid, serialize_string);
        SERIALIZE(number_of_teams, json_object_new_int64);
        SERIALIZE(from_table, json_object_new_int64);
        SERIALIZE(start_idx, json_object_new_int64);
        SERIALIZE(end_idx, json_object_new_int64);
        SERIALIZE(randomly, json_object_new_boolean);
        SERIALIZE(generate, json_object_new_boolean);
        SERIALIZE(preload, json_object_new_boolean);
        SERIALIZE(optional, json_object_new_boolean);
        SERIALIZE(previous_season, json_object_new_boolean);
        #undef SERIALIZE
    }
    return choose_team_array;
}

SERIALIZE_GARRAY_FUNC_DEF(bygfoot_json_serialize_cup_round_waits,
                          CupRoundWait,
                          bygfoot_json_serialize_cup_round_wait);

json_object *
bygfoot_json_serialize_cup_round_wait(const CupRoundWait *wait)
{
    json_object *wait_obj = json_object_new_object();
    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(wait_obj, wait, field, serialize_func);

    SERIALIZE(cup_sid, serialize_string);
    SERIALIZE(cup_round, json_object_new_int64);
    #undef SERIALIZE

    return wait_obj;
}

json_object *
bygfoot_json_serialize_cup_history(const GPtrArray *history)
{
    json_object *history_array = json_object_new_array_ext(history->len);
    gint i;

    for (i = 0; i < history->len; i++) {
        GPtrArray *teams = g_ptr_array_index(history, i);
        json_object_array_add(history_array,
                              bygfoot_json_serialize_team_ptrs(teams, NULL));
    }
    return history_array;
}

json_object *
bygfoot_json_serialize_transfers(const GArray *transfers)
{
    json_object *transfers_array = json_object_new_array_ext(transfers->len);
    gint i;

    for (i = 0; i < transfers->len; i++) {
        const Transfer *transfer = &g_array_index(transfers, Transfer, i);
        json_object_array_add(transfers_array,
                              bygfoot_json_serialize_transfer(transfer));
    }
    return transfers_array;
}

struct json_object *
bygfoot_json_serialize_transfer(const Transfer *transfer)
{
    struct json_object *transfer_obj = json_object_new_object();

    #define SERIALIZE_TRANSFER_FIELD(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(transfer_obj, transfer, field, serialize_func);

    SERIALIZE_TRANSFER_FIELD(tm, bygfoot_json_serialize_team_ptr);
    SERIALIZE_TRANSFER_FIELD(id, json_object_new_int64);
    SERIALIZE_TRANSFER_FIELD(time, json_object_new_int64);
    json_object_object_add(transfer_obj, "fee", serialize_int_array(transfer->fee, QUALITY_END));
    json_object_object_add(transfer_obj, "wage", serialize_int_array(transfer->wage, QUALITY_END));
    SERIALIZE_TRANSFER_FIELD(offers, bygfoot_json_serialize_transfer_offers);

    return transfer_obj;
}

struct json_object *
bygfoot_json_serialize_transfer_offers(const GArray *offers)
{
    struct json_object *offers_array_obj =
            json_object_new_array_ext(offers->len);
    gint i;

    for (i = 0; i < offers->len; i++) {
        const TransferOffer *offer = &g_array_index(offers, TransferOffer, i);
        json_object_array_add(offers_array_obj,
                              bygfoot_json_serialize_transfer_offer(offer));
    }
    return offers_array_obj;
}

struct json_object *
bygfoot_json_serialize_transfer_offer(const TransferOffer *offer)
{
    
    struct json_object *offer_obj = json_object_new_object();

    #define SERIALIZE_TRANSFER_OFFER_FIELD(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(offer_obj, offer, field, serialize_func);

    SERIALIZE_TRANSFER_OFFER_FIELD(tm, bygfoot_json_serialize_team_ptr);
    SERIALIZE_TRANSFER_OFFER_FIELD(fee, json_object_new_int64);
    SERIALIZE_TRANSFER_OFFER_FIELD(wage, json_object_new_int64);
    SERIALIZE_TRANSFER_OFFER_FIELD(status, json_object_new_int64);
}

json_object*
bygfoot_json_serialize_fixture_ptr(const Fixture *fixture)
{
    struct json_object *fixture_obj = json_object_new_object();
    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(fixture_obj, fixture, field, serialize_func);

    SERIALIZE(id, json_object_new_int64);
    #undef SERIALIZE

    return fixture_obj;
}

json_object *
bygfoot_json_serialize_live_game(LiveGame live_game)
{
    struct json_object *live_game_obj;

    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(live_game_obj, (&live_game), field, serialize_func);

    SERIALIZE(fix, bygfoot_json_serialize_fixture_ptr);
    SERIALIZE(fix_id, json_object_new_int64);
    json_object_object_add(live_game_obj, "team_names", serialize_gchar_array(live_game.team_names, 2));
    SERIALIZE(attendance, json_object_new_int64);
    json_object_object_add(live_game_obj, "subs_left", serialize_int_array(live_game.subs_left, 2));
    SERIALIZE(started_game, json_object_new_int64);
    SERIALIZE(stadium_event, json_object_new_int64);
    SERIALIZE(team_values, bygfoot_json_serialize_live_game_team_values);
    SERIALIZE(home_advantage, json_object_new_double);
    SERIALIZE(units, bygfoot_json_serialize_live_game_units);
    SERIALIZE(stats, bygfoot_json_serialize_live_game_stats);
    SERIALIZE(team_state, bygfoot_json_serialize_live_game_team_state_array);
    SERIALIZE(action_ids, bygfoot_json_serialize_live_game_action_ids);
    #undef SERIALIZE

    return live_game_obj;
}

json_object *
bygfoot_json_serialize_live_game_team_values(const gfloat (*team_values)[4])
{
    json_object *team_values_array = json_object_new_array_ext(2);
    gint i;

    for (i = 0; i < 2; i++) {
        const float *values = team_values[i];
        json_object_array_add(team_values_array, serialize_float_array(values, GAME_TEAM_VALUE_END));
    }
    return team_values_array;
}

json_object *
bygfoot_json_serialize_live_game_units(const GArray *units)
{
    json_object *units_array = json_object_new_array_ext(units->len);
    gint i;

    for (i = 0; i < units->len; i++) {
        const LiveGameUnit *unit = &g_array_index(units, LiveGameUnit, i);
        json_object_array_add(units_array, bygfoot_json_serialize_live_game_unit(unit));
    }
    return units_array;
}

json_object *
bygfoot_json_serialize_live_game_unit(const LiveGameUnit *unit)
{
    json_object *unit_obj = json_object_new_object();

    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(unit_obj, unit, field, serialize_func);

    SERIALIZE(possession, json_object_new_int64);
    SERIALIZE(area, json_object_new_int64);
    SERIALIZE(minute, json_object_new_int64);
    SERIALIZE(time, json_object_new_int64);
    json_object_object_add(unit_obj, "result", serialize_int_array(unit->result, 2));
    SERIALIZE(event, bygfoot_json_serialize_live_game_event);
    #undef SERIALIZE

    return unit_obj;
}

json_object *
bygfoot_json_serialize_live_game_event(LiveGameEvent event)
{
    json_object *event_obj = json_object_new_object();

    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(event_obj, (&event), field, serialize_func);

    SERIALIZE(type, json_object_new_int64);
    SERIALIZE(verbosity, json_object_new_int64);
    SERIALIZE(team, json_object_new_int64);
    SERIALIZE(player, json_object_new_int64);
    SERIALIZE(player2, json_object_new_int64);
    SERIALIZE(commentary, serialize_string);
    SERIALIZE(commentary_id, json_object_new_int64);
    #undef SERIALIZE

    return event_obj;

}

json_object *
bygfoot_json_serialize_live_game_stats(LiveGameStats stats)
{
    json_object *stats_obj = json_object_new_object();

    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(stats_obj, (&stats), field, serialize_func);

    SERIALIZE(possession, json_object_new_double);
    SERIALIZE(values, bygfoot_json_serialize_live_game_stats_values);
    SERIALIZE(players, bygfoot_json_serialize_live_game_stats_players);
    #undef SERIALIZE

    return stats_obj;
}

json_object *
bygfoot_json_serialize_live_game_stats_values(gint (*values)[9])
{
    static const struct key_index {
        const gchar *key;
        gint index;
    } value_fields[] = {
        { "goals_regular", LIVE_GAME_STAT_VALUE_GOALS_REGULAR },
        { "shots", LIVE_GAME_STAT_VALUE_SHOTS },
        { "shot_percentage", LIVE_GAME_STAT_VALUE_SHOT_PERCENTAGE },
        { "possession", LIVE_GAME_STAT_VALUE_POSSESSION },
        { "penalties", LIVE_GAME_STAT_VALUE_PENALTIES },
        { "fouls", LIVE_GAME_STAT_VALUE_FOULS },
        { "cards", LIVE_GAME_STAT_VALUE_CARDS },
        { "reds", LIVE_GAME_STAT_VALUE_REDS } ,
        { "injuries", LIVE_GAME_STAT_VALUE_INJURIES },
        { NULL, LIVE_GAME_STAT_VALUE_END }
    };

    json_object *values_array = json_object_new_array_ext(2);
    gint i;

    for (i = 0; i < 2; i++) {
        const struct key_index *iter;
        json_object *value_obj = json_object_new_object();
        json_object_array_add(values_array, value_obj);
        for (iter = value_fields; iter; iter++) {
            json_object_object_add(value_obj, iter->key, json_object_new_int64(values[i][iter->index]));
        }
    }
    return values_array;
}

json_object *
bygfoot_json_serialize_live_game_stats_players(GPtrArray* (*players)[5])
{

    json_object *players_array = json_object_new_array_ext(2);
    gint i;

    for (i = 0; i < 2; i ++) {
        json_object *stats_array = json_object_new_array_ext(LIVE_GAME_STAT_ARRAY_END);
        gint j;
        json_object_array_add(players_array, stats_array);
        for (j = 0; j < LIVE_GAME_STAT_ARRAY_END; j++) {
            const GPtrArray *player_stats = players[i][j];
            gint k;
            for (k = 0; k < player_stats->len; k++) {
                const Player *player = g_ptr_array_index(player_stats, k);
                json_object_array_add(stats_array, bygfoot_json_serialize_player_ptr(player));
            }
        }
    }
    return players_array;
}

json_object *
bygfoot_json_serialize_live_game_team_state_array(LiveGameTeamState *team_states)
{
    json_object *team_state_array = json_object_new_array_ext(2);
    gint i;

    for (i = 0; i < 2; i++) {
        const LiveGameTeamState *team_state = team_states + i;
        json_object_array_add(team_state_array,
                              bygfoot_json_serialize_live_game_team_state(team_state));
    }
    return team_state_array;
}

json_object *
bygfoot_json_serialize_live_game_team_state(const LiveGameTeamState *team_state)
{
    json_object *team_state_obj = json_object_new_object();

    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(team_state_obj, team_state, field, serialize_func);

    SERIALIZE(structure, json_object_new_int64);
    SERIALIZE(style, json_object_new_int64);
    SERIALIZE(boost, json_object_new_boolean);
    json_object_object_add(team_state_obj, "player_ids", serialize_int_array(team_state->player_ids, 11));
    #undef SERIALIZE

    return team_state_obj;
}

json_object *
bygfoot_json_serialize_live_game_action_ids(GArray **action_ids)
{
    json_object *action_ids_array = json_object_new_array_ext(2);
    gint i;

    for (i = 0; i < 2; i++) {
        json_object_array_add(action_ids_array, serialize_int_garray(action_ids[i]));
    }
    return action_ids_array;
}
    
json_object*
bygfoot_json_serialize_season_stats(const GArray *stats)
{
    json_object *stats_array = json_object_new_array_ext(stats->len);
    gint i;

    for (i = 0; i < stats->len; i++) {
        SeasonStat *stat = &g_array_index(stats, SeasonStat, i);
        json_object_array_add(stats_array, bygfoot_json_serialize_season_stat(stat));
    }
    return stats_array;
}

json_object *
bygfoot_json_serialize_season_stat(const SeasonStat *stat)
{
    json_object *stat_obj = json_object_new_object();

    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(stat_obj, stat, field, serialize_func);

    SERIALIZE(season_number, json_object_new_int64);
    SERIALIZE(league_champs, bygfoot_json_serialize_champ_stats);
    SERIALIZE(cup_champs, bygfoot_json_serialize_champ_stats);
    SERIALIZE(league_stats, bygfoot_json_serialize_league_stats);
    #undef SERIALIZE

    return stat_obj;
}

json_object *
bygfoot_json_serialize_champ_stats(const GArray *stats)
{
    json_object *stats_array = json_object_new_array_ext(stats->len);
    gint i;

    for (i = 0 ; i < stats->len; i++) {
        const ChampStat *stat = &g_array_index(stats, ChampStat, i);
        json_object_array_add(stats_array, bygfoot_json_serialize_champ_stat(stat));
    }
    return stats_array;
}

json_object *
bygfoot_json_serialize_champ_stat(const ChampStat *stat)
{
    json_object *stat_obj = json_object_new_object();

    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(stat_obj, stat, field, serialize_func);

    SERIALIZE(team_name, serialize_string);
    SERIALIZE(cl_name, serialize_string);
    #undef SERIALIZE

    return stat_obj;
}
    
json_object *
bygfoot_json_serialize_league_stats(const GArray *stats)
{
    json_object *stats_array = json_object_new_array_ext(stats->len);
    gint i;

    for (i = 0; i < stats->len; i++) {
        const LeagueStat *stat = &g_array_index(stats, LeagueStat, i);
        json_object_array_add(stats_array, bygfoot_json_serialize_league_stat(stat));
    }
    return stats_array;
}

json_object *
bygfoot_json_serialize_league_stat(const LeagueStat *stat)
{
    json_object *stat_obj = json_object_new_object();
    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(stat_obj, stat, field, serialize_func);

    SERIALIZE(league_symbol, serialize_string);
    SERIALIZE(league_name, serialize_string);
    SERIALIZE(teams_off, bygfoot_json_serialize_stats);
    SERIALIZE(teams_def, bygfoot_json_serialize_stats);
    SERIALIZE(player_scorers, bygfoot_json_serialize_stats);
    SERIALIZE(player_goalies, bygfoot_json_serialize_stats);
    #undef SERIALIZE

    return stat_obj;
}

json_object *
bygfoot_json_serialize_stats(const GArray *stats)
{
    json_object *stats_array = json_object_new_array_ext(stats->len);
    gint i;

    for (i = 0; i < stats->len; i++) {
        const Stat *stat = &g_array_index(stats, Stat, i);
        json_object_array_add(stats_array, bygfoot_json_serialize_stat(stat));
    }
    return stats_array;
}

json_object *
bygfoot_json_serialize_stat(const Stat *stat)
{
    json_object *stat_obj = json_object_new_object();

    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(stat_obj, stat, field, serialize_func);

    SERIALIZE(team_name, serialize_string);
    SERIALIZE(value1, json_object_new_int64);
    SERIALIZE(value2, json_object_new_int64);
    SERIALIZE(value3, json_object_new_int64);
    SERIALIZE(value_string, serialize_string);
    #undef SERIALIZE

    return stat_obj;
}
    
json_object *
bygfoot_json_serialize_jobs(const GArray *jobs)
{
    json_object *jobs_array = json_object_new_array_ext(jobs->len);
    gint i;

    for (i = 0; i < jobs->len; i++) {
        const Job *job = &g_array_index(jobs, Job, i);
        json_object_array_add(jobs_array, bygfoot_json_serialize_job(job));
    }
    return jobs_array;
}

json_object *
bygfoot_json_serialize_job(const Job *job)
{
    json_object *job_obj = json_object_new_object();
    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(job_obj, job, field, serialize_func);

    SERIALIZE(type, json_object_new_int64);
    SERIALIZE(time, json_object_new_int64);
    SERIALIZE(country_file, serialize_string);
    SERIALIZE(country_name, serialize_string);
    SERIALIZE(league_name, serialize_string);
    SERIALIZE(league_layer, json_object_new_int64);
    SERIALIZE(country_rating, json_object_new_int64);
    SERIALIZE(talent_percent, json_object_new_int64);
    SERIALIZE(team_id, json_object_new_int64);
    #undef SERIALIZE

    return job_obj;
}

SERIALIZE_GARRAY_FUNC_DEF(bygfoot_json_serialize_tables,
                          Table,
                          bygfoot_json_serialize_table);

json_object *
bygfoot_json_serialize_table(const Table *table)
{
    json_object *table_obj = json_object_new_object();

    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(table_obj, table, field, serialize_func);

    SERIALIZE(name, serialize_string);
    SERIALIZE(clid, json_object_new_int64);
    SERIALIZE(round, json_object_new_int64);
    SERIALIZE(elements, bygfoot_json_serialize_table_elements);
    #undef SERIALIZE

    return table_obj;
}

SERIALIZE_GARRAY_FUNC_DEF(bygfoot_json_serialize_table_elements,
                          TableElement,
                          bygfoot_json_serialize_table_element);

json_object *
bygfoot_json_serialize_table_element(const TableElement *element)
{
    json_object *element_obj = json_object_new_object();

    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(element_obj, element, field, serialize_func);

    SERIALIZE(team, bygfoot_json_serialize_team_ptr);
    SERIALIZE(old_rank, json_object_new_int64);
    json_object_object_add(element_obj, "played",
                           json_object_new_int64(element->values[TABLE_PLAYED]));
    json_object_object_add(element_obj, "won",
                           json_object_new_int64(element->values[TABLE_WON]));
    json_object_object_add(element_obj, "draw",
                           json_object_new_int64(element->values[TABLE_DRAW]));
    json_object_object_add(element_obj, "lost",
                           json_object_new_int64(element->values[TABLE_LOST]));
    json_object_object_add(element_obj, "gf",
                           json_object_new_int64(element->values[TABLE_GF]));
    json_object_object_add(element_obj, "ga",
                           json_object_new_int64(element->values[TABLE_GA]));
    json_object_object_add(element_obj, "gd",
                           json_object_new_int64(element->values[TABLE_GD]));
    json_object_object_add(element_obj, "pts",
                           json_object_new_int64(element->values[TABLE_PTS]));
    #undef SERIALIZE

    return element_obj;
}

SERIALIZE_GARRAY_FUNC_DEF(bygfoot_json_serialize_fixtures,
                          Fixture,
                          bygfoot_json_serialize_fixture);

json_object *
bygfoot_json_serialize_fixture(const Fixture *fixture)
{
    json_object *fixture_obj = json_object_new_object();

    #define SERIALIZE(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(fixture_obj, fixture, field, serialize_func);

    SERIALIZE(clid, json_object_new_int64);
    SERIALIZE(id, json_object_new_int64);
    SERIALIZE(round, json_object_new_int64);
    SERIALIZE(replay_number, json_object_new_int64);
    SERIALIZE(week_number, json_object_new_int64);
    SERIALIZE(week_round_number, json_object_new_int64);
    SERIALIZE(teams, bygfoot_json_serialize_fixture_teams);
    SERIALIZE(result, bygfoot_json_serialize_fixture_result);
    SERIALIZE(home_advantage, json_object_new_boolean);
    SERIALIZE(second_leg, json_object_new_boolean);
    SERIALIZE(decisive, json_object_new_boolean);
    /* live_game not serialized, because it is temporary */
    #undef SERIALIZE

    return fixture_obj;
}

json_object *
bygfoot_json_serialize_fixture_teams(Team * const *teams)
{
    json_object *teams_array = json_object_new_array_ext(2);
    gint i;

    for (i = 0; i < 2; i++) {
        const Team *team = teams[i];
        json_object_array_add(teams_array, bygfoot_json_serialize_team_ptr(team));
    }
    return teams_array;
}

json_object *
bygfoot_json_serialize_fixture_result(const gint (*result)[3])
{
    json_object *result_array = json_object_new_array_ext(2);
    gint i;

    for (i = 0; i < 2; i++) {
        static const struct key_index {
            const gchar *key;
            gint index;
        } result_fields[] = {
            { "goals_regulation", 0 },
            { "goals_extra_time", 1 },
            { "goals_penalty_shootout", 2 },
            { NULL, 0 }
        };
        const struct key_index *iter;
        json_object *result_obj = json_object_new_object();
        json_object_array_add(result_array, result_obj);
        for (iter = result_fields; iter->key; iter++) {
            gint value = result[i][iter->index];
            json_object_object_add(result_obj, iter->key, json_object_new_int64(value));
        }
    }
    return result_array;
}
