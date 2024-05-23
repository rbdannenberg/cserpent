//
// Created by anthony on 5/17/24.
//

#include "builtin_functions.h"
#include "any_utils.h"
#include <data_structures/array.h>

int64_t len(Any x) {
    if (is_ptr(x)) {
        Basic_obj *basic_ptr= to_ptr(x);
        if (basic_ptr->get_tag() == tag_array) {
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