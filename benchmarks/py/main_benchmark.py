import time
from matrix_mult import test_matmul
from nqueen import nq_solve
from binary_trees import test_trees
import gc

checkCorrectness = False
writeToFile = True
filename = "../../benchmarks/benchmarks.csv"

def benchmark(func, arg, iterations=100, warmup=10):
    times = []
    # warmup
    for _ in range(warmup):
        func(arg)
    gc.collect()
    for _ in range(iterations):
        start = time.time()
        func(arg)
        end = time.time()
        elapsed = end - start
        times.append(elapsed)
    avg_time = sum(times) / iterations
    max_time = max(times)
    print(f"Average time for n = {arg} over {iterations} iterations: {avg_time}")
    print(f"Max time for n = {arg} over {iterations} iterations: {max_time}")
    return avg_time


def main():
    if checkCorrectness:
        test_trees(10)
        test_matmul(10)
        nq_solve(10)
    times = []
    print("Benchmarking Matrix Multiplication...")
    times.append(benchmark(test_matmul, 20))
    times.append(benchmark(test_matmul, 50))
    times.append(benchmark(test_matmul, 100))
    print("Benchmarking Binary Trees...")
    times.append(benchmark(test_trees, 10))
    times.append(benchmark(test_trees, 15))
    times.append(benchmark(test_trees, 17))
    print("Benchmarking N Queens...")
    times.append(benchmark(nq_solve, 10, 5))
    times.append(benchmark(nq_solve, 12, 5))
    times.append(benchmark(nq_solve, 14, 5))
    if writeToFile:
        with open(filename, 'a') as f:
            print("Writing to file...")
            f.write("Python, " + ", ".join(map(str, times)) + "\n")
    print("Done!")

if __name__ == "__main__":
    main()