#pragma once
#include <cstdlib>
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
struct Cursor {
    size_t line;
    size_t col;
};


/** struct rect represents area of the screen*/
struct Rect {
    size_t y, x;    // Top left corner
    size_t width, height;

    void print()
    {
        log_l("COMMON", "Rect {y: %d, x: %d, w: %d, h: %d}", y, x, width, height);
    }
};
