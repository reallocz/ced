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
 * Dimensions:
 * 	window:
 * 		x, y = coordinates of the top left corner
 * 		rows, cols = total width and height
 * 	statusline:
 * 		width = window.cols
 * 		height = 2 (constant)
 * 		y = window.rows - height
 * 		x = 0
 * 	margin:
 *		width = *variable*
 *		height = window.rows - statusline.height
 *		y = 0
 *		x = 0
 *	textarea:
 *		width = window.cols - margin.width
 *		height = window.rows - statusline.height
 *		y = 0
 *		x = margin.width
 */


/**
 * Window handle
 * This handle is used for accessing all the window related
 * functions. A handle < 0 represents an invalid window.
 * Internally, the window submodule maintains a pool of
 * window structs that can be reused. This pool is
 * implemented as an array. The handle is a valid index
 * to a window struct in that array. An invalid handle
 * is represented by INVALID_ID
 */
typedef int hWindow;

struct win_props {
    // Dimensions
    unsigned int wy, wx, wrows, wcols;
    // statusline
    unsigned int sy, sx, swidth, sheight;
    // margin
    unsigned int my, mx, mwidth, mheight;
    // textarea
    unsigned int ty, tx, twidth, theight;
};


/** initialize ncurses and the window submodule.*/
void win_init();
void win_exit();

/** Create a window at (x, y) of size (cols, rows) */
hWindow win_create(int y, int x, int rows, int cols);
/** destroys the window. doesn't alter attached buffer*/
void win_destroy(hWindow win);

/** updates window properties */
void win_update(hWindow win);

/** return's the handle to ncurses WINDOW* */
WINDOW* win_nwin(hWindow win);

/** sets/replaces the window buffer */
int win_buffer_set(hWindow win, hBuffer buf);
/** returns the buffer set on the window
 * or INVALID_ID if a buffer isn't set */
hBuffer win_buffer_get(hWindow win);

/** moves window cursor to (x,y)*/
void win_cursor_set(hWindow win, int y, int x);
/** query cursor positions */
void win_cursor_get(hWindow win, int* y, int* x);

/** returns the window properties struct*/
struct win_props win_props_get(hWindow win);
int win_props_set(hWindow win, struct win_props props);

/** Drawing / rendering */
/** Refresh's window */
void win_draw_refresh(hWindow win);
/** Draws buffer on to the textarea */
void win_draw_buffer(hWindow win);

void win_pprint(hWindow win);

