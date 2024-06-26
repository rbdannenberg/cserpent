//
// Created by anthony on 5/30/24.
//
#include <any.h>
#include <iostream>
#include <fstream>
#include <chrono>

extern void test_matmul(Any n);
extern void nq_solve(Any n);
extern void test_trees(Any n);

typedef void (*BenchmarkFunction)(Any);
static constexpr bool checkOnly = false;
static constexpr bool writeToFile = false;
static const std::string filename = "../benchmarks/optimization.csv";
static const std::string optimization = "Link Time Optimization";

/**
 * Benchmark times for binary trees for n = 10, 15, 17, iterations = 100. Print average and highest time.
 */
template <BenchmarkFunction Func>
double benchmark(Any argument, int iterations=100, int warmup=0) {
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
    std::cout << "Max time for n = " << to_int(argument) << " over " << iterations << " iterations: " << max
              << std::endl;
    return avg;
}


int main() {
    if (checkOnly) {
        test_trees(10);
        test_matmul(10);
        nq_solve(10);
        return 0;
    }
    std::vector<double> times{};
    std::cout << "Benchmarking Matrix Multiplication..." << std::endl;
    times.push_back(benchmark<test_matmul>(10));
    times.push_back(benchmark<test_matmul>(50));
    times.push_back(benchmark<test_matmul>(100));
    std::cout << "Benchmarking Binary Trees..." << std::endl;
    times.push_back(benchmark<test_trees>(10));
    times.push_back(benchmark<test_trees>(15));
    times.push_back(benchmark<test_trees>(17));
    std::cout << "Benchmarking N Queens..." << std::endl;
    times.push_back(benchmark<nq_solve>(10));
    times.push_back(benchmark<nq_solve>(12));
    times.push_back(benchmark<nq_solve>(14));
    benchmark<nq_solve>(15);
    if (writeToFile) {
        std::cout << "Writing to file..." << std::endl;
        std::ofstream file {filename, std::ios::app};
        for (double time : times) {
            file << time << ", ";
        }
        file << optimization << std::endl;
        file.close();
    }
    std::cout << "Done." << std::endl;
    return 0;
}