#pragma once
#include "buffer_view.h"
#include "window.h"
#include "common.h"


void draw_bview(WINDOW* nwin, const struct buffer_view* bv,
        const struct context* context);

void draw_margin(WINDOW* nwin, struct margin mgn, struct rect area,
        const struct context* context);

void draw_statusline(WINDOW* nwin, struct statusline sline,
        struct rect area, const struct context* context);
