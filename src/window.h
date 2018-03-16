#pragma once
#include <ncurses.h>
#include "common.h"
#include "buffer_view.h"

/** window.h
 * A window is a rectangular division of the terminal.
 * Windows can:
 * 	- not overlap
 * 	- be resized
 * 	- be moved around
 * 	- be hidden
 * 	- be discarded
 *
 * Anatomy of a window:
 *	 _______________
 *	| |		|	1 - margin
 *	|1|	3	|	2 - statusline
 *	|_|_____________|       3 - buffer view
 *	|_______________|<- 2
 */

struct statusline {
    const char *mode, *bufname;
    struct cursor cur;
    struct buffer_gap gap;
};

struct margin {
    unsigned int width;
    unsigned int start; // First line number
    unsigned int end;   // Last line number
};

struct window {
    unsigned int id;
    WINDOW* nwin;

    struct buffer_view bview;
    struct statusline sline;
    struct margin margin;
};


/** Create a window */
struct window* win_create(struct buffer_view bview);

/** destroys the window. doesn't alter attached buffer*/
void win_destroy(struct window* win);

/** Update window margin, statusline etc */
void win_update(struct window* win);

/** Draw the window */
void win_draw(const struct window* win, const char* mode, struct rect area);

void win_pprint(struct window* win);

