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
#include "any_compare.h"

#define ITERATIONS 10000000000

#ifndef CLOCK_MONOTONIC_RAW
#define CLOCK_MONOTONIC_RAW CLOCK_MONOTONIC
#endif


int main()
{
    struct timespec t1, t2;

    // warmup
    int64_t ab = 0;
    for (int64_t i = 0; i < 1000000; i++) {
        ab = addone_baseline(ab);
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &t1);
    ab = 0;
    for (int64_t i = 0; i < ITERATIONS; i++) {
        ab = addone_baseline(ab);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &t2);
    double usec = ((uint64_t) t2.tv_sec - (uint64_t) t1.tv_sec)* 1000000 +
            ((uint64_t) t2.tv_nsec) * 0.001 - ((uint64_t) t1.tv_nsec) * 0.001;
    printf("Final value: %lld\n", ANY_AS_INT(ab));
    printf("Runtime baseline: %g msec\n", usec / 1000.0);

    // warmup
    Any ai = INT2ANY(0);
    for (int64_t i = 0; i < 1000000; i++) {
        ai = addone_uint(ai);
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &t1);
    ai = INT2ANY(0);
    for (int64_t i = 0; i < ITERATIONS; i++) {
        ai = addone_uint(ai);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &t2);
    usec = ((uint64_t) t2.tv_sec - (uint64_t) t1.tv_sec)* 1000000 +
            ((uint64_t) t2.tv_nsec) * 0.001 - ((uint64_t) t1.tv_nsec) * 0.001;
    printf("Final value: %lld\n", ANY_AS_INT(ai));
    printf("Runtime with uint64: %g msec\n", usec / 1000.0);
    
    // warmup
    Anyu au0 = INT2ANYU(0);
    for (int64_t i = 0; i < 1000000; i++) {
        au0 = addone_union(au0);
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &t1);
    Anyu au = INT2ANYU(0);
    for (int64_t i = 0; i < ITERATIONS; i++) {
        au = addone_union(au);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &t2);
    usec = ((uint64_t) t2.tv_sec - (uint64_t) t1.tv_sec)* 1000000 +
            ((uint64_t) t2.tv_nsec) * 0.001 - ((uint64_t) t1.tv_nsec) * 0.001;
    printf("Final value: %lld\n", ANYU_AS_INT(au));
    printf("Runtime with union: %g msec\n", usec / 1000.0);

}
