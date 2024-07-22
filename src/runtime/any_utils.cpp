//
// Created by anthony on 5/14/24.
//

#include "any_utils.h"
#include "csstring.h"
#include "symbol.h"
#include <iostream>


double force_real(Any x) {
    if (is_real(x)) return to_real(x);
    else if (is_int(x)) return static_cast<double>(to_int(x));
    else type_error(x);
}

int64_t force_int(Any x) {
    if (is_int(x)) return to_int(x);
    else if (is_real(x)) return static_cast<int64_t>(to_real(x));
    else if (is_str(x)) {
        std::cerr << "String to integer conversion not implemented" << std::endl;
    }
    else type_error(x);
}

String force_str(Any x) {
    if (is_str(x)) return to_str(x);
    else if (is_symbol(x)) return String {temp_str(to_symbol(x))};
    else if (is_int(x)) return String {std::to_string(to_int(x))};
    else if (is_real(x)) return String {std::to_string(to_real(x))};
    else if (is_ptr(x)) {
        std::cerr << "Pointer to string conversion not implemented" << std::endl;
    }
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

