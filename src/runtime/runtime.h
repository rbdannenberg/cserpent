#pragma once

void runtime_init();
void runtime_mark_roots();
void cs_global_mark();  // must be defined by application to mark all globals
                        // that are not referenced by symbols


struct Cs_frame {
public:
    Header header;
    void set(int i, Any x) { ((Heap_obj *) (&header + 1))->set_slot(i, x); }
};

// Cs_frame is used as the superclass for Frame, which is allocated on the
// stack for local variables. The Frame instance is always named L, and to
// set a local variable in L, always use the LSET macro. The LSET macro 
// was introduced to allow all sets to be followed by 
//     std::atomic_signal_fence(std::memory_order_seq_cst);
// which was supposed to prevent optimization from reordering or
// eliminating the store, but that did not explain or fix a problem, and it
// could add some overhead, so for now, we are running without the fence.
// (The fence was also added, then removed, from CS_FUNCTION_ENTRY and 
// CS_FUNCTION_EXIT macros.)
//
#define LSET(i, x) L.set(i, x);

// macro to set L.header and gc_stack_top when there
// are L or parameters of type Any:
#define CS_FUNCTION_ENTRY(n) \
    if (n > 0) memset(&L, 0, sizeof(L)); \
    L.header.initialize(tag_frame, gc_frame_color, n); \
    L.header.header += ((int64_t) gc_stack_top) >> 3; \
    gc_stack_top = (void *) &L;

// macro to pop the frame stack and adjust gc_frame_ptr if needed on exit
// the parameter is the function result expression
#define CS_FUNCTION_EXIT(result) \
    gc_stack_top = L.header.get_next(); \
    if ((Frame *) gc_frame_ptr == &L) \
        gc_frame_ptr = (Gc_frame *) gc_stack_top; \
    return (result);

extern Symbol *css_append;
extern Symbol *css_last;
extern Symbol *css_insert;
extern Symbol *css_unappend;
extern Symbol *css_uninsert;
extern Symbol *css_reverse;
extern Symbol *css_copy;
extern Symbol *css_set_len;
extern Symbol *css_get_name;
extern Symbol *css_get_inst_slot_count;
extern Symbol *css_get_inst_any_slots;
extern Symbol *css_get_member_table;
extern Symbol *css_Class;
extern Symbol *css_Obj;
