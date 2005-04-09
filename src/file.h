#ifndef FILE_H
#define FILE_H

#include "bygfoot.h"
#include "player_struct.h"
#include "user_struct.h"

void
file_add_support_directory_recursive                   (const gchar     *directory);

gchar*
file_find_support_file                       (const gchar     *filename, gboolean warning);

gboolean
file_get_next_opt_line(FILE *fil, gchar *opt_name, gchar *opt_value);

void
file_load_opt_file(const gchar *filename, OptionList *optionlist);

void
file_save_opt_file(const gchar *filename, OptionList *optionlist);

GPtrArray*
file_dir_get_contents(const gchar *dir_name, const gchar *prefix, const gchar *suffix);

void
file_get_player_names(gint number_of_names);

gboolean
file_my_fopen(const gchar *filename, gchar *bits, FILE **fil, gboolean abort_program);

GPtrArray*
file_get_country_files(void);

void
file_load_conf_files(void);

void
file_load_user_conf_file(User *user);

void
file_check_home_dir(void);

void
file_check_home_dir_copy_conf_files(void);

void
file_check_home_dir_copy_definition_files(void);

gboolean
file_my_system(const gchar *command);

const gchar*
file_get_first_support_dir(void);

void
file_compress_files(const gchar *destfile, const gchar *prefix);

void
file_decompress(const gchar *filename);

#endif
