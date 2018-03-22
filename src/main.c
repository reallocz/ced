#include <assert.h>
#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ced.h"
#include "log.h"
#include "term.h"
#include "common.h"

/** Cleanup submodules */
void onexit()
{
    // Logs exit last
    term_exit();
    log_exit();
}


int main()
{
    atexit(onexit);

    // init submodules
    // Logs init first
    log_init();

    log_l("MAIN", "\n--------INIT BEGIN-------");
    term_init();
    log_l("MAIN", "\n---------INIT END--------\n");

    struct cedopts opts;
    opts.bcount = 2;
    opts.bviews[0] = bv_create(DOCUMENT, TEXTPATH "table.txt");
    opts.bviews[1] = bv_create(DOCUMENT, TEXTPATH "kepler.txt");

    ced_init(opts);
    ced_run();
}
