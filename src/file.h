#ifndef FILE_H
#define FILE_H

#include "bygfoot.h"

void
file_add_support_directory_recursive                   (const gchar     *directory);

gchar*
file_find_support_file                       (const gchar     *filename);

gboolean
file_get_next_line(FILE *fil, gchar *buf);

GPtrArray*
file_dir_get_contents(const gchar *dir_name, const gchar *prefix);

void
file_get_player_names(gint number_of_names);

gboolean
my_fopen(const gchar *filename, gchar *bits, FILE **fil, gboolean abort_program);

#endif
