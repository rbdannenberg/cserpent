//
// Created by anthony on 6/3/24.
//
#include <iostream>
#include "any.h"
#include "builtin_functions/builtin_functions.h"
#include "any_utils.h"
#include "data_structures/array.h"

int adder(int x, int y) {
    return x + y;
}

Symbol adder_symbol {"adder", nullptr, &adder};

void print(Any x) {
    if (is_int(x)) {
        std::cout << to_int(x) << std::endl;
    }
    else if (is_real(x)) {
        std::cout << to_real(x) << std::endl;
    }
    else if (is_ptr(x)) {
        std::cout << to_ptr(x) << std::endl;
    }
    else type_error(x);
}


//Any apply(const Symbol& function, Array argarray) {
//    if (function.fn_ptr != nullptr) {
//        (*function.fn_ptr)
//    }
//    else {
//        std::cout << "No associated function" << std::endl;
//    }
//}

Any apply(std::string function_name, Array argarray) {
    return (symbol_table[function_name]->fn_ptr)(unpack(argarray));
}

print(apply('adder', [5, 7]));
// How to go from the string "adder" to the reference to the Symbol on the heap?

Dictionary symbol_table;
symbol_table["adder"] = new Symbol("adder", nullptr, nullptr);



// 1) Keep a lookup table of strings to symbols
    // But then why not just use that lookup table as the symbol table itself?
// 2) Convert "adder" to adder_symbol during runtime (i.e. use C++'s collection of global variables as the "lookup table"
// 3) Replace Symbols entirely and only global variables and their equivalents (e.g. invoke for apply)


print(funcall('adder', 5, 9));
fn = 'adder'
args = [7,9]
print(apply(fn, args))
