#ifndef FILE_H
#define FILE_H

#include "bygfoot.h"
#include "misc.h"

GPtrArray*
file_dir_get_contents(const gchar *dir_name, const gchar *prefix);

void
file_dir_free_contents(GPtrArray *dir_contents);

#endif
