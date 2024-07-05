//
// Created by anthony on 6/4/24.
//
#pragma once
#include <string>
#include <iostream>
#include "basic_obj.h"


//// all user-defined objects inherit from this, which has a class
//// pointer in slots[0]
class Obj : public Basic_obj {
public:
//    std::string class_name; //TODO: reference?
//    // note: I removed the pointer here
//    Obj(std::string name);

    uint64_t get_any_slots();
    
    //Symbol& name;  // symbol denoting class name
    /* There used to be pure virtual functions here call and get, but because of issues with memory management,
     * we will move towards attaching a dictionary to every Cs_class object that maps Symbols to fields and methods.
     * In essence, this is a custom vtable implementation that gives us more control over memory layout.
     */

};

void check_dispatch(const std::string& method, const Array& args, const Dictionary& kwargs, size_t args_len, size_t kwargs_len);


class Cs_class;
extern Cs_class *Cs_class_class;

// This is the class of class descriptors. A Cs_class has these fields:
//   slots[0] - the "class class", the class of descriptors
//   slots[1] - the class name, a symbol (Symbol pointer)
//   slots[2] - the number of slots in class instances (int)
//   slots[3] - the bit map of slots that are of type Any
class Cs_class : public Obj {
  public:
    // make sure object gets allocated with enough space for 4 slots:
    int64_t more_slots[3];
    Cs_class() { };
    Cs_class(Symbol *name, int64_t slot_count, int64_t any_slots) {
        slots[0] = Cs_class_class;
        slots[1] = name;
        slots[2].integer = slot_count;
        slots[3].integer = any_slots;
    };
    Symbol *get_name() { return reinterpret_cast<Symbol *>(slots[1].integer); }
    int64_t get_inst_slot_count() { return slots[2].integer; }
    int64_t get_inst_any_slots() { return slots[3].integer; }
};


