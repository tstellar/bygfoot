#ifndef BYGFOOT_STRUCT_H
#define BYGFOOT_STRUCT_H

enum BygfootFrontend {
    BYGFOOT_FRONTEND_GTK2
};

/** This struct holds all of the global state for a bygfoot game.  The goal
 * is for ths struct to eventually replace all the global variables.
*/
typedef struct
{
    gchar *id;

    /** @name Frontend functions */
    /* @{ */
    void (*show_progress)(gfloat, const gchar *, gint);
    /* @} */
} Bygfoot;

#endif
