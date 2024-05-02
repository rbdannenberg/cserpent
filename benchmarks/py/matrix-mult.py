# Benchmark Source: https://github.com/attractivechaos/plb2/blob/master/src/python/matmul.py
import time

def matmul(a, b):
    n, m, p = len(a), len(b[0]), len(a[0])
    c = []
    for i in range(n):
        ci = [0.0] * m
        for k in range(p):
            aik, bk = a[i][k], b[k]

            for j in range(m):
                ci[j] += aik * bk[j]
                c.append(ci)

    return c

def main(n):
    tmp = 1. / n / n
    a = [[tmp * (i - j) * (i + j) for j in range(n)] for i in range(n)]
    b = [[tmp * (i - j) * (i + j) for j in range(n)] for i in range(n)]
    d = matmul(a, b)
    #print(d[n//2][n//2])


# BENCHMARKING: Input size: 10, 50, and 100 
for i in range(500):
    start_time = time.time()
    #main(50)
    for iter in range(100):
        main(10)
    end_time = time.time()
    execution_time = end_time - start_time

    exec_ms = int(execution_time*1000)
    print(exec_ms)
