// any_compare.cpp - benchmark for uint64_t and union representation of Any
//
// test will be calling a function that adds 1 to an Any parameter
// and returns Any, with all type tests and conversions as macros, but
// comparing Any as uint64_t vs. Anyu as union type.

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#define _POSIX_C_SOURCE 199309L
#include <time.h>

#ifndef CLOCK_MONOTONIC_RAW
#define CLOCK_MONOTONIC_RAW CLOCK_MONOTONIC
#endif

typedef uint64_t Any;

#define INT_TAG 0xFFFC000000000000uLL
#define ANY_IS_INT(a) (((uint64_t) (a) & INT_TAG) == INT_TAG)
#define ANY_AS_INT(a) ((int64_t) (((a) << 14) >> 14))
#define INT2ANY(a) ((a) | INT_TAG)

typedef union { int64_t si; uint64_t ui; double d; } Anyu;
Anyu anyu_temp;
#define ANYU_IS_INT(a) (((a).ui & INT_TAG) == INT_TAG)
#define ANYU_AS_INT(a) (((a).si << 14) >> 14)
#define INT2ANYU(a) (anyu_temp.si = (int64_t) ((a) | INT_TAG), anyu_temp)
// This would be better if we could treat integer a as an Anyu:
// #define INT2ANYU(a) ((Anyu) ((a) | INT_TAG))

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

int main()
{
    struct timespec t1, t2;

    clock_gettime(CLOCK_MONOTONIC_RAW, &t1);
    int64_t ab = 0;
    for (int64_t i = 0; i < 1000000000; i++) {
        ab = addone_baseline(ab);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &t2);
    double usec = ((uint64_t) t2.tv_sec - (uint64_t) t1.tv_sec)* 1000000 +
            ((uint64_t) t2.tv_nsec) * 0.001 - ((uint64_t) t1.tv_nsec) * 0.001;
    printf("Final value: %lld\n", ANY_AS_INT(ab));
    printf("Runtime baseline: %g msec\n", usec / 1000.0);

    clock_gettime(CLOCK_MONOTONIC_RAW, &t1);
    Any ai = INT2ANY(0);
    for (int64_t i = 0; i < 1000000000; i++) {
        ai = addone_uint(ai);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &t2);
    usec = ((uint64_t) t2.tv_sec - (uint64_t) t1.tv_sec)* 1000000 +
            ((uint64_t) t2.tv_nsec) * 0.001 - ((uint64_t) t1.tv_nsec) * 0.001;
    printf("Final value: %lld\n", ANY_AS_INT(ai));
    printf("Runtime with uint64: %g msec\n", usec / 1000.0);
    
    clock_gettime(CLOCK_MONOTONIC_RAW, &t1);
    Anyu au = INT2ANYU(0);
    for (int64_t i = 0; i < 1000000000; i++) {
        au = addone_union(au);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &t2);
    usec = ((uint64_t) t2.tv_sec - (uint64_t) t1.tv_sec)* 1000000 +
            ((uint64_t) t2.tv_nsec) * 0.001 - ((uint64_t) t1.tv_nsec) * 0.001;
    printf("Final value: %lld\n", ANYU_AS_INT(au));
    printf("Runtime with union: %g msec\n", usec / 1000.0);

}
