#include "term.h"
#include "log.h"
#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>


static struct {
    const char* tag;
    size_t rows, cols;    // Global width and height

    WINDOW* stdscr;
} G;


int term_init()
{
    G.tag = "TERM";

    // Initialize ncurses
    setlocale(LC_ALL, "");
    G.stdscr = initscr();
    start_color();
    getmaxyx(G.stdscr, G.rows, G.cols);
    cbreak();
    noecho();
    keypad(G.stdscr, TRUE);
    log_l(G.tag, "Init success");
    log_lc("stdscr { rows: %d, cols %d }\n", G.rows, G.cols);
    return 0;
}


void term_exit()
{
    endwin();
    log_l(G.tag, "Term exit");
}


void term_update() { getmaxyx(G.stdscr, G.rows, G.cols); }

int term_rows() { return G.rows; }

int term_cols() { return G.cols; }

void term_size(size_t* rows, size_t* cols)
{
    *rows = G.rows;
    *cols = G.cols;
}
