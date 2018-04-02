#include "ced.h"
#include "buffer.h"
#include "common.h"
#include "input.h"
#include "input_keys.h"
#include "log.h"
#include "term.h"
#include "window.h"
#include "fileutils.h"
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
    BufferView& bview = win.Bview();
    const Cursor& cur = bview.getCursor();

    if (ev.type == INP_ALPHA || ev.type == INP_NUM || ev.type == INP_SYMBOL || ev.key == k_space) {
        bview.currentLine().addCh(ev.key, cur);
        bview.cmovFwd(1);
    } else if (ev.key == k_enter) {
        bview.getBuffer().splitLine(cur);
    } else if (ev.type == INP_SPECIAL) {
        switch (ev.key) {
        case k_backspace:
            bview.currentLine().delCh(cur);
            bview.cmovBack(1);
            break;
        case k_esc:
            context.setMode(Mode::Normal);
            break;
        default:
            break;
        }
    }
}


void Ced::normalCb(inpev ev)
{
    BufferView& bview = win.Bview();
    if (ev.key == 'i') {
        // Switch to insert mode
        context.setMode(Mode::Insert);
    } else if (ev.key == ':') {
        // Switch to comman mode
        context.setMode(Mode::Command);
    } else if (ev.key == k_f1) {
        quit = 1;
    } else if (ev.key == k_f2) {
        FileUtil::saveBuffer(win.Bview().getBuffer(), "doc.txt");
    } else if (ev.key == 'h') {
        bview.cmovBack(1);
    } else if (ev.key == 'l') {
        bview.cmovFwd(1);
    } else if (ev.key == 'j') {
        bview.cmovLnext(1);
    } else if (ev.key == 'k') {
        bview.cmovLprev(1);
    } else if (ev.key == 'u') {
        bview.scrollUp(1);
    } else if (ev.key == 'd') {
        bview.scrollDown(1);
    } else if (ev.key == '$') {
        bview.cmovLend();
    } else if (ev.key == '0') {
        bview.cmovLstart();
    } else if (ev.key == '1') {
        nextBview();
        win.changeBufferView(&bviews[currentBview]);
    } else if (ev.key == 'r') {
        // Toggle relative line numbers
        win.margin.relative = !win.margin.relative;
    } else if (ev.key == 'D') {
        bview.currentLine().deleteGap();
        //bview.currentLine().clear();
    }
}


void Ced::commandCb(inpev ev)
{
    if (ev.key == k_esc) {
        // Switch to normal mode
        context.setMode(Mode::Normal);
    } else if (ev.key == k_enter) {
        // Execute command and clear cmdline buffer
        Command cmd = win.cline.parse();
        cmd.pprint();
        if (!cmd.valid) {
            log_e(TAG, "Unknown command: %s", win.cline.data());
        } else {
            execCommand(cmd);
        }
        // Reset cmdline
        win.cline.clear();
        context.setMode(Mode::Normal);
    } else if (ev.type == INP_ALPHA || ev.type == INP_NUM || ev.type == INP_SYMBOL || ev.key == k_space) {
        win.cline.addCh(ev.key);
    } else if (ev.key == k_backspace) {
        win.cline.delCh();
    } else {
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

void Ced::execCommand(const Command& cmd)
{
    if (cmd.type == Command::Type::Bufsave) {
        Buffer& buffer = win.Bview().getBuffer();
        if (strlen(cmd.args) == 0) {
            // Save
            FileUtil::saveBuffer(buffer, buffer.getName());
        } else {
            // Save As arg
            FileUtil::saveBuffer(buffer, cmd.args);
            // TODO(realloc): add error checking
            // TODO(realloc): update buffer name to arg
        }
        return;
    }
}
