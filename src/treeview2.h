#ifndef TREEVIEW2_H
#define TREEVIEW2_H

#include "bygfoot.h"

enum
{
    TREEVIEW_MMATCH_COL_REPLAY = 6,
    TREEVIEW_MMATCH_COL_REMOVE,
    TREEVIEW_MMATCH_COL_EXPORT,
    TREEVIEW_MMATCH_COL_END
};

void
treeview2_create_mmatches(GtkListStore *ls);

void
treeview2_set_up_mmatches(GtkTreeView *treeview);

void
treeview2_show_mmatches(void);


#endif
