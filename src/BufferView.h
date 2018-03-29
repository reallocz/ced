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
    Cursor cur;
    Buffer buffer{Buffer::Type::Scratch};
    Rect bounds;

public:
    BufferView();
    BufferView(enum Buffer::Type type, const char* filename);
    void update();

    inline Rect Bounds() const { return bounds; }
    void setBounds(Rect newbounds);

    inline Cursor getCursor() const { return cur; };
    void setCursor(Cursor newcur);

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
    Cursor relcur() const;
};
