#ifndef LOAD_SAVE_H
#define LOAD_SAVE_H

#include "bygfoot.h"

void
load_save_save_game(const gchar* filename);

void
load_save_load_game(const gchar* filename);

void
load_save_last_save_set(const gchar *filename);

gchar*
load_save_last_save_get(void);

void
load_save_autosave(void);

#endif
