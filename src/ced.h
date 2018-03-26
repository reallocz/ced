#pragma once
#include "input.h"

/** ced.h
 * All the main editor logic lies here.
 * The main function initializes submodules and the control
 * jumps right to ced_run()
 */
#define BVIEW_LIMIT 8

namespace Ced
{

struct Opts {
    struct buffer_view bviews[BVIEW_LIMIT];
    unsigned int bcount;
};

void init(Opts opts);

/** Initialize logic and run.  */
void run();

// Input callbacks
/** Input callback for MODE_INSERT */
void insert_input_cb(inpev ev);
/** Input callback for MODE_NORMAL */
void normal_input_cb(inpev ev);
/** Input callback for MODE_COMMAND */
void command_input_cb(inpev ev);
}
