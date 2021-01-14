
#include "bygfoot.h"
#include "file.h"
#include "backend_filesystem.h"
#include "backend_server.h"
#include "load_save.h"
#include "gui.h"
#include "misc.h"
#include "start_end.h"
#include "user.h"
#include "start_end.h"
#include "xml_country.h"

void
bygfoot_init(Bygfoot *bygfoot, enum BygfootFrontend frontend,
                               enum BygfootBackend backend)
{
    memset(bygfoot, 0, sizeof(*bygfoot));

    bygfoot->frontend = frontend;

    switch(frontend) {
    case BYGFOOT_FRONTEND_GTK2:
        bygfoot->show_progress = gui_show_progress;
        break;
    }
    switch (backend) {
    case BYGFOOT_BACKEND_FILESYSTEM:
        bygfoot->get_country_list = file_get_country_files;
        bygfoot->load_bygfoot = bygfoot_filesystem_load_bygfoot;
        break;
    }
}

gboolean
bygfoot_init_server_backend(Bygfoot *bygfoot, char *server, int port)
{
    bygfoot->backend_server = server;
    bygfoot->backend_port = port;
    bygfoot->get_country_list = bygfoot_server_get_country_list;
    return TRUE;
}

#if 0
void
bygfoot_set_save_dir(Bygfoot *bygfoot, const gchar *dir)
{
    if (!dir) {
        /* FIXME: This is not safe */
        bygfoot->save_dir = g_malloc0(256);
        file_get_bygfoot_dir(bygfoot->save_dir);
    }
    /* FIXME: Should memcpy here since we will be freeing this. */
    bygfoot->save_dir = dir;
}
#endif

void
bygfoot_load_bygfoot(Bygfoot *bygfoot, const gchar *id)
{
    bygfoot->load_bygfoot(bygfoot, id);
}

gboolean
bygfoot_set_id(Bygfoot *bygfoot, const gchar *id)
{
    return FALSE;
#if 0
    const int generated_id_len = 8;
    int i;
    GRand *rand;
    if (id) {
    	if (!bygfoot->is_bygfoot_id_unique(bygfoot, id))
            return FALSE;
        bygfoot->id = id;
        return TRUE;
    }
    bygfoot->id = g_malloc0(generated_id_len);
    rand = g_rand_new();
    do {
        for (i = 0; i < generated_id_len - 1; i++) {
            char a = g_rand_int(rand);
            snprintf(bygfoot->id + i, "%x", a);
        }
    } while (bygfoot->backend->is_bygfoot_id_unique(bygfoot->id));
    g_rand_free(rand);
    return TRUE;
#endif
}

Country *bygfoot_load_country(Bygfoot *bygfoot, const gchar *country_name)
{
    xml_country_read(country_name, NULL);
    return &country;
}

User *bygfoot_add_user(Bygfoot *bygfoot, const gchar *username, Team *tm)
{
    User new_user = user_new();
    new_user.tm = tm;
    new_user.team_id = tm->id;
    
    /* FIXME: Use strnlen */
    if(strlen(username) > 0)
        misc_string_assign(&new_user.name, username);

    g_array_append_val(users, new_user);
    return &g_array_index(users, User, users->len - 1);
}

void bygfoot_start_game(Bygfoot *bygfoot)
{
    unsigned i;

    start_new_game();
    for (i = 0; i < users->len; i++)
        user_set_up_team_new_game(&usr(i));
}

void bygfoot_show_progress(const Bygfoot *bygfoot, gfloat value, const gchar *text, gint pictype)
{
    if (bygfoot->show_progress)
        bygfoot->show_progress(value, text, pictype);
}

GPtrArray *bygfoot_get_country_list(const Bygfoot *bygfoot)
{
    return bygfoot->get_country_list(bygfoot);
}
