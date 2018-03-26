#pragma once
#include <ncurses.h>

/** term.h
 * Manage curses and other terminal functions
 */

namespace Term
{
extern unsigned int rows, cols;
extern WINDOW* stdscr;

int init();
void exit();
void update();
};
