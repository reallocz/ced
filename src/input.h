#pragma once
#include "window.h"
#include "input_keys.h"

enum inp_type{
        INSERT,
        DELETE,
        EDIT,
        MOVE,
        SAVE,
        QUIT,
        NOP
};

union inp_payload {
        char ch;
        int i;
};

typedef struct {
	hWindow win;	// Handle to window
        enum inp_type type;
	enum inp_key key;
        union inp_payload data;
} inpev;


/** Initialize input submodule */
int inp_init();
int inp_exit();

/** Poll ncurses for input for the set window.
 * The input event is passed on to the set window handler
 */
void inp_poll();

/** Poll ncurses for input for a window */
inpev inp_winpoll(hWindow win);

/** Set an input handler function */
int inp_sethandler(hWindow win, void (*handler) (inpev));
void inp_unsethandler();


