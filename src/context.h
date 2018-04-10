#pragma once
#include "common.h"
#include "log.h"

struct context {
    enum mode mode;
    struct rect bounds;
    unsigned long flags;
};

inline void ctx_pprint(struct context* ctx)
{
    log_l("CONTEXT", "Context {mode: %d, flags: %d", ctx->mode,
          ctx->flags);
}
