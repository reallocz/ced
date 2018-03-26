#include "term.h"
#include "log.h"
#include <clocale>
#include <ncurses.h>
#include <cstdio>
#include <cstdlib>

#define TAG "TERM"

namespace Term
{
unsigned int rows, cols;
WINDOW* stdscr;

int init()
{
    // Initialize ncurses
    setlocale(LC_ALL, "");
    stdscr = initscr();
    start_color();
    getmaxyx(stdscr, rows, cols);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    log_l(TAG, "Init success");
    log_lc("stdscr { rows: %d, cols %d }\n",
           rows, cols);
    return 0;
}

void exit()
{
    endwin();
    log_l(TAG, "Term exit");
}


void update()
{
    getmaxyx(stdscr, rows, cols);
}
}  // namespace Term
