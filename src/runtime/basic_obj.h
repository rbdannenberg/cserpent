//
// Created by anthony on 7/5/24.
//
#pragma once
#include <cstdint>
#include "gc.h"

union Any;

enum Tag {
    tag_free,
    tag_symbol,
    tag_integer,
    tag_string,
    tag_array,
    tag_dict,
    tag_object,
    tag_file };

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

    // on return, Basic_obj header will have #SLOTS initialized.  If
    // #SLOTS is zero, then slots[0] will be an unencoded integer slot
    // count. The number of slots is based on size which is rounded up
    // to a multiple of 8 (the slot size).  Initially, TAG will be
    // tag_object, and the superclass constructors for other types of
    // objects should set TAG accordingly.

    // All Cserpent objects (class Obj, with TAG==tag_object, have at
    // most 64 slots, so #SLOTS is never zero. (Other types such as
    // array data or strings can have any number of slots.) The limit
    // of 64 allows for a 64-word of bits indicating whether each slot
    // is a pointer to another Basic_obj or is of type Any. Either
    // way, if the bit is 1 AND the high-order slot bits are zero, we
    // trace the pointer in the low-order (48) bits in the mark phase
    // of GC.  If the bit is 0, the slot has a declared static type
    // known to the compiler, e.g. the integer 0 will be stored as all
    // 0, and it can be accessed as an integer without type checking
    // and sign extending as would be done for integers stored in an
    // Any.

    // If TAG is not tag_object, the layout is fixed, and this
    // Basic_obj is sub-classed. E.g. Array inherits from Basic_obj
    // and has tag==tag_array.

    void *operator new(size_t size);

    void operator delete(void *p) { assert(false);  /* use GC instead */ };
    
    // Note: since Basic_obj has variable size, the real constructor
    // is csmalloc, which sets the header and possibly puts large
    // sizes in slot[0]:
    Basic_obj() { }

    Tag get_tag(); // replace after implementation
    void set_tag(Tag t);
    bool has_tag(int tag) { return tag == get_tag(); }

    Gc_color get_color();
    void set_color(Gc_color c);

    // for use by GC: sets a gray-listed object to white and sets the
    // next pointer to NULL since object is no longer on any list.
    // Preserves the type tag and slot count in the header.
    void set_white();

    bool has_color(int tag);

    Basic_obj *get_next();
    void set_next(Basic_obj *ptr);

    int64_t get_size();

    // get_slot_count() returns the number of slots allocated. This may
    // be larger than the number of slots requested, needed, or used.
    // There may be "extra" slots at the end of the object. It is the
    // object's responsibility to know it's "actual" or requested size
    // if it is important. Since Obj has get_any_slots() to return 1-bits
    // at the slots of type Any, the garbage collector will not treat
    // "extra" object slots as Any or pointers, thus garbage in extra
    // slots will not cause problems. For other Basic_obj's such as
    // Array data, there is an array length pointer to tell GC which
    // slots are valid Any data and which are "extra" to ignore.
    int64_t get_slot_count();

    void set_slot(int i, Any x);
};


// get the size of a Basic_obj in bytes as a function of the slot count:
int64_t slot_count_to_size(int64_t n);
