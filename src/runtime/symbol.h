//
// Created by anthony on 7/7/24.
//
// Symbol slots:
//   slots[0] = name (string)
//   slots[1] = value or pointer to global
//   slots[2] = function pointer
//   slots[3] = type (integer)
//   slots[4] = class if type is class

#pragma once
#include <string>
#include <cstdint>
#include <limits>

class Symbol : public Heap_obj {
public:
    // we need symbol name, symbol value, function value, stype, cs_class
    // (if stype == OBJ, then cs_class points to the class descriptor object
    int64_t more_slots[4];

//    Symbol(Any name, Any *value = nullptr,
//           Any func = Any((Heap_obj *) nullptr),
//           Any_type stype = Any_type::ANY, Cs_class *cs_class = nullptr);
    Symbol(const char *name, uint64_t value = 0,
           Any func = Any((Heap_obj *) nullptr),
           Any_type stype = Any_type::DIRECT,  // change this with set_value
           Cs_class *cs_class = nullptr);

    Any name() const { return slots[0]; }
    Any *value() { return (Any *) (slots[1].integer); }
    Any func() { return slots[2]; }
    Any_type symbol_type() { return (Any_type) (slots[3].integer); }
    Cs_class *symbol_class() { return (Cs_class *) (slots[4].integer); }

    void set_value(Any *x, Any_type xtype) {
        slots[1].integer = (uint64_t) x; slots[3].integer = (uint64_t) xtype; }

    void set_symbol_value(Any anyval);
    void set_symbol_value(int64_t intval);
    void set_symbol_value(double realval);
    void set_symbol_value(bool boolval);
    void set_symbol_value(Symbol *symval);
    void set_symbol_value(Array *arrayval);
    void set_symbol_value(Dict *dictval);
};

Symbol *intern(const char *name);

Any *set_any_global(Any *global_addr, Any value);
Any *set_any_global(Any *global_addr, Heap_obj *value);
Any *set_any_global(Any *global_addr, const char *value);
Any *set_any_global(Any *global_addr, double value);
Any *set_any_global(Any *global_addr, int64_t value);
Any *set_any_global(Any *global_addr, int value);

extern Dict *cs_symbol_table;
extern Symbol *css_t;

std::ostream& operator<<(std::ostream& os, const Symbol *x);

