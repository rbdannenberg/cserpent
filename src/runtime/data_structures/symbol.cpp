//
// Created by anthony on 7/7/24.
//

#include <iostream>
#include <stdexcept>
#include <cassert>
#include <utility>
#include "any.h"
#include "gc.h"
#include "basic_obj.h"
#include "csstring.h"
#include "symbol.h"
#include "array.h"
#include "dict.h"

Dict *cs_symbol_table;

Any symbol_t;

Symbol::Symbol(Any name, Any value, Any func)
{
    // precondition: name is not in symbol table
    set_slot(0, name);
    set_slot(1, value);
    set_slot(2, func);
    Any symbol(this);
    cs_symbol_table->insert(name, symbol);
}


Symbol::Symbol(const char *name, Any value, Any func)
{
    // precondition: name is not in symbol table
    Any any_name = name;
    set_slot(0, any_name);
    set_slot(1, value);
    set_slot(2, func);
    Any symbol(this);
    cs_symbol_table->insert(name, symbol);
}


std::ostream& operator<<(std::ostream& os, Symbol *x) {
    os << get_c_str(x->name());
    return os;
}

