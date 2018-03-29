#pragma once
#include "BufferView.h"
#include "common.h"
#include "context.h"
#include "window.h"


void draw_bview(WINDOW* nwin, const BufferView& bv,
                const Context& context);

void draw_margin(WINDOW* nwin, const Margin& mgn, Rect area,
                 const Context& context);

void draw_statusline(WINDOW* nwin, const StatusLine& sline,
                     Rect area, const Context& context);

void draw_cmdline(WINDOW* nwin, const CmdLine& cline,
                  Rect area, const Context& context);
