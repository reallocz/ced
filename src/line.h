#pragma once
#include "common.h"
#include <stddef.h>

#define GAPSIZE 4
/** Add gap if gap size is less than this limit */
#define GAP_ADD_LIMIT 1

#define INGAP(lnptr, i) \
    ((i >= lnptr->gapcol && \
      i < lnptr->gapcol + lnptr->gapsize))

typedef struct line {
    int id;
    size_t len; /** length of line */
    char* data;       /** line data */
    size_t gapcol;
    size_t gapsize;
} Line;


Line ln_create_empty(void);
Line ln_create(size_t len, char* data);

size_t ln_truelen(const Line* ln);

void ln_addch(Line* ln, char ch, struct cursor cur);
void ln_delch(Line* ln, struct cursor cur);
void ln_clear(Line* ln);

void ln_addgap(Line* ln);
int ln_addgap_optional(Line* ln);
void ln_movegap(Line* ln, struct cursor cur);

void ln_pprint(Line* ln);


