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

/** Set the editor mode for the focussed window */
void ced_set_mode(enum ced_mode mode);
/** Set window focus */
void ced_set_window_focus(hWindow win);

// Input callbacks
/** Input callback for MODE_INSERT */
void ced_insert_input_cb(inpev ev);
/** Input callback for MODE_NORMAL */
void ced_normal_input_cb(inpev ev);
/** Input callback for MODE_META */
void ced_meta_input_cb(inpev ev);

// Drawing
void ced_draw_statusline(hWindow win, enum ced_mode mode);
void ced_draw_numberline(hWindow win, enum ced_mode mode);
void ced_draw_textarea(hWindow win);
