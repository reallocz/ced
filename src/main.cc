#include "ced.h"
#include "common.h"
#include "log.h"
#include "term.h"
#include <assert.h>
#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/** Cleanup submodules */
void onexit()
{
    // Logs exit last
    Term::exit();
    log_exit();
}


int main()
{
    atexit(onexit);

    // init submodules
    // Logs init first
    log_init();

    log_l("MAIN", "\n--------INIT BEGIN-------");
    Term::init();
    log_l("MAIN", "\n---------INIT END--------\n");

    Ced::Opts opts;
    opts.bcount    = 2;
    opts.bviews[0] = bv_create(DOCUMENT, TEXTPATH "table.txt");
    opts.bviews[1] = bv_create(DOCUMENT, TEXTPATH "kepler.txt");

    Ced::init(opts);
    Ced::run();
}
