#ifndef JSON_INTERFACE_H
#define JSON_INTERFACE_H

#include <json-c/json_object.h>
#include "bygfoot.h"


int bygfoot_json_main(Bygfoot *bygfoot, const CommandLineArgs *cl_args);

struct json_object *bygfoot_json_call_get_countries(Bygfoot *bygfoot, const json_object *args);

#endif
