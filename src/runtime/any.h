#include <string>
#pragma once

union Any {
    // why int64_t? for any2int? should we explicitly cast to uint64_t
    int64_t integer;
    double real;
};

// enum type?

// PRECONDITION: 50-bits complement <=> Top 15 bits are all 0's or 1's
// OCCUPY: 0xFFFC - 0xFFFF
Any constructor(int64_t x);

// PRECONDITION: Not NaN or Inf
// OCCUPY: 0x0001 - 0x7FFF; 0x8001 - 0xFFF0
Any constructor(double x);

// PRECONDITION: Top 16 bits are 0
Any constructor(void* x);

// PRECONDITION: Length of string is at most 5
Any constructor(std::string x);

bool is_int(Any x);
bool is_real(Any x);
bool is_ptr(Any x);
bool is_shortstr(Any x);
bool is_symbol(Any x);

// these to_* functions do not check that the underlying type is correct
// for that, use as_* functions
int64_t to_int(Any x);
double to_real(Any x);
void *to_ptr(Any x);
std::string to_shortstr(Any x);

// as_* functions are used in compiled code to ensure that the type
// matches the expected type. as_* calls are generated from source
// code that looks like expression.as_int
int64_t as_int(Any x);
double as_real(Any x);
void *as_ptr(Any x);

/** @brief Obtains the underlying type of an Any, mainly for debugging */
std::string get_type(Any x);

//
//enum Tag { tag_free,
//       tag_symbol,
//       tag_integer,
//       tag_string,
//       tag_array,
//       tag_arraydata,
//       tag_dict,
//       tag_object,
//       tag_file };
//
//
//// all garbage-collected objects inherit this base class
////
//class Basic_obj {
//  public:
//    /* fields in header:
//     *
//     * header format is:
//     *  +---------------------------------------------+
//     *  | TAG      | COLOR    | #SLOTS    | NEXT      |
//     *  | (5 bits) | (2 bits) | (12 bits) | (45 bits) |
//     *  +---------------------------------------------+
//     * The NEXT field is a pointer to 8-byte aligned data.
//     * It is shifted right 3 bits because the low-order bits
//     * are zero. To use the pointer, shift it left 3 bits.
//     */
//    uint64_t header;  // contains type info, GC color, size
//
//    Tag get_tag();
//    void set_tag(Tag t);
//    bool has_tag(int tag);
//
//    Color get_color();
//    void set_color(uint8_t c);
//    bool has_color(int tag);
//
//    Obj *get_next();
//    void set_next(Obj *ptr);
//};
//
//
//// CSerpent string, with tag tag_string. For now, this is just
//// a C++ std:string "boxed" in a Basic_obj so it can be GC'd:
////
//class CSString : public Basic_obj {
//  public:
//    string str;
//}
//
//
//class Symbol : public Basic_obj {
//  public:
//    CSString *name;
//    Any value;
//    // probably need a function pointer here too
//};
//
//
//// all user-defined object inherit from this, which has a class
//// pointer
////
//class Obj : public Basic_obj {
//  public:
//    Symbol *type_symbol;  // symbol denoting class name
//};

