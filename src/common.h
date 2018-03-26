#pragma once
#include "log.h"

#define TEXTPATH "../text/"

/** Create a rect */
#define RECT(py, px, pw, ph)                        \
    {                                               \
        .y = py, .x = px, .width = pw, .height = ph \
    }

/** Evals to 1 if two rects are identical */
#define RECTSAME(A, B) \
    (A.y == B.y && A.x == B.x && A.width == B.width && A.height == B.height)

/** Eval to 1 if 'f' is set in 'flags' */
#define ISFLAGSET(flags, f) \
    (flags & f) == f ? 1 : 0

#define SETFLAG(flags, f) \
    flags |= f

#define UNSETFLAG(flags, f) \
    flags &= ~f

enum flags {
    Farea_update = 1 << 1,
};

/** Cursor.h */
struct cursor {
    unsigned int line;
    unsigned int col;
};


/** struct rect represents area of the screen*/
struct rect {
    unsigned int y, x;    // Top left corner
    unsigned int width, height;
};

void printrect(const char* name, struct rect r);
