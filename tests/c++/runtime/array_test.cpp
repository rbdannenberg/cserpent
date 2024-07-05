//
// Created by anthony on 7/4/24.
//
#include "data_structures/array.h"
#include "op_overload.h"
#include <cassert>
#include <iostream>
#include <cstdint>

void test_array() {
    auto a = *(new Array {});
    auto b = *(new Array (4, 3));
    assert(len(a) == 0);
    assert(len(b) == 4);
    a.append(static_cast<int64_t>(1));
    a.append(static_cast<int64_t>(2));
    assert(len(a) == 2);
    assert(a[0] == static_cast<int64_t>(1));
    b.set(0, static_cast<int64_t>(9));
    assert(b[0] == static_cast<int64_t>(9));
}