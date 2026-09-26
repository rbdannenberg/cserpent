#include "any.h"
#include "gc.h"
#include "header.h"
#include "heap_obj.h"

int64_t Header::get_slot_count()
{
    int64_t nslots = (header >> 45) & 0xFFF;
    if (nslots == 0) {
        Heap_obj *hobj = (Heap_obj *) (this + 1);
        return hobj->slots[0].integer;
    }
    return nslots;
}

