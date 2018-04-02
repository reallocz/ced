#pragma once
#include "bufferview.h"
#include "commandline.h"
#include "common.h"
#include "context.h"
#include <ncurses.h>

#define CMDLINE_SIZE 256

/** window.h
 * A window is a rectangular division of the terminal.
 * Windows can:
 * 	- not overlap
 * 	- be resized
 * 	- be moved around
 * 	- be hidden
 * 	- be discarded
 *
 * Anatomy of a window:
 *	 _______________
 *	| |		|	1 - margin
 *	|1|	3	|	2 - statusline
 *	|_|_____________|       3 - buffer view
 *	|_______________|<- 2
 */


struct StatusLine {
    const char* bufname;
    Cursor cur;
    size_t gaplen;
    size_t gapcol;
};


struct Margin {
    size_t width;
    size_t start;    // First line number
    size_t linecount;
    bool relative{false};    // Relative line number
};

class Window
{
private:
    size_t id;
    WINDOW* nwin;

public:
    StatusLine sline{};
    Margin margin{};
    CommandLine cline;
    BufferView* bview{};

public:
    Window();
    void destroy();

    void draw(const Context& context);
    void update(const Context& context);
    void changeBufferView(BufferView* bv);

    inline WINDOW* Nwin() const { return nwin; };
    inline BufferView& Bview() { return *bview; }
};
