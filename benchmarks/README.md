How to benchmark:

Configure CMAKE -DCMAKE_BUILD_TYPE=Release as a command line argument to cmake.
This will enable optimizations and disable debug symbols.

To test the code's correctness, set the checkOnly global variable to True in
both benchmarks/py/main_benchmark.py and benchmarks/cpp/main_benchmark.cpp. Turn on print
statements in the respective functions.

Run the C++ benchmark by building the runtime_benchmark target in the build directory, then running
the executable

Run the Python benchmark by running the main_benchmark.py script in the benchmarks/py directory.

To test the code's performance, set the checkOnly global variable to False, and turn off print
statements in the respective functions.

Run the benchmarks as described above.