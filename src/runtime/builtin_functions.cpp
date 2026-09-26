//
// Created by anthony on 5/17/24.
//

#include <cmath>
#include "any.h"
#include "gc.h"
#include "header.h"
#include "heap_obj.h"
#include "obj.h"
#include "op_overload.h"
#include "any_utils.h"
#include "array.h"
#include "csstring.h"
#include "symbol.h"
#include "dict.h"
#include <cstring>
#include "builtin_functions.h"
#include "any_utils.h"

int64_t len(Any x) {
    if (x.integer && is_in_heap(x)) {
        Heap_obj *heap_obj= to_heap_obj(x);
        if (heap_obj->get_tag() == tag_array) {
            return to_array(x)->len();
        }
    } else if (is_str(x)) {
        int64_t length;
        get_c_str(x, &length);
        return length;
    } else {
        type_error(x);
    }
}


Any max(Any lhs, Any rhs) {
    if (lhs > rhs) {
        return lhs;
    } else {
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
    } else if (is_real(lhs)) {
        return to_real(lhs) / rhs;
    } else type_error(lhs);
}




int64_t find(Any s, Any pattern,  Any start, Any end)
{
    return find(s, pattern, as_int(start), as_int(end));
}


int64_t find(Any s, Any pattern,  Any start, int64_t end)
{
    return find(s, pattern, as_int(start), end);
}

    
int64_t find(Any s, Any pattern,  int64_t start, int64_t end)
{
    if (is_str(pattern)) {
        return find(s, get_c_str(pattern), start, end);
    } else type_error(pattern);
}


int64_t find(Any s, const char *pattern, Any start, Any end)
{
    return find(s, pattern, as_int(start), as_int(end));
}


int64_t find(Any s, const char *pattern, Any start, int64_t end)
{
    return find(s, pattern, as_int(start), end);
}


int64_t find(Any s, const char *pattern, int64_t start, int64_t end)
{
    if (is_str(s)) {
        return find(get_c_str(s), pattern, start, end);
    } else type_error(Any(pattern));
}


int64_t find(const char *s, const char *pattern, Any start, Any end)
{
    return find(s, pattern, as_int(start), as_int(end));
}


int64_t find(const char *s, const char *pattern, Any start, int64_t end)
{
    return find(s, pattern, as_int(start), end);
}


int64_t find(const char *s, const char *pattern, int64_t start, int64_t end)
{
    int s_len = strlen(s);
    if (end == std::numeric_limits<int64_t>::max()) {
        end = s_len;
    }
    if (end < 0) {
        end = s_len + end;
    }
    if (start < 0) {
        start = s_len + start;
    }
    // do bounds checking: 0 <= start <= end <= s_len
    if (start < 0 || start > end || end > s_len) {
        throw std::out_of_range("subseq: out of range");
    }
    const char *loc = strstr(s + start, pattern);
    if (!loc || loc > (s + end - strlen(pattern))) {
        return -1;
    }
    return loc - s;
}


Any subseq(Any s, Any start, Any end)
{
    return subseq(s, as_int(start), as_int(end));
}


Any subseq(Any s, int64_t start, Any end)
{
    return subseq(s, start, as_int(end));
}


Any subseq(Any s, Any start, int64_t end)
{
    return subseq(s, as_int(start), end);
}


Any subseq(Any s, int64_t start,  int64_t end)
{
    if (is_str(s)) {
        // Convert Any to StringPtr, call string subseq, return as Any
        return subseq(get_c_str(s), start, end);
    } else if (is_array(s)) {
        return Any{subseq(to_array(s), start, end)};
    }
    // TODO: short strings
    type_error(s);
}


String toupper(String s) {
    char upper[256];
    char *u = upper;
    if (is_str(s)) {
        for (const char *p = get_c_str(s); *p != 0; p++) {
            check(u < upper + 255);
            *u++ = toupper(*p);
        }
        *u = '\0';
        return String{upper};
    } else {
        type_error(s);
    }
}


String tolower(String s) {
    char lower[256];
    char *l = lower;
    if (is_str(s)) {
        for (const char *p = get_c_str(s); *p != 0; p++) {
            check(l < lower + 255);
            *l++ = tolower(*p);
        }
        *l = '\0';
        return String{lower};
    } else {
        type_error(s);
    }
}
/*
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
