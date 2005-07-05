#ifndef LG_COMMENTARY_STRUCT_H
#define LG_COMMENTARY_STRUCT_H

/** Structure describing a commentary
    for the live game.  */
typedef struct
{
    /** The commentary text (possibly containing tokens). */
    GString *text;
    /** A condition (if not fulfilled, the commentary doesn't get
	shown). */
    GString *condition;
    /** Priority of the commentary (compared to
	the other ones for the same event).
	The higher the priority the higher the
	probability that the commentary gets picked. */
    gint priority;
    /** An id to keep track of already used commentaries in the
	live game (so as not to use the same one too frequently). */
    gint id;
} LGCommentary;

#endif

