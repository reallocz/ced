#pragma once
#include "input_keys.h"
#include "window.h"

/** Input key types */
enum inp_type {
    INP_ALPHA,    /* Alphabets */
    INP_NUM,      /* Numbers */
    INP_SYMBOL,   /* Symbols and punctuations */
    INP_SPECIAL,  /* TAB, SPACE, ENTER, ALT etc */
    INP_ARROW,    /* Arrow keys */
    INP_FUNCTION, /* f1..f12 */
    INP_UNKNOWN   /* Unknown key */
};


/** Input event generated on inp_poll() */
typedef struct {
    enum inp_type type;
    int key; /* Can be cast to char */
} inpev;


/** Poll ncurses for input events.
 * The input event is passed on to the handler.
 */
inpev inp_poll(struct window* win);

/** Return the type of given int */
enum inp_type inp_classify(int c);
