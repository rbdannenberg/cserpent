#pragma once

#include "any.h"
#include "any_utils.h"
#include <iostream>


Any operator+ (Any lhs, int64_t rhs) {
    if (is_int(lhs)) {
        return constructor(to_int(lhs) + rhs);
    }
    else if (is_real(lhs)) {
        return constructor(to_real(lhs) + static_cast<double>(rhs));
    }
    else {
        std::cerr << "Any has un-addable type" << std::endl;
        exit(1);
    }
}

Any operator+ (Any lhs, double rhs) {
    return constructor(force_real(lhs) + rhs);
}

Any operator+ (Any lhs, Any rhs) {
    if (is_int(rhs)) {
        return lhs + to_int(rhs);
    }
    else if (is_real(rhs)) {
        return lhs + to_real(rhs);
    }
}