// addone.cpp - separate compilation to block compiler from realizing the whole
// loop can be evaluated at compile time

#include <stdlib.h>
#include <stdint.h>
#include "any_compare.h"

Anyu anyu_temp;

int64_t addone_baseline(int64_t a)
{
    return a + 1;
}

Any addone_uint(Any a)
{
    if (ANY_IS_INT(a)) {
        return INT2ANY(ANY_AS_INT(a) + 1);
    } else {
        exit(1);
    }
}

Anyu addone_union(Anyu a)
{
    if (ANYU_IS_INT(a)) {
        return INT2ANYU(ANYU_AS_INT(a) + 1);
    } else {
        exit(1);
    }
}

Anyu2 addone_union2(Anyu2 a)
{
    if (ANYU2_IS_INT(a)) {
        return INT2ANYU2(ANYU2_AS_INT(a) + 1);
    } else {
        exit(1);
    }
}
