#include "ced.h"
#include "common.h"
#include "log.h"
#include "term.h"
#include <cassert>
#include <clocale>
#include <cstdio>
#include <cstdlib>
#include <ncurses.h>
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

    Ced::Opts opts{};
    opts.bcount    = 2;
    opts.bviews[0] = BufferView(Buffer::Type::Document, TEXTPATH "table.txt");
    opts.bviews[1] = BufferView(Buffer::Type::Document, TEXTPATH "kepler.txt");

    Ced ced(opts);
    ced.run();
}
