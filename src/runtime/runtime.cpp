// runtime.cpp - runtime_init() implementation
//
// Roger B. Dannenberg
// Sep 2024

#include <assert.h>
#include "csmem.h"
#include "obj.h"
#include "runtime.h"

Cs_class *cs_obj_class = nullptr;

void runtime_init()
{
    csmem_init();  // memory heap initialization
    main_init();   // creates cs_class_class, the superclass of classes
    gc_stack_top = NULL;
}


void runtime_mark_roots()
{
    basic_obj_make_gray(cs_obj_class);
}
