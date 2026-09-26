// runtime.cpp - runtime_init() implementation
//
// Roger B. Dannenberg
// Sep 2024

#include <assert.h>
#include "any.h"
#include "op_overload.h"
#include "gc.h"
#include "header.h"
#include "heap_obj.h"
#include "csmem.h"
#include "obj.h"
#include "array.h"
#include "dict.h"
#include "symbol.h"
#include "runtime.h"
#include "obj.h"

Symbol *css_append;
Symbol *css_last;
Symbol *css_insert;
Symbol *css_unappend;
Symbol *css_uninsert;
Symbol *css_reverse;
Symbol *css_copy;
Symbol *css_set_len;
Symbol *css_get_name;
Symbol *css_get_inst_slot_count;
Symbol *css_get_inst_any_slots;
Symbol *css_get_member_table;
Symbol *css_Class;
Symbol *css_Obj;

bool gc_enabled = false;


void runtime_init()
{
    csmem_init();  // memory heap initialization
    // create some special signposts for Dict entries that will be ignored
    // by GC and and cannot match "real" strings because of missing EOS
    DICT_EMPTY.integer = SHORT_TAG + 0x010101010101;
    DICT_DELETED.integer = SHORT_TAG + 0x020202020202;

    pparms = new Array();
    kparms = new Dict();
    cs_symbol_table = new Dict();
    css_t = new Symbol("t", (uint64_t) &css_t, nil, Any_type::SYMBOL);
    // 't' evaluates to itself
    css_append = new Symbol("append");
    css_last = new Symbol("last");
    css_insert = new Symbol("insert");
    css_unappend = new Symbol("unappend");
    css_uninsert = new Symbol("uninsert");
    css_reverse = new Symbol("reverse");
    css_copy = new Symbol("copy");
    css_set_len = new Symbol("set_len");
    css_get_name = new Symbol("get_name");
    css_get_inst_slot_count = new Symbol("get_inst_slot_count");
    css_get_inst_any_slots = new Symbol("get_inst_any_slots");
    css_get_member_table = new Symbol("get_member_table");
    gc_stack_top = NULL;
    gc_enabled = true;
}


void runtime_mark_roots()
{
    make_heap_obj_gray(cs_symbol_table);
    make_heap_obj_gray(kparms);
    make_heap_obj_gray(pparms);
    cs_global_mark();
}
