// csmem.h -- memory management API
#include <cstdlib>

// allocate one object of type t
#define CSMALLOCT(t) (t *) csmalloc(sizeof(t))

// allocate n objects of type t
#define CSMALLOCTN(t, n) (t *) csmalloc(sizeof(t) * n)

// free an object
#define CSFREE(x) csfree(x)

void csmem_init();
void *csmalloc(size_t len);
void csfree(void *x);
int64_t csheapsize();
int64_t csallocated();
int64_t cschunkmem();  // how much is remaining in current chunk
#define SUMMARY 1
#ifdef SUMMARY
void cssummary();
#endif
