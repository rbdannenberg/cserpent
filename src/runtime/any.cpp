#include <string>
#include <cstring>
#include <cstdint>
#include <cstdio>
#include "any.h"
#include "op_overload.h"
#include "gc.h"
#include "header.h"
#include "heap_obj.h"
#include "csstring.h"
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

/// @note integer promotion rules - int64_t & uint64_t, 
//    int64_t gets cast to uint64_t

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

/*
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
*/

Any::Any(Heap_obj *x) {
    heap_obj = x;
}

/*Any::Any(String *x) {
    // make an Any to reference a String.
    integer = reinterpret_cast<uint64_t>(x) | BIGSTR_TAG;
}

Any::Any(StringPtr x) {
    // make an Any to reference a StringPtr.
    integer = reinterpret_cast<uint64_t>(x.ptr) | BIGSTR_TAG;
}
*/

Any::Any(Symbol *x) {
    integer = reinterpret_cast<uint64_t>(x) | SYMBOL_TAG;
}

/*
Any::Any(Symbol &x) {
    integer = reinterpret_cast<uint64_t>(&x) | SYMBOL_TAG;
}


Any::Any(Array *x) {
    integer = reinterpret_cast<uint64_t>(x);
}

Any::Any(ArrayPtr x) {
    heap_obj = x.ptr;
}
*/

/*
Any::Any(Dict &x) {
    heap_obj = x;
}
*/


Any::Any(Obj *x) {
    obj = x;
}


Any::Any(const char *x) {
    size_t len = strlen(x);
    if (len < 6) {
        integer = SHORT_TAG;
        strncpy(bytes + SHORTSTR_BASE, x, 6);
    } else {
        Big_string *ss = new Big_string{x};
        integer = reinterpret_cast<uint64_t>(ss) | BIGSTR_TAG;
    }
}

Any::Any(std::string &x) {
    size_t len = x.size();
    if (len < 6) {
        integer = SHORT_TAG;
        strncpy(bytes + SHORTSTR_BASE, x.c_str(), 6);
    } else {
        Big_string *ss = new Big_string(x);
        integer = reinterpret_cast<uint64_t>(ss) | BIGSTR_TAG;
    }
}

Any::Any(bool x) {
    heap_obj = x ? css_t : nullptr;
}

/*
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
   integer = reinterpret_cast<uint64_t>(x) | BIGSTR_TAG;
   return *this;
}

Any& Any::operator=(StringPtr x) {
    integer = reinterpret_cast<uint64_t>(x.ptr) | BIGSTR_TAG;
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
*/

/* handled by Heap_obj
Any& Any::operator=(Array *x) {
    integer = reinterpret_cast<uint64_t>(x);
    return *this;
}
*/

/*
Any& Any::operator=(ArrayPtr x) {
    integer = reinterpret_cast<uint64_t>(x.ptr);
    return *this;
}
*/

/* handled by Heap_obj
Any &Any::operator=(Dict *x) {
    integer = reinterpret_cast<uint64_t>(x);
    return *this;
}

Any &Any::operator=(Obj *x) {
    integer = reinterpret_cast<uint64_t>(x);
    return *this;
}
*/

/*
Any &Any::operator=(Heap_obj *x) {
    integer = reinterpret_cast<uint64_t>(x);
    return *this;
}
*/

/*
Any& Any::operator=(bool x) {
    integer = x ? reinterpret_cast<uint64_t>(css_t) : 0;
    return *this;
}

Any& Any::operator=(const char *s) {
    size_t len = strlen(s);
    if (len < 6) {
        integer = SHORT_TAG;
        strncpy(bytes + SHORTSTR_BASE, s, 6);
    } else {
        String *ss = new String(s);
        integer = reinterpret_cast<uint64_t>(ss) | BIGSTR_TAG;
    }
    return *this;
}
*/

bool is_int(Any x) {
    return (x.integer & INT_TAG) == INT_TAG;
}

bool is_real(Any x) {
    return x.integer - BIAS < REAL_LIMIT;
}

// is_in_heap - test if Any is on the heap (not number or short string)
//     is_in_heap is true for null pointers as well
bool is_in_heap(Any x) {
    uint64_t tag = x.integer & TAG_MASK;
    return (tag == PTR_TAG) || (tag == SYMBOL_TAG) || (tag == BIGSTR_TAG);
}
/*
bool is_heap_obj(Any x) {
    uint64_t tag = x.integer & TAG_MASK;
    return (tag == PTR_TAG ? !(to_header(x)->get_tag() == tag_object) :
                             (tag == SYMBOL_TAG) || (tag == BIGSTR_TAG));
}
*/
bool is_obj(Any x) {
    return ((x.integer & TAG_MASK) == PTR_TAG) &&
           (to_header(x)->get_tag() == tag_object);
}

bool is_array(Any x) {
    return ((x.integer & TAG_MASK) == PTR_TAG) &&
           (to_header(x)->get_tag() == tag_array);
}

bool is_dict(Any x) {
    return ((x.integer & TAG_MASK) == PTR_TAG) &&
           (to_header(x)->get_tag() == tag_dict);
}

bool is_str(Any x) {
    uint64_t tag = x.integer & TAG_MASK;
    return (tag == BIGSTR_TAG || tag == SHORT_TAG);
}

bool is_short(Any x) {
    return (x.integer & TAG_MASK) == SHORT_TAG;
}

bool is_big_string(Any x) {
    return (x.integer & TAG_MASK) == BIGSTR_TAG;
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

bool to_bool(Any x) {
    return (x.integer != 0);
}


Heap_obj *to_heap_obj(Any x) {
    // precondition: is_heap_obj()
    return x.heap_obj;
}


Header *to_header(Any x) {
    return x.heap_obj->get_header();
}


Obj *to_obj(Any x) {
    return x.obj;
}


Big_string *to_big_string(Any x) {
    // precondition: is_big_string()
    return reinterpret_cast<Big_string *>(x.integer & ~TAG_MASK);
}

const char *to_short_c_str(const Any &x) {
    // precondition: is_short()
    return reinterpret_cast<const char *>(&(x.integer)) + SHORTSTR_BASE;
}

Symbol *to_symbol(Any x) {
    // precondition: is_symbol()
    return reinterpret_cast<Symbol *>(x.integer & ~TAG_MASK);
}

/*std::string to_shortstr(Any x) {
    return std::string {&((char *) &(x.integer)) + BIAS};
}*/

Array *to_array(Any x) {
    return reinterpret_cast<Array *>(x.heap_obj);
}

Dict *to_dict(Any x) {
    return reinterpret_cast<Dict *>(x.heap_obj);
}


int64_t as_int(Any x) {
    if (is_int(x)) return to_int(x);
    else if (is_real(x)) return (int64_t) to_real(x);
    else {
        printf("as_int needs int or real argument\n");
        exit(1);
    }
}


double as_real(Any x) {
    if (is_int(x)) return (double) to_int(x);
    else if (is_real(x)) return to_real(x);
    else {
        printf("as_real needs int or real argument\n");
        exit(1);
    }
}


String as_string(Any x) {
    check(is_str(x));
    return x;
}


Symbol *as_symbol(Any x) {
    check(is_symbol(x));
    return to_symbol(x);
}


Array *as_array(Any x) {
    check(is_array(x));
    return to_array(x);
}


Dict *as_dict(Any x) {
    check(is_dict(x));
    return to_dict(x);
}


// as_heap_object returns Heap_obj * including nullptr
Heap_obj *as_heap_obj(Any x) {
    check(is_in_heap(x));
    return to_heap_obj(x);
}


Any_type get_type(Any x) {
    if (x.integer == 0) return Any_type::NIL;
    else if (is_int(x)) return Any_type::INT;
    else {
        switch (x.integer & TAG_MASK) {
            case BIGSTR_TAG:
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
    if (is_int(x)) return "integer";
    else if (is_real(x)) return "real";
    else if (is_str(x)) return "string";
    else if (is_symbol(x)) return "symbol";
    else if (is_array(x)) return "array";
    else if (is_dict(x)) return "dict";
    else if (is_obj(x)) return "obj";
    else return "unknown";
}


const char *get_c_str(const Any &s, int64_t *len_ptr)
{
    const char *str;
    if (is_big_string(s)) {
        Big_string *bigstr = to_big_string(s);
        str = bigstr->get_c_str();
        /*
        std::cout << "# get_c_str Any @ " << &s << " bigstr @ " <<
                bigstr << " cstr @ " << (void *) str << " type " <<
                get_type_str(s) << " tag " << (int) get_type(s) << "\n    \"" <<
                str << "\"" << std::endl;
        */
        if (len_ptr) {
            *len_ptr = bigstr->len();
        }
    } else if (is_short(s)) {
        str = to_short_c_str(s);
        if (len_ptr) {
            *len_ptr = strlen(str);
        }
    } else {
        type_error(s);
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

Any Any::append(Any x) {
    if (is_array(*this)) {
        to_array(*this)->append(x);
    } else if (is_str(*this) && is_str(x)) {
        std::string s{get_c_str(*this)};
        s.append(get_c_str(x));
        return Any{s};
    } else {
        type_error(*this);
    }
    return *this;
}

Any Any::append(bool x) {
    append(Any{x});
    return *this;
}

Any Any::append(int64_t x) {
    append(Any{x});
    return *this;
}

Any Any::append(double x) {
    append(Any{x});
    return *this;
}


Any Any::call(Symbol *method) {
    if (integer && is_in_heap(*this)) {
        Heap_obj *heap_obj = to_heap_obj(*this);
        // printf("Any::call Any tag is %d, type_str %s\n", heap_obj->get_tag(),
        //        get_type_str(*this).c_str());
        switch (heap_obj->get_tag()) {
            case tag_object: {
                // TODO: define and use to_obj():
                Obj *obj_ptr = to_obj(*this);
                return obj_ptr->call(method);
            }
            case tag_array: {
                return to_array(*this)->call(method);
            }
            case tag_dict: {
                return to_dict(*this)->call(method);
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
