//
// Created by anthony on 6/4/24.
//
#pragma once
#include <cstdint>
#include <string>
#include <iostream>

union Any;
class Dictionary;
class Array;

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
    uint64_t header;   // contains type info, GC color, size
    int64_t slots[1];  // can actually be any number of slots

    // on return, Basic_obj header will have #SLOTS initialized
    // and slots[1] will be an unencoded integer slot count if
    // the slot count is greater than 4095, in which case #SLOTS
    // will be zero. The number of slots is based on size which
    // is rounded up to a multiple of 8 (the slot size). The
    // TAG will be tag_object, and the superclass constructors
    // for other types of objects should set TAG accordingly.
    //
#ifdef CSMALLOC
    void *operator new(size_t size);

    void operator delete(void *p) noexcept;
#endif
    Basic_obj() { };

    Tag get_tag(); // replace after implementation
    void set_tag(Tag t);
    bool has_tag(int tag);

    Color get_color();
    void set_color(uint8_t c);
    bool has_color(int tag);

    Basic_obj *get_next();
    void set_next(Basic_obj *ptr);

    int64_t get_size();
    int64_t get_slot_count();
};

//// all user-defined object inherit from this, which has a class
//// pointer
class Obj : public Basic_obj {
public:
//    std::string class_name; //TODO: reference?
//    // note: I removed the pointer here
//    Obj(std::string name);

    //Symbol& name;  // symbol denoting class name
    virtual Any call(const std::string& member_name, const Array& args, const Dictionary& kwargs) =0;
    virtual Any& get(const std::string& member_name)=0;
    // probably need a const version of get
};

void check_dispatch(const std::string& method, const Array& args, const Dictionary& kwargs, size_t args_len, size_t kwargs_len);

