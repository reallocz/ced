#pragma once
#include <ncurses.h>
#include "buffer.h"


/** window.h
 * A window is a rectangular area of the terminal.
 * Window's cannot overlap.
 * Windows can be:
 * 	- resized
 * 	- moved around
 * 	- hidden
 * 	- discarded
 */


typedef int hWindow;


/** initialize ncurses and the window submodule.*/
void win_init();
void win_exit();

/** Create a window at (x, y) of size (cols, rows) */
hWindow win_create(int y, int x, int rows, int cols);
void win_destory(hWindow win);


/** Set's window buffer. Replaces the old buffer */
int win_setbuffer(hWindow win, hBuffer buf);

/** Get a handle to ncurses WINDOW* */
WINDOW* win_getnwin(hWindow win);

/** get window size: rows and columns */
void win_getsize(hWindow win, int* rows, int* cols);
/** get window cursor position */
void win_get_cursor(hWindow win, int* y, int* x);
/** Move window cursor */
void win_set_cursor(hWindow win, int y, int x);

/** returns the set window buffer. -1 if no buffer is set */
hBuffer win_getbuffer(hWindow win);

void win_pprint(hWindow win);

