#include "common.h"
#include "log.h"

void printrect(const char* name, struct rect r)
{
    log_l(name, "{y: %d, x: %d, w: %d, h: %d}", r.y, r.x, r.width, r.height);
}
