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
        src/runtime/op_overload.cpp
)
add_library(runtime STATIC
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
        src/runtime/csmem.cpp
        src/runtime/obj.h
        #src/runtime/data_structures/string.cpp
        src/runtime/data_structures/string.h
        # src/runtime/data_structures/symbol.cpp
        src/runtime/data_structures/symbol.h
        src/runtime/data_structures/dictionary.cpp
        src/runtime/data_structures/dictionary.h)
target_include_directories(runtime PUBLIC src/runtime)
add_executable(runtime_benchmarks
        benchmarks/C++/matrix_mult.cpp
        benchmarks/C++/nqueen.cpp
        benchmarks/C++/main_benchmark.cpp
        benchmarks/C++/dictionary.cpp
        #benchmarks/C++/binary_trees.cpp
        #benchmarks/C++/symbols.cpp
)
add_dependencies(runtime_benchmarks runtime)
target_include_directories(runtime_benchmarks PUBLIC src/runtime)
target_link_libraries(runtime_benchmarks runtime)
add_executable(runtime_test
        benchmarks/C++/dictionary.cpp
)
add_dependencies(runtime_test runtime)
target_include_directories(runtime_test PUBLIC src/runtime)
target_link_libraries(runtime_test runtime)
