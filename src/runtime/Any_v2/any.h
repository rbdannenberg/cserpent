#include <string>
#pragma once

union AnyC {
    // why int64_t? for any2int? should we explicitly cast to uint64_t
    int64_t integer;
    double doub;
};

// PRECONDITION: 50-bits complement <=> Top 15 bits are all 0's or 1's
// OCCUPY: 0xFFFC - 0xFFFF
AnyC constructor(int64_t x);

// PRECONDITION: Not NaN or Inf
// OCCUPY: 0x0001 - 0x7FFF; 0x8001 - 0xFFF0
AnyC constructor(double x);

// PRECONDITION: Top 16 bits are 0
AnyC constructor(void* x);

// NOT IMPLEMENTED
AnyC constructor(std::string x);

bool is_int(AnyC x);
bool is_double(AnyC x);
bool is_ptr(AnyC x);
bool is_shortstr(AnyC x);
bool is_symbol(AnyC x);

int64_t to_int(AnyC x);
double to_double(AnyC x);
void* to_ptr(AnyC x);
std::string to_shortstr(AnyC x);

// check is like assert except it always executes, even in optimized code
// since the expression could be int, bool, or a test for non-null pointer,
// we use a macro:
#define check(c) if (!(c)) { printf("Failed at %s:%d (%s)\n", \
                                    __FILE__, __LINE__, #c); exit(1); }

// as_* functions are used in compiled code to ensure that the type
// matches the expected type. as_* calls are generated from source
// code that looks like expression.as_int

int64_t as_int(AnyC x) {
    check(is_int(x));
    return to_int(x);
}

double as_double(AnyC x) {
    check(is_double(x));
    return to_double(x);
}

void *as_ptr(AnyC x) {
    check(is_ptr(x));
    return to_ptr(x);
}


enum { tag_free,
       tag_symbol,
       tag_integer,
       tag_string,
       tag_array,
       tag_arraydata,
       tag_dict,
       tag_object,
       tag_file } Tag;


// all garbage-collected objects inherit this base class
//
class Basic_obj {
  public:
    /* fields in header:
     * 
     * header format is:
     *  +---------------------------------------------+
     *  | TAG      | COLOR    | #SLOTS    | NEXT      |
     *  | (5 bits) | (2 bits) | (12 bits) | (45 bits) |
     *  +---------------------------------------------+
     * The NEXT field is a pointer to 8-byte aligned data.
     * It is shifted right 3 bits because the low-order bits
     * are zero. To use the pointer, shift it left 3 bits.
     */
    uint64_t header;  // contains type info, GC color, size

    Tag get_tag();
    void set_tag(Tag t);
    bool has_tag(int tag);

    Color get_color();
    void set_color(uint8_t c);
    bool has_color(int tag);

    Obj *get_next();
    void set_next(Obj *ptr);
};


// CSerpent string, with tag tag_string. For now, this is just
// a C++ std:string "boxed" in a Basic_obj so it can be GC'd:
//
class CSString : public Basic_obj {
  public:
    string str;
}


class Symbol : public Basic_obj {
  public:
    CSString *name;
    AnyC value;
    // probably need a function pointer here too
};


// all user-defined object inherit from this, which has a class
// pointer
//
class Obj : public Basic_obj {
  public:
    Symbol *type_symbol;  // symbol denoting class name
};

// to_symbol

// Operator overload
