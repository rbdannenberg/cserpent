cmake_minimum_required(VERSION 3.29)
project(cserpent)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wno-return-type) # suppress warnings about type_error (NOT A FULL SOLUTION)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
add_definitions(-DFREE)

# normal set of include paths:
set(CS_INCLUDES src/runtime src/runtime/data_structures src/gc)

# cserpent - What's this? binary-trees.c and matrix-mult.c both define main()
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

# cserpent runtime library
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
        src/runtime/obj.cpp
        #src/runtime/data_structures/string.cpp
        # src/runtime/data_structures/string.h
        # src/runtime/data_structures/symbol.cpp
        # src/runtime/data_structures/symbol.h
        src/runtime/data_structures/dictionary.cpp
        src/runtime/data_structures/dictionary.h
        src/runtime/data_structures/csstring.h 
        src/runtime/data_structures/csstring.cpp 
        src/runtime/data_structures/symbol.h 
        src/runtime/data_structures/symbol.cpp 
        src/gc/gc.h
        src/gc/gc.cpp
)


# runtime_benchmarks - runs benchmarks using main() in main_benchmark.cpp
target_include_directories(runtime PUBLIC ${CS_INCLUDES})
add_executable(runtime_benchmarks
        benchmarks/C++/matrix_mult.cpp
        benchmarks/C++/nqueen.cpp
        benchmarks/C++/main_benchmark.cpp
        benchmarks/C++/binary_trees.cpp
        benchmarks/C++/C/binary_trees_c.cpp
        benchmarks/C++/C/matrix_mult_c.cpp
        benchmarks/C++/C/nqueen_c.cpp
)
target_include_directories(runtime_benchmarks PUBLIC ${CS_INCLUDES})
target_link_libraries(runtime_benchmarks runtime)

# runtime_test - runs runtime tests using main() in main_test.cpp
add_executable(runtime_test
        src/runtime/test/dictionary_test.cpp
        src/runtime/test/main_test.cpp
        src/runtime/test/binary_trees_test.cpp
        src/runtime/test/test_string.cpp
)
target_include_directories(runtime_test PUBLIC ${CS_INCLUDES})
target_link_libraries(runtime_test runtime)

# string_test - tests strings using main() in test_string.cpp
add_executable(string_test src/runtime/data_structures/csstring.h
        src/runtime/data_structures/csstring.cpp
        src/runtime/test/test_string.cpp)
target_include_directories(string_test PUBLIC ${CS_INCLUDES})

# mem_test - memory tests (no GC testing yet)
add_executable(mem_test src/runtime/test/memtest.cpp)
target_include_directories(mem_test PUBLIC ${CS_INCLUDES})
target_link_libraries(mem_test runtime)
