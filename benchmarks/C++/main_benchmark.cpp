//
// Created by anthony on 5/30/24.
//
#include <any.h>
#include <iostream>
#include <chrono>

extern void test_matmul(Any n);
extern void nq_solve(Any n);


int main() {
    auto start = std::chrono::high_resolution_clock::now();

    std::cout << "Testing matrix multiplication..." << std::endl;
    test_matmul(10);
    test_matmul(50);
    test_matmul(100);
    std::cout << "Testing n queens..." << std::endl;
    nq_solve(5);
    nq_solve(8);
    nq_solve(15);
    std::cout << "All tests passed!" << std::endl;

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << "s\n";
}
