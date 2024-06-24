//
// Created by anthony on 5/30/24.
//
#include <any.h>
#include <iostream>
#include <chrono>

extern void test_matmul(Any n);
extern void nq_solve(Any n);
extern void test_trees(Any n);

typedef void (*BenchmarkFunction)(Any);
bool checkOnly = false;

/**
 * Benchmark times for binary trees for n = 10, 15, 17, iterations = 100. Print average and highest time.
 */
template <BenchmarkFunction Func>
void benchmark(Any argument, int iterations=100, int warmup=10) {
    std::vector<double> times{};
    // warmup
    for (size_t i = 0; i < warmup; i++) {
        std::invoke(Func, argument);
    }
    for (size_t i = 0; i < iterations; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        std::invoke(Func, argument);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        times.push_back(elapsed.count());
    }
    double sum = 0;
    double max = 0;
    for (auto t: times) {
        sum += t;
        if (t > max) {
            max = t;
        }
    }
    std::cout << "Average time for n = " << to_int(argument) << " over " << iterations << " iterations: "
              << sum / iterations << std::endl;
    std::cout << "Max time for n = " << to_int(argument) << " over " << iterations << " iterations: " << max
              << std::endl;
}

int main() {
    if (checkOnly) {
        test_trees(10);
        test_matmul(10);
        nq_solve(10);
        return 0;
    }
    std::cout << "Benchmarking Binary Trees..." << std::endl;
    benchmark<test_trees>(10, 1, 1);
    benchmark<test_trees>(15, 1, 1);
    benchmark<test_trees>(17, 1, 1);
    std::cout << "Benchmarking Matrix Multiplication..." << std::endl;
    benchmark<test_matmul>(10, 1, 1);
    benchmark<test_matmul>(50, 1, 1);
    benchmark<test_matmul>(100, 1, 1);
    std::cout << "Benchmarking N Queens..." << std::endl;
    benchmark<nq_solve>(10, 1, 1);
    benchmark<nq_solve>(12, 1, 1);
    benchmark<nq_solve>(15, 1, 1);
    std::cout << "Done." << std::endl;
    return 0;
}