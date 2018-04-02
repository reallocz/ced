#pragma once
#include "common.h"
#include <cstdlib>

class Line
{
    size_t id;
    size_t gapcol;
    size_t gaplen;
    size_t len; /** length of line (includes gapsize) */
    char* data; /** line data */

public:
    Line();
    Line(size_t len, char* data);

    // Operators
    const char operator[](std::size_t index) const;

    // Props
    auto Len() const { return len; }
    auto trueLen() const { return len - gaplen; }
    auto Id() const { return id; }
    auto gapCol() const { return gapcol; }
    auto gapLen() const { return gaplen; }

    // Return true if i is inside the gap
    bool inGap(size_t i) const;


    void addCh(char ch, const Cursor& cur);
    void delCh(const Cursor& cur);

    bool deleteGap();
    void clear();
    bool clearToEnd(size_t from);

    void pprint() const;

private:
    // Gap
    bool addGap();
    bool addGapOptional();
    bool moveGap(const size_t col);
};
