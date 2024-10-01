//
// Created by anthony on 6/4/24.
//
#pragma once
#include <string>
#include <iostream>
#include <utility>

class Cs_class;
extern Cs_class *cs_class_class;  // Cs_class inherits from Obj, so instqnces of
// Cs_class must have a class pointer. The pointer is cs_class_class.

//// all user-defined objects inherit from this, which has a class
//// pointer in slots[0]
class Obj : public Basic_obj {
public:
    // Note: there should be no member variables here.
    // All data should be stored in slots.

    Obj();
    Obj(Cs_class *class_ptr);

    uint64_t get_any_slots();
    
    /* There used to be pure virtual functions here call and get, but
     * because of issues with memory management, we will move towards
     * attaching a dictionary to every Cs_class object that maps
     * Symbols to fields and methods.  In essence, this is a custom
     * vtable implementation that gives us more control over memory
     * layout.
     */
    Cs_class * get_class_ptr();

    /// Although we don't need to compile to "call" for known Object types, this is
    /// helpful for encapsulation.
    Any call(Symbol *method, Array *args, Dict *kwargs);
    void set_class_ptr(Cs_class * c_ptr);
};

void check_dispatch(Symbol *method, Array *args,
                    Dict *kwargs, size_t args_len,
                    size_t kwargs_len);


// The symbol table should exist globally, because Symbols can be generated
// on the fly. Instead of populating them with the correct mappings each
// time, just treat them as unique strings/keys into the dictionary.
using MemberFn = std::function<Any(Obj*, Array*, Dict*)>;
using MemberTable = std::unordered_map<Symbol *, MemberFn>;

extern MemberTable cs_class_table;

inline constexpr size_t member_table_slot_count =
        (sizeof(MemberTable) + sizeof(int64_t) - 1) / sizeof(int64_t) - 1;


// This is the class of class descriptors. A Cs_class has these fields:
//   slots[0] - the "class class", the class of descriptors
//   slots[1] - the class name, a symbol (Symbol pointer)
//   slots[2] - the number of slots in class instances (int)
//   slots[3] - the bit map of slots that are of type Any
class Cs_class : public Obj {
  public:
    // make sure object gets allocated with enough space for 5 slots:
    int64_t more_slots[5];  // more_slots[0] aliases with slots[1]
    
    // Notice Obj {Cs_class_class}. The class of all Cs_class objects is
    // Cs_class_class!
    Cs_class(Symbol *name, int64_t slot_count, int64_t any_slots,
             MemberTable *table, Cs_class *parent=nullptr) :
            Obj {cs_class_class} {
        set_slot(1, name);
        slots[2].integer = slot_count;
        slots[3].integer = any_slots;
        if (parent != nullptr) {
            MemberTable parent_table_copy = *(parent->get_member_table());
            // since merge alters the argument, we use a temporary copy
            table->merge(std::move(parent_table_copy));
        }
        slots[4].integer = reinterpret_cast<int64_t>(table);
        slots[5].integer = reinterpret_cast<int64_t>(parent);
        // A: we could potentially copy the table wholesale, but that mucks
        // around with memory a bit too much for my liking. Get it right
        // first then attempt to refactor.
    }
    [[nodiscard]] Symbol *get_name() { return to_symbol(slots[1]); }
    [[nodiscard]] int64_t get_inst_slot_count() const {
        return slots[2].integer; }
    [[nodiscard]] int64_t get_inst_any_slots() const {
        return slots[3].integer; }
    [[nodiscard]] MemberTable* get_member_table() const {
        // reference so we can refactor later:
        return reinterpret_cast<MemberTable *>(slots[4].integer); }
    [[nodiscard]] Cs_class* get_parent() const {
        return reinterpret_cast<Cs_class *>(slots[5].integer); }
    [[nodiscard]] MemberFn find_function(Symbol *function_name) {
        MemberTable *table = get_member_table();
        auto it = table->find(function_name);
        if (it == table->end()) {
            throw std::runtime_error(
                    "Function not found in class or parent class.");
        }
        return it->second;
    }
};

// global symbol table (this should be a dictionary when they are implemented):
//extern Array *cs_symbols;
// look for globals::cs_symbol_table in the globals directory

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
inline Any Adder_call_add(Obj* self, Array *args, const Dict& kwargs) {
    return static_cast<Adder*>(self)->add(to_int(args[0]));
}

// compiler checks that there aren't members with the same name.
MemberTable Adder_table {};
Adder_table.emplace(Symbol{'x'}, std::make_pair {Adder_get_x, nil});
Adder_table.emplace(Symbol{'y'}, std::make_pair {Adder_get_y, nil});
Adder_table.emplace(Symbol{'add'}, std::make_pair {nil, Adder_call_add});
Cs_class(Symbol {'Adder'}, 2, 0, &Adder_table);
*/
