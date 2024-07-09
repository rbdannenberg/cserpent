//
// Created by anthony on 5/30/24.
//
#include <any.h>
#include "obj.h"
#include <iostream>
#include <fstream>
#include <chrono>

// These return strings to be printed to check for correctness
extern std::string test_matmul(Any n);
extern std::string nq_solve(Any n);
extern std::string test_trees(Any n);
//std::string test_trees(Any n) {
//    return "Not implemented.";
//}
extern std::string test_matmul_c(int n);
extern std::string nq_solve_c(int n);
extern std::string test_trees_c(int n);
static std::string test_trees_c_any(Any n) {
    return test_trees_c(to_int(n));
}
static std::string test_matmul_c_any(Any n) {
    return test_matmul_c(to_int(n));
}
static std::string nq_solve_c_any(Any n) {
    return nq_solve_c(to_int(n));
}

typedef std::string (*BenchmarkFunction)(Any);
static constexpr bool checkCorrectness = true;
static constexpr bool writeToFile = false;
static const std::string filename = "../benchmarks/benchmarks.csv";
static const std::string optimization = "Initial";

/**
 * Benchmark times for binary trees for n = 10, 15, 17, iterations = 100. Print average and highest time.
 */
template <BenchmarkFunction Func>
double benchmark(Any argument, int iterations=100, int warmup=3) {
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
    double avg = sum / iterations;
    std::cout << "Average time for n = " << to_int(argument) << " over " << iterations << " iterations: "
              << avg << std::endl;
//    std::cout << "Max time for n = " << to_int(argument) << " over " << iterations << " iterations: " << max
//              << std::endl;
    return avg;
}

void benchmarkCSerpent() {
    if (checkCorrectness) {
        std::cout << test_trees(10) << std::endl;
        std::cout << test_trees_c(10) << std::endl;
        std::cout << test_matmul(10) << std::endl;
        std::cout << test_matmul_c(10) << std::endl;
        std::cout << nq_solve(10) << std::endl;
        std::cout << nq_solve_c(10) << std::endl;
    }
    std::vector<double> times{};
    std::cout << "CSerpent++: " << std::endl;
    std::cout << "Benchmarking Matrix Multiplication..." << std::endl;
    times.push_back(benchmark<test_matmul>(20));
    times.push_back(benchmark<test_matmul>(50));
    times.push_back(benchmark<test_matmul>(100));
    std::cout << "Benchmarking Binary Trees..." << std::endl;
    times.push_back(benchmark<test_trees>(10));
    times.push_back(benchmark<test_trees>(15));
    times.push_back(benchmark<test_trees>(17));
    std::cout << "Benchmarking N Queens..." << std::endl;
    times.push_back(benchmark<nq_solve>(10, 10));
    times.push_back(benchmark<nq_solve>(12, 10));
    times.push_back(benchmark<nq_solve>(14, 10));
    if (writeToFile) {
        std::cout << "Writing to file..." << std::endl;
        std::ofstream file {filename, std::ios::app};
        file << "CSerpent++, ";
        for (double time : times) {
            file << time << ", ";
        }
        file << optimization << std::endl;
        file.close();
    }
}

void benchmarkC() {
    std::vector<double> times_c{};
    std::cout << "Baseline C: " << std::endl;
    std::cout << "Benchmarking Matrix Multiplication..." << std::endl;
    times_c.push_back(benchmark<test_matmul_c_any>(20));
    times_c.push_back(benchmark<test_matmul_c_any>(50));
    times_c.push_back(benchmark<test_matmul_c_any>(100));
    std::cout << "Benchmarking Binary Trees..." << std::endl;
    times_c.push_back(benchmark<test_trees_c_any>(10));
    times_c.push_back(benchmark<test_trees_c_any>(15));
    times_c.push_back(benchmark<test_trees_c_any>(17));
    std::cout << "Benchmarking N Queens..." << std::endl;
    times_c.push_back(benchmark<nq_solve_c_any>(10, 10));
    times_c.push_back(benchmark<nq_solve_c_any>(12, 10));
    times_c.push_back(benchmark<nq_solve_c_any>(14, 10));
    if (writeToFile) {
        std::cout << "Writing to file..." << std::endl;
        std::ofstream file {filename, std::ios::app};
        file << "C, ";
        for (double time : times_c) {
            file << time << ", ";
        }
        file << optimization << std::endl;
        file.close();
    }
}

int main() {
    main_init();
//    benchmarkC();
    benchmarkCSerpent();
    std::cout << "Done." << std::endl;
    return 0;
}