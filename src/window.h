#pragma once
#include <ncurses.h>
#include "buffer.h"

/** window.h
 * A window is a rectangular division of the terminal.
 * Windows can:
 * 	- not overlap
 * 	- be resized
 * 	- be moved around
 * 	- be hidden
 * 	- be discarded
 *
 * A window consists of a margin, statusline and
 * the text area. The margin lies on the left,
 * statusline at the bottom and the rest is the
 * text area.
 *
 * Anatomy of a window:
 *	 _______________
 *	| |		|	1 - margin
 *	| |		|	2 - statusline
 *	|1|	3	|	3 - text area
 *	| |		|
 *	|_|_____________|
 *	|	2 	|
 *	|_______________|
 *
 *  NOTE: A window must always have a valid buffer attached
 *  to it!!
 */

struct window {
    unsigned int id;
    WINDOW* nwin;
    struct buffer* buffer;

    // Statusline
    struct {
        const char *bufname, *mode;
        unsigned int row, col; // current position in the buffer
    } stl;

    // Margin
    struct {
        unsigned int width;
    } mgn;

    // Cursor
    unsigned int curx, cury;
};


/** Create a window */
struct window* win_create(struct buffer* buffer);

/** destroys the window. doesn't alter attached buffer*/
void win_destroy(struct window* win);

enum window_pane {
    FULL,
    LEFT,
    RIGHT,
};

/** Draw the window */
void win_draw(const struct window* win, const char* mode,
        unsigned int y, unsigned int x,
        unsigned int height, unsigned int width);

void win_pprint(struct window* win);

