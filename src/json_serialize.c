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
#include "json_serialize.h"
#include "league_struct.h"

static void
serialize_gchar_array_callback(gpointer string, gpointer user_data)
{
    struct json_object *obj = (struct json_object*)user_data;
    json_object_array_add(obj, json_object_new_string(string));
}

static struct json_object*
serialize_gchar_array(GPtrArray* ptr_array)
{
    struct json_object *array_obj = json_object_new_array_ext(ptr_array->len);
    int i;

    g_ptr_array_foreach(ptr_array, serialize_gchar_array_callback, array_obj);

    return array_obj;
}


static GHashTable*
fields_to_hash_table(gchar **fields)
{
    gchar *field;
    GHashTable *hash_table;

    if (!fields)
        return NULL;

    hash_table = g_hash_table_new(g_str_hash, g_str_equal);
    for (field = *fields; field; field++) {
        g_hash_table_add(hash_table, field);
    }
    return hash_table;
}
struct json_object *
bygfoot_json_serialize_country_list(GPtrArray *country_list)
{
    return serialize_gchar_array(country_list);
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

struct json_object *
bygfoot_json_serialize_country(const Country *country)
{
    struct json_object *country_obj = json_object_new_object();

    #define SERIALIZE_COUNTRY_FIELD(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(country_obj, country, field, serialize_func)
    SERIALIZE_COUNTRY_FIELD(name, json_object_new_string);
    SERIALIZE_COUNTRY_FIELD(symbol, json_object_new_string);
    SERIALIZE_COUNTRY_FIELD(sid, json_object_new_string);
    SERIALIZE_COUNTRY_FIELD(rating, json_object_new_int64);
    SERIALIZE_COUNTRY_FIELD(reserve_promotion_rules, json_object_new_int64);
//    SERIALIZE_COUNTRY_FIELD(leagues, bygfoot_json_serialize_league_array);
//    SERIALIZE_COUNTRY_FIELD(cups, bygfoot_json_serialize_cup_array);
//    SERIALIZE_COUNTRY_FIELD(allcups, bygfoot_json_serialize_cup_ptr_array);

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

    #define SERIALIZE_LEAGUE_FIELD(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD(league_obj, league, field, serialize_func);

    SERIALIZE_LEAGUE_FIELD(name, json_object_new_string);
    SERIALIZE_LEAGUE_FIELD(short_name, json_object_new_string);
    SERIALIZE_LEAGUE_FIELD(sid, json_object_new_string);
    SERIALIZE_LEAGUE_FIELD(symbol, json_object_new_string);
    SERIALIZE_LEAGUE_FIELD(names_file, json_object_new_string);
}

struct json_object *
bygfoot_json_serialize_stadium(Stadium stadium, GHashTable *fields)
{
    struct json_object *stadium_obj = json_object_new_object();
    Stadium *stadium_ptr = &stadium;

    #define SERIALIZE_STADIUM_FIELD(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD_FILTER(stadium_obj, stadium_ptr, field, serialize_func, fields);

    SERIALIZE_STADIUM_FIELD(name, json_object_new_string);
    SERIALIZE_STADIUM_FIELD(capacity, json_object_new_int64);
    SERIALIZE_STADIUM_FIELD(average_attendance, json_object_new_int64);
    SERIALIZE_STADIUM_FIELD(possible_attendance, json_object_new_int64);
    SERIALIZE_STADIUM_FIELD(safety, json_object_new_double);
    SERIALIZE_STADIUM_FIELD(ticket_price, json_object_new_double);

    return stadium_obj;
}

struct json_object *
bygfoot_json_serialize_team(const Team *team, GHashTable *fields)
{

    struct json_object *team_obj = json_object_new_object();

    #define SERIALIZE_TEAM_FIELD(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD_FILTER(team_obj, team, field, serialize_func, fields);
    #define SERIALIZE_TEAM_FIELD_STRUCT(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD_STRUCT(team_obj, team, field, serialize_func, fields);

    SERIALIZE_TEAM_FIELD(name, json_object_new_string);
    SERIALIZE_TEAM_FIELD(symbol, json_object_new_string);
    SERIALIZE_TEAM_FIELD(names_file, json_object_new_string);
    SERIALIZE_TEAM_FIELD(def_file, json_object_new_string);
    SERIALIZE_TEAM_FIELD(strategy_sid, json_object_new_string);
    SERIALIZE_TEAM_FIELD(clid, json_object_new_int64);
    SERIALIZE_TEAM_FIELD(id, json_object_new_int64);
    SERIALIZE_TEAM_FIELD(structure, json_object_new_int64);
    SERIALIZE_TEAM_FIELD(style, json_object_new_int64);
    SERIALIZE_TEAM_FIELD(boost, json_object_new_int64);
    SERIALIZE_TEAM_FIELD(average_talent, json_object_new_double);
    SERIALIZE_TEAM_FIELD(luck, json_object_new_double);
    SERIALIZE_TEAM_FIELD_STRUCT(stadium, bygfoot_json_serialize_stadium);
    //SERIALIZE_TEAM_FIELD_STRUCT(players, bygfoot_json_serialize_players);
    SERIALIZE_TEAM_FIELD(first_team_sid, json_object_new_string);
    SERIALIZE_TEAM_FIELD(first_team_id, json_object_new_int64);
    SERIALIZE_TEAM_FIELD(reserve_level, json_object_new_int64);

    return team_obj;
}

static void
serialize_team_pointers_callback(gpointer team, gpointer user_data)
{
    gchar *fields[] = {
        "name",
        "id"
    };
    GHashTable *hash_table = fields_to_hash_table(fields);
    struct json_object *obj = (struct json_object*)user_data;
    json_object_array_add(obj, bygfoot_json_serialize_team((Team*)team, hash_table));
}

struct json_object *
bygfoot_json_serialize_team_ptrs(GPtrArray *team_ptrs, GHashTable *fields)
{
    struct json_object *array_obj = json_object_new_array_ext(team_ptrs->len);

    g_ptr_array_foreach(team_ptrs, serialize_team_pointers_callback, array_obj);

    return array_obj;
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

    SERIALIZE_CUP_ROUND_FIELD(name, json_object_new_string);
    SERIALIZE_CUP_ROUND_FIELD(home_away, json_object_new_boolean);
    SERIALIZE_CUP_ROUND_FIELD(replay, json_object_new_int64);
    SERIALIZE_CUP_ROUND_FIELD(neutral, json_object_new_boolean);
    SERIALIZE_CUP_ROUND_FIELD(randomise_teams, json_object_new_boolean);
    SERIALIZE_CUP_ROUND_FIELD(round_robin_number_of_groups, json_object_new_int64);
    SERIALIZE_CUP_ROUND_FIELD(round_robin_number_of_advance, json_object_new_int64);
    SERIALIZE_CUP_ROUND_FIELD(round_robin_number_of_best_advance, json_object_new_int64);
    SERIALIZE_CUP_ROUND_FIELD(round_robins, json_object_new_int64);
    //SERIALIZE_CUP_ROUND_FIELD(rr_breaks, bygfoot_json_serialize_int_array);
    SERIALIZE_CUP_ROUND_FIELD(new_teams, json_object_new_int64);
    SERIALIZE_CUP_ROUND_FIELD(byes, json_object_new_int64);
    SERIALIZE_CUP_ROUND_FIELD(delay, json_object_new_int64);
    //SERIALIZE_CUP_ROUND_FIELD(two_match_weeks, bygfoot_json_serialize_int_int_array);
    SERIALIZE_CUP_ROUND_FIELD_STRUCT(team_ptrs, bygfoot_json_serialize_team_ptrs);
    //SERIALIZE_CUP_ROUND_FIELD(choose_teams, bygfoot_json_serialize_choose_teams);
    //SERIALIZE_CUP_ROUND_FIELD(tables, bygfoot_json_serialize_tables);
    //SERIALIZE_CUP_ROUND_FIELD(waits, bygfoot_json_seriialize_waits);

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

struct json_object *
bygfoot_json_serialize_cup(const Cup *cup, GHashTable *fields)
{
    struct json_object *cup_obj = json_object_new_object();

    #define SERIALIZE_CUP_FIELD(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD_FILTER(cup_obj, cup, field, serialize_func, fields);
    #define SERIALIZE_CUP_FIELD_STRUCT(field, serialize_func) \
            SERIALIZE_OBJECT_FIELD_STRUCT(cup_obj, cup, field, serialize_func, fields);

    SERIALIZE_CUP_FIELD(name, json_object_new_string);
    SERIALIZE_CUP_FIELD(short_name, json_object_new_string);
    SERIALIZE_CUP_FIELD(symbol, json_object_new_string);
    SERIALIZE_CUP_FIELD(sid, json_object_new_string);
    SERIALIZE_CUP_FIELD(id, json_object_new_int64);
    SERIALIZE_CUP_FIELD(group, json_object_new_int64);
    SERIALIZE_CUP_FIELD(last_week, json_object_new_int64);
    SERIALIZE_CUP_FIELD(week_gap, json_object_new_int64);
    SERIALIZE_CUP_FIELD(add_week, json_object_new_int64);
    SERIALIZE_CUP_FIELD(yellow_red, json_object_new_int64);
    SERIALIZE_CUP_FIELD(talent_diff, json_object_new_double);
    SERIALIZE_CUP_FIELD(next_fixture_update_week, json_object_new_int64);
    SERIALIZE_CUP_FIELD(next_fixture_update_week_round, json_object_new_int64);
    SERIALIZE_CUP_FIELD(properties, serialize_gchar_array);
    SERIALIZE_CUP_FIELD_STRUCT(rounds, bygfoot_json_serialize_cup_rounds);
//    SERIALIZE_CUP_FIELD(bye
    SERIALIZE_CUP_FIELD_STRUCT(teams, bygfoot_json_serialize_team_ptrs);
    SERIALIZE_CUP_FIELD(team_names, serialize_gchar_array);
//    SERIALIZE_CUP_FIELD(fixtures,
//    SERIALIZE_CUP_FIELD(week_breaks
//    SERIALIZE_CUP_FIELD(skip_weeks_with
    return cup_obj;
}
