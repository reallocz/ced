#pragma once
#include "commands.h"

#define CMDBUFCAP 128

class CommandLine
{
    char buffer[CMDBUFCAP]{};
    unsigned int cur;
public:
    CommandLine();
    void addCh(char ch);
    void delCh();
    void clear();
    Command parse();
    const char* data() const { return buffer; }
    unsigned int getCursor() const { return cur; }
};
