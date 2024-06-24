import time
from matrix_mult import test_matmul
from nqueen import nq_solve
from binary_trees import test_trees

checkOnly = False

def benchmark(func, arg, iterations=100, warmup=10):
    times = []
    # warmup
    for _ in range(warmup):
        func(arg)
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


def main():
    if checkOnly:
        test_trees(10)
        test_matmul(10)
        nq_solve(10)
        return
    print("Benchmarking Binary Trees...")
    benchmark(test_trees, 10)
    benchmark(test_trees, 15)
    benchmark(test_trees, 17)
    print("Benchmarking Matrix Multiplication...")
    benchmark(test_matmul, 10)
    benchmark(test_matmul, 50)
    benchmark(test_matmul, 100)
    print("Benchmarking N Queens...")
    benchmark(nq_solve, 10, 5, 2)
    benchmark(nq_solve, 12, 5, 2)
    benchmark(nq_solve, 15, 5, 2)

if __name__ == "__main__":
    main()