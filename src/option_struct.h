#ifndef OPTION_STRUCT_H
#define OPTION_STRUCT_H

/** A struct representing an option or a constant. */
typedef struct
{
    GString *name, *string_value;
    gint value;
} Option;

/** An array of name-value pairs with an associated keyed data list for
    quick access.  */
typedef struct
{
    GArray *list;
    GData *datalist;
} OptionList;

#endif
