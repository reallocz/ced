#pragma once
#include "input_keys.h"
#include "window.h"

/** Input key types */
enum class InputType {
    Alpha,    /* Alphabets */
    Num,      /* Numbers */
    Symbol,   /* Symbols and punctuations */
    Special,  /* TAB, SPACE, ENTER, ALT etc */
    Arrow,    /* Arrow keys */
    Function, /* f1..f12 */
    Unknown   /* Unknown key */
};


/** Input event generated on inp_poll() */
typedef struct {
    enum InputType type;
    int key; /* Can be cast to char */
} inpev;


/** Poll ncurses for input events.
 * The input event is passed on to the handler.
 */
inpev inp_poll(Window& win);

/** Return the type of given int */
enum InputType inp_classify(int c);
