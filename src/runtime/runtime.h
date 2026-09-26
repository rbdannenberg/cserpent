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


// macro to set locals.header and gc_stack_top when there
// are locals or parameters of type Any:
#define CS_FUNCTION_ENTRY(locals, n) \
    locals.header.initialize(tag_frame, gc_frame_color, n); \
    locals.header.header += ((int64_t) gc_stack_top) >> 3; \
    gc_stack_top = (void *) &locals;

// macro to pop the frame stack and adjust gc_frame_ptr if needed on exit
// the parameter is the function result expression
#define CS_FUNCTION_EXIT(locals, result) \
    gc_stack_top = locals.header.get_next(); \
    if ((Frame *) gc_frame_ptr == &locals) \
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
