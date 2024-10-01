//
// Created by anthony on 7/7/24.
//


#pragma once
#include <string>
#include <cstdint>
#include <limits>

class Symbol : public Basic_obj {
public:
    // we need symbol name, symbol value and function value:
    int64_t more_slots[2];

    Symbol(Any name, Any value = nil, Any func = nil);
    Symbol(const char *name, Any value = nil, Any func = nil);

    Any name() { return slots[0]; }
    Any value() { return slots[1]; }
    Any func() { return slots[2]; }
};

extern Dict *cs_symbol_table;
extern Any symbol_t;

std::ostream& operator<<(std::ostream& os, const Symbol *x);

