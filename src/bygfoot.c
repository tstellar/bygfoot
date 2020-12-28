
#include "bygfoot.h"
#include "gui.h"
#include "misc.h"
#include "start_end.h"
#include "user.h"
#include "xml_country.h"

void
bygfoot_init(Bygfoot *bygfoot, enum BygfootFrontend frontend)
{
    memset(bygfoot, 0, sizeof(*bygfoot));
    switch(frontend) {
    case BYGFOOT_FRONTEND_GTK2:
        bygfoot->show_progress = gui_show_progress;
        break;
    }
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
