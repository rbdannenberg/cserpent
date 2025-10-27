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

    Symbol(Any name, Any value = NULL, Any func = NULL,
           Any_type stype = ANY, Cs_class *cs_class = NULL);
    Symbol(const char *name, Any value = NULL, Any func = NULL,
           Any_type stype = ANY, Cs_class *cs_class = NULL);

    Any *name() { return slots; }
    Any *value() { return slots + 1; }
    Any *func() { return slots + 2; }
    Any_type *symbol_type() { return (Any_type *) (slots + 3); }
    Cs_class **symbol_class() { return (Cs_class *) (slots + 4); }
};

extern Dict *cs_symbol_table;
extern Any symbol_t;

std::ostream& operator<<(std::ostream& os, const Symbol *x);

