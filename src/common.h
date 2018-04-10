#pragma once
#include "log.h"
#include <stddef.h>

#define TEXTPATH "../text/"

/** Create a rect */
#define RECT(py, px, pw, ph)                        \
    {                                               \
        .y = py, .x = px, .width = pw, .height = ph \
    }

/** Evals to 1 if two rects are identical */
#define RECTSAME(A, B)                                 \
    (A.y == B.y && A.x == B.x && A.width == B.width && \
     A.height == B.height)

/** Eval to 1 if 'f' is set in 'flags' */
#define ISFLAGSET(flags, f) (flags & f) == f ? 1 : 0

#define SETFLAG(flags, f) flags |= f

#define UNSETFLAG(flags, f) flags &= ~f

enum flags {
    Farea_update = 1 << 1,
};

/** Cursor.h */
struct cursor {
    size_t line;
    size_t col;
};


/** struct rect represents area of the screen*/
struct rect {
    size_t y, x;    // Top left corner
    size_t width, height;
};


/** Editor modes */
enum mode { MODE_NORMAL, MODE_INSERT, MODE_COMMAND };

/** Map for mode -> string */
static const char* mode_str[] = {
    "NORMAL",
    "INSERT",
    "COMMAND",
};

void printrect(const char* name, struct rect r);
