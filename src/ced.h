#pragma once
#include "BufferView.h"
#include "context.h"
#include "input.h"

/** ced.h
 * All the main editor logic lies here.
 * The main function initializes submodules and the control
 * jumps right to ced_run()
 */
#define BVIEW_LIMIT 8


class Ced
{
    int quit;
    Context context{};
    Window win{};
    BufferView bviews[BVIEW_LIMIT]{};
    unsigned int bcount{};
    unsigned int currentBview{};


public:
    struct Opts {
        BufferView bviews[BVIEW_LIMIT];
        unsigned int bcount;
    };


private:
    void parseOpts(Opts opts);
    void nextBview();
    void execCommand(struct command cmd);

public:
    Ced(Opts opts);
    ~Ced();

    void run();
    /** Input callback for MODE_INSERT */
    void insertCb(inpev ev);
    /** Input callback for MODE_NORMAL */
    void normalCb(inpev ev);
    /** Input callback for MODE_COMMAND */
    void commandCb(inpev ev);
};
