#ifndef OPTION_STRUCT_H
#define OPTION_STRUCT_H

/** An array of name-value pairs with an associated keyed data list for
    quick access.  */
typedef struct
{
    GArray *list;
    GData *datalist;
} OptionList;

#endif
