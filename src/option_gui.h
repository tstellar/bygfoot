#ifndef OPTION_GUI_H
#define OPTION_GUI_H

#include "bygfoot.h"

void
option_gui_write_bool_widgets(gint **bool_options, GtkToggleButton **bool_widgets);

void
option_gui_write_spin_widgets(gint **spin_options, GtkSpinButton **spin_widgets);

void
option_gui_write_entry_widgets(GString **entry_options, GtkEntry **entry_widgets);

void
option_gui_set_up_window(void);


#endif

