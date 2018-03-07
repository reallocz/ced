#pragma once
#include "window.h"
#include "input_keys.h"

/** Input key types */
enum inp_type{
    INP_ALPHA,      /* Alphabets */
    INP_NUM,        /* Numbers */
    INP_SYMBOL,     /* Symbols and punctuations */
    INP_SPECIAL,    /* TAB, SPACE, ENTER, ALT etc */
    INP_ARROW,      /* Arrow keys */
    INP_FUNCTION,   /* f1..f12 */
    INP_UNKNOWN     /* Unknown key */
};


/** Input event generated on inp_poll() */
typedef struct {
    enum inp_type type;
    int key;    /* Can be cast to char */
} inpev;


struct inp_handler {
    const char* name;           /** Name for identification */
    hWindow window;                /** handle used for polling */
    void (*callback) (inpev);   /** Callback function */
};


/** Initialize input submodule */
int inp_init();
int inp_exit();

/** Set an input handler */
int inp_set_handler(struct inp_handler handler);
/** Return's the set handler */
struct inp_handler inp_get_handler();
/** Remove input handler function */
void inp_remove_handler();

/** Poll ncurses for input events.
 * The input event is passed on to the handler.
 */
void inp_poll();

/** Return the type of given int */
enum inp_type inp_classify(int c);

