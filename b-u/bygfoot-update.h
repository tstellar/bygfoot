#ifndef BYGFOOT_UPDATE_H
#define BYGFOOT_UPDATE_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include <stdio.h>
#include <stdlib.h>

#define SMALL 10000
#define BIG 1000000

gint status;
GtkWidget *update_window;
GString *update_script;
GString *command_line;

enum
{
    STATUS_NORMAL = 0,
    STATUS_FS_PACK,
    STATUS_FS_TMP,
    STATUS_FS_COUNTRY,
    STATUS_END
};

#endif
