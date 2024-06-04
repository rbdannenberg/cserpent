import time
from matrix_mult import test_matmul
from nqueen import nq_solve

def main():
    start = time.time()

    print("Testing matrix multiplication...")
    test_matmul(10)
    test_matmul(50)
    test_matmul(100)
    print("Testing n queens...")
    nq_solve(5)
    nq_solve(8)
    nq_solve(15)
    print("All tests passed!")

    end = time.time()
    elapsed = end - start
    print("Elapsed time: ", elapsed, "s")

if __name__ == "__main__":
    main()