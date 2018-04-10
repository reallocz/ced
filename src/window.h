#pragma once
#include "buffer_view.h"
#include "common.h"
#include "context.h"
#include <ncurses.h>

#define CMDLINE_SIZE 256

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
    const char* bufname;
    struct cursor cur;
    struct buffer_gap gap;
};

struct margin {
    unsigned int width;
    unsigned int start;    // First line number
    unsigned int linecount;
};

struct cmdline {
    char buffer[CMDLINE_SIZE];
};

struct window {
    unsigned int id;
    WINDOW* nwin;

    struct statusline sline;
    struct margin margin;
    struct cmdline cmdline;
    struct buffer_view* bview;
};


/** Create a window */
struct window* win_create(struct buffer_view* bview);

/** destroys the window. doesn't alter attached buffer*/
void win_destroy(struct window* win);

/** attach a buffer_view to the window*/
void win_setbview(struct window* win, struct buffer_view* bv);

/** Update window margin, statusline etc */
void win_update(struct window* win, struct context* context);

/** Draw the window */
void win_draw(const struct window* win,
              const struct context* context);

void win_pprint(struct window* win);
