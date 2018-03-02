#pragma once


/** Handle to line */
typedef unsigned int hLine;


////////////////////////////////////////
/// Constructor and Destructors
////////////////////////////////////////

int line_init();
int line_exit();

/** Create a new line */
hLine line_create();

/** Destroy line */
void line_destroy(hLine line);


////////////////////////////////////////
/// Memory management
////////////////////////////////////////

/** Increase the width of gap */
void line_add_gap(hLine line);


////////////////////////////////////////
/// Insert and delete
////////////////////////////////////////

/** Add char at current cursor pos */
void line_addch(hLine line, char c);

/** Delete char at current cursor pos and move cur left */
void line_delch(hLine line);


////////////////////////////////////////
/// Movement
////////////////////////////////////////

/** Move cursor to the left */
void line_move_cur_left(hLine line);

/** Move cursor to right */
void line_move_cur_right(hLine line);


////////////////////////////////////////
/// Accessors
////////////////////////////////////////

/** Get cursor position */
int line_get_cursor(hLine line);


////////////////////////////////////////
/// Debug
////////////////////////////////////////
void line_pprint(hLine line);
void line_print(hLine line);
