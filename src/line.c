#include "line.h"
#include "log.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define TAG "LINE"

static size_t ids = 0;


Line ln_create_empty(void)
{
    Line ln = {
        .id = ids++,
        .len = 0,
        .data = NULL,
        .gapcol = 0,
        .gapsize = 0,
    };
    return ln;
}


Line ln_create(size_t len, char* data)
{
    Line ln = ln_create_empty();
    ln.len = len;
    ln.data = data;
    return ln;
}


size_t ln_truelen(const Line* ln)
{
    assert(ln);
    return ln->len - ln->gapsize;
}


void ln_addch(Line* ln, char ch, struct cursor cur)
{
    ln_movegap(ln, cur);
    ln_addgap_optional(ln);

    ln->data[ln->gapcol] = ch;
    // shrink gap
    ln->gapcol++;
    ln->gapsize--;
}


void ln_delch(Line* ln, struct cursor cur)
{
    if (cur.col < 1) {
        return;
    }
    ln_movegap(ln, cur);
    ln->gapcol--;
    ln->gapsize++;
}


void ln_clear(Line* ln)
{
    assert(ln);
    ln->gapcol = 0;
    ln->gapsize = ln->len - 1;
}


Line ln_split(Line* ln, const struct cursor cur)
{
    assert(ln);
    assert(cur.col < ln_truelen(ln));
    ln_movegap(ln, cur);

    size_t newlen = ln_truelen(ln) - cur.col;
    char* newdata = malloc(newlen);
    assert(newdata);
    size_t count = 0;
    for(size_t i = cur.col; i < ln->len; ++i) {
        if(!INGAP(ln, i)) {
            newdata[count++] = ln->data[i];
        }
    }
    assert(count == newlen);

    // Create new line
    Line newline = ln_create(newlen, newdata);

    // Update current line
    ln->gapsize = ln->len - cur.col;

    return newline;
}


void ln_addgap(Line* ln)
{

    assert(ln);
    size_t newlinelen = ln->len + GAPSIZE;
    size_t newgapsize = ln->gapsize + GAPSIZE;

    char* newdata = calloc(newlinelen, sizeof(char));
    assert(newdata);

    /* Copy memory to new data */
    // from start to gap
    memcpy(newdata, ln->data, ln->gapcol * sizeof(char));
    // from end of gap to end of buffer
    memcpy(&newdata[ln->gapcol + newgapsize],
           &ln->data[ln->gapcol + ln->gapsize],
           ln->len - (ln->gapcol + ln->gapsize));
    // free old buffer
    free(ln->data);
    ln->data  = newdata;
    ln->len   = newlinelen;
    ln->gapsize = newgapsize;
}


int ln_addgap_optional(Line* ln)
{
    assert(ln);
    if (ln->gapsize < GAP_ADD_LIMIT) {
        size_t oldgap = ln->gapsize;
        ln_addgap(ln);
        log_l(TAG, "Gap added %d -> %d", oldgap, ln->gapsize);
        return 1;
    }
    return 0;
}


void ln_movegap(Line* ln, struct cursor cur)
{
    assert(ln);

    size_t offset = 0;
    int diff            = cur.col - ln->gapcol;

    if (diff > 0) {
        // Cursor is ahead of the gap
        offset = diff;
        for (size_t i = 0; i < offset; ++i) {
            ln->data[ln->gapcol] =
                ln->data[ln->gapcol + ln->gapsize];
            ln->gapcol++;
        }
        log_l(TAG, "%s: diff=%d, offset=%d", __func__, diff, offset);
    } else if (diff < 0) {
        // Cursor is behind the gap
        offset = -1 * diff;
        for (size_t i = 0; i < offset; ++i) {
            ln->data[ln->gapcol + ln->gapsize - 1] =
                ln->data[ln->gapcol - 1];
            ln->gapcol--;
        }
        log_l(TAG, "%s: diff=%d offset=%d", __func__, diff, offset);
    } else {
        // gap is in sync do nothing.
    }
    return;
}

