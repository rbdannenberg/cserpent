// memtest.cpp -- simple tests for csmem.{cpp,h}
//
// Roger B. Dannenberg
// June 2024

#include <cstdlib>
#include "any.h"
#include "csmem.h"
#include <iostream>

using namespace std;

constexpr int nobjs = 1000;
Basic_obj *objects[nobjs];
long allocation_j[nobjs];


int main()
{
    csmem_init();
    srandom(12345);

    for (long i = 0; i < nobjs; i++) {
        objects[i] = nullptr;
    }

    for (long j = 0; j < 1000000; j++) {
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
                assert(objects[index]->slots[i] == index + i);
            }            
            csfree(objects[index]);
        }
        // allocate new object
        objects[index] = (Basic_obj *) csmalloc(size);
        allocation_j[index] = j;  // at what iteration was this allocated?

        // fill object slots
        int64_t nslots = objects[index]->get_slot_count();
        int start = (nslots >= 4096);
        for (int i = start; i < nslots; i++) {
            objects[index]->slots[i] = index + i;
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

    cout << "Total memory in heap: " << csheapsize() << endl;
    cout << "    less available memory in chunk: " <<
            (csheapsize() - cschunkmem()) << endl;
    cout << "Total allocated: " << csallocated() << endl;
    cout << "Total of object sizes: " << total << endl;
    
}
