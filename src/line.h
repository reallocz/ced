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

    unsigned int Len() const { return len; }
    unsigned int trueLen() const { return len - gaplen; }

    const char operator[](std::size_t index) const;
    char& operator[](std::size_t index);

    bool inGap(unsigned int i) const;

    void pprint() const;
    unsigned int Id() { return id; }

    void addCh(char ch, const Cursor& cur);
    void delCh(const Cursor& cur);

private:
    bool addGap();
    bool addGapOptional();
    bool moveGap(const Cursor& cur);
};
