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


/** Window handle: 0 for an invalid window */
typedef unsigned int hWindow;

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
void win_destroy(hWindow win);

/** Updates window properties based on the current state */
void win_update(hWindow win);


/** win_nwin return's the handle to ncurses WINDOW* */
WINDOW* win_nwin(hWindow win);

/** Set's window buffer. Replaces the old buffer */
int win_set_buffer(hWindow win, hBuffer buf);
/** win_get_buffer returns the hBuffer set on the window or
 * BUFFER_DEFID if a buffer isn't set
 */
hBuffer win_get_buffer(hWindow win);

/** win_set_cursor moves window cursor to (x,y)*/
void win_set_cursor(hWindow win, int y, int x);
/** win_get_cursor query cursor positions */
void win_get_cursor(hWindow win, int* y, int* x);

struct win_props win_get_props(hWindow win);

void win_pprint(hWindow win);

