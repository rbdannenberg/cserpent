//
// Created by anthony on 5/17/24.
//

#include "builtin_functions.h"
#include "any_utils.h"
#include <data_structures/array.h>
#include <cmath>

int64_t len(Any x) {
    if (is_ptr(x)) {
        Basic_obj *basic_ptr= to_ptr(x);
        if (basic_ptr->get_tag() == tag_arraydata) {
            return len(to_array(x));
        }
    }
    else if (is_shortstr(x)) {
        return to_shortstr(x).length();
    }
    else {
        type_error(x);
    }
}

int64_t pow(int base, Any exp) {
    if (is_int(exp)) {
        return static_cast<int64_t>(std::pow(base, to_int(exp)));
    }
    else {
        type_error(exp);
    }
}

int64_t idiv(Any lhs, int rhs) {
    if (is_int(lhs)) {
        return to_int(lhs) / rhs;
    }
    else if (is_real(lhs)) {
        return to_real(lhs) / rhs;
    }
    else type_error(lhs);
}
