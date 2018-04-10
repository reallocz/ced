#include "ced.h"
#include "buffer.h"
#include "commandline.h"
#include "common.h"
#include "context.h"
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

static struct {
    int quit;    // Return to main if 1
    struct window* win;
    struct context* context;

    struct buffer_view bviews[BVIEW_LIMIT];
    size_t bcount;
} G;


/** Return 1 on parse error */
static int ced_parseopts(struct cedopts opts)
{
    // Set up buffer_views
    if (opts.bcount == 0) {
        // Init scratch buffer
        G.bcount    = 1;
        G.bviews[0] = bv_create(SCRATCH, TEXTPATH "scratch.txt");
    } else {
        assert(opts.bviews);
        G.bcount = opts.bcount;
        for (size_t i = 0; i < G.bcount; ++i) {
            G.bviews[i] = opts.bviews[i];
        }
    }
    return 0;
}

void ced_init(struct cedopts opts)
{
    if (ced_parseopts(opts) != 0) {
        log_l(TAG, "Invalid opts. quitting");
        // TODO(realloc): more diag
        return;
    }

    // Initialize globals
    G.quit = 0;

    term_update();

    // Create context
    struct context* ctx = malloc(sizeof(struct context));
    assert(ctx);
    ctx->mode   = MODE_NORMAL;
    ctx->bounds = (struct rect) RECT(0, 0, term_cols(), term_rows());
    ctx->flags  = 0;
    SETFLAG(ctx->flags, Farea_update);
    G.context = ctx;
}

static size_t currentbview = 0;
static struct buffer_view* next_bview()
{
    currentbview++;
    if (currentbview >= G.bcount) {
        currentbview = 0;
    }
    return &G.bviews[currentbview];
}

void ced_run(void)
{

    // Create window and set buffer
    G.win = win_create(&G.bviews[0]);

    // Main loop
    while (!G.quit) {
        // Check for window resize
        {
            term_update();
            struct rect newbounds =
                RECT(0, 0, term_cols(), term_rows());
            G.context->bounds = newbounds;
        }

        win_update(G.win, G.context);
        win_draw(G.win, G.context);

        if (G.context->mode == MODE_NORMAL) {
            inp_poll("NORMAL", G.win, ced_normal_input_cb);
        } else if (G.context->mode == MODE_INSERT) {
            inp_poll("INSERT", G.win, ced_insert_input_cb);
        } else if (G.context->mode == MODE_COMMAND) {
            inp_poll("INSERT", G.win, ced_command_input_cb);
        }
    }
}


void ced_insert_input_cb(inpev ev)
{
    struct buffer* buf = &G.win->bview->buffer;
    struct cursor cur  = G.win->bview->cur;

    if (ev.type == INP_ALPHA || ev.type == INP_NUM ||
        ev.type == INP_SYMBOL || ev.key == k_space ||
        ev.key == k_enter) {
        buf_addch(buf, ev.key, cur);
        bv_cmov_fwd(G.win->bview, 1);
    }

    if (ev.type == INP_SPECIAL) {
        switch (ev.key) {
        case k_backspace:
            buf_delch(buf, cur);
            bv_cmov_back(G.win->bview, 1);
            break;
        case k_esc: G.context->mode = MODE_NORMAL; break;
        default: break;
        }
    }
}


void ced_normal_input_cb(inpev ev)
{
    /*struct buffer* buf = G.win->buffer;*/

    if (ev.key == 'i') {
        // Switch to insert mode
        G.context->mode = MODE_INSERT;
    } else if (ev.key == ':') {
        // Switch to comman mode
        G.context->mode = MODE_COMMAND;
    } else if (ev.key == k_f1) {
        G.quit = 1;
    } else if (ev.key == k_f2) {
        buf_save_to_disk(&G.win->bview->buffer, "doc.txt");
        // TODO(realloc): prompt for name
    } else if (ev.key == 'h') {
        bv_cmov_back(G.win->bview, 1);
    } else if (ev.key == 'l') {
        bv_cmov_fwd(G.win->bview, 1);
    } else if (ev.key == 'j') {
        bv_cmov_lnext(G.win->bview, 1);
    } else if (ev.key == 'k') {
        bv_cmov_lprev(G.win->bview, 1);
    } else if (ev.key == 'u') {
        bv_scrollup(G.win->bview, 1);
    } else if (ev.key == 'd') {
        bv_scrolldown(G.win->bview, 1);
    } else if (ev.key == '$') {
        bv_cmov_lend(G.win->bview);
    } else if (ev.key == '0') {
        bv_cmov_lstart(G.win->bview);
    } else if (ev.key == '1') {
        win_setbview(G.win, next_bview());
    } else if (ev.key == 'R') {
        G.win->margin.relative = !G.win->margin.relative;
    }
}

// Proto
static void exec_command(struct command cmd);

void ced_command_input_cb(inpev ev)
{
    struct cmdline* cl = &G.win->cmdline;

    if (ev.key == k_esc) {
        // Switch to normal mode
        G.context->mode = MODE_NORMAL;
    } else if (ev.key == k_enter) {
        // Execute command and clear cmdline buffer
        exec_command(cmd_parse(cl));
        // Reset buffer
        cmd_clear(cl);
        G.context->mode = MODE_NORMAL;
    } else if (ev.type == INP_ALPHA || ev.type == INP_NUM ||
               ev.type == INP_SYMBOL || ev.key == k_space) {
        cmd_addch(cl, ev.key);
    } else if (ev.key == k_backspace) {
        cmd_delch(cl);
    } else {
        // Go back to MODE_NORMAL
        G.context->mode = MODE_NORMAL;
    }
}


static void exec_command(struct command cmd)
{
    cmd_pprint(cmd);
    if (!cmd.valid) {
        log_e(TAG, "Unknown command: %s", cmd.cmdstr);
        return;
    }

    if (cmd.type == CMD_QUIT) {
        G.quit = 1;
    } else if (cmd.type == CMD_BUFSAVE) {
        if (strlen(cmd.args) == 0) {
            // Save
            buf_save_to_disk(&G.win->bview->buffer,
                             G.win->bview->buffer.name);
        } else {
            // Save As arg
            buf_save_to_disk(&G.win->bview->buffer, cmd.args);
            // TODO(realloc): add error checking
            // TODO(realloc): update buffer name to arg
        }
        return;
    }
}
