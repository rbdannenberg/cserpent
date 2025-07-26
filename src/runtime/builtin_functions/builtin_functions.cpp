//
// Created by anthony on 5/17/24.
//

#include <cmath>
#include "any.h"
#include "gc.h"
#include "heap_obj.h"
#include "obj.h"
#include "builtin_functions.h"
#include "op_overload.h"
#include "any_utils.h"
#include "data_structures/array.h"
#include "csstring.h"
#include "symbol.h"
#include "dict.h"
#include <cstring>

int64_t len(Any x) {
    if (is_heap_obj(x)) {
        Heap_obj *heap_obj= to_heap_obj(x);
        if (heap_obj->get_tag() == tag_array) {
            return len(to_array(x));
        }
    } else if (is_string(x)) {
        return len(to_string(x));
    } else if (is_short(x)) {
        return strlen(x.bytes + SHORTSTR_BASE);
    } else {
        type_error(x);
    }
}

Any max(Any lhs, Any rhs) {
    if (lhs > rhs) {
        return lhs;
    }
    else {
        return rhs;
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

/*
int64_t find(String *s, String *pattern, int64_t start, int64_t end) {
    if (is_string(s)) {
        return find(to_string(s), pattern, start, end);
    }
    else {
        type_error(s);
    }
}

int64_t find(String *s, Any pattern, int64_t start, int64_t end) {
    if (is_str(pattern)) {
        return find(s, to_string(pattern), start, end);
    }
    else {
        type_error(s);
    }
}
int64_t find(Any s, Any pattern, int64_t start, int64_t end) {
    if (is_string(s)) {
        return find(to_string(s), pattern, start, end);
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
        if (to_heap_obj(s)->get_tag() == tag_array) {
            return subseq(to_array(s), start, end);
        }
        type_error(s);
    }
}

String toupper(Any s) {
    if (is_str(s)) return toupper(to_str(s));
    else type_error(s);
}

Any is_equal(Any lhs, Any rhs) {
    bool res;
    if (is_ptr(lhs)) {
         res = lhs.integer == rhs.integer;
    }
    else res = lhs == rhs;
    return Any {res};
}

Any apply(Symbol function, Array *argarray) {
    return std::invoke(globals::cs_symbol_table.get_function(function),
                       argarray, empty_dict);
}

Any sendapply(Obj& obj, Symbol method, Array *argarray) {
    return obj.call(method, argarray, empty_dict);
}

Any sendapply(Any obj, Symbol method, Array *argarray) {
    if (is_ptr(obj)) {
        return obj.call(method, argarray, empty_dict);
    }
    else {
        type_error(obj);
    }
}
*/
