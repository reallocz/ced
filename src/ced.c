#include "ced.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "log.h"
#include "window.h"
#include "input.h"
#include "buffer.h"
#include "term.h"
#include "input_keys.h"
#include "common.h"

#define TAG "CED"

/** Map for mode -> string */
static const char* mode_str[] = {
    "NORMAL",
    "INSERT"
};

static struct {
    int quit; // Return to main if 1
    struct window *win;
    struct context *context;

    struct buffer_view bv1;
    struct buffer_view bv2;
} G;


void ced_run()
{
    // Initialize globals
    G.quit = 0;
    term_update();

    // Context
    struct context* ctx = malloc(sizeof( struct context));
    assert(ctx);
    ctx->mode = MODE_NORMAL;
    ctx->modestr = mode_str[MODE_NORMAL];
    ctx->bounds = (struct rect) RECT(0, 0, term_cols(), term_rows());
    ctx->flags = 0;
    SETFLAG(ctx->flags, Farea_update);
    G.context = ctx;
    G.bv1 = bv_create(DOCUMENT, "interject.txt");
    G.bv2 = bv_create(DOCUMENT, "test.txt");

    // Create window and set buffer
    /*struct buffer_view bview = bv_create(SCRATCH, "interject.txt");*/
    /*struct buffer_view bview = bv_create( buf_create_test() );*/
    /*struct buffer_view bview = bv_create( buf_create_empty(DOCUMENT) );*/

    /*struct buffer_view bview = bv_create(buf_create_test());*/

    G.win = win_create(&G.bv1);

    // Main loop
    while(!G.quit) {
        // Check for window resize
        {
            term_update();
            struct rect newbounds = RECT(0, 0, term_cols(), term_rows());
            G.context->bounds = newbounds;
            log_l(TAG, "window resized!");
        }

        if(G.context->mode == MODE_NORMAL) {
            win_update(G.win, G.context);
            win_draw(G.win, G.context);
            inp_poll("NORMAL", G.win, ced_normal_input_cb);
        } else {
            win_update(G.win, G.context);
            win_draw(G.win, G.context);
            inp_poll("INSERT", G.win, ced_insert_input_cb);
        }

    }
}


void ced_insert_input_cb(inpev ev)
{
    struct buffer* buf = &G.win->bview->buffer;
    struct cursor cur = G.win->bview->cur;

    if(ev.type == INP_ALPHA || ev.type == INP_NUM
            || ev.type == INP_SYMBOL || ev.key == k_space
            || ev.key == k_enter)
    {
        buf_addch(buf, ev.key, cur);
        bv_cmov_fwd(G.win->bview, 1);
    }

    if(ev.type == INP_SPECIAL)
    {
        switch(ev.key) {
        case k_backspace:
            buf_delch(buf, cur);
            bv_cmov_back(G.win->bview, 1);
            break;
        case k_esc:
            G.context->mode = MODE_NORMAL;
            break;
        default:
            break;
        }
    }
}


void ced_normal_input_cb(inpev ev)
{
    /*struct buffer* buf = G.win->buffer;*/

    if(ev.key == 'i') {
        // Switch to insert mode
        G.context->mode = MODE_INSERT;
        return;
    }
    if (ev.key == k_f1) {
        G.quit = 1;
        return;
    }
    if(ev.key == k_f2) {
        buf_save_to_disk(&G.win->bview->buffer, "doc.txt"); // TODO prompt for name
        return;
    }
    if(ev.key == 'h') {
        bv_cmov_back(G.win->bview, 1);
        return;
    }
    if(ev.key == 'l') {
        bv_cmov_fwd(G.win->bview, 1);
        return;
    }
    if(ev.key == 'j') {
        bv_cmov_lnext(G.win->bview, 1);
        return;
    }
    if(ev.key == 'k') {
        bv_cmov_lprev(G.win->bview, 1);
        return;
    }
    if(ev.key == 'u') {
        bv_scrollup(G.win->bview, 1);
        return;
    }
    if(ev.key == 'd') {
        bv_scrolldown(G.win->bview, 1);
        return;
    }
    if(ev.key == '$') {
        bv_cmov_lend(G.win->bview);
        return;
    }
    if(ev.key == '0') {
        bv_cmov_lstart(G.win->bview);
        return;
    }
    if(ev.key == '1') {
        win_setbview(G.win, &G.bv1);
        return;
    }
    if(ev.key == '2') {
        win_setbview(G.win, &G.bv2);
        return;
    }
}

