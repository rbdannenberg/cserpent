cmake_minimum_required(VERSION 3.29)
project(cserpent)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wno-return-type -Wplacement-new=0) # suppress warnings about type_error (NOT A FULL SOLUTION)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)

# normal set of include paths:
set(CS_INCLUDES src/runtime src/runtime/data_structures src/runtime/builtin_functions src/gc src/runtime/globals)

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
        src/runtime/data_structures/dict.cpp
        src/runtime/data_structures/dict.h
        src/runtime/data_structures/csstring.h
        src/runtime/data_structures/csstring.cpp
        src/gc/gc.h
        src/gc/gc.cpp
        src/runtime/heap_obj.cpp
        src/runtime/heap_obj.h
        src/runtime/data_structures/symbol.cpp
        src/runtime/data_structures/symbol.h
        src/runtime/runtime.cpp
        src/runtime/runtime.h
)
target_include_directories(runtime PUBLIC ${CS_INCLUDES}) 


add_subdirectory(benchmarks/byhand)


# runtime_test - runs runtime tests using main() in main_test.cpp
add_executable(runtime_test
        tests/c++/runtime/dictionary_test.cpp
        tests/c++/runtime/main_test.cpp
        tests/c++/runtime/test_string.cpp
        tests/c++/runtime/test_string_any.cpp
        tests/c++/runtime/array_test.cpp
)
message(STATUS "in cserpent: CS_INCLUDES is ${CS_INCLUDES}") 
target_include_directories(runtime_test PUBLIC ${CS_INCLUDES})  
target_link_libraries(runtime_test runtime)

# memtest - memory tests (no GC testing yet)
add_executable(memtest tests/c++/runtime/memtest.cpp)
target_include_directories(memtest PUBLIC ${CS_INCLUDES}) 
target_link_libraries(memtest runtime) 

# gctest - tests for garbage collector
add_executable(gctest tests/c++/runtime/gctest.cpp)
target_include_directories(gctest PUBLIC ${CS_INCLUDES}) 
target_link_libraries(gctest runtime) 

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


if(EXISTS compilation_output/compilation_output.cpp) 
  add_executable(compilation_output
      compilation_output/compilation_output.h
          compilation_output/compilation_output.cpp
      compilation_output/main.cpp
  )
  target_include_directories(compilation_output PUBLIC ${CS_INCLUDES})
  target_link_libraries(compilation_output runtime)
endif()

if(EXISTS classes_require/account.cpp)
  add_executable(classes_test
      classes_require/account.cpp
      classes_require/account.h
          classes_require/classes_require.cpp
          classes_require/classes_require.h
          classes_require/main.cpp
          classes_require/global_vars.h
          classes_require/named_account.h
          classes_require/named_account.cpp
  )
  target_include_directories(classes_test PUBLIC ${CS_INCLUDES})
  target_link_libraries(classes_test runtime)
endif()

