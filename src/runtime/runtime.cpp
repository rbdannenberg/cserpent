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

Any symbol_append;
Any symbol_last;
Any symbol_insert;
Any symbol_unappend;
Any symbol_uninsert;
Any symbol_reverse;
Any symbol_copy;
Any symbol_set_len;
Any symbol_get_name;
Any symbol_get_inst_slot_count;
Any symbol_get_inst_any_slots;
Any symbol_get_member_table;
Any symbol_Class;
Any symbol_Obj;


void runtime_init()
{
    csmem_init();  // memory heap initialization
    main_init();   // creates cs_class_class, the superclass of classes
    // create some special signposts for Dict entries that will be ignored
    // by GC and and cannot match "real" strings because of missing EOS
    DICT_EMPTY.integer = SHORT_TAG + 0x010101010101;
    DICT_DELETED.integer = SHORT_TAG + 0x020202020202;

    cs_symbol_table = new Dict();
    symbol_t = new Symbol("t", nil, nil);
    cs_symbol_table->insert(to_symbol(symbol_t)->name(), symbol_t);

    symbol_append = new Symbol("append", nil, nil);
    symbol_last = new Symbol("last", nil, nil);
    symbol_insert = new Symbol("insert", nil, nil);
    symbol_unappend = new Symbol("unappend", nil, nil);
    symbol_uninsert = new Symbol("uninsert", nil, nil);
    symbol_reverse = new Symbol("reverse", nil, nil);
    symbol_copy = new Symbol("copy", nil, nil);
    symbol_set_len = new Symbol("set_len", nil, nil);
    symbol_get_name = new Symbol("get_name", nil, nil);
    symbol_get_inst_slot_count = new Symbol("get_inst_slot_count", nil, nil);
    symbol_get_inst_any_slots = new Symbol("get_inst_any_slots", nil, nil);
    symbol_get_member_table = new Symbol("get_member_table", nil, nil);
    symbol_Class = new Symbol("Class", nil, nil);
    symbol_Obj = new Symbol("Obj", nil, nil);

    gc_stack_top = NULL;
}


void runtime_mark_roots()
{
    heap_obj_make_gray(cs_obj_class);
    heap_obj_make_gray(cs_symbol_table);
}
