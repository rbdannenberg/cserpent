//
// Created by anthony on 6/4/24.
//
#pragma once
#include <string>
#include <iostream>
#include <utility>

class Cs_class;

//// all user-defined objects inherit from this, which has a class
//// pointer in slots[0]
class Obj {
public:
    // Note: there should be no member variables here.
    // All data should be stored in slots.

    Obj();
    Obj(Cs_class *class_ptr);

    void *operator new(size_t size);

    Header *get_header() { return ((Header *) this) - 1; }

    int64_t get_slot_count() { return get_header()->get_slot_count(); }

    Gc_color get_color() { return get_header()->get_color(); }

    // the use of virtual here ensures that a vtable is created, making
    // the object 8 bytes bigger than it would be otherwise. In most
    // implementations, this also means that the constructor allocating
    // memory at addr will return the object as being at address addr + 8
    // and the vtable pointer will be at a negative offset (obj_address - 8).
    virtual uint64_t get_any_slots();
    
    Any slots[1];  // can actually be any number of slots

    /* There used to be pure virtual functions here call and get, but
     * because of issues with memory management, we will move towards
     * attaching a dictionary to every Cs_class object that maps
     * Symbols to fields and methods.  In essence, this is a custom
     * vtable implementation that gives us more control over memory
     * layout.
     */
    Cs_class *get_class_ptr();
    bool isinstance(Cs_class *cs_class);

    /// Although we don't need to compile to "call" for known Object types, 
    /// this is helpful for encapsulation.
    Any call(Symbol *method);
    void set_class_ptr(Cs_class * c_ptr);

    void set_slot(int i, Any x);
    void set_slot(int i, int64_t x) { slots[i].integer = x; }
    void set_slot(int i, double x) { slots[i].real = x; }
    void set_slot(int i, bool x) { slots[i].integer = x; }
};

Obj *to_Obj(Any x);
Obj *as_Obj(Any x);

extern Array *pparms;  // positional parameters for dynamic function/method calls
// writing (*pparms)[i] for indexing is ugly, so we use PPARMS[i]:
#define PPARMS (*pparms)
extern Dict *kparms;  // keyword parameters for dynamic function/method calls
#define KPARMS (*kparms)

void check_dispatch(const char *method, size_t max_args_len,
                    bool allow_kw = false);
void check_dispatch(Symbol *method, size_t max_args_len,
                    bool allow_kw = false);


// The symbol table should exist globally, because Symbols can be generated
// on the fly. Instead of populating them with the correct mappings each
// time, just treat them as unique strings/keys into the dictionary.
using MemberFn = std::function<Any(Obj*)>;
using MemberTable = std::unordered_map<Symbol *, MemberFn>;

extern MemberTable cs_class_table;
extern MemberTable cs_obj_table;

inline constexpr size_t member_table_slot_count =
        (sizeof(MemberTable) + sizeof(int64_t) - 1) / sizeof(int64_t) - 1;


// This is the class of class descriptors. A Cs_class has these fields:
//   slots[0] - pointer to the superclass Cs_class
//   slots[1] - the class name, a symbol (Symbol pointer)
//   slots[2] - the number of slots in class instances (int)
//   slots[3] - the bit map of slots that are of type Any
//   slots[4] - pointer to the MemberTable

class Cs_class : public Obj {
  public:
    // make sure object gets allocated with enough space for 5 slots:
    int64_t more_slots[4];  // more_slots[0] aliases with slots[1]
    
    Cs_class(Symbol *name, int64_t slot_count, int64_t any_slots,
             MemberTable *table, Cs_class *parent=nullptr);
    [[nodiscard]] Symbol *get_name() { return to_symbol(slots[1]); }
    [[nodiscard]] int64_t get_inst_slot_count() const {
        return slots[2].integer; }
    [[nodiscard]] int64_t get_inst_any_slots() const {
        return slots[3].integer; }
    [[nodiscard]] MemberTable* get_member_table() const {
        // reference so we can refactor later:
        return reinterpret_cast<MemberTable *>(slots[4].integer); }
    [[nodiscard]] Cs_class **get_superclass() {
        return reinterpret_cast<Cs_class **>(&(slots[0].integer)); }
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


extern Cs_class csg_cs_class;

Any cs_class_get_class_name(Obj* self, const Array &args, const Dict &kwargs);
Any cs_class_get_inst_slot_count(Obj* self, const Array &args,
                                 const Dict &kwargs);
Any cs_class_get_inst_any_slots(Obj* self, const Array &args,
                                const Dict &kwargs);
Any cs_class_get_member_table(Obj* self, const Array &args, const Dict &kwargs);

// global symbol table (this should be a dictionary when they are implemented):
//extern Array *cs_symbols;
// look for globals::cs_symbol_table in the globals directory

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
