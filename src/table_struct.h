#ifndef TABLE_STRUCT_H
#define TABLE_STRUCT_H

/**
   Table element values.
   @see TableElement
   @see Table
*/
enum TableElementValues
{
    TABLE_PLAYED = 0,
    TABLE_WON,
    TABLE_DRAW,
    TABLE_LOST,
    TABLE_GF,
    TABLE_GA,
    TABLE_PTS,
    TABLE_END
};

/**
   An element representing a team in the tables.
   @see Table
   @see #TableElementValues
*/
typedef struct
{
    gint team_id;
    gint values[TABLE_END];
} TableElement;

/**
   A table belonging to a league or a cup with round robin.
   @see TableElement
*/
typedef struct
{
    GString *name;
    gint league_id;
    GArray *elements;
} Table;

#endif
