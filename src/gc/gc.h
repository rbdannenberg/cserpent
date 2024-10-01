// gc.h - garbage collection declarations
#pragma once

// enable tracing object allocation/marking/freeing
#define GC_DEBUG 0
// enable expensive GC debugging/checking:
#define GC_DEBUG_2 0
#if GC_DEBUG
class Basic_obj;

// if obj matches gc_trace_ptr, print msg and some object info
void gc_trace(Basic_obj *obj, const char *msg);

// same as gc_trace, but also prints index, e.g. a slot number
void gc_trace_2(Basic_obj *obj, const char *msg, int index);
#else
#define gc_trace(ptr, msg)
#define gc_trace_2(ptr, msg, index)
#endif

enum Gc_color {
    GC_FREE = 0,
    GC_BLACK = 1,
    GC_GRAY = 2,
    GC_WHITE = 3
};

enum Gc_states {
    GC_START,
    GC_MARK,
    GC_MARKB,
    GC_MARK_ARRAY,
    GC_MARK_ARRAYB,
    GC_MARK2,
    GC_MARK_STACK,
    GC_MARK3,
    GC_SWEEP,
    GC_SWEEP1,
    GC_SWEEP2,
    GC_FINAL,
    GC_IDLE
};

extern Gc_states gc_state;


// Every stack frame is different because every stack frame has
// different local variables of type Any, but every stack frame starts
// with a header, giving the number of locals, followed by that many
// fields of type Any. Gc_frame is a "generic" stack frame where
// locals are accessed as anys[0], anys[1], ...
//
extern struct Gc_frame {
    uint64_t header;
    Any anys[1];
} *gc_frame_ptr;

extern void *gc_stack_top;  // link to top stack frame
extern Gc_color gc_frame_color;  // initial color for new frames
extern Gc_color gc_initial_color;
extern Basic_obj *gc_gray_list;
extern bool gc_write_block;
extern bool gc_local_write_block;
extern Array *gc_array;
extern int64_t gc_array_index;

// how many mark/sweep cycles have been completed?
extern int64_t gc_cycles;

void if_node_make_gray(Any x);
void basic_obj_make_gray(Basic_obj *obj);
void gc_poll();

// this function is defined by the runtime system to call basic_obj_make_gray
// on all heap objects accessible through globals
void gc_mark_roots();

#if GC_DEBUG
void gc_heap_print();
void gc_gray_check();
#else
#define gc_heap_print()
#define gc_gray_check()
#endif

#if GC_DEBUG_2
void gc_heap_check();
#else
// gc_heap_check is a no-op:
#define gc_heap_check()
#endif
