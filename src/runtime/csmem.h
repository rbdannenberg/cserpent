// csmem.h -- memory management API

// allocate one object of type t
#define CSMALLOCT(t) (t *) csmalloc(sizeof(t))

// allocate n objects of type t
#define CSMALLOCTN(t, n) (t *) csmalloc(sizeof(t) * n)

// free an object
#define CSFREE(x) csfree(x)

void *csmalloc(size_t len);
void *csfree(void *x);

