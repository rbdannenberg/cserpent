cmake_minimum_required(VERSION 3.29)
project(cserpent)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wno-return-type -Wplacement-new=0) # suppress warnings about type_error (NOT A FULL SOLUTION)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
add_definitions(-DFREE -DCSMALLOC)

# normal set of include paths:
set(CS_INCLUDES src/runtime src/runtime/data_structures src/runtime/builtin_functions src/gc)

# cserpent executable was here previously - was never used so removed it

# cserpent runtime library
add_library(runtime STATIC
        src/runtime/any.cpp
        src/runtime/any.h
        src/runtime/any_utils.cpp
        src/runtime/any_utils.h
        src/runtime/op_overload.cpp
        src/runtime/op_overload.h
        src/runtime/builtin_functions/builtin_functions.cpp
        src/runtime/builtin_functions/builtin_functions.h
        src/runtime/data_structures/array.cpp
        src/runtime/data_structures/array.h
        src/runtime/csmem.h
        src/runtime/csmem.cpp
        src/runtime/obj.h
        src/runtime/obj.cpp
        src/runtime/data_structures/array.h
        src/runtime/data_structures/array.cpp
        src/runtime/data_structures/dictionary.cpp
        src/runtime/data_structures/dictionary.h
        src/runtime/data_structures/csstring.h
        src/runtime/data_structures/csstring.cpp
        src/gc/gc.h
        src/gc/gc.cpp
        src/runtime/basic_obj.cpp
        src/runtime/basic_obj.h
        src/runtime/data_structures/symbol.cpp
        src/runtime/data_structures/symbol.h
        src/runtime/globals/symbol_table.cpp
        src/runtime/globals/symbol_table.h
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
        tests/c++/runtime/dictionary_test.cpp
        tests/c++/runtime/main_test.cpp
        tests/c++/runtime/test_string.cpp
        tests/c++/runtime/test_string_any.cpp
        tests/c++/runtime/array_test.cpp
)
target_include_directories(runtime_test PUBLIC ${CS_INCLUDES})
target_link_libraries(runtime_test runtime)

# mem_test - memory tests (no GC testing yet)
add_executable(mem_test tests/c++/runtime/memtest.cpp)
target_include_directories(mem_test PUBLIC ${CS_INCLUDES})
target_link_libraries(mem_test runtime)

add_executable(cpptest tests/c++/runtime/cpptest.cpp)

add_executable(reprtest
    tests/c++/reprtest/any_compare.h
    tests/c++/reprtest/any_compare.cpp
    tests/c++/reprtest/addone.cpp
)

add_executable(stringtest
    src/runtime/data_structures/csstring.h
    src/runtime/data_structures/csstring.cpp
    tests/c++/runtime/test_string.cpp)
target_include_directories(stringtest PUBLIC ${CS_INCLUDES})