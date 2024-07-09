//
// Created by anthony on 7/9/24.
//
#pragma once
#include "any.h"
#include "symbol.h"
#include "array.h"
#include "dictionary.h"

#include <map>
namespace globals {
    class SymbolTable {
        using GlobalFn = std::function<Any(const Array&, const Dictionary&)>;
        std::unordered_map<Symbol, std::pair<Any, GlobalFn>> table;
        void set_value(const Symbol& variable_name, Any value);
        Any get_value(const Symbol& variable_name);
        void set_function(const Symbol& function_name, GlobalFn fn);
        GlobalFn get_function(const Symbol& function_name);
    };

    extern SymbolTable cs_symbol_table;
}


