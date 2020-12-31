
#include <json-c/json.h>
#include <json-c/json_tokener.h>
#include <json-c/json_object.h>
#include "json_interface.h"
#include "json_serialize.h"
#include "user.h"
#include "league_struct.h"
#include "misc.h"
#include "team.h"
#include "option.h"
#include "start_end.h"

static int bygfoot_json_do_commands(Bygfoot *bygfoot, const json_object *commands);
static int bygfoot_json_do_add_user(Bygfoot *bygfoot, const json_object *args);
static struct json_object *bygfoot_json_call_new_bygfoot(Bygfoot *bygfoot, const json_object *args);
static struct json_object *bygfoot_json_call_load_bygfoot(Bygfoot *bygfoot, const json_object *args);
static struct json_object *bygfoot_json_call_add_country(Bygfoot *bygfoot, const json_object *args);
static struct json_object *bygfoot_json_call_add_user(Bygfoot *bygfoot, const json_object *args);
static struct json_object *bygfoot_json_call_start_bygfoot(Bygfoot *bygfoot, const json_object *args);
static struct json_object *bygfoot_json_call_simulate_games(Bygfoot *bygfoot,
                                                      const json_object *args);
static struct json_object *bygfoot_json_call_get_tables(Bygfoot *bygfoot,
                                                        const json_object *args);
static struct json_object *bygfoot_json_call_get_players(Bygfoot *bygfoot,
                                                         const json_object *args);
static struct json_object *bygfoot_json_call_get_fixtures(Bygfoot *bygfoot,
                                                          const json_object *args);
static struct json_object *bygfoot_json_response_error(const char *command,
                                               const char *error);
static json_object *
bygfoot_json_fixture_to_json(const Fixture *fixture);
static json_object * bygfoot_json_player_to_json(const Player *player);
static json_object *bygfoot_json_user_to_json(const User *user);
static json_object *bygfoot_json_table_to_json(const Table *table);
    
struct json_field {
    const gchar *name;
    enum json_type type;
};

int bygfoot_json_main(Bygfoot *bygfoot, const CommandLineArgs *cl_args)
{
    gchar *contents;
    GError *error;
    struct json_object *json;
    enum json_tokener_error json_error;

    if (!g_file_get_contents(cl_args->json_filename, &contents, NULL, &error)) {
        misc_print_error(&error, FALSE);
        return 1;
    }
   
    json = json_tokener_parse_verbose(contents, &json_error);
    if (!json) {
        fprintf(stderr, "Failed to parse json %s:\n", cl_args->json_filename);
        fprintf(stderr, "%s\n",
                json_tokener_error_desc(json_error));
        return 1;
    }

    /* Handle configuration options */
    /* TODO: Get option from json */
    //bygfoot_set_save_dir(bygfoot, NULL);

    json_object_object_foreach(json, key, val) {
    	if (!strcmp("commands", key))
        	return bygfoot_json_do_commands(bygfoot, val);
    }
    fprintf(stderr, "commands key is not present");
    return 1;
}

static int bygfoot_json_do_commands(Bygfoot *bygfoot, const json_object *commands)
{
    size_t i, num_commands;

    static const struct json_func {
        const gchar *command;
        json_object* (*func)(Bygfoot *, const json_object *);
    } json_funcs[] = {
    	{ "new_bygfoot", bygfoot_json_call_new_bygfoot },
        { "load_bygfoot", bygfoot_json_call_load_bygfoot },
        { "get_countries", bygfoot_json_call_get_countries },
        { "add_country", bygfoot_json_call_add_country },
        { "add_user", bygfoot_json_call_add_user },
        { "start_bygfoot", bygfoot_json_call_start_bygfoot },
        { "simulate_games", bygfoot_json_call_simulate_games },
        { "get_tables", bygfoot_json_call_get_tables },
        { "get_players", bygfoot_json_call_get_players },
        { "get_fixtures", bygfoot_json_call_get_fixtures },
        { NULL, NULL}
    };

    if (!json_object_is_type(commands, json_type_array)) {
        fprintf(stderr, "json commands should be in an array\n");
        return 1;
    }

    num_commands = json_object_array_length(commands);
    for (i = 0; i < num_commands; i++) {
        const json_object *command = json_object_array_get_idx(commands, i);
        struct json_object *response = NULL;
        // TODO: CHECK COMMAND SIZE

        json_object_object_foreach(command, key, val) {
            const struct json_func *json_func;
            for (json_func = json_funcs; json_func->command; json_func++) {
                if (!strcmp(json_func->command, key)) {
                    response = json_func->func(bygfoot, val);
                    break;
                }
            }
            
            if (!response) {
                response = bygfoot_json_response_error("commands", "command not recognized");
            }
            fprintf(stderr, "%s\n", json_object_to_json_string_ext(response, JSON_C_TO_STRING_PRETTY));
            json_object_put(response);
        }
    }
    return 0;
}

static const gchar *
bygfoot_json_validate_bygfoot_id(const json_object *args, json_object **error)
{
    const gchar *id;
    /* Get the id field */
    json_object *id_obj = json_object_object_get(args, "id");

    if (id_obj) {
        if(!json_object_is_type(id_obj, json_type_string)) {
            *error = bygfoot_json_response_error("",
                                                 "field 'id' must be a string");
            return NULL;
        }
        return json_object_get_string(id_obj);
    }

    *error = bygfoot_json_response_error("", "field 'id' is missing or NULL");
    return NULL;
}

static struct json_object *
bygfoot_json_call_load_bygfoot(Bygfoot *bygfoot, const json_object *args)
{
    const gchar *id = NULL;
    json_object *error = NULL;
    struct json_object *id_obj, *response, *data, *bygfoot_obj;

    id = bygfoot_json_validate_bygfoot_id(args, &error);
    if (!id) {
        return error;
    }
    bygfoot_load_bygfoot(bygfoot, id);
}

static struct json_object *
bygfoot_json_call_new_bygfoot(Bygfoot *bygfoot, const json_object *args)
{
    const gchar *id = NULL;
    static const struct json_field fields [] = {
        { "id", json_type_string }
    };
    const struct json_field *field;
    struct json_object *id_obj, *response, *data, *bygfoot_obj;

    /* Get the id field */
    id_obj = json_object_object_get(args, "id");
    if (id_obj) {
        if(!json_object_is_type(id_obj, json_type_string)) 
            return bygfoot_json_response_error("new_bygfoot", "field 'id' must be a string");
        id = json_object_get_string(id_obj);
    }
    if (!bygfoot_set_id(bygfoot, id)) {
        return bygfoot_json_response_error("new_bygfoot", "field 'id' must be unique");
    }
    bygfoot_obj = json_object_new_object();
    data = json_object_new_object();
    response = json_object_new_object();
    json_object_object_add(bygfoot_obj, "id",
                           json_object_new_string(bygfoot->id));
    json_object_object_add(data, "bygfoot", bygfoot_obj);
    json_object_object_add(response, "success", data);
    return response;
}

struct json_object *
bygfoot_json_call_get_countries(Bygfoot *bygfoot, const json_object *args)
{
    struct json_object *response = json_object_new_object();
    GPtrArray *country_list = bygfoot_get_country_list(bygfoot);
    json_object_object_add(response, "success",
                           bygfoot_json_serialize_country_list(country_list));
    return response;
}

static json_object *
bygfoot_json_call_add_country(Bygfoot *bygfoot, const json_object *args)
{
    gchar const *country_name = NULL;
    static const struct json_field fields [] = {
        { "name", json_type_string },
        { NULL, json_type_null }
    };
    const struct json_field *field;

    for (field = fields; field->name; field++ ) {
        struct json_object *value = json_object_object_get(args, field->name);
        if (!value)
            continue;
        if (!json_object_is_type(value, field->type))
            continue;
    
        if (!strcmp(field->name, "name"))
            country_name = json_object_get_string(value);
    }

    if (!country_name)
        return bygfoot_json_response_error("add_country", "field 'country' is required");

    bygfoot_load_country(bygfoot, country_name);
    return json_object_new_string("success");
}

static json_object  *bygfoot_json_call_add_user(Bygfoot *bygfoot, const json_object *args)
{
    const char *command = "add_user";
    const char *username = NULL;
    const char *country_name = NULL;
    const char *team_name = NULL;
    struct json_object *response;
    Country *country;
    Team *tm;
    User *user;
    json_object_object_foreach(args, key, val) {
        if (!strcmp("username", key)) {
            if (!json_object_is_type(val, json_type_string)) {
                return bygfoot_json_response_error(command, "username must be of type string");
            }
            username = json_object_get_string(val);
        }
        if (!strcmp("country", key)) {
            if (!json_object_is_type(val, json_type_string)) {
                return bygfoot_json_response_error(command, "country must be of type string");
            }
            country_name = json_object_get_string(val);
        }
        if (!strcmp("team", key)) {
            if (!json_object_is_type(val, json_type_string)) {
                return bygfoot_json_response_error(command, "team must be of type string");
            }
            team_name = json_object_get_string(val);
        }
    }

    if (!username) {
        return bygfoot_json_response_error(command, "field 'username' is required");
    }
    if (!country_name) {
        return bygfoot_json_response_error(command, "field 'country' is required");
    }
    if (!team_name) {
        return bygfoot_json_response_error(command, "field 'team' is required");
    }

    country = bygfoot_load_country(bygfoot, country_name);
    tm = team_of_sid(team_name, country);
    user = bygfoot_add_user(bygfoot, username, tm);

    response = json_object_new_object();
    json_object_object_add(response, "success", bygfoot_json_user_to_json(user));
    return response;
}

static json_object  *bygfoot_json_call_start_bygfoot(Bygfoot *bygfoot,
                                                  const json_object *args)
{
    unsigned i;
    bygfoot_start_game(bygfoot);

    /* FIXME: We should really be doing this when the user is added, but
     * the user options get loaded during the bygfoot_start_bygfoot() call and w
     * can't add options before that.
     */
    for (i = 0; i < users->len; i++) {
        /* We need to set this option to avoid activating the GUI.
         * FIXME: More work is needed to separate the logic from the GUI */	
        option_set_int("int_opt_user_show_live_game", &usr(i).options, 0);
    }
    return json_object_new_string("success");
}

static struct json_object  *bygfoot_json_call_simulate_games(Bygfoot *bygfoot,
                                                      const json_object *args)
{
    int32_t rounds = 0;
    int32_t weeks = 0;
    int32_t seasons = 0;
    unsigned i;
    json_object_object_foreach(args, key, val) {
        if (!strcmp("rounds", key)) {
            rounds = json_object_get_int(val); 
        } else if (!strcmp("weeks", key)) {
            weeks = json_object_get_int(val);
        } else if (!strcmp("seasons", key)) {
            seasons = json_object_get_int(val);
        }
    }

    for (i = 0; i < rounds; i++) {
        end_week_round(bygfoot);
    }
    return json_object_new_string("success");
}

static json_object *
bygfoot_json_call_get_fixtures(Bygfoot *bygfoot, const json_object *args)
{
    struct json_object *fixtures_obj = json_object_new_array();
    struct json_object *response, *data;
    int i;
    for (i = 0; i < country.leagues->len; i++) {
        const League *league = &g_array_index(country.leagues, League, i);
        int j;
        for (j = 0; j < league->fixtures->len; j++) {
            const Fixture *fixture = &g_array_index(league->fixtures, Fixture, j);
            json_object_array_add(fixtures_obj, bygfoot_json_fixture_to_json(fixture));
        }
    }
    for (i = 0; i < country.cups->len; i++) {
        const Cup *cup = &g_array_index(country.cups, Cup, i);
        int j;
        for (j = 0; j < cup->fixtures->len; j++) {
            const Fixture *fixture = &g_array_index(cup->fixtures, Fixture, j);
            json_object_array_add(fixtures_obj, bygfoot_json_fixture_to_json(fixture));
        }
    }

    data = json_object_new_object();
    response = json_object_new_object();

    json_object_object_add(data, "fixtures", fixtures_obj);
    json_object_object_add(response, "success", data);
    return response;
}

static json_object *
bygfoot_json_call_get_tables(Bygfoot *bygfoot, const json_object *args)
{
    gchar const *country_name = NULL;
    static const struct json_field fields [] = {
        { NULL, json_type_null }
    };
    const struct json_field *field;
    int i;
    struct json_object *response, *data, *tables;

    for (field = fields; field->name; field++ ) {
        struct json_object *value = json_object_object_get(args, field->name);
        if (!value)
            continue;
        if (!json_object_is_type(value, field->type))
            continue;
    }

    tables = json_object_new_array();
    //GArray *leagues, *cups;
    for (i = 0; i < country.leagues->len; i++) {
        const League *league = &g_array_index(country.leagues, League, i);
        int j;
        for (j = 0; j < league->tables->len; j++) {
            const Table *table = &g_array_index(league->tables, Table, j);
            json_object_array_add(tables, bygfoot_json_table_to_json(table));
        }
    }
    response = json_object_new_object();
    data = json_object_new_object();
    json_object_object_add(data, "tables", tables);
    json_object_object_add(response, "success", data);
    return response;
}

static json_object *
bygfoot_json_call_get_players(Bygfoot *bygfoot, const json_object *args)
{
    struct json_object *players_obj = json_object_new_array();
    struct json_object *response, *data;
    int i;
    for (i = 0; i < country.leagues->len; i++) {
        const League *league = &g_array_index(country.leagues, League, i);
        int j;
        for (j = 0; j < league->teams->len; j++) {
            const Team *team = &g_array_index(league->teams, Team, j);
            int k;
            for (k = 0; k < team->players->len; k++) {
                const Player *player = &g_array_index(team->players, Player, k);
                json_object_array_add(players_obj, bygfoot_json_player_to_json(player));
            }

        }
    }
    response = json_object_new_object();
    data = json_object_new_object();
    json_object_object_add(data, "players", players_obj);
    json_object_object_add(response, "success", data);
    return response;
}

static json_object *
bygfoot_json_live_game_stats_to_json(const LiveGameStats *stats, gint team_index)
{

}

static json_object *
bygfoot_json_fixture_stats_to_json(const Fixture *fixture, gint team_index)
{
    struct json_object *team_obj = json_object_new_object();
    struct json_object *stats_obj = json_object_new_object();

    static const struct key_index {
        const gchar *key;
        gint index;
    } result_fields[] = {
        { "goals_regulation", 0 },
        { "goals_extra_time", 1 },
        { "goals_penalty_shootout", 2 },
        { NULL, 0 }
    };

    static const struct key_index live_stats_fields[] = {
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

    const struct key_index *iter;

    if (!fixture->live_game)
        return stats_obj;

    for (iter = result_fields; iter->key; iter++) {
        gint value = fixture->result[team_index][iter->index];
        json_object_object_add(stats_obj, iter->key, json_object_new_int64(value));
    }

    /* Note it seems like fixture->live_game does not persist when the game is
     * over, so that is why we are not including it in the json. */
    return stats_obj;
}

static json_object *
bygfoot_json_fixture_to_json(const Fixture *fixture)
{
    struct json_object *fixture_obj = json_object_new_object();
    struct json_object *home_team_obj = json_object_new_object();
    struct json_object *away_team_obj = json_object_new_object();
    json_object_object_add(fixture_obj, "id", json_object_new_uint64(fixture->id));
    json_object_object_add(fixture_obj, "league_id", json_object_new_uint64(fixture->clid));
    json_object_object_add(fixture_obj, "round", json_object_new_int64(fixture->round));
    json_object_object_add(fixture_obj, "replay_number", json_object_new_int64(fixture->replay_number));
    json_object_object_add(fixture_obj, "week", json_object_new_int64(fixture->week_number));
    json_object_object_add(fixture_obj, "round", json_object_new_int64(fixture->week_round_number));

    json_object_object_add(home_team_obj, "id", json_object_new_int64(fixture->team_ids[0]));
    json_object_object_add(home_team_obj, "stats",
                           bygfoot_json_fixture_stats_to_json(fixture, 0));
    json_object_object_add(fixture_obj, "home_team", home_team_obj);

    json_object_object_add(away_team_obj, "id", json_object_new_int64(fixture->team_ids[1]));
    json_object_object_add(away_team_obj, "stats",
                           bygfoot_json_fixture_stats_to_json(fixture, 1));
    json_object_object_add(fixture_obj, "away_team", away_team_obj);

    return fixture_obj;
}

static json_object *
bygfoot_json_player_games_goals_to_json(const GArray *stats)
{
    int i;
    struct json_object *stats_obj = json_object_new_array_ext(stats->len);
    for (i = 0; i < stats->len; i++) {
        const PlayerGamesGoals *league_stats = &g_array_index(stats,
                                                              PlayerGamesGoals, i);
        struct json_object *league_stats_obj = json_object_new_object();
        json_object_object_add(league_stats_obj, "league_id",
                               json_object_new_uint64(league_stats->clid));
        json_object_object_add(league_stats_obj, "games",
                               json_object_new_uint64(league_stats->games));
        json_object_object_add(league_stats_obj, "goals",
                               json_object_new_uint64(league_stats->goals));
        json_object_object_add(league_stats_obj, "shots",
                               json_object_new_uint64(league_stats->shots));
        json_object_array_add(stats_obj, league_stats_obj);
    }
    return stats_obj;
}

static json_object *
bygfoot_json_player_to_json(const Player *player)
{
    struct json_object *player_obj = json_object_new_object();
    json_object_object_add(player_obj, "name", json_object_new_string(player->name));
    json_object_object_add(player_obj, "season_stats",
                           bygfoot_json_player_games_goals_to_json(player->games_goals));

    return player_obj;
}

static json_object *
bygfoot_json_team_to_json(const Team *team)
{
    struct json_object *team_obj = json_object_new_object();

    json_object_object_add(team_obj, "name",
                           json_object_new_string(team->name));
    return team_obj;
}

static json_object *
bygfoot_json_table_element_to_json(const TableElement *element)
{
    struct json_object *element_obj = json_object_new_object();

    json_object_object_add(element_obj, "team",
                           bygfoot_json_team_to_json(element->team));
    json_object_object_add(element_obj, "old_rank",
                           json_object_new_uint64(element->old_rank));
    json_object_object_add(element_obj, "played",
                           json_object_new_uint64(element->values[TABLE_PLAYED]));
    json_object_object_add(element_obj, "won",
                           json_object_new_uint64(element->values[TABLE_WON]));
    json_object_object_add(element_obj, "draw",
                           json_object_new_uint64(element->values[TABLE_DRAW]));
    json_object_object_add(element_obj, "lost",
                           json_object_new_uint64(element->values[TABLE_LOST]));
    json_object_object_add(element_obj, "gf",
                           json_object_new_uint64(element->values[TABLE_GF]));
    json_object_object_add(element_obj, "ga",
                           json_object_new_uint64(element->values[TABLE_GA]));
    json_object_object_add(element_obj, "gd",
                           json_object_new_int64(element->values[TABLE_GD]));
    json_object_object_add(element_obj, "pts",
                           json_object_new_uint64(element->values[TABLE_PTS]));
    return element_obj;
}

static json_object *
bygfoot_json_table_to_json(const Table *table)
{
    struct json_object *table_obj = json_object_new_object();
    struct json_object *elements_obj = json_object_new_array_ext(table->elements->len);
    int i;
    
    json_object_object_add(table_obj, "name", json_object_new_string(table->name));
    json_object_object_add(table_obj, "id", json_object_new_uint64(table->clid));
    json_object_object_add(table_obj, "round", json_object_new_int64(table->round));


    for (i = 0; i < table->elements->len; i++) {
        const TableElement *element = &g_array_index(table->elements,
                                                     TableElement, i);
        json_object_array_add(elements_obj,
                              bygfoot_json_table_element_to_json(element));
    }
    json_object_object_add(table_obj, "elements", elements_obj);
    return table_obj;
}

static json_object *bygfoot_json_user_to_json(const User *user)
{
    struct json_object *json = json_object_new_object();
    struct json_object *user_obj = json_object_new_object();
    json_object_object_add(user_obj, "name", json_object_new_string(user->name));
    json_object_object_add(json, "user", user_obj);
    return json;
}

static struct json_object *bygfoot_json_response_error(const char *command,
                                                       const char *error)
{
    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "error", json_object_new_string(error));
    return json;
}

static void bygfoot_json_error_to_console(const char *command, const char *error)
{
    struct json_object *json = bygfoot_json_response_error(command, error);
    fprintf(stderr, "%s\n", json_object_to_json_string_ext(json, JSON_C_TO_STRING_PRETTY));
    json_object_put(json);
}
