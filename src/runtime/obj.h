//
// Created by anthony on 6/4/24.
//
#pragma once
#include <cstdint>

enum Tag { tag_free,
    tag_symbol,
    tag_integer,
    tag_string,
    tag_array,
    tag_arraydata,
    tag_dict,
    tag_object,
    tag_file };

enum Color { white, grey, black };

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

    Tag get_tag() {return tag_array;}; // replace after implementation
    void set_tag(Tag t);
    bool has_tag(int tag);

    Color get_color();
    void set_color(uint8_t c);
    bool has_color(int tag);

    // What do these do?
    //Obj *get_next();
    //void set_next(Obj *ptr);
};

//// all user-defined object inherit from this, which has a class
//// pointer
//class Obj : public Basic_obj {
//public:
//    // note: I removed the pointer here
//    Symbol& name;  // symbol denoting class name
//};
