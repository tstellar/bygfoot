#ifndef MAIN_H
#define MAIN_H

#include "bygfoot.h"

void
main_init(gint *argc, gchar ***argv);

void
main_init_variables(void);

void
main_exit_program(gint exit_code, gchar *exit_message)
__attribute__ ((noreturn));


void
main_parse_cl_arguments(gint *argc, gchar ***argv);

#endif
