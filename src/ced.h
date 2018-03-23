#pragma once
#include "input.h"

/** ced.h
 * All the main editor logic lies here.
 * The main function initializes submodules and the control
 * jumps right to ced_run()
 */
#define BVIEW_LIMIT 8

struct cedopts {
    struct buffer_view bviews[BVIEW_LIMIT];
    unsigned int bcount;
};

void ced_init(struct cedopts opts);

/** Initialize logic and run.
 * NOTE: All the submodules must be initialized before calling
 * ced_run()
 */
void ced_run();

// Input callbacks
/** Input callback for MODE_INSERT */
void ced_insert_input_cb(inpev ev);
/** Input callback for MODE_NORMAL */
void ced_normal_input_cb(inpev ev);
/** Input callback for MODE_COMMAND */
void ced_command_input_cb(inpev ev);
