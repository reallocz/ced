#include "ced.h"
#include "buffer.h"
#include "commands.h"
#include "common.h"
#include "input.h"
#include "input_keys.h"
#include "log.h"
#include "term.h"
#include "window.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define TAG "CED"

Ced::Ced(Opts opts)
{
    parseOpts(opts);

    quit = 0;
    Term::update();

    // Create context
    context.setMode(Mode::Normal);
    context.bounds = (Rect) RECT(0, 0, Term::cols, Term::rows);
    context.flags  = 0;
    SETFLAG(context.flags, Farea_update);
}


Ced::~Ced() = default;


void Ced::parseOpts(Opts opts)
{
    // Set up BufferViews
    if (opts.bcount == 0) {
        // Init scratch buffer
        bcount    = 1;
        bviews[0] = BufferView(Buffer::Type::Scratch, TEXTPATH "scratch.txt");
    } else {
        static_assert(opts.bviews);
        bcount = opts.bcount;
        for (int i = 0; i < bcount; ++i) {
            bviews[i] = opts.bviews[i];
        }
    }

    currentBview = 0;
}


void Ced::run()
{
    // set buffer
    win.changeBufferView(&bviews[0]);

    // Main loop
    while (!quit) {
        // Check for window resize
        {
            Term::update();
            Rect newbounds = RECT(0, 0, Term::cols, Term::rows);
            context.bounds        = newbounds;
        }

        win.update(context);
        win.draw(context);

        inpev ev = inp_poll(win);
        if (context.mode == Mode::Normal) {
            normalCb(ev);
        } else if (context.mode == Mode::Insert) {
            insertCb(ev);
        } else if (context.mode == Mode::Command) {
            commandCb(ev);
        }
    }
}


void Ced::insertCb(inpev ev)
{
    //Buffer* buf       = &win.Bview().buffer;
    //Cursor cur = win.Bview().cur;

    //if (ev.type == INP_ALPHA || ev.type == INP_NUM || ev.type == INP_SYMBOL || ev.key == k_space || ev.key == k_enter) {
        //buf->addCh(ev.key, cur);
        //win.Bview().cmovFwd(1);
    //}

    //if (ev.type == INP_SPECIAL) {
        //switch (ev.key) {
        //case k_backspace:
            //buf->delCh(cur);
            //win.Bview().cmovBack(1);
            //break;
        //case k_esc:
            //context.setMode(Mode::Normal);
            //break;
        //default:
            //break;
        //}
    //}
}


void Ced::normalCb(inpev ev)
{
    if (ev.key == 'i') {
        // Switch to insert mode
        context.setMode(Mode::Insert);
    } else if (ev.key == ':') {
        // Switch to comman mode
        context.setMode(Mode::Command);
    } else if (ev.key == k_f1) {
        quit = 1;
    } else if (ev.key == k_f2) {
        //win.Bview().buffer.saveToDisk("doc.txt");
        // TODO(realloc): prompt for name
    } else if (ev.key == 'h') {
        win.Bview().cmovBack(1);
    } else if (ev.key == 'l') {
        win.Bview().cmovFwd(1);
    } else if (ev.key == 'j') {
        win.Bview().cmovLnext(1);
    } else if (ev.key == 'k') {
        win.Bview().cmovLprev(1);
    } else if (ev.key == 'u') {
        win.Bview().scrollUp(1);
    } else if (ev.key == 'd') {
        win.Bview().scrollDown(1);
    } else if (ev.key == '$') {
        win.Bview().cmovLend();
    } else if (ev.key == '0') {
        win.Bview().cmovLstart();
    } else if (ev.key == '1') {
        nextBview();
        win.changeBufferView(&bviews[currentBview]);
    }
}


void Ced::commandCb(inpev ev)
{
    static int i = 0;
    if (ev.key == k_esc) {
        // Switch to normal mode
        context.setMode(Mode::Normal);
    } else if (ev.key == k_enter) {
        // Execute command and clear cmdline buffer
        execCommand(cmd_parse_string(win.cmdline.buffer));
        // Reset buffer
        i                     = 0;
        win.cmdline.buffer[i] = '\0';
        context.setMode(Mode::Normal);
    } else if (ev.type == INP_ALPHA || ev.type == INP_NUM || ev.type == INP_SYMBOL || ev.key == k_space) {
        win.cmdline.buffer[i++] = ev.key;
        win.cmdline.buffer[i]   = '\0';
    } else if (ev.key == k_backspace) {
        i--;
        if (i < 0) {
            i = 0;
        }
        win.cmdline.buffer[i] = '\0';
    } else {
        // Go back to Mode::Normal
        context.setMode(Mode::Normal);
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
        Buffer& buffer = win.Bview().getBuffer();
        if (strlen(cmd.args) == 0) {
            // Save
            buffer.saveToDisk();
        } else {
            // Save As arg
            buffer.saveToDiskAs(cmd.args);
            // TODO(realloc): add error checking
            // TODO(realloc): update buffer name to arg
        }
        return;
    }
}
