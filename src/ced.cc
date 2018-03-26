#include "ced.h"
#include "buffer.h"
#include "commands.h"
#include "common.h"
#include "input.h"
#include "input_keys.h"
#include "log.h"
#include "term.h"
#include "window.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAG "CED"

Ced::Ced(Opts opts)
{
    parseOpts(opts);

    quit = 0;
    Term::update();

    // Create context
    context.mode   = MODE_NORMAL;
    context.bounds = (struct rect) RECT(0, 0, Term::cols, Term::rows);
    context.flags  = 0;
    SETFLAG(context.flags, Farea_update);
}


Ced::~Ced()
{
}


void Ced::parseOpts(Opts opts)
{
    // Set up buffer_views
    if (opts.bcount == 0) {
        // Init scratch buffer
        bcount    = 1;
        bviews[0] = bv_create(SCRATCH, TEXTPATH "scratch.txt");
    } else {
        assert(opts.bviews);
        bcount = opts.bcount;
        for (int i = 0; i < bcount; ++i) {
            bviews[i] = opts.bviews[i];
        }
    }

    currentBview = 0;
}


void Ced::run()
{
    // Create window and set buffer
    win = win_create(&bviews[0]);

    // Main loop
    while (!quit) {
        // Check for window resize
        {
            Term::update();
            struct rect newbounds = RECT(0, 0, Term::cols, Term::rows);
            context.bounds        = newbounds;
        }

        win_update(win, context);
        win_draw(win, context);

        inpev ev = inp_poll(win);
        if (context.mode == MODE_NORMAL) {
            normalCb(ev);
        } else if (context.mode == MODE_INSERT) {
            insertCb(ev);
        } else if (context.mode == MODE_COMMAND) {
            commandCb(ev);
        }
    }
}


void Ced::insertCb(inpev ev)
{
    struct buffer* buf = &win->bview->buffer;
    struct cursor cur  = win->bview->cur;

    if (ev.type == INP_ALPHA || ev.type == INP_NUM || ev.type == INP_SYMBOL || ev.key == k_space || ev.key == k_enter) {
        buf_addch(buf, ev.key, cur);
        bv_cmov_fwd(win->bview, 1);
    }

    if (ev.type == INP_SPECIAL) {
        switch (ev.key) {
        case k_backspace:
            buf_delch(buf, cur);
            bv_cmov_back(win->bview, 1);
            break;
        case k_esc:
            context.mode = MODE_NORMAL;
            break;
        default:
            break;
        }
    }
}


void Ced::normalCb(inpev ev)
{
    /*struct buffer* buf = win->buffer;*/

    if (ev.key == 'i') {
        // Switch to insert mode
        context.mode = MODE_INSERT;
    } else if (ev.key == ':') {
        // Switch to comman mode
        context.mode = MODE_COMMAND;
    } else if (ev.key == k_f1) {
        quit = 1;
    } else if (ev.key == k_f2) {
        buf_save_to_disk(&win->bview->buffer, "doc.txt");
        // TODO(realloc): prompt for name
    } else if (ev.key == 'h') {
        bv_cmov_back(win->bview, 1);
    } else if (ev.key == 'l') {
        bv_cmov_fwd(win->bview, 1);
    } else if (ev.key == 'j') {
        bv_cmov_lnext(win->bview, 1);
    } else if (ev.key == 'k') {
        bv_cmov_lprev(win->bview, 1);
    } else if (ev.key == 'u') {
        bv_scrollup(win->bview, 1);
    } else if (ev.key == 'd') {
        bv_scrolldown(win->bview, 1);
    } else if (ev.key == '$') {
        bv_cmov_lend(win->bview);
    } else if (ev.key == '0') {
        bv_cmov_lstart(win->bview);
    } else if (ev.key == '1') {
        nextBview();
        win_setbview(win, &bviews[currentBview]);
    }
}


void Ced::commandCb(inpev ev)
{
    static int i = 0;
    if (ev.key == k_esc) {
        // Switch to normal mode
        context.mode = MODE_NORMAL;
    } else if (ev.key == k_enter) {
        // Execute command and clear cmdline buffer
        execCommand(cmd_parse_string(win->cmdline.buffer));
        // Reset buffer
        i                      = 0;
        win->cmdline.buffer[i] = '\0';
        context.mode           = MODE_NORMAL;
    } else if (ev.type == INP_ALPHA || ev.type == INP_NUM || ev.type == INP_SYMBOL || ev.key == k_space) {
        win->cmdline.buffer[i++] = ev.key;
        win->cmdline.buffer[i]   = '\0';
    } else if (ev.key == k_backspace) {
        i--;
        if (i < 0) {
            i = 0;
        }
        win->cmdline.buffer[i] = '\0';
    } else {
        // Go back to MODE_NORMAL
        context.mode = MODE_NORMAL;
    }
}


void Ced::nextBview()
{
    currentBview++;
    if (currentBview >= bcount) {
        currentBview = 0;
    }
}

void Ced::execCommand(struct command cmd)
{
    cmd_pprint(cmd);
    if (!cmd.valid) {
        log_e(TAG, "Unknown command: %s", cmd.cmdstr);
        return;
    }


    if (cmd.type == CMD_BUFSAVE) {
        if (strlen(cmd.args) == 0) {
            // Save
            buf_save_to_disk(&win->bview->buffer,
                             win->bview->buffer.name);
        } else {
            // Save As arg
            buf_save_to_disk(&win->bview->buffer,
                             cmd.args);
            // TODO add error checking
            // TODO update buffer name to arg
        }
        return;
    }
}
