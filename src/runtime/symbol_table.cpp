//
// Created by anthony on 7/9/24.
//
#include "any.h"
#include "gc.h"
#include "header.h"
#include "heap_obj.h"
#include "array.h"
#include "dict.h"
#include "obj.h"
#include "symbol.h"
#include "symbol_table.h"
#include "op_overload.h"
#include "csstring.h"
extern String string_11;

/* this is wrong because new Symbol() does an insert 
Symbol *intern(const char *name)
{
    int64_t index = cs_symbol_table->find(Any{name}, true);
    Any s = (*cs_symbol_table)[index + 1];
    if (!is_symbol(s)) {
        Symbol *sym = new Symbol(name);
        (*cs_symbol_table).set(index + 1, Any{sym});
        return sym;
    } else {
        return to_symbol(s);
    }
}
*/


Symbol *intern(char const *name_string)
{
    // std::cout << "in intern(" << name_string << ") string_11 is " << string_11 << std::endl;
    Any name{name_string};
    // std::cout << "in intern(" << name_string << ") Any of string is " << name << std::endl;
    // std::cout << "in intern(" << name_string << ") string_11 is now " << string_11 << std::endl;
    int64_t index = cs_symbol_table->find(name, false);
    Symbol *s;
    if (index >= 0) {
        s = to_symbol((*cs_symbol_table)[index + 1]);
    } else {
        s = new Symbol(name_string);  // create a new symbol if not found, which will insert it into the symbol table
    }
    return s;
}

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
