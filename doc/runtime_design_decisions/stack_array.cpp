//
// Created by anthony on 6/26/24.
//
#include "any.h"

/// @file This is an example to demonstrate the challenge of allocating CSerpent arrays on the stack.

Any some_function(void) {
    Any a = std::vector<int> {1, 2, 3};
    return a;
}

/// a has to be 64 bits in size, which std::vector is not. If it is a pointer:

Any some_function1(void) {
    std::vector<int> a {1, 2, 3};
    return &a;
}

/// This is a problem because a is allocated on the stack and will be deallocated when the function returns.
/// Returning a pointer to a stack-allocated variable is a bad idea.

// How about move semantics? Can a rvalue reference fit in a 64 bit variable?
Any some_function2(void) {
    std::vector<int> a {1, 2, 3};
    return std::move(a);
}
