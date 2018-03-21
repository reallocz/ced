#include <assert.h>
#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ced.h"
#include "log.h"
#include "term.h"

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

    // Run program
    ced_run();
}
