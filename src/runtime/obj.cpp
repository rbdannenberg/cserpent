//
// Created by anthony on 6/5/24.
//
#include "obj.h"
#include <data_structures/array.h>

#include <utility>

void check_dispatch(const std::string& method, const Array& args, const Dictionary& kwargs, size_t args_len, size_t kwargs_len) {
    if (len(args) != args_len || kwargs_len != 0) { /// change when kwargs are implemented
        std::cout << "Dispatch error by method: " << method << std::endl;
    }
}

//Obj::Obj(std::string name) : class_name {std::move(name)} {}