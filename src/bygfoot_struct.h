#ifndef BYGFOOT_STRUCT_H
#define BYGFOOT_STRUCT_H

#include <glib.h>

enum BygfootFrontend {

    BYGFOOT_FRONTEND_NONE,
    BYGFOOT_FRONTEND_GTK2,
    BYGFOOT_FRONTEND_CONSOLE,
};

enum BygfootBackend {
    BYGFOOT_BACKEND_NONE,
    BYGFOOT_BACKEND_FILESYSTEM,
    BYGFOOT_BACKEND_SERVER
};

/** This struct holds all of the global state for a bygfoot game.  The goal
 * is for ths struct to eventually replace all the global variables.
*/
typedef struct bygfoot
{
    gchar *id;
    gchar *save_dir;

    gchar *backend_server;
    gint   backend_port;

    enum BygfootFrontend frontend;

    /** @name Frontend functions */
    /* @{ */
    void (*show_progress)(gfloat, const gchar *, gint);
    /* @} */

    /** @name Backend functions */
    /* @{ */
    /* This function returns an array of gchar* that describe the countries
     * a user can choose from.  The string format for the countries is
     * <Continent>/<...>/<country>
     */
    GPtrArray *(*get_country_list)(const struct bygfoot *);
    void (*load_bygfoot)(const struct bygfoot *, const gchar *); 
    gboolean (*is_bygfoot_id_unique)(struct bygfoot *, const gchar *);
    /* @} */
} Bygfoot;


#endif
