#ifndef GUI_H
#define GUI_H

#include "bygfoot.h"
#include "support.h"

void
add_support_directory_recursive                   (const gchar     *directory);

gchar*
find_support_file                       (const gchar     *filename);

#endif
