//
// Created by anthony on 6/5/24.
//
#include "obj.h"
#include <data_structures/array.h>
#include "csmem.h"
#include <utility>

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
             (((uint64_t) tag) << 59);
}


int64_t Basic_obj::get_slot_count()
{
    int64_t nslots = (header >> 45) & 0xFFF;
    // when slots is zero, size is big and stored in the first slot
    // as unencoded integer
    return (nslots == 0) ? slots[0] : nslots;
}


int64_t Basic_obj::get_size()
{
    return offsetof(Basic_obj, slots) + get_slot_count() * sizeof(Any);
}

void check_dispatch(const std::string& method, const Array& args, const Dictionary& kwargs, size_t args_len, size_t kwargs_len) {
    if (len(args) != args_len || kwargs_len != 0) { /// change when kwargs are implemented
        std::cout << "Dispatch error by method: " << method << std::endl;
    }
}

//Obj::Obj(std::string name) : class_name {std::move(name)} {}