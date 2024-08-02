//
// Created by anthony on 5/14/24.
//

#include "any_utils.h"
#include "csstring.h"
#include "symbol.h"
#include "array.h"
#include "dictionary.h"
#include <iostream>
#include <sstream>


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
    std::stringstream ss;
    switch (get_type(x)) {
        case Any_type::STR: return to_str(x);
        case Any_type::SYMBOL: return String {temp_str(to_symbol(x))};
        case Any_type::INT: return String {std::to_string(to_int(x))};
        case Any_type::REAL:
            ss << to_real(x);
            return String {ss.str()};
        case Any_type::ARRAY: return String {debug_str(to_array(x))};
        case Any_type::DICT: return String {debug_str(to_dict(x))};
        case Any_type::NIL: return String {"nil"};
        case Any_type::T: return String {"t"};
        case Any_type::OBJ: {
            return String {temp_str((reinterpret_cast<Obj*>(x.integer)->get_class_ptr()->get_name()))};
        }
        default: type_error(x);
    }
}


Any type_error(Any x) {
    throw std::runtime_error("Any has incorrect type: " + get_type_str(x));
}

Any type_error(Any x, const std::string& function) {
    std::cerr << "Any has incorrect type in function " << function << ": " << get_type_str(x) << std::endl;
    exit(1);
}

