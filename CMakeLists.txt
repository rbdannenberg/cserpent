cmake_minimum_required(VERSION 3.29)
project(cserpent)

set(CMAKE_CXX_STANDARD 14)

add_executable(cserpent
        benchmarks/C/binary-trees.c
        benchmarks/C/matrix-mult.c
        benchmarks/C/sudoku.c
        src/runtime/any.cpp
        src/runtime/any.h
        src/runtime/any_utils.cpp
        src/runtime/any_utils.h
        src/runtime/data_structures/array_raw.h
        src/runtime/csmem.h
        src/runtime/main.cpp
        src/runtime/op_overload.cpp
        src/runtime/data_structures/string.h)
add_executable(runtime
        src/runtime/any.cpp
        src/runtime/any.h
        src/runtime/any_utils.cpp
        src/runtime/any_utils.h
        src/runtime/data_structures/array_raw.h
        src/runtime/csmem.h
        src/runtime/op_overload.cpp
        src/runtime/main.cpp
        src/runtime/op_overload.h
        src/runtime/builtin_functions/builtin_functions.cpp
        src/runtime/builtin_functions/builtin_functions.h
        src/runtime/data_structures/array.cpp
        src/runtime/data_structures/array.h
        src/runtime/csmem.cpp)
target_include_directories(runtime PUBLIC src/runtime)
add_executable(runtime_tests
        benchmarks/srp/matrix_mult.cpp)
add_dependencies(runtime_tests runtime)
target_include_directories(runtime_tests PUBLIC src/runtime)
