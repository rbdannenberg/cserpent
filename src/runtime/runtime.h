#pragma once

extern Cs_class *cs_obj_class; 

void runtime_init();
void runtime_mark_roots();

struct Cs_frame {
public:
    int64_t header;
    void set(int i, Any x) { ((Heap_obj *) this)->set_slot(i, x); }
};


// macro to set locals.header and gc_stack_top when there
// are locals or parameters of type Any:
#define STD_FUNCTION_ENTRY(locals, n) \
    locals.header = (((int64_t) tag_frame) << 59) + \
                    (((int64_t) gc_frame_color) << 57) + \
                    (((int64_t) (n)) << 45) + \
                    (((int64_t) gc_stack_top) >> 3); \
    gc_stack_top = (void *) &locals;

// macro to pop the frame stack and adjust gc_frame_ptr if needed on exit
// the parameter is the function result expression
#define STD_FUNCTION_EXIT(locals, result) \
    gc_stack_top = (void *) ((locals.header & ~0xFFFFE00000000000uLL) << 3); \
    if ((Frame *) gc_frame_ptr == &locals) \
        gc_frame_ptr = (Gc_frame *) gc_stack_top; \
    return (result);

extern Any css_append;
extern Any css_last;
extern Any css_insert;
extern Any css_unappend;
extern Any css_uninsert;
extern Any css_reverse;
extern Any css_copy;
extern Any css_set_len;
extern Any css_get_name;
extern Any css_get_inst_slot_count;
extern Any css_get_inst_any_slots;
extern Any css_get_member_table;
extern Any css_Class;
extern Any css_Obj;
