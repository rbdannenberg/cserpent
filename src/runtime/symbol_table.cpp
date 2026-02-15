//
// Created by anthony on 7/9/24.
//

/*

#include <iostream>
#include "any.h"
#include "symbol.h"
#include "symbol_table.h"

    void SymbolTable::set_function(const Symbol &function_name, GlobalFn fn) {
        auto it = table.find(function_name);
        if (it != table.end()) {
            if (it->second.second != nullptr) {
                std::cerr << "Function already exists: " << function_name << std::endl;
                throw std::runtime_error("");
            }
            it->second.second = fn;
        } else {
            table[function_name].second = fn;
        }
    }

    SymbolTable::GlobalFn SymbolTable::get_function(
            const Symbol &function_name) {
        auto it = table.find(function_name);
        if (it != table.end()) {
            return it->second.second;
        } else {
            std::cerr << "Function not found: " << function_name << std::endl;
            throw std::runtime_error("");
            return {};
        }
    }
*/
