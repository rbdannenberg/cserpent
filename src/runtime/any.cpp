#include <string>
#include <cstring>
#include <cstdint>
#include <cstdio>
#include "any.h"
#include "gc.h"
#include "obj.h"
#include "csstring.h"
#include "array.h"
#include "symbol.h"
#include "any_utils.h"
#include "dictionary.h"

constexpr uint64_t BIAS         =    0x1000000000000uLL;
constexpr uint64_t INT_TAG      = 0xFFFC000000000000uLL;
constexpr uint64_t TAG_MASK     = 0xFFFF000000000000uLL;
constexpr uint64_t PTR_TAG      = 0x0000000000000000uLL;
constexpr uint64_t SHORTSTR_TAG = 0xFFFA000000000000uLL;
constexpr size_t num_chars = 5; // 8 bytes - 2 (tag) - 1 (nul terminator)


/**
Hexadecimal to Binary Conversion Table:

Hex | Binary   | Hex | Binary   | Hex | Binary   | Hex | Binary
----|----------|-----|----------|-----|----------|-----|--------
 0  | 0000     | 4   | 0100     | 8   | 1000     | C   | 1100
 1  | 0001     | 5   | 0101     | 9   | 1001     | D   | 1101
 2  | 0010     | 6   | 0110     | A   | 1010     | E   | 1110
 3  | 0011     | 7   | 0111     | B   | 1011     | F   | 1111
*/

/// @note integer promotion rules - int64_t & uint64_t, int64_t gets cast to uint64_t

// Default constructor: 0, nullptr, nil
Any::Any() : integer {0} {}

Any::Any(int64_t x) {
#ifdef DEBUG
    int64_t tmp = x & 0xFFFE000000000000;
    if (tmp != 0 && tmp != 0xFFFE000000000000) {
        throw std::runtime_error("Precondition failed: integer value corrupted:");
    }
#endif
    integer = static_cast<uint64_t>(x) | INT_TAG;
}

Any::Any(int x) {
    integer = static_cast<uint64_t>(x) | INT_TAG;
}

Any::Any(double x) {
    real = x;
#ifdef DEBUG
    if ((integer & 0x7FF0000000000000) == DOUBLE_UP) {
        throw std::runtime_error("Precondition failed: Nan or Inf value.");
        return {};
    }
#endif
    integer += BIAS;
}

Any::Any(void* x) {
    // not integer = reinterpret_cast<uint64_t>(x);?
    integer = *reinterpret_cast<uint64_t*>(&x);
#ifdef DEBUG
    if (integer & TAG_MASK) {
        std::cerr << "Precondition failed: pointer corrupted" << std::endl;
        return {};
    }
#endif
}
Any::Any(std::string x) {
    // 2 possible ways:
    // 1: get 8 bytes starting from x, discard the last 2 bytes, shift right and add tag
    // 2: insert tag bytes in front of string, get 8 bytes starting from front.
    // The following is way no. 1:
#ifdef DEBUG
    if (x.length() > num_chars) {
        std::cerr << "Precondition failed: string too long" << std::endl;
        return {};
    }
#endif
    // reads 8 bytes
    uint64_t tmp = *((uint64_t *) (x.c_str()));
    integer = (tmp >> 16) | SHORTSTR_TAG;
    // warning: this copies a lot of garbage after the nul terminator into the Any
}

Any::Any(const Array& x) {
    integer = reinterpret_cast<uint64_t>(&x);
    //TODO: &(x.data)?
}

Any::Any(const Dictionary &x) {
    integer = reinterpret_cast<uint64_t>(&x);
}

Any::Any(const Obj &x) {
    integer = reinterpret_cast<uint64_t>(&x);
}

Any& Any::operator=(int64_t x) {
#ifdef DEBUG
    int64_t tmp = x & 0xFFFE000000000000;
    if (tmp != 0 && tmp != 0xFFFE000000000000) {
        throw std::runtime_error("Precondition failed: integer value corrupted:");
    }
#endif
    integer = static_cast<uint64_t>(x) | INT_TAG;
    return *this;
}

Any& Any::operator=(int x) {
    integer = static_cast<uint64_t>(x) | INT_TAG;
    return *this;
}

Any& Any::operator=(double x) {
    real = x;
#ifdef DEBUG
    if ((integer & 0x7FF0000000000000) == DOUBLE_UP) {
        throw std::runtime_error("Precondition failed: Nan or Inf value.");
        return {};
    }
#endif
    integer += BIAS;
    return *this;
}

Any& Any::operator=(void* x) {
    // not integer = reinterpret_cast<uint64_t>(x);?
    integer = *reinterpret_cast<uint64_t*>(&x);
#ifdef DEBUG
    if (integer & TAG_MASK) {
        std::cerr << "Precondition failed: pointer corrupted" << std::endl;
        return {};
    }
#endif
    return *this;
}

Any& Any::operator=(const Array& x) {
    integer = reinterpret_cast<uint64_t>(&x);
    return *this;
}

Any &Any::operator=(const Dictionary &x) {
    integer = reinterpret_cast<uint64_t>(&x);
    return *this;
}

Any &Any::operator=(const Obj &x) {
    integer = reinterpret_cast<uint64_t>(&x);
    return *this;
}

bool is_int(Any x) {
    return (x.integer & INT_TAG) == INT_TAG;
}

bool is_real(Any x) {
    return x.integer - BIAS < 0xFFF9000000000000uLL;
}

bool is_ptr(Any x) {
    return (x.integer & TAG_MASK) == PTR_TAG;
}

bool is_shortstr(Any x) {
    return (x.integer & TAG_MASK) == SHORTSTR_TAG;
}

int64_t to_int(Any x) {
    return (static_cast<int64_t>(x.integer) << 14) >> 14;
}

double to_real(Any x) {
    x.integer -= BIAS;
    return x.real;
}

Basic_obj* to_ptr(Any x) {
    return reinterpret_cast<Basic_obj*>(x.integer);
}

std::string to_shortstr(Any x) {
    // copies from 3rd byte onwards (skips tag) until nul terminator is encountered
    return std::string {&((char *) &(x.integer))[2]};
}

Array& to_array(Any x) {
    return *reinterpret_cast<Array*>(x.integer);
}

// check is like assert except it always executes, even in optimized code
// since the expression could be int, bool, or a test for non-null pointer,
// we use a macro:
#define check(c) if (!(c)) { printf("Failed at %s:%d (%s)\n", \
                                    __FILE__, __LINE__, #c); exit(1); }


int64_t as_int(Any x) {
    check(is_int(x));
    return to_int(x);
}

double as_real(Any x) {
    check(is_real(x));
    return to_real(x);
}

void *as_ptr(Any x) {
    check(is_ptr(x));
    return to_ptr(x);
}

std::string get_type(Any x) {
    if (is_ptr(x)) return "pointer";
    else if (is_int(x)) return "integer";
    else if (is_real(x)) return "real";
    else if (is_shortstr(x)) return "short string";
    else return "unknown";
}



Any::operator int64_t() {
    return as_int(*this);
}

Any::operator double() {
    return as_real(*this);
}

void Any::append(Any x) {
    if (is_ptr(*this)) {
        Basic_obj *basic_ptr = to_ptr(*this);
        if (basic_ptr->get_tag() == tag_arraydata) {
            Array *arr(static_cast<Array *>(basic_ptr));
            arr->append(x);
        }
    }
    else {
        type_error(*this);
    }
}

void Any::append(int64_t x) {
    append(Any {x});
}

void Any::append(double x) {
    append(Any {x});
}

//Any::Any(const Basic_obj &x) {
//    integer = reinterpret_cast<uint64_t>(&x);
//}

/* TODO: implement calls to Obj methods
Any Any::call(const std::string &method, const Array &args, const Dictionary &kwargs) {
    if (is_ptr(*this)) {
        Basic_obj *basic_ptr = to_ptr(*this);
        if (true) {
//        if (basic_ptr->get_tag() == tag_object) { /// @note: get_tag is not implemented
            Obj *obj_ptr = reinterpret_cast<Obj*>(basic_ptr);
            return obj_ptr->call(method, args, kwargs);
//
//            std::string name = obj_ptr->class_name;
//            Symbol C = global::symbol_table[name];
//            return std::invoke(C.call, obj_ptr, method, args, kwargs);
        }
    }
    else {
        type_error(*this);
    }
}
 */

/* TODO: implement field access from Obj
Any& Any::get(const std::string &member) {
    if (is_ptr(*this)) {
        Basic_obj *basic_ptr = to_ptr(*this);
        if (true) {
            Obj *obj_ptr = reinterpret_cast<Obj*>(basic_ptr);
            return obj_ptr->get(member);
        }
    }
    else {
        type_error(*this);
    }
}
*/
