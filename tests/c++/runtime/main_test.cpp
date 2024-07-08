//
// Created by anthony on 6/6/24.
//
#include <iostream>
extern void test_dictionary();
extern int test_string();
extern int test_string_any();
extern void test_array();

int main() {

//    test_dictionary();
    test_string();
    test_array();
    test_string_any();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
