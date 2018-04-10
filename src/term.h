#pragma once
#include <stddef.h>

/** term.h
 * Manage curses and other terminal functions
 */

/** Initialize terminal and its properties */
int term_init(void);

void term_exit(void);

/** Update terminal width and height */
void term_update(void);

/** query terminal props */
int term_rows(void);
int term_cols(void);
void term_size(size_t* rows, size_t* cols);
