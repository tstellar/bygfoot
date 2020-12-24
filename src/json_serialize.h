#ifndef JSON_SERIALIZE_H
#define JSON_SERIALIZE_H

#include "bygfoot.h"
#include "cup_struct.h"
#include "league_struct.h"
#include "json_compat.h"

struct json_object *
bygfoot_json_serialize_country_list(GPtrArray *country_list);

struct json_object *
bygfoot_json_serialize_country(const Country *country);

struct json_object *
bygfoot_json_serialize_league_array(const GArray *league_array);

struct json_object *
bygfoot_json_serialize_league(const League *league);

struct json_object *
bygfoot_json_serialize_team_ptrs(GPtrArray *team_ptrs,
                                 GHashTable *fields);

struct json_object *
bygfoot_json_serialize_cup(const Cup *cup, GHashTable *fields);
#endif
