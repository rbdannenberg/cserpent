//
// Created by anthony on 6/4/24.
//
#pragma once
#include <utility>

#include "obj.h"
#include "any.h"
typedef std::function<Any(Obj*, const std::string&, const Array&, const Dictionary&)> CallFn;
typedef std::function<Any(Obj*, const std::string&)> GetFn;

/// Basic_obj used for tagging, not for garbage collection (static storage duration)
class Symbol : Basic_obj {
public:
    // note: I removed the pointer here
    std::string name;
    Any value;
    void *fn_ptr;
    // add type information here?
    CallFn call_ptr;
    GetFn get_ptr;

    Symbol(std::string name, CallFn call_ptr, GetFn get_ptr) :
        name {std::move(name)}, value {}, fn_ptr {}, call_ptr {std::move(call_ptr)}, get_ptr {std::move(get_ptr)} {}
    //Symbol(std::string name, Any value, void *ptr) :
    //  name {std::move(name)}, value {value}, fn_ptr {ptr} {}
};


//template<class T>
//Any call_object_method(Any(T::*func), T& obj
//                       , const std::string& method_name, Array& args, Dictionary& kwargs) {
//    return std::invoke(func, obj, method_name, args, kwargs);
//}

