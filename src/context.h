#pragma once
#include "common.h"


struct Context {
    enum mode mode;
    struct rect bounds;
    unsigned long flags;
};
