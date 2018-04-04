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
    auto length() const { return len; }
    auto trueLength() const { return len - gaplen; }
    auto Id() const { return id; }
    auto gapCol() const { return gapcol; }
    auto gapLen() const { return gaplen; }

    // Return true if i is inside the gap
    bool inGap(size_t i) const;

    // Create a new line from 'from' char to end
    Line split(size_t from);

    void addCh(char ch, const Cursor& cur);
    void delCh(const Cursor& cur);

    void clear();

    void pprint() const;

private:
    // Gap
    bool addGap();
    bool addGapOptional();
    bool moveGap(const size_t col);
};
