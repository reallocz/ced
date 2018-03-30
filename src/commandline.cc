#include "commandline.h"


CommandLine::CommandLine()
{
    buffer[0] = '\0';
    cur       = 0;
}

void CommandLine::addCh(char ch)
{
    if (cur < CMDBUFCAP - 1) {
        buffer[cur++] = ch;
        buffer[cur] = '\0';
    }
}

void CommandLine::delCh()
{
    cur--;
    if (cur < 0) {
        cur = 0;
    }
    buffer[cur] = '\0';
}


void CommandLine::clear()
{
    cur = 0;
    buffer[cur] = '\0';
}

Command CommandLine::parse()
{
    return Command(buffer);
}

