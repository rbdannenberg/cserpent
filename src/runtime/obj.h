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

enum Tag {
    tag_free,
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
    Any slots[1];  // can actually be any number of slots

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
    bool has_tag(int tag) { return tag == get_tag(); }

    Gc_color get_color();
    void set_color(Gc_color c);
    bool has_color(int tag);

    Basic_obj *get_next();
    void set_next(Basic_obj *ptr);

    int64_t get_size();
    int64_t get_slot_count();

    void set_slot(int i, Any x) {
        Basic_obj *xptr;
        if (write_block && x.integer && is_ptr(x) &&
            (xptr = to_ptr(x))->get_color() == GC_BLACK &&
            get_color() != GC_BLACK) {
            basic_obj_make_gray(xptr);
        }
        slots[i] = x;
    }
};


// get the size of a Basic_obj in bytes as a function of the slot count:
int64_t slot_count_to_size(int64_t n);


//// all user-defined objects inherit from this, which has a class
//// pointer in slots[0]
class Obj : public Basic_obj {
public:
//    std::string class_name; //TODO: reference?
//    // note: I removed the pointer here
//    Obj(std::string name);

    uint64_t get_any_slots();
    
    //Symbol& name;  // symbol denoting class name
    /* For now, I'm removing these to enable compilation of subclasses without
     * defining call and get methods. I now think these should be implemented
     * by attaching a dictionary to every Cs_class object where you can look
     * up fields and methods. Then call can be non-virtual since slot[0]
     * points to the Cs_class (similar to a vtable).
     *
    virtual Any call(const std::string& member_name, const Array& args, const Dictionary& kwargs) =0;
    virtual Any& get(const std::string& member_name)=0;
    // probably need a const version of get
     */
};

void check_dispatch(const std::string& method, const Array& args, const Dictionary& kwargs, size_t args_len, size_t kwargs_len);


class Cs_class;
extern Cs_class *Cs_class_class;

// This is the class of class descriptors. A Cs_class has these fields:
//   slots[0] - the "class class", the class of descriptors
//   slots[1] - the class name, a symbol (Symbol pointer)
//   slots[2] - the number of slots in class instances (int)
//   slots[3] - the bit map of slots that are of type Any
class Cs_class : public Obj {
  public:
    // make sure object gets allocated with enough space for 4 slots:
    int64_t more_slots[3];
    Cs_class() { };
    Cs_class(Symbol *name, int64_t slot_count, int64_t any_slots) {
        slots[0] = Cs_class_class;
        slots[1] = name;
        slots[2].integer = slot_count;
        slots[3].integer = any_slots;
    };
    Symbol *get_name() { return reinterpret_cast<Symbol *>(slots[1].integer); }
    int64_t get_inst_slot_count() { return slots[2].integer; }
    int64_t get_inst_any_slots() { return slots[3].integer; }
};


