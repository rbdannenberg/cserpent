//
// Created by anthony on 7/7/24.
//


#pragma once
#include <string>
#include <cstdint>
#include <limits>

class Symbol : public Heap_obj {
public:
    // we need symbol name, symbol value, function value, stype, cs_class
    // (if stype == OBJ, then cs_class points to the class descriptor object
    int64_t more_slots[4];

    Symbol(Any name, Any value = (void *) nullptr, Any func = (void *) nullptr,
           Any_type stype = Any_type::NIL, Cs_class *cs_class = nullptr);
    Symbol(const char *name, Any value = (void *) nullptr,
           Any func = (void *) nullptr, Any_type stype = Any_type::NIL,
           Cs_class *cs_class = nullptr);

    Any *name() { return slots; }
    Any *value() { return slots + 1; }
    Any *func() { return slots + 2; }
    Any_type *symbol_type() { return (Any_type *) (slots + 3); }
    Cs_class **symbol_class() { return (Cs_class **) (slots + 4); }
};

Any *set_any_global(Any *global_addr, Any value);

extern Dict *cs_symbol_table;
extern Symbol *css_t;

std::ostream& operator<<(std::ostream& os, const Symbol *x);

