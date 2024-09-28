//
// Created by anthony on 7/9/24.
//
#pragma once
#include <map>

namespace globals {
    // SymbolTable maps from Symbol to a value/function pair.
    class SymbolTable {
        using GlobalFn = std::function<Any(const Array &, const Dictionary &)>;
        std::unordered_map<Symbol, std::pair<Any, GlobalFn>> table;
    public:

        void set_value(const Symbol &variable_name, Any value);

        Any get_value(const Symbol &variable_name);

        void set_function(const Symbol &function_name, GlobalFn fn);

        GlobalFn get_function(const Symbol &function_name);
        
        void add_symbol(const Symbol &symbol);
    };

    extern SymbolTable cs_symbol_table;
}


