/*
   file.h

   Bygfoot Football Manager -- a small and simple GTK2-based
   football management game.

   http://bygfoot.sourceforge.net

   Copyright (C) 2005  Gyözö Both (gyboth@bygfoot.com)

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef FILE_H
#define FILE_H

#include "bygfoot.h"
#include "player_struct.h"
#include "user_struct.h"

#ifdef G_OS_UNIX
#define file_mkdir(dirname) mkdir(dirname, (S_IRUSR | S_IWUSR | S_IXUSR))
#else
#define file_mkdir(dirname) mkdir(dirname)
#endif

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

gboolean
file_my_fopen(const gchar *filename, gchar *bits, FILE **fil, gboolean abort_program);

GPtrArray*
file_get_country_files(void);

void
file_load_conf_files(void);

void
file_load_user_conf_file(User *user);

gboolean
file_check_home_dir(void);

void
file_check_home_dir_get_conf_files(GPtrArray **files_to_copy);

void
file_check_home_dir_get_definition_files(GPtrArray **files_to_copy);

void
file_check_home_dir_get_definition_dir(const gchar *dirname, const gchar *basename,
				       GPtrArray **files_to_copy);

void
file_check_home_dir_copy_files(GPtrArray **files_to_copy);

gboolean
file_my_system(const gchar *command);

const gchar*
file_get_first_support_dir(void);

const gchar*
file_get_first_support_dir_suffix(const gchar *suffix);

void
file_compress_files(const gchar *destfile, const gchar *prefix);

void
file_decompress(const gchar *filename);

void
file_remove_files(const gchar *files);

void
file_copy_file(const gchar *source_file, const gchar *dest_file);

void
file_get_bygfoot_dir(gchar *dir);

void
file_load_hints_file(void);

#endif
