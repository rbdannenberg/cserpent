// runner.cpp - shared code to run C++ benchmarks
//
// This program is a main program and is designed to be linked to
// various different benchmark implementations, each of which provides
// the same run_benchmark(n) and benchmark_info() functions.
//     run_benchmark(int n)
// is called by main() to run the benchmark with parameter n
// (which the benchmark may choose to ignore).
// 
// This is a main program. The command line is:
//     prgname n [-i iters] [-w warmup] [-c comment]
// where n is passed to run_benchmark(), iters is the number of iterations
// and warmup is the number of times run_benchmark is called before any
// measurements are made. Default iters = 100, default warmup = 3.
// comment is a string that is copied to an output file (see below). The
// string should be quoted if it has any spaces, e.g. -c "vers 3" The
// default comment is the empty string.
// 
// The output is a line of text:
//     prgname n iters warmup avg min max
// where avg is the average time per iteration in seconds,
// min is the minimum iteration time, and max is the maximum time
// 
// The output is also appended to benchmarkname.data with additional fields
// for the date, time, info, and comment:
//     prgname n iters warmup avg min max DD-MM-YYYY HH:MM:SS "info" "comment"
// where info is obtained by calling benchmark_info() which returns a C++
// string. This string can be used to describe variants of the benchmark.
// Note that this program adds quotes to the info string and
// the comment string (which is what was passed on the command line).
//
// Created by anthony on 5/30/24.
// New version mostly written from comments above by MS Copilot. -RBD
//
#include <iostream>
#include <chrono>
#include <fstream>
#include <string>
#include <ctime>
#include "any.h"
#include "gc.h"
#include "op_overload.h"
#include "basic_obj.h"
#include "obj.h"
#include "runtime.h"

// Function prototypes for benchmark implementations
void run_benchmark(int n);
std::string benchmark_info();

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0]
                  << " n [-i iters] [-w warmup] [-c comment]\n";
        return 1;
    }

    int n = std::stoi(argv[1]);
    int iters = 100;
    int warmup = 3;
    std::string comment = "";

    for (int i = 2; i < argc; ++i) {
        if (std::string(argv[i]) == "-i" && i + 1 < argc) {
            iters = std::stoi(argv[++i]);
        } else if (std::string(argv[i]) == "-w" && i + 1 < argc) {
            warmup = std::stoi(argv[++i]);
        } else if (std::string(argv[i]) == "-c" && i + 1 < argc) {
            comment = argv[++i];
        }
    }

    runtime_init();

    // Warmup phase
    for (int i = 0; i < warmup; ++i) {
        run_benchmark(n);
    }

    // Benchmark phase
    double total_time = 0.0;
    double min_time = std::numeric_limits<double>::max();
    double max_time = 0.0;

    for (int i = 0; i < iters; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        run_benchmark(n);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;

        double time = duration.count();
        total_time += time;
        if (time < min_time) min_time = time;
        if (time > max_time) max_time = time;
    }

    double avg_time = total_time / iters;

    // Output results
    std::cout << argv[0] << " " << n << " " << iters << " " << warmup << " "
              << avg_time << " " << min_time << " " << max_time << "\n";

    // Append results to file
    std::ofstream outfile(std::string(argv[0]) + ".data", std::ios::app);
    if (outfile.is_open()) {
        auto now = std::chrono::system_clock::
                   to_time_t(std::chrono::system_clock::now());
        std::tm* now_tm = std::localtime(&now);
        char date_time[20];
        std::strftime(date_time, sizeof(date_time), "%d-%m-%Y %H:%M:%S",
                      now_tm);

        outfile << argv[0] << " " << n << " " << iters << " " << warmup << " "
                << avg_time << " " << min_time << " " << max_time << " "
                << date_time << " \"" << benchmark_info() << "\" \""
                << comment << "\"\n";
        outfile.close();
    } else {
        std::cerr << "Unable to open file for writing\n";
    }

    return 0;
}

#ifdef OLD_ORIGINAL_IMPLEMENTATION_FROM_MAIN_BENCHMARK_CPP
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
static constexpr bool writeToFile = true;
static const std::string filename = "../benchmarks/benchmarks.csv";
static const std::string optimization = "New Dynamic Dispatch";

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
//        std::cout << test_trees(10) << std::endl;
//        std::cout << test_trees_c(10) << std::endl;
//        std::cout << test_matmul(10) << std::endl;
//        std::cout << test_matmul_c(10) << std::endl;
        std::cout << nq_solve(10) << std::endl;
//        std::cout << nq_solve_c(10) << std::endl;
    }
    return;
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
//    benchmarkC();
    main_init();
//    benchmarkC();
    benchmarkCSerpent();
    std::cout << "Done." << std::endl;
    return 0;
}

#endif
