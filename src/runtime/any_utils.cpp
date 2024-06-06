//
// Created by anthony on 5/14/24.
//

#include "any_utils.h"
#include <iostream>

double force_real(Any x) {
    if (is_real(x)) return to_real(x);
    else if (is_int(x)) return static_cast<double>(to_int(x));
    else type_error(x);
}


Any type_error(Any x) {
    std::cerr << "Any has incorrect type: " << get_type(x) << std::endl;
    exit(1);
}

Any type_error(Any x, const std::string& function) {
    std::cerr << "Any has incorrect type in function " << function << ": " << get_type(x) << std::endl;
    exit(1);
}

