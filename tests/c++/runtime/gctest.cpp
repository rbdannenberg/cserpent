// gctest.cpp -- simple tests for garbage collection
//
// Roger B. Dannenberg
// June 2024

// This is a modified version of memtest.cpp where there are no calls to csfree
// and gc is used instead.

#include <cstdlib>
#include "any.h"
#include "gc.h"
#include "obj.h"
#include "array.h"
#include "csmem.h"
#include <iostream>
#include <string>

using namespace std;

constexpr int nobjs = 1000000;
constexpr int obj_cache_size = 10;

Obj *allocation_j[nobjs];

Cs_class *cs_obj_class = NULL;

Array *the_array = nullptr;
Obj *obj_cache[obj_cache_size];

void gc_mark_roots()
{
    // the only "global" in this test is cs_obj_class and cs_class_class, but
    // cs_class_class is referenced by cs_obj_class, so no need to mark it here.
    heap_obj_make_gray(cs_class_class);
    heap_obj_make_gray(cs_obj_class);
    for (int i = 0; i < obj_cache_size; i++) {
        /* if (obj_cache[i]) {
            printf("%d: %p\n", i, obj_cache[i]);
        } */
        heap_obj_make_gray(obj_cache[i]);
    }
    heap_obj_make_gray(the_array);
}

int main()
{
    csmem_init();
    main_init();
    srandom(12345);
    // clear the cache:
    memset(obj_cache, 0, sizeof(Obj *) * obj_cache_size);
    
    // allocate nobjs. Do not hold references to them, so all objects will
    // be freed except for Cs_class objects. Each object has a random size,
    // but we need to give a class to each, so we'll make a class that
    // computes the number of slots from the object's header. The class will
    // return a bitmap to indicate Any slots. The first slot is the class
    // pointer, so it is always 1. Beyond that, we'll have up to 3 more
    // pointer slots bounded by the total number of slots. Since the GC
    // only looks at as many bits as there are slots, it is OK if there
    // are some extra bits, although note that some objects have more slots
    // than needed, so if we try to allocate 2 slots, we might actually get
    // 3, and the third will be designated as "Any" - so be careful.
    //
    // As we allocate objects, we will keep a small cache of objects and
    // set some Any slots in the cache to newly created objects to create
    // some references from object to object in the heap. The cache will
    // be marked in gc_mark_roots() as if they are global variables.
    //
    // For arrays, we create an array every 1000 objects or so and put it
    // in  the_array. Then, every
    // 10th object is written to a random spot in the array. Eventually,
    // the array is replaced, so the objects are freed. The test will see
    // if the objects are finally freed. We can also scan the array and
    // make sure it does not have pointers to freed objects.

    cs_obj_class = new Cs_class {Symbol("Obj"), 1, 0b1111, &cs_class_table};
    printf("cs_obj_class has %lld slots\n", cs_obj_class->get_slot_count());

    for (long j = 0; j < nobjs; j++) {
        // pick a random size - since we can only have up to about 64 slots
        // due to the use of bitmaps to indicate pointers, we'll pick a small
        // size. For this test, we'll use a class that contains no pointers,
        // so regardless of size, the pointer bitmap will be 1, meaning slot[0]
        // points to the class and no other slots are pointers.
        long slots = 2 + random() % 60;

        // allocate new object
        gc_heap_check();
        Obj *obj = (Obj *) csmalloc(slots * 8);
        // printf("allocated obj %p size %ld\n", obj, slots * 8);
        obj->set_slot(0, cs_obj_class);
        allocation_j[j] = obj;  // what was allocated on this iteration?

        // fill object slots
        int64_t nslots = obj->get_slot_count();  // may be > requested
        for (int i = 1; i < 4 && i < nslots; i++) {
            obj->set_slot(i, nullptr);
        }
        
        for (int i = 4; i < nslots; i++) {
            obj->slots[i].integer = j + i;
        }

        // write a slot in the cache, maybe to point to this new object
        Obj *o = obj_cache[random() % obj_cache_size];
        if (o) {  // careful: initially the cache is all nullptr
            int slot = 1 + (random() % 3);
            // for small objects, slot may be too big. Just skip it.
            if (slot < o->get_slot_count()) {
                // only write object into slot about half the time
                if (random() % 2 == 0) {
                    o->set_slot(slot, obj);
                } else {
                    o->set_slot(slot, 123);  // set to a nan-boxed integer
                }
            }
        }

        // Every once in awhile, write a new object into the cache
        // Every 10 objects means the average lifetime in the cache is 100
        if (random() % 10 == 0) {
            gc_trace(obj, "storing obj in cache");
            obj_cache[random() % obj_cache_size] = obj;
            // now obj has a reference to it. Does GC need to know?
            if (gc_write_block) {
                heap_obj_make_gray(obj);
            }
        }

        // ARRAY TESTING

        if (j % 1000 == 10) { // every 10th object goes into the_array
            if (the_array) {
                int64_t n = the_array->len();
                int64_t r = random() % n;  // random index to the_array
                the_array->set(r, obj);
            }
        }

        if (j % 1000 == 0) {
            if (the_array) {  // test for valid-looking elements
                int64_t n = the_array->len();
                for (int k = 0; k < n; k++) {
                    Obj *elem = (Obj *) to_heap_obj((*the_array)[k]);
                    assert(elem == nullptr || (int64_t) elem > 0x100000000);
                    assert(elem == nullptr || elem->get_tag() == tag_object);
                    assert(elem == nullptr || elem->get_color() != GC_FREE);
                }
            }
            the_array = new Array(random() % 50 + 1, nullptr);
        }

        // END ARRAY TESTING

        // Now that we took care of obj, either creating a reference to it
        // or not, we can do some garbage collecting.
        if (j % 1 == 0) {  // run gc every N objects (not sure what the
            gc_poll();       //    value should be)
        }

        gc_heap_check();  // debugging consistency check
        gc_gray_check();
    }
    
    // run a full cycle of GC without creating anything to see how much
    // we can free. Before starting, erase the obj_cache to free those objects:
    for (int i = 0; i < obj_cache_size; i++) {
        // check: none of the cached objects has been freed:
        assert(!obj_cache[i] || obj_cache[i]->get_color() != GC_FREE);
        obj_cache[i] = nullptr;
    }
    the_array = nullptr;

    cout << "Number of currently allocated objects: " <<
            cs_current_object_count << endl;
    int gc_cycle_target = (int) gc_cycles + 2;  // finish this one, run one more
    while (gc_cycles < gc_cycle_target) {
        extern int64_t gc_high_water;  // hack this to allow GC to run again
        gc_high_water = 0;
        gc_poll();
    }
    cout << "Completed full GC cycle without new allocation." << endl;
    gc_heap_print();

#ifdef SUMMARY
    cssummary();
#endif

    cout << "Total memory in heap: " << cs_heapsize << endl;
    cout << "Total memory in heap not counting unused chunk memory: " <<
            (cs_heapsize - cs_chunkmem()) << endl;
    cout << "Total allocated bytes: " << cs_current_bytes_allocated << endl;
    cout << "Number of currently allocated objects: " <<
            cs_current_object_count << endl;
    cout << "Number of created objects: " << cs_allocations << endl;
    cout << "Completed GC cycles: " << gc_cycles << endl;
}
