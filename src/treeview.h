#ifndef TREEVIEW_H
#define TREEVIEW_H

#include "bygfoot.h"

gint
treeview_get_index(GtkTreeView *treeview, gint column);

void
treeview_clear(GtkTreeView *treeview);

GtkTreeModel*
treeview_create_team_selection_list(gboolean show_cup_teams);

void
treeview_set_up_team_selection_treeview (GtkTreeView *treeview);

void
treeview_show_team_list(GtkTreeView *treeview, gboolean show_cup_teams);

#endif
