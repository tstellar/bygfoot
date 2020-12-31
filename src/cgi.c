#include <json-c/json_object.h>
#include <stdio.h>
#include "bygfoot_struct.h"
#include "json_interface.h"

/* country */
#define URI_COMPONENT_MAX_LEN 7

#define DATA_MAX_LEN 256

#if 0
static struct json_object *
read_data()
{
    struct json_tokener *tokener = json_tokener_new();
}
#endif

int bygfoot_cgi_main(Bygfoot *bygfoot)
{
    const char uri_separator = '/';
    const gchar *path_info = g_getenv("PATH_INFO");
    if (!path_info || path_info[0] != uri_separator) {
        fprintf(stderr, "Error parsing PATH_INFO");
        return 1;
    }
    path_info++;
    if (g_str_has_prefix(path_info, "country")) {
        struct json_object *country_list_obj =
                bygfoot_json_call_get_countries(bygfoot, NULL);
        printf("%s", json_object_to_json_string(country_list_obj));
        return 0;
    }
    return 1;
}
