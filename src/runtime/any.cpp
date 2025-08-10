#include <string>
#include <cstring>
#include <cstdint>
#include <cstdio>
#include "any.h"
#include "op_overload.h"
#include "gc.h"
#include "heap_obj.h"
#include "obj.h"
#include "any_utils.h"
#include "array.h"
#include "dict.h"
#include "symbol.h"
#include "csstring.h"

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
    // check for int out of range: if x is positive, high-order bits are
    // zero, so tmp == 0. If x is negative, high-order bits are 1's and
    // tmp == ~INT_MASK.
    int64_t tmp = x & ~INT_MASK;
    if (tmp != 0 && tmp != ~INT_MASK){
        throw std::runtime_error("Precondition failed: integer value corrupted:");
    }
#endif
    integer = (static_cast<uint64_t>(x) & INT_MASK) | INT_TAG;
}

Any::Any(int x) {
    integer = (static_cast<uint64_t>(x) & INT_MASK) | INT_TAG;
}

Any::Any(double x) {
    real = x;
    integer += BIAS;
    assert(is_real(*this));
}

Any::Any(void* x) {
    // not integer = reinterpret_cast<uint64_t>(x);?
    integer = reinterpret_cast<uint64_t>(x);
#ifdef DEBUG
    if (integer & TAG_MASK) {
        std::cerr << "Precondition failed: pointer corrupted" << std::endl;
        return {};
    }
#endif
}

Any::Any(String *x) {
    // make an Any to reference a String.
    integer = reinterpret_cast<uint64_t>(x) | STR_TAG;
}

Any::Any(StringPtr x) {
    // make an Any to reference a StringPtr.
    integer = reinterpret_cast<uint64_t>(x.ptr) | STR_TAG;
}

Any::Any(Symbol *x) {
    integer = reinterpret_cast<uint64_t>(x) | SYMBOL_TAG;
}

Any::Any(Array *x) {
    integer = reinterpret_cast<uint64_t>(x);
}

Any::Any(ArrayPtr x) {
    integer = reinterpret_cast<uint64_t>(x.ptr);
}

Any::Any(Dict *x) {
    integer = reinterpret_cast<uint64_t>(x);
}

Any::Any(Obj *x) {
    integer = reinterpret_cast<uint64_t>(x);
}

Any::Any(const char *x) {
    size_t len = strlen(x);
    if (len < 6) {
        integer = SHORT_TAG;
        strncpy(bytes + SHORTSTR_BASE, x, 6);
    } else {
        String *ss = new String(x);
        integer = reinterpret_cast<uint64_t>(ss) | STR_TAG;
    }
}

Any::Any(std::string &x) {
    size_t len = x.size();
    if (len < 6) {
        integer = SHORT_TAG;
        strncpy(bytes + SHORTSTR_BASE, x.c_str(), 6);
    } else {
        String *ss = new String(x);
        integer = reinterpret_cast<uint64_t>(ss) | STR_TAG;
    }
}

Any::Any(bool x) {
    integer = x ? symbol_t.integer : 0;
}

Any& Any::operator=(int64_t x) {
#ifdef DEBUG
    // check for int out of range: if x is positive, high-order bits are
    // zero, so tmp == 0. If x is negative, high-order bits are 1's and
    // tmp == ~INT_MASK.
    int64_t tmp = x & ~INT_MASK;
    if (tmp != 0 && tmp != ~INT_MASK) {
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
    integer += BIAS;
    assert(is_real(x));
    return *this;
}

Any& Any::operator=(String *x) {
   integer = reinterpret_cast<uint64_t>(x) | STR_TAG;
   return *this;
}

Any& Any::operator=(StringPtr x) {
    integer = reinterpret_cast<uint64_t>(x.ptr) | STR_TAG;
    return *this;
}

Any& Any::operator=(Symbol *x) {
    integer = reinterpret_cast<uint64_t>(x) | SYMBOL_TAG;
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

Any& Any::operator=(Array *x) {
    integer = reinterpret_cast<uint64_t>(x);
    return *this;
}

Any& Any::operator=(ArrayPtr x) {
    integer = reinterpret_cast<uint64_t>(x.ptr);
    return *this;
}

Any &Any::operator=(Dict *x) {
    integer = reinterpret_cast<uint64_t>(x);
    return *this;
}

Any &Any::operator=(Obj *x) {
    integer = reinterpret_cast<uint64_t>(x);
    return *this;
}

Any& Any::operator=(bool x) {
    integer = x ? symbol_t.integer : 0;
    return *this;
}

Any& Any::operator=(const char *s) {
    size_t len = strlen(s);
    if (len < 6) {
        integer = SHORT_TAG;
        strncpy(bytes + SHORTSTR_BASE, s, 6);
    } else {
        String *ss = new String(s);
        integer = reinterpret_cast<uint64_t>(ss) | STR_TAG;
    }
    return *this;
}


bool is_int(Any x) {
    return (x.integer & INT_TAG) == INT_TAG;
}

bool is_real(Any x) {
    return x.integer - BIAS < REAL_LIMIT;
}

bool is_heap_obj(Any x) {
    return (x.integer & TAG_MASK) == PTR_TAG;
}

bool is_str(Any x) {
    uint64_t tag = x.integer & TAG_MASK;
    return (tag == STR_TAG || tag == SHORT_TAG);
}

bool is_short(Any x) {
    return (x.integer & TAG_MASK) == SHORT_TAG;
}

bool is_string(Any x) {
    return (x.integer & TAG_MASK) == STR_TAG;
}

bool is_symbol(Any x) {
    return (x.integer & TAG_MASK) == SYMBOL_TAG;
}

int64_t to_int(Any x) {
    // precondition: is_int()
    return (static_cast<int64_t>(x.integer) << 15) >> 15;
}

double to_real(Any x) {
    // precondition: is_real()
    x.integer -= BIAS;
    return x.real;
}

Heap_obj* to_heap_obj(Any x) {
    // precondition: is_heap_obj()
    return reinterpret_cast<Heap_obj*>(x.integer);
}

String *to_string(Any x) {
    // precondition: is_string()
    return reinterpret_cast<String *>(x.integer & ~TAG_MASK
                                      );
}

Symbol *to_symbol(Any x) {
    // precondition: is_symbol()
    return reinterpret_cast<Symbol *>(x.integer & ~TAG_MASK);
}

/*std::string to_shortstr(Any x) {
    return std::string {&((char *) &(x.integer)) + BIAS};
}*/

Array *to_array(Any x) {
    return reinterpret_cast<Array *>(x.integer);
}

Dict *to_dict(Any x) {
    return reinterpret_cast<Dict *>(x.integer);
}

Obj *to_obj(Any x) {
    return reinterpret_cast<Obj *>(x.integer);
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

String *as_string(Any x) {
    check(is_string(x));
    return to_string(x);
}

Symbol *as_symbol(Any x) {
    check(is_symbol(x));
    return to_symbol(x);
}

Array *as_array(Any x) {
    check(is_heap_obj(x));
    return to_array(x);
}

Heap_obj *as_heap_obj(Any x) {
    check(is_heap_obj(x));
    return to_heap_obj(x);
}

Any_type get_type(Any x) {
    if (x.integer == 0) return Any_type::NIL;
    else if (is_int(x)) return Any_type::INT;
    else {
        switch (x.integer & TAG_MASK) {
            case STR_TAG:
                return Any_type::STRING;
            case SHORT_TAG:
                return Any_type::SHORT;
            case SYMBOL_TAG:
                return Any_type::SYMBOL;
            case PTR_TAG:
                switch (to_heap_obj(x)->get_tag()) {
                    case tag_array:
                        return Any_type::ARRAY;
                    case tag_dict:
                        return Any_type::DICT;
                    case tag_object:
                        return Any_type::OBJ;
                    default:
                        throw std::runtime_error("Unknown type");
                }
            default:
                return Any_type::REAL;
        }
    }
}

std::string get_type_str(Any x) {
    if (is_heap_obj(x)) return "pointer";
    else if (is_int(x)) return "integer";
    else if (is_real(x)) return "real";
    else if (is_str(x)) return "string";
    else if (is_symbol(x)) return "symbol";
    else return "unknown";
}


const char *get_c_str(const Any *s, int64_t *len_ptr)
{
    const char *str;
    if (is_string(*s)) {
        str = to_string(*s)->get_c_str();
        if (len_ptr) {
            *len_ptr = to_string(*s)->len();
        }
    } else {
        str = s->bytes + SHORTSTR_BASE;
        if (len_ptr) {
            *len_ptr = strlen(str);
        }
    }
    return str;
}

/* Automatic coercions here. I am hiding them because they can allow you
 to call unexpected functions, e.g. you might expect to call bar(Any, int) but
 you mistakenly write bar(Any, int). This will not fail if there is a
 bar(int, int) somewhere if we define Any::operator int64_t(). I think it is
 better (but maybe a little uglier) to explicitly write bar(as_int(Any), int)
 if that's really what you want to do. I don't like it when the compiler
 generates extra checks and conversion code that's not visible in the program.
 
Any::operator int64_t() {
    return as_int(*this);
}

Any::operator double() {
    return as_real(*this);
}

Any::operator String *() {
    return as_string(*this);
}

Any::operator Symbol *() {
    return as_symbol(*this);
}

Any::operator Array *() {
    return as_array(*this);
}

Any::operator bool() {
    return integer != 0;
}

 */
        
static bool is_string_or_symbol(Any x) {
    return is_str(x) || is_symbol(x);
}


bool Any::is(Any x) {
    return integer == x.integer;
}

void Any::append(Any x) {
    if (is_heap_obj(*this)) {
        to_array(*this)->append(x);
    } else {
        type_error(*this);
    }
}

void Any::append(int64_t x) {
    append(Any {x});
}

void Any::append(double x) {
    append(Any {x});
}


Any Any::call(Any method, Array *args, Dict *kwargs) {
    if (is_heap_obj(*this)) {
        Heap_obj *heap_obj = to_heap_obj(*this);
        switch (heap_obj->get_tag()) {
            case tag_object: {
                // TODO: define and use to_obj():
                Obj *obj_ptr = reinterpret_cast<Obj*>(heap_obj);
                return obj_ptr->call(as_symbol(method), args, kwargs);
            }
            case tag_array: {
                return to_array(*this)->call(method, args, kwargs);
            }
            default:
                type_error(*this);
        }
    } else {
        type_error(*this);
    }
}

Any::Any(const std::ostream &x) {
    integer = 0;
}
