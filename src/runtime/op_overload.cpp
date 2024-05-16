#pragma once

#include "any_utils.h"
#include "op_overload.h"
#include <iostream>


// Right now, we are ignoring the fact that strings and lists can be added to
Any operator+ (Any lhs, int64_t rhs) {
    if (is_int(lhs)) {
        return { to_int(lhs) + rhs };
    }
    else if (is_real(lhs)) {
        return { to_real(lhs) + static_cast<double>(rhs) };
    }
    else type_error(lhs);
}

Any operator+ (Any lhs, int rhs) {
    return lhs + static_cast<int64_t>(rhs);
}

Any operator+ (Any lhs, double rhs) {
    return { force_real(lhs) + rhs };
}

Any operator+ (Any lhs, Any rhs) {
    if (is_int(rhs)) {
        return lhs + to_int(rhs);
    }
    else if (is_real(rhs)) {
        return lhs + to_real(rhs);
    }
}

// Symmetrical + operators
Any operator+ (int64_t lhs, Any rhs) {
    return rhs + lhs;
}

Any operator+ (int lhs, Any rhs) {
    return rhs + lhs;
}

Any operator+ (double lhs, Any rhs) {
    return rhs + lhs;
}