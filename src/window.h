#pragma once
#include <ncurses.h>


typedef unsigned int hWindow;

/** initialize ncurses and window */
void win_init();

hWindow win_create();

void win_exit();

WINDOW* win_getnwin(hWindow w);

