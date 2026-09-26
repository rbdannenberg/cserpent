//
// Created by anthony on 7/5/24.
//

#include "any.h"
#include "gc.h"
#include "header.h"
#include "heap_obj.h"
#include "csmem.h"

void *Heap_obj::operator new(size_t size)
{
    size += sizeof(Header);
    return (Heap_obj *) ((char *) csmalloc(size) + sizeof(Header));
}


void Heap_obj::set_slot(int i, Any x)
{
    check(i >= 0 && i < get_slot_count());
    SLOT_WRITE_BLOCK(this, x);
    slots[i] = x;
}


// compute size of Heap_obj in bytes from the slot count:
int64_t slot_count_to_size(int64_t n)
{
    return sizeof(Header) + n * sizeof(Any);
}
