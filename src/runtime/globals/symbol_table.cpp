//
// Created by anthony on 7/9/24.
//

#include "symbol_table.h"

namespace globals {
    void SymbolTable::set_value(const Symbol &variable_name, Any value) {
        auto it = table.find(variable_name);
        if (it != table.end()) {
            it->second.first = value;
        } else {
            table[variable_name] = {value, nullptr};
        }
    }

    Any SymbolTable::get_value(const Symbol &variable_name) {
        auto it = table.find(variable_name);
        if (it != table.end()) {
            return it->second.first;
        } else {
            std::cerr << "Variable not found: " << variable_name << std::endl;
            throw std::runtime_error("");
            return {};
        }
    }

    void SymbolTable::set_function(const Symbol &function_name, GlobalFn fn) {
        auto it = table.find(function_name);
        if (it != table.end()) {
            if (it->second.second != nullptr) {
                std::cerr << "Function already exists: " << function_name << std::endl;
                throw std::runtime_error("");
            }
            it->second.second = fn;
        } else {
            table[function_name] = {nil, fn};
        }
    }

    SymbolTable::GlobalFn SymbolTable::get_function(const Symbol &function_name) {
        auto it = table.find(function_name);
        if (it != table.end()) {
            return it->second.second;
        } else {
            std::cerr << "Function not found: " << function_name << std::endl;
            throw std::runtime_error("");
            return {};
        }
    }

    SymbolTable cs_symbol_table{};
}