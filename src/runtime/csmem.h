// csmem.h -- memory management API
#include <cstdlib>
#include <cstdint>

// allocate one object of type t
#define CSMALLOCT(t) (t *) csmalloc(sizeof(t))

// allocate n objects of type t
#define CSMALLOCTN(t, n) (t *) csmalloc(sizeof(t) * n)

// free an object
#define CSFREE(x) csfree(x)

#define CHUNK_SIZE 1000000

class Chunk;

extern int64_t cs_heapsize;
extern int64_t cs_allocated;
extern Chunk *cs_chunk_list;


class Chunk {
public:
    Chunk *next;       // next chunk
    int64_t size;      // size of entire chunk
    char *next_free;   // pointer to unallocated memory
    char chunk[16];    // the block of memory

    static void new_chunk(int64_t n);
    
    int64_t unused() {
        return chunk + size - next_free;
    }

    void *allocate(int64_t n) {
        assert((n & 7) == 0);  // expects 8-byte alignment
        void *result = nullptr;
        if (next_free + n <= chunk + size) {
            result = next_free;
            next_free += n;
        }
        return result;
    }
};


void csmem_init();
void *csmalloc(size_t len);
void csfree(void *x);
int64_t cs_chunkmem();  // how much is remaining in current chunk
#define SUMMARY 1
#ifdef SUMMARY
void cssummary();
#endif
