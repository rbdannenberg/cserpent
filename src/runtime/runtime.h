#pragma once

extern Cs_class *cs_obj_class; 

void runtime_init();
void runtime_mark_roots();

// macro to set locals.header and gc_stack_top when there
// are locals or parameters of type Any:
#define STD_FUNCTION_ENTRY(n) \
    locals.header = (((int64_t) tag_frame) << 59) + \
                    (((int64_t) gc_frame_color) << 57) + \
                    (((int64_t) (n)) << 45) + \
                    (((int64_t) gc_stack_top) >> 3); \
    gc_stack_top = (void *) &locals;

// macro to pop the frame stack and adjust gc_frame_ptr if needed on exit
// the parameter is the function result expression
#define STD_FUNCTION_EXIT(result) \
    gc_stack_top = (void *) ((locals.header & ~0xFFFFE00000000000uLL) << 3); \
    if ((Frame *) gc_frame_ptr == &locals) \
        gc_frame_ptr = (Gc_frame *) gc_stack_top; \
    return (result);
