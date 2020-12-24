#ifndef BYGFOOT_STRUCT_H
#define BYGFOOT_STRUCT_H

#include <glib.h>

enum BygfootFrontend {
    BYGFOOT_FRONTEND_GTK2,
    BYGFOOT_FRONTEND_CONSOLE,
};

/** This struct holds all of the global state for a bygfoot game.  The goal
 * is for ths struct to eventually replace all the global variables.
*/
typedef struct
{
    /** Which kind of frontend is being use. See #enum BygfootFrontend. */
    enum BygfootFrontend frontend;

    /** @name Frontend functions */
    /* @{ */
    void (*show_progress)(gfloat, const gchar *, gint);
    gdouble (*get_progress_bar_fraction)(void);
    /* @} */
} Bygfoot;


#endif
