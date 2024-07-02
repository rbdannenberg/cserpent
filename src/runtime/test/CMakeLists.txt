# runtime/test/CMakeLists.txt
#
# Roger B. Dannenberg
# Jun 2024

cmake_minimum_required(VERSION 3.24)

project(runtimetests)
set(CMAKE_CXX_STANDARD 11)

set(runtime_src
    ../any.h ../any.cpp 
    ../any_utils.h ../any_utils.cpp 
    ../obj.h ../obj.cpp
    ../csmem.h ../csmem.cpp
    ../../gc/gc.h ../../gc/gc.cpp
    ../data_structures/symbol.h ../data_structures/symbol.cpp
    ../data_structures/array.h ../data_structures/array.cpp)


add_executable(memtest
    memtest.cpp
    ${runtime_src}
    )
target_include_directories(memtest PRIVATE .. ../data_structures ../../gc) 


add_executable(maintest
    main_test.cpp
    dictionary_test.cpp
    binary_trees_test.cpp
    ${runtime_src})
target_include_directories(maintest PRIVATE .. ../data_structures ../../gc)

add_executable(cpptest
    cpptest.cpp)
target_include_directories(cpptest PRIVATE .. ../data_structures ../../gc)
