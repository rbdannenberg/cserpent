// memtest.cpp -- simple tests for csmem.{cpp,h}
//
// Roger B. Dannenberg
// June 2024

#include <cstdlib>
#include "any.h"
#include "gc.h"
#include "obj.h"
#include "csmem.h"
#include <iostream>
#include <string>

using namespace std;

constexpr int nobjs = 1000000;
Heap_obj *objects[nobjs];
long allocation_j[nobjs];

// This test does not use GC so gc_mark_roots() is only here to satisfy
// the linker
void gc_mark_roots()
{
    return;
}


int main()
{
    csmem_init();
    srandom(12345);
    
    // this has nothing to do with memtest, but I'm just curious about
    // C++ string implementation
    string foo("this is longer enough to show it is stored elsewhere");
    cout << "string size " << sizeof(foo) << endl;

    for (long i = 0; i < nobjs; i++) {
        objects[i] = nullptr;
    }
    
    // simplest test first: free immediately
    for (long i = 0; i < 100; i++) {
        long size = ((random() % 1000) & 0x07) + 16;
        Heap_obj *obj = (Heap_obj *) csmalloc(size);
        csfree(obj);
    }
    
    // test special size 47353
    Heap_obj *obj = (Heap_obj *) csmalloc(47353);
    csfree(obj);

    for (long j = 0; j < 1000; j++) {
        // pick a random size using a pseudo-float approach to bias toward
        // picking smaller sizes
        long logsize = random() % 17 + 4;  // 4 to 20
        long min_size = 1 << logsize;  // 16 to ~1M
        // pick final size in range from 2^logsize to 2^(logsize+1)
        long size = min_size + random() % min_size;
        // pick a random location
        long index = random() % nobjs;
        // free current object if any
        if (objects[index]) {
            // check slot values to make sure they are correct
            int64_t nslots = objects[index]->get_slot_count();
            int start = (nslots >= 4096);
            for (int i = start; i < nslots; i++) {
                assert(objects[index]->slots[i].integer == index + i);
            }            
            csfree(objects[index]);
        }

        // allocate new object
        objects[index] = (Heap_obj *) csmalloc(size);
        allocation_j[index] = j;  // at what iteration was this allocated?

        // fill object slots
        int64_t nslots = objects[index]->get_slot_count();
        int start = (nslots >= 4096);
        for (int i = start; i < nslots; i++) {
            objects[index]->slots[i].integer = index + i;
        }
    }

    // sum up sizes of all objects
    int64_t total = 0;
    for (int i = 0; i < nobjs; i++) {
        total += objects[i] ? objects[i]->get_size() : 0;
    }
    
#ifdef SUMMARY
    cssummary();
#endif

    cout << "Total memory in heap: " << cs_heapsize << endl;
    cout << "    less available memory in chunk: " <<
            (cs_heapsize - cs_chunkmem()) << endl;
    cout << "Total current bytes allocated: " <<
            cs_current_bytes_allocated << endl;
    cout << "Total of object sizes: " << total << endl;
    
}
