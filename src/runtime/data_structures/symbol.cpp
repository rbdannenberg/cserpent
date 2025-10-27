//
// Created by anthony on 7/7/24.
//

#include <iostream>
#include <stdexcept>
#include <cassert>
#include <utility>
#include "any.h"
#include "gc.h"
#include "heap_obj.h"
#include "obj.h"
#include "runtime.h"
#include "csstring.h"
#include "symbol.h"
#include "array.h"
#include "dict.h"
#include <cstring>

Dict *cs_symbol_table;

Any symbol_t;

Symbol::Symbol(Any name, Any value, Any func,
               Any_type stype, Cs_class *cs_class)
{
    // precondition: name is not in symbol table
    set_tag(tag_symbol);
    set_slot(0, name);
    set_slot(1, value);
    set_slot(2, func);
    *symbol_type() = stype;  // not a heap pointer
    set_slot(4, cs_class);
    Any symbol(this);  // nan-box this into an Any to place in cs_symbol_table
    cs_symbol_table->insert(name, symbol);
}


Symbol::Symbol(const char *name_string, Any value, Any func)
{
    // precondition: name is not in symbol table
    // This is tricky: since this constructor puts a Heap_obj on the heap,
    // but there is no reference to it yet, it could get GC'd when we convert
    // name to an Any or do a symbol table insert, so we have to store this
    // as a local variable:
    struct Frame : public Cs_frame {
        Any result;  // this
    } L;
    constexpr int sl_result = 0;
    memset(&L, 0, sizeof(L));
    STD_FUNCTION_ENTRY(L, 1);
    L.set(sl_result, this);
    set_slot(1, value);  // store Any parameters first to protect them from GC
    set_slot(2, func);
    // now we can safely evaluate expressions that might allocate memory and
    // invoke GC, including conversion of string to Any:
    set_slot(0, Any(name_string));
    cs_symbol_table->insert(name(), L.result);
}


std::ostream& operator<<(std::ostream& os, Symbol *x) {
    os << get_c_str(x->name());
    return os;
}

