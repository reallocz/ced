#pragma once


/** Handle to line */
typedef unsigned int hLine;

int line_init();
int line_exit();

/** Create a new line */
hLine line_create();

/** Destroy line */
void line_destroy(hLine line);

/** Increase the width of gap */
void line_add_gap(hLine line);

/** Add char at current cursor pos */
void line_addch(hLine line, char c);

/** Delete char at current cursor pos and move cur left */
void line_delch(hLine line);

/** Move cursor to the left */
void line_move_cur_left(hLine line);

/** Move cursor to right */
void line_move_cur_right(hLine line);

/** Get cursor position */
int line_get_cursor(hLine line);

void line_pprint(hLine line);
void line_print(hLine line);
