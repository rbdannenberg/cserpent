//
// Created by anthony on 7/5/24.
//

#include "any.h"
#include "gc.h"
#include "basic_obj.h"
#include "csmem.h"

void *Basic_obj::operator new(size_t size) {
    return csmalloc(size);
}


// get the object pointed to by this object's header
Basic_obj *Basic_obj::get_next()
{
    assert((uint64_t)((header  & ~0xFFFFE00000000000uLL) << 3) == 0 ||
           (uint64_t)((header  & ~0xFFFFE00000000000uLL) << 3) > 0x100000000);
    return (Basic_obj *) ((header & ~0xFFFFE00000000000uLL) << 3);
}


void Basic_obj::set_next(Basic_obj *ptr)
{
    assert(!ptr || (int64_t) ptr > 0x100000000);
    header = (header & ~0x00001FFFFFFFFFFFuLL) |
             (((uint64_t) ptr) >> 3);
}


Tag Basic_obj::get_tag()
{
    return (Tag) ((header >> 59) & 0x1F);
}


void Basic_obj::set_tag(Tag tag)
{
    header = (header & ~0xF800000000000000uLL) |
             (static_cast<uint64_t>(tag) << 59);
}


Gc_color Basic_obj::get_color()
{
    return static_cast<Gc_color>((header >> 57) & 0x03);
}


void Basic_obj::set_color(Gc_color c)
{
    // if already gray and being set to gray, something is wrong:
    assert(get_color() != GC_GRAY || c != GC_GRAY);

    header = (header & ~0x0600000000000000uLL) |
             (static_cast<uint64_t>(c) << 57);
}


void Basic_obj::set_white()
{
    assert(get_color() == GC_GRAY);
    header = (header & 0xF9FFE00000000000uLL) | 0x0600000000000000uLL;
}


int64_t Basic_obj::get_slot_count()
{
    int64_t nslots = (header >> 45) & 0xFFF;
    // when slots is zero, size is big and stored in the first slot
    // as unencoded integer
    return (nslots == 0) ? slots[0].integer : nslots;
}


int64_t Basic_obj::get_size()
{
    return offsetof(Basic_obj, slots) + get_slot_count() * sizeof(Any);
}


void Basic_obj::set_slot(int i, Any x) {
    Basic_obj *xptr;
    assert(i >= 0 && i < get_slot_count());
    if (gc_write_block && x.integer && is_basic_obj(x) &&
        (xptr = to_basic_obj(x))->get_color() == GC_BLACK &&
        get_color() != GC_BLACK) {
        basic_obj_make_gray(xptr);
    }
    slots[i] = x;
}


// compute size of Basic_obj in bytes from the slot count:
int64_t slot_count_to_size(int64_t n)
{
    return sizeof(Basic_obj) + (n - 1) * sizeof(Any);
}

