#ifndef FILE_H
#define FILE_H

#include "bygfoot.h"
#include "player_struct.h"
#include "user_struct.h"

void
file_add_support_directory_recursive                   (const gchar     *directory);

gchar*
file_find_support_file                       (const gchar     *filename);

gboolean
file_get_next_opt_line(FILE *fil, gchar *opt_name, gchar *opt_value);

GPtrArray*
file_dir_get_contents(const gchar *dir_name, const gchar *prefix);

void
file_get_player_names(gint number_of_names);

gboolean
file_my_fopen(const gchar *filename, gchar *bits, FILE **fil, gboolean abort_program);

void
file_get_definitions_dir(gchar *dir);

void
file_write_opt_names(gchar opt_names[][50], gchar conf_file_names[][100]);

void
file_write_user_opt_names(gchar user_opt_names[][50],
			  gchar player_list_att_names[][PLAYER_LIST_ATTRIBUTE_END][50]);

void
file_load_conf_files(void);

void
file_save_conf(void);

void
file_load_user_conf_file(User *user);

void
file_load_const_file(void);

#endif
