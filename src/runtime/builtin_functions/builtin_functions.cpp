//
// Created by anthony on 5/17/24.
//

#include "builtin_functions.h"
#include "any_utils.h"
#include <data_structures/array.h>
#include "csstring.h"
#include <cmath>

int64_t len(Any x) {
    if (is_ptr(x)) {
        Basic_obj *basic_ptr= to_ptr(x);
        if (basic_ptr->get_tag() == tag_array) {
            return len(to_array(x));
        }
    }
    else if (is_str(x)) {
        return len(to_str(x));
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

int64_t find(Any s, const String& pattern, int64_t start, int64_t end) {
    if (is_str(s)) {
        return find(to_str(s), pattern, start, end);
    }
    else {
        type_error(s);
    }
}

int64_t find(const String& s, Any pattern, int64_t start, int64_t end) {
    if (is_str(pattern)) {
        return find(s, to_str(pattern), start, end);
    }
    else {
        type_error(s);
    }
}
int64_t find(Any s, Any pattern, int64_t start, int64_t end) {
    if (is_str(s) && is_str(pattern)) {
        return find(to_str(s), to_str(pattern), start, end);
    }
    else {
        type_error(s);
    }
}

Any subseq(Any s, int64_t start, int64_t end) {
    if (is_str(s)) {
        return subseq(to_str(s), start, end);
    }
    else if (is_ptr(s)) {
        if (to_ptr(s)->get_tag() == tag_array) {
            return subseq(to_array(s), start, end);
        }
        type_error(s);
    }
}

String toupper(Any s) {
    if (is_str(s)) return toupper(to_str(s));
    else type_error(s);
}