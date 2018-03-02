#pragma once

/** term.h
 * Manage curses and other terminal functions
 */

/** Initialize terminal and its properties */
int term_init();

int term_quit();

/** query terminal props */
int term_rows();
int term_cols();
void term_size(int *rows, int *cols);

