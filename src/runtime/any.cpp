#include <string>
#include <cstring>
#include <cstdint>
#include <cstdio>
#include "any.h"
#include "gc.h"
#include "obj.h"
#include "any_utils.h"
#include "array.h"
#include "dictionary.h"
#include "csstring.h"

constexpr uint64_t BIAS         =    0x1000000000000uLL;
constexpr uint64_t INT_TAG      = 0xFFFC000000000000uLL;
constexpr uint64_t TAG_MASK     = 0xFFFF000000000000uLL;
constexpr uint64_t PTR_TAG      = 0x0000000000000000uLL;
constexpr uint64_t STR_TAG      = 0xFFFA000000000000uLL;
constexpr uint64_t SYMBOL_TAG   = 0xFFFB000000000000uLL;
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


Any::Any(String x) {
#ifdef DEBUG
    if (x.tag != static_cast<int16_t>(0xFFFA)) {
        std::cerr << "Precondition failed: string corrupted" << std::endl;
        return {};
    }
#endif
    integer = 0; // So the potential pointer does not get freed
    std::swap(x.data, integer); // Any takes on the information in the String
    // Any copies of the resulting Any will point to the same std::string - "controlled aliasing"
    // This is fine since no operation modifies the contents of the std::string, only returning a new
    // String, separating the aliasing and maintaining value semantics for both Any and String.
    // This is similar to copy-on-write.
    // Thus, only the String-to-Any constructor, String-to-Any assignment and Any-to-String to_str need
    // special care. Default Any copy/move constructor/assignment are preserved.
    // Currently, the only leaks are 1 per String to Any conversion. This is acceptable.
}

Any::Any(Symbol x) {
#ifdef DEBUG
    if (x.tag != static_cast<int16_t>(0xFFFB)) {
        std::cerr << "Precondition failed: symbol corrupted" << std::endl;
        return {};
    }
#endif
    integer = 0;
    std::swap(x.data, integer);
}

Any::Any(const Array& x) {
    integer = reinterpret_cast<uint64_t>(&x);
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

Any& Any::operator=(String x) {
    integer = 0;
    std::swap(integer, x.data);
    return *this;
}

Any& Any::operator=(Symbol x) {
    integer = 0;
    std::swap(integer, x.data);
    return *this;
}

//Any& Any::operator=(void* x) {
//    // not integer = reinterpret_cast<uint64_t>(x);?
//    integer = *reinterpret_cast<uint64_t*>(&x);
//#ifdef DEBUG
//    if (integer & TAG_MASK) {
//        std::cerr << "Precondition failed: pointer corrupted" << std::endl;
//        return {};
//    }
//#endif
//    return *this;
//}

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

bool is_str(Any x) {
    return (x.integer & TAG_MASK) == STR_TAG;
}

bool is_symbol(Any x) {
    return (x.integer & TAG_MASK) == SYMBOL_TAG;
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

String to_str(Any x) {
    String result {};
    result.data = x.integer;
    String copy = result; // We want to invoke String's copy constructor so that the original Any isn't affected.
    result.data = 0;
    return copy;
}

Symbol to_symbol(Any x) {
    Symbol result {};
    result.data = x.integer;
    Symbol copy = result;
    result.data = 0;
    return copy;
}

//std::string to_shortstr(Any x) {
//    // copies from 3rd byte onwards (skips tag) until nul terminator is encountered
//    return std::string {&((char *) &(x.integer))[2]};
//}

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
    else if (is_str(x)) return "string";
    else if (is_symbol(x)) return "symbol";
    else return "unknown";
}



Any::operator int64_t() {
    return as_int(*this);
}

Any::operator double() {
    return as_real(*this);
}



static bool is_string_or_symbol(Any x) {
    uint64_t string_or_symbol_mask = 0xFFFE000000000000uLL;
    // return (x.integer & string_or_symbol_mask) == STR_TAG;
    return is_str(x) || is_symbol(x);
}


void Any::append(Any x) {
    if (is_ptr(*this)) {
        Basic_obj *basic_ptr = to_ptr(*this);
        if (basic_ptr->get_tag() == tag_array) {
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

Any Any::call(const Symbol& method, const Array &args, const Dictionary &kwargs) {
    if (is_ptr(*this)) {
        Basic_obj *basic_ptr = to_ptr(*this);
        if (basic_ptr->get_tag() == tag_object) {
            Obj *obj_ptr = reinterpret_cast<Obj*>(basic_ptr);
            return obj_ptr->call(method, args, kwargs);
        }
    }
    else {
        type_error(*this);
    }
}

