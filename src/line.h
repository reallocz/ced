#pragma once
#include <cstdlib>
#include "common.h"

class Line
{
    unsigned int id;
    unsigned int gapcol;
    unsigned int gaplen;
    unsigned int len; /** length of line (includes gapsize) */
    char* data;       /** line data */

public:
    Line();
    Line(unsigned int len, char* data);

    // Length
    unsigned int Len() const { return len; }
    unsigned int trueLen() const { return len - gaplen; }
    unsigned int Id() { return id; }

    // Return true if i is inside the gap
    bool inGap(unsigned int i) const;

    // Operators
    const char operator[](std::size_t index) const;

    void addCh(char ch, const Cursor& cur);
    void delCh(const Cursor& cur);
    void clear();

    void pprint() const;
private:
    // Gap
    bool addGap();
    bool addGapOptional();
    bool moveGap(const Cursor& cur);
};

