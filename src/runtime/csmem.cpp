// csmem.cpp - memory allocation
//
// Created by anthony on 5/17/24.
//
#include "any.h"
#include "gc.h"
#include "obj.h"
#include <iostream>
#include <cassert>
#include <cstring>
#include "csmem.h"

int64_t cs_heapsize = 0;
int64_t cs_current_bytes_allocated = 0;
int64_t cs_current_object_count = 0;
int64_t cs_allocations = 0;


//----------- heap consists of a list of large memory areas ----------
// A Chunk is allocated using malloc, but never freed (malloc() is
// non-blocking but free() can block)

// the list of chunks is used to enumerate all allocated objects
Chunk *cs_chunk_list = nullptr;

void Chunk::new_chunk(int64_t n)
{
   n += offsetof(Chunk, chunk);
   cs_heapsize += n;  // how much has been malloc'd
   Chunk *chunk = (Chunk *) malloc((size_t) n);
   chunk->next = cs_chunk_list;
   cs_chunk_list = chunk;
   chunk->size = n;
   chunk->next_free = chunk->chunk;
}
    

int64_t cs_chunkmem()
{
    return cs_chunk_list->unused();
}


//----------- free lists store freed Basic_objects ----------
#define LOG2_MAX_LINEAR_BYTES 9 // up to (512 - 16) byte chunks
#define MAX_LINEAR_BYTES (1 << LOG2_MAX_LINEAR_BYTES)
#define LOG2_MAX_EXPONENTIAL_BYTES 25 // up to 16MB = 2^24
#define MAX_EXPONENTIAL_BYTES (1 << LOG2_MAX_EXPONENTIAL_BYTES)
#define LOG2_MEM_QUANTUM 4  // linear sizes increment by this
#define MEM_QUANTUM (1 << LOG2_MEM_QUANTUM)

// blocks from size 16 to 512-16 in steps of 16
Basic_obj *linear_free[MAX_LINEAR_BYTES / MEM_QUANTUM - 1];
// blocks from size 512 to 16MB increasing by factors of 2
// (index 0 -> 512, index 15-> 16MB)
Basic_obj *exponential_free[LOG2_MAX_EXPONENTIAL_BYTES - LOG2_MAX_LINEAR_BYTES];


void csmem_init()
{
    memset(linear_free, 0, sizeof(linear_free));
    memset(exponential_free, 0, sizeof(exponential_free));
}


// get the log2 of block size at least as big as size
static int power_of_2_block_size(size_t size)
{
    int log = LOG2_MAX_LINEAR_BYTES;
    assert(size < ((size_t) 1 << 63));  // otherwise loop does not terminate
    while (size > ((size_t) 1 << log)) {
        log++;
    }
    
    return log;
}


// find head of free list for this size object.
// Sets *size to the actual object size, which is >= requested size
static Basic_obj **head_ptr_for_size(size_t *size)
{
    // index to linear_free: shift to divide by MEM_QUANTUM. We want to
    // round up to next multiple of MEM_QUANTUM, so we want something like
    // (size + 15) / 16. But the index should be offset by one because we
    // store size 16 objects at index 0. This gives (size + 15) / 16 - 1,
    // but that simplifies to (size - 1) / 16:
    size_t index = (*size - 1) >> LOG2_MEM_QUANTUM;
    if (index < MAX_LINEAR_BYTES / MEM_QUANTUM - 1) {
        *size = (index + 1) << LOG2_MEM_QUANTUM;
        return &linear_free[index];
    }
    // otherwise we allocate from exponential_free:
    int log2size = power_of_2_block_size(*size);
    if (log2size < LOG2_MAX_EXPONENTIAL_BYTES) {
        *size = (size_t) (1 << log2size);
        // index 0 corresponds to size of MAX_LINEAR_BYTES (512):
        return &exponential_free[log2size - LOG2_MAX_LINEAR_BYTES];
    }
    return nullptr;  // too big
}


void *csmalloc(size_t size)
{
    // size constraints: size < 16 makes theoretical sense, but every object
    // should have at least the header (8 bytes) and 1 slot (8 bytes), so
    // anything less than 16 is probably an error. While zero slots is
    // imaginable (a class with only methods and no instance variables),
    // we use 0 slots in the header to designate a large object with the
    // actual slot count stored in the first slot location, so we have no
    // way to encode 0 slots.
    assert(size >= 16);
    Basic_obj **head = head_ptr_for_size(&size);
    int64_t slots = (size - 1) >> 3;  // 8 bytes per slot, not counting header,
    // so if there are 2 slots, the object size is 24, and 23 / 8 = 2.;
    // now size is the actual allocation size, not the object size
    cs_current_bytes_allocated += (slots + 1) << 3;  // allocated includes
            // unused bytes, if there is fragmentation (e.g.
            // head_ptr_for_size(&size) will round size up to the next
            // allocation size, so this may create some unused space in
            // an object.) I would prefer to count only requested, i.e.
            // used, bytes, but when we free objects, we do not have any
            // record of what was originally requested, and to know
            // this would require more storage and or more work.
    void *result;

    assert(head);  // must have a freelist corresponding to size
    // check for plausible pointer
    assert(*head == nullptr || ((int64_t) *head) > 0x100000000);
    
    if (*head) {  // return object from list if possible
        result = (void *) *head;
        *head = (*head)->get_next();
        goto got_it;
    }

    // otherwise return object from chunk if possible
    if (!cs_chunk_list) {  
        Chunk::new_chunk(CHUNK_SIZE);
    }

    result = cs_chunk_list->allocate(size);
    if (!result) {  // can't allocate from chunk, need a new one
        // optimization: If the current chunk has more than 1000
        // bytes free (10%), then allocate a chunk solely for the
        // new request (at least 1000 bytes, so overhead of using
        // a whole chunk is very small). Also do this if the new
        // request is bigger than CHUNK_SIZE. Otherwise, start a
        // new chunk.
        if (cs_chunk_list->unused() > 1000 || size > CHUNK_SIZE) {
            Chunk *old_chunk = cs_chunk_list;
            Chunk::new_chunk(size);
            result = cs_chunk_list->allocate(size);
            // now new chunk is cs_chunk_list, but we want the old_chunk to
            // remain at the head of the list for further allocations
            cs_chunk_list->next = old_chunk->next;
            old_chunk->next = cs_chunk_list;
            cs_chunk_list = old_chunk;
            // now we have cs_chunk_list = old_chunk -> new chunk -> rest
            // so next allocation can continue from old chunk
        } else {
            int64_t chunk_size = (size > CHUNK_SIZE) ? size : CHUNK_SIZE;
            Chunk::new_chunk(chunk_size);
            result = cs_chunk_list->allocate(size);
        }
        assert(result);
    }
got_it:  // set header and return object
    // rewrite slots because each size category supports at least 2
    // different slot counts, e.g. it could be 2 or 3 and be on the
    // same free list:
    if (slots >= 4096) {
        ((Basic_obj *) result)->slots[0].integer = slots;
        slots = 0;
    }
    ((Basic_obj *) result)->header = (((int64_t) tag_object)<< 59) +
                                     (((int64_t) initial_color) << 57) +
                                     (slots << 45);
    assert(((Basic_obj *) result)->get_size() == size);
    cs_current_object_count++;
    cs_allocations++;
    gc_trace((Basic_obj *) result, "allocated");
    return result;
}


#ifdef SUMMARY

long list_len(Basic_obj *head) {
    long len = 0;
    while (head) {
        len++;
        head = head->get_next();
    }
    return len;
}

// print summary of what's on freelists
void cssummary()
{
    std::cout << "SUMMARY OF FREE LISTS" << std::endl;
    long total_free = 0;
    long elem_size = 16;
    for (int i = 0; i < MAX_LINEAR_BYTES / MEM_QUANTUM - 1; i++) {
        long len = list_len(linear_free[i]);
        long total = len * elem_size;
        std::cout << "size " << elem_size << " len " << len << " total " <<
                     total << std::endl;
        total_free += total;
        elem_size += MEM_QUANTUM;
    }
    elem_size = MAX_LINEAR_BYTES;
    for (int i = 0; i < LOG2_MAX_EXPONENTIAL_BYTES - LOG2_MAX_LINEAR_BYTES;
         i++) {
        long len = list_len(exponential_free[i]);
        long total = len * elem_size;
        std::cout << "size " << elem_size << " len " << len << " total " <<
                     total << std::endl;
        total_free += total;
        elem_size *= 2;
    }
    std::cout << "-------- total free bytes " << total_free << std::endl;
    std::cout << cs_current_object_count <<
                 " currently allocated objects" << std::endl;
}
#endif


void csfree(void *object)
{
    // can only free Basic_obj objects
    Basic_obj *obj = (Basic_obj *) object;
    
    // check for plausible pointer
    assert(((int64_t) obj) > 0x100000000);
    
    gc_trace(obj, "freed");
    size_t size = (size_t) (obj->get_size());
    cs_current_bytes_allocated -= size;

    Basic_obj **head = head_ptr_for_size(&size);
    assert(head);
    obj->set_next(*head);
    obj->set_tag(tag_free);
    *head = obj;
    cs_current_object_count--;
    // printf("pointer to obj size %lld at %p -> %p\n",
    //       obj->get_size(), head, obj);
}

