//
// Created by anthony on 6/4/24.
//
#pragma once
#include <string>
#include <iostream>
#include "basic_obj.h"
#include "symbol.h"

class Cs_class;
extern Cs_class *Cs_class_class;

//// all user-defined objects inherit from this, which has a class
//// pointer in slots[0]
class Obj : public Basic_obj {
public:
    // Note: there should be no member variables here. All data should be stored in slots.

    Obj(Cs_class *class_ptr);

    uint64_t get_any_slots();
    
    /* There used to be pure virtual functions here call and get, but because of issues with memory management,
     * we will move towards attaching a dictionary to every Cs_class object that maps Symbols to fields and methods.
     * In essence, this is a custom vtable implementation that gives us more control over memory layout.
     */
    Cs_class * get_class_ptr();

    /// Although we don't need to compile to "call" for known Object types, this is
    /// helpful for encapsulation.
    Any call(const Symbol& method, const Array& args, const Dictionary& kwargs);
    void set_class_ptr(Cs_class * c_ptr);
};

void check_dispatch(const Symbol& method, const Array& args, const Dictionary& kwargs, size_t args_len, size_t kwargs_len);


// The symbol table should exist globally, because Symbols can be generated on the fly. Instead of populating them with
// the correct mappings each time, just treat them as unique strings/keys into the dictionary.
using MemberFn = std::function<Any(Obj*, const Array&, const Dictionary&)>;
using MemberTable = std::unordered_map<Symbol, MemberFn>;

inline constexpr size_t member_table_slot_count = (sizeof(MemberTable) + sizeof(int64_t) - 1) / sizeof(int64_t) - 1;


// This is the class of class descriptors. A Cs_class has these fields:
//   slots[0] - the "class class", the class of descriptors
//   slots[1] - the class name, a symbol (Symbol pointer)
//   slots[2] - the number of slots in class instances (int)
//   slots[3] - the bit map of slots that are of type Any
class Cs_class : public Obj {
  public:
    // make sure object gets allocated with enough space for 5 slots:
    int64_t more_slots[4];
    // Notice Obj {Cs_class_class}. The class of all Cs_class objects is Cs_class_class!
    Cs_class(Symbol name, int64_t slot_count, int64_t any_slots, MemberTable *table) : Obj {Cs_class_class}{
        slots[1] = name;
        slots[2].integer = slot_count;
        slots[3].integer = any_slots;
        slots[4].integer = reinterpret_cast<int64_t>(table);
        // A: we could potentially copy the table wholesale, but that mucks around with memory a bit
        // too much for my liking. Get it working first then attempt to refactor.
    };
    [[nodiscard]] Symbol *get_name() const { return reinterpret_cast<Symbol *>(slots[1].integer); }
    [[nodiscard]] int64_t get_inst_slot_count() const { return slots[2].integer; }
    [[nodiscard]] int64_t get_inst_any_slots() const { return slots[3].integer; }
    [[nodiscard]] MemberTable* get_member_table() const { return reinterpret_cast<MemberTable *>(slots[4].integer); } // reference so we can refactor later
};

// global symbol table (this should be a dictionary when they are implemented):

extern Array *cs_symbols;

// initialize this module
void main_init();


/**
/// <adder.h>
class Adder : public Obj {
public:
    int x;
    int y;
    int add(int z) {
        return x + y + z;
    }
};
// <class_globals.cpp>
inline Any Adder_get_x(Obj* self) {
    return static_cast<Adder*>(self)->x;
}
inline Any Adder_get_y(Obj* self) {
    return static_cast<Adder*>(self)->y;
}
inline Any Adder_call_add(Obj* self, const Array& args, const Dictionary& kwargs) {
    return static_cast<Adder*>(self)->add(to_int(args[0]));
}

// compiler checks that there aren't members with the same name.
MemberTable Adder_table {};
Adder_table.emplace(Symbol{'x'}, std::make_pair {Adder_get_x, nil});
Adder_table.emplace(Symbol{'y'}, std::make_pair {Adder_get_y, nil});
Adder_table.emplace(Symbol{'add'}, std::make_pair {nil, Adder_call_add});
Cs_class(Symbol {'Adder'}, 2, 0, &Adder_table);
*/
