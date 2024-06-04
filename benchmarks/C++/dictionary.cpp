//
// Created by anthony on 6/4/24.
//
#include <any.h>
#include <data_structures/dictionary.h>
#include <iostream>

int main() {
    Dictionary& D_native = *(new Dictionary {});
    Any D = *(new Dictionary {});
    D['a'] = 5;
    D['b'] = 8;
    std::cout << D['a']  << std::endl;
    D['a'] = 3.5;
    std::cout << to_real(D['a']) << std::endl;

    //D[6] = "hello";

}
