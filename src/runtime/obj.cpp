//
// Created by anthony on 6/5/24.
//
#include "any.h"
#include "gc.h"
#include "obj.h"
#include "csstring.h"
#include "array.h"
#include "symbol.h"
#include "csmem.h"
#include <utility>

Cs_class *Cs_class_class = nullptr;

#ifdef CSMALLLOC
void *Basic_obj::operator new(size_t size) {
    return csmalloc(size);
}


void Basic_obj::operator delete(void *p) noexcept {
    csfree(p);
}
#endif

// get the object pointed to by this object's header
Basic_obj *Basic_obj::get_next()
{
    return (Basic_obj *) ((header & ~0xFFFFE00000000000uLL) << 3);
}


void Basic_obj::set_next(Basic_obj *ptr)
{
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


// compute size of Basic_obj in bytes from the slot count:
int64_t slot_count_to_size(int64_t n)
{
    return sizeof(Basic_obj) + (n - 1) * sizeof(Any);
}

/* TODO: dynamic method calls
void check_dispatch(const std::string& method, const Array& args, const Dictionary& kwargs, size_t args_len, size_t kwargs_len) {
    if (len(args) != args_len || kwargs_len != 0) { /// change when kwargs are implemented
        std::cout << "Dispatch error by method: " << method << std::endl;
    }
}
*/

uint64_t Obj::get_any_slots()
{
    Cs_class *cs_class = (Cs_class *) (slots[0].integer);
    return cs_class->get_inst_any_slots();
}


//Obj::Obj(std::string name) : class_name {std::move(name)} {}

// Cserpent initialization - must be called to create runtime structures
void csobj_init()
{
    Cs_class_class = new Cs_class(new Symbol(new String("Class"), nullptr),
                                  4, 3); // 4 slots, first 2 are type Any
    // the class of Cs_class_class is Cs_class_class!
    Cs_class_class->set_slot(0, Cs_class_class);
}
