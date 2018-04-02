#pragma once
#include "buffer.h"
#include "common.h"
#include "ncurses.h"

/** buffer_view.h
 * represents a part of a buffer */

class BufferView
{
private:
    size_t start;    // View starts from
    Cursor cur{};
    Buffer buffer{Buffer::Type::Scratch};
    Rect bounds{};

public:
    BufferView();
    BufferView(enum Buffer::Type type, const char* filename);
    void update();

    void cmovFwd(size_t n);
    void cmovBack(size_t n);

    void cmovLnext(size_t n);
    void cmovLprev(size_t n);
    void cmovLstart();
    void cmovLend();

    void cmovInline();

    void scrollUp(size_t n);
    void scrollDown(size_t n);

    Cursor relcur() const;

    // Accessors
    inline size_t getStart() const { return start; }

    inline Rect getBounds() const { return bounds; }
    void setBounds(Rect newbounds);

    inline Cursor getCursor() const { return cur; };
    void setCursor(Cursor newcur);

    inline Buffer& getBuffer() { return buffer; };
    inline const Buffer& getBuffer() const { return buffer; }

    const Line& currentLine() const
    {
        return buffer.getLine(cur.line);
    }
    Line& currentLine() { return buffer.getLine(cur.line); }
};
