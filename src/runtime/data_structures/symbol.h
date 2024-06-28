//
// Created by anthony on 6/4/24.
//
#pragma once
#include <utility>

typedef std::function<Any(Obj*, const std::string&, const Array&, const Dictionary&)> CallFn;
typedef std::function<Any(Obj*, const std::string&)> GetFn;

// Symbol is a built-in class with the following:
//     slot[0] - the string name of the symbol, a (CSerpent) String
//     slot[1] - global value (type Any)
//     slot[2] - global function (not type Any)
class Symbol : public Basic_obj {
public:
    // make sure object gets allocated with enough space for 3 slots:
    int64_t more_slots[2];
    
    // void *fn_ptr;
    // add type information here?
    // CallFn call_ptr;
//    GetFn get_ptr;

    Symbol(String *name, CallFn call_ptr /*, GetFn get_ptr */) {
        slots[0] = name;
        slots[1] = nullptr;  // initial global value (should be special UNDEFINED value)
        slots[2].integer = 0;
        // TODO: need to assign representation of function to slots[32]
        // reinterpret_cast<unint64_t>(call_ptr); ?
    }
    //Symbol(std::string name, Any value, void *ptr) :
    //  name {std::move(name)}, value {value}, fn_ptr {ptr} {}
};


// global symbol table (this should be a dictionary when they are implemented):
extern Array *cs_symbols;

// initialize this module
void symbol_init();

//template<class T>
//Any call_object_method(Any(T::*func), T& obj
//                       , const std::string& method_name, Array& args, Dictionary& kwargs) {
//    return std::invoke(func, obj, method_name, args, kwargs);
//}

