#pragma once
#include "buffer_view.h"
#include "common.h"
#include "window.h"


void draw_bview(WINDOW* nwin, const struct buffer_view* bv,
                const struct context* context);

void draw_margin(WINDOW* nwin, struct margin mgn, struct rect area,
                 const struct cursor cur,
                 const struct context* context);

void draw_statusline(WINDOW* nwin, struct statusline sline,
                     struct rect area, const struct context* context);

void draw_cmdline(WINDOW* nwin, struct cmdline cline,
                  struct rect area, const struct context* context);
