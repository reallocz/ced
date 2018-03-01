#pragma once
#include <ncurses.h>


/** window.h
 * A window is a rectangular area of the terminal.
 * Window's cannot overlap.
 * Windows can be:
 * 	- resized
 * 	- moved around
 * 	- hidden
 * 	- discarded
 */


typedef unsigned int hWindow;

/** initialize ncurses and the window submodule.*/
void win_init();

/** Create a window at (x, y) of size (cols, rows) */
hWindow win_create(int y, int x, int rows, int cols);

void win_exit();

WINDOW* win_getnwin(hWindow win);

void win_pprint(hWindow win);
