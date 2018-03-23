#pragma once

/** term.h
 * Manage curses and other terminal functions
 */

/** Initialize terminal and its properties */
int term_init();

void term_exit();

/** Update terminal width and height */
void term_update();

/** query terminal props */
int term_rows();
int term_cols();
void term_size(unsigned int* rows, unsigned int* cols);
