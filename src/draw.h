#pragma once
#include "buffer_view.h"
#include "window.h"
#include "common.h"


void draw_bview(WINDOW* nwin, struct buffer_view bv, struct rect area,
        struct context* context);

void draw_margin(WINDOW* nwin, struct margin mgn, struct rect area,
        struct context* context);

void draw_statusline(WINDOW* nwin, struct statusline sline,
        struct rect area, struct context* context);
