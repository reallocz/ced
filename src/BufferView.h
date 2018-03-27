#pragma once
#include "buffer.h"
#include "common.h"
#include "ncurses.h"

/** buffer_view.h
 * represents a part of a buffer */

class BufferView
{
public:
    unsigned int start;    // View starts from
    struct cursor cur;
    Buffer buffer{Buffer::Type::Scratch};
    struct rect bounds;

public:
    BufferView();
    BufferView(enum Buffer::Type type, const char* filename);
    void update();

    inline struct rect Bounds() const { return bounds; }
    void setBounds(struct rect newbounds);

    inline struct cursor Cursor() const { return cur; };
    void setCursor(struct cursor newcur);

    void cmovFwd(unsigned int n);
    void cmovBack(unsigned int n);

    void cmovLnext(unsigned int n);
    void cmovLprev(unsigned int n);
    void cmovLstart();
    void cmovLend();

    void cmovInline();

    void scrollUp(unsigned int n);
    void scrollDown(unsigned int n);

    inline unsigned int Start() const { return start; }
    struct cursor relcur() const;
};
