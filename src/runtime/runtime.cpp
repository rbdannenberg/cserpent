// runtime.cpp - runtime_init() implementation
//
// Roger B. Dannenberg
// Sep 2024

#include <assert.h>
#include "csmem.h"
#include "any.h"
#include "op_overload.h"
#include "gc.h"
#include "heap_obj.h"
#include "obj.h"
#include "array.h"
#include "dict.h"
#include "symbol.h"
#include "runtime.h"

Cs_class *cs_obj_class = nullptr;

Any css_append;
Any css_last;
Any css_insert;
Any css_unappend;
Any css_uninsert;
Any css_reverse;
Any css_copy;
Any css_set_len;
Any css_get_name;
Any css_get_inst_slot_count;
Any css_get_inst_any_slots;
Any css_get_member_table;
Any css_Class;
Any css_Obj;

bool gc_enabled = false;


void runtime_init()
{
    csmem_init();  // memory heap initialization
    // create some special signposts for Dict entries that will be ignored
    // by GC and and cannot match "real" strings because of missing EOS
    DICT_EMPTY.integer = SHORT_TAG + 0x010101010101;
    DICT_DELETED.integer = SHORT_TAG + 0x020202020202;

    cs_symbol_table = new Dict();
    css_t = new Symbol("t", nil, nil);
    *(css_t->value()) = &css_t;  // 't' evaluates to itself
    
    // note: cs_obj_class is the top-most class, no superclass
    // cs_class_class inherits from cs_obj_class
    // Every class is an instance of cs_class_class, including
    // cs_obj_class and cs_class_class itself.
    css_Obj = new Symbol("Obj", nil, nil);
    cs_obj_class = new Cs_class { to_symbol(css_Obj), 1, 0b1,
                                  &cs_class_table };

    css_Class = new Symbol("Class", nil, nil);
    cs_class_class = new Cs_class { to_symbol(css_Class), 5, 0b1,
                                    &cs_class_table, cs_obj_class };

    css_append = new Symbol("append", nil, nil);
    css_last = new Symbol("last", nil, nil);
    css_insert = new Symbol("insert", nil, nil);
    css_unappend = new Symbol("unappend", nil, nil);
    css_uninsert = new Symbol("uninsert", nil, nil);
    css_reverse = new Symbol("reverse", nil, nil);
    css_copy = new Symbol("copy", nil, nil);
    css_set_len = new Symbol("set_len", nil, nil);
    css_get_name = new Symbol("get_name", nil, nil);
    css_get_inst_slot_count = new Symbol("get_inst_slot_count", nil, nil);
    css_get_inst_any_slots = new Symbol("get_inst_any_slots", nil, nil);
    css_get_member_table = new Symbol("get_member_table", nil, nil);

    gc_stack_top = NULL;
    gc_enabled = true;
}


void runtime_mark_roots()
{
    heap_obj_make_gray(cs_obj_class);
    heap_obj_make_gray(cs_symbol_table);
}
