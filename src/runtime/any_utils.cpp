//
// Created by anthony on 5/14/24.
//

#include "any.h"
#include "gc.h"
#include "heap_obj.h"
#include "obj.h"
#include "op_overload.h"
#include "any_utils.h"
#include "csstring.h"
#include "symbol.h"
#include "array.h"
#include "dict.h"
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

Any force_str(Any x) {
    switch (get_type(x)) {
        case Any_type::STRING: return x;
        case Any_type::SYMBOL: return to_symbol(x)->name();
        case Any_type::INT: {
            std::string ias(std::to_string(to_int(x)));
            return Any(ias);
        }
        case Any_type::REAL: {
            std::stringstream ss;
            ss << to_real(x);
            std::string ras(ss.str());
            return Any(ras);
        }
        case Any_type::ARRAY: {
            std::string aas(debug_str(to_array(x)));
            Any array_as_string(aas);
            return array_as_string;
        }
        case Any_type::DICT: {
            std::string das(debug_str(to_dict(x)));
            return Any(das);
        }
        case Any_type::OBJ: {
            Any oas(to_obj(x)->get_class_ptr()->get_name());
            return oas;
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

