#pragma once

/** Create a rect */
#define RECT(py, px, pw, ph)\
    {.y = py, .x = px, .width = pw, .height = ph}

/** Eval to 1 if 'f' is set in 'flags' */
#define FLAGSET(flags, f)\
    (flags & f) == f ? 1 : 0

/** Cursor.h */
struct cursor {
    unsigned int line;
    unsigned int col;
};


/** struct rect represents area of the screen*/
struct rect {
    unsigned int y, x; // Top left corner
    unsigned int width, height;
};


/** Editor modes */
enum mode {
    MODE_NORMAL,
    MODE_INSERT,
};

