#pragma once
#include "common.h"

enum class Mode {
    Normal = 0,
    Insert,
    Command,
};

struct Context {
    unsigned long flags;
    enum Mode mode;
    const char* modestr;
    Rect bounds;

    void setMode(enum Mode mode);
};


inline void Context::setMode(enum Mode mode)
{
    this->mode = mode;
    switch (mode) {
    case Mode::Normal:
        modestr = "Normal";
        break;
    case Mode::Insert:
        modestr = "Insert";
        break;
    case Mode::Command:
        modestr = "Command";
        break;
    }
}
