#pragma once
#include "input.h"

/** ced.h
 * All the main editor logic lies here.
 * The main function initializes submodules and the control
 * jumps right to ced_run()
 */


/** Editor modes */
enum ced_mode {
	MODE_NORMAL,
	MODE_INSERT,
	MODE_META
};

/** Initialize logic and run.
 * NOTE: All the submodules must be initialized before calling
 * ced_run()
 */
void ced_run();

/** Set the editor mode */
void ced_set_mode(hWindow win, enum ced_mode mode);

// Input handlers
/** Input handler for MODE_INSERT */
void ced_insert_input_handler(inpev ev);
/** Input handler for MODE_NORMAL */
void ced_normal_input_handler(inpev ev);
/** Input handler for MODE_META */
void ced_meta_input_handler(inpev ev);

