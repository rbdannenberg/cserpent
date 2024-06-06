# runtime/test/CMakeLists.txt
#
# Roger B. Dannenberg
# Jun 2024

cmake_minimum_required(VERSION 3.24)

project(runtimetests)
set(CMAKE_CXX_STANDARD 11)


add_executable(memtest
    memtest.cpp
    ../any.h ../any.cpp
    ../any_utils.h ../any_utils.cpp
    ../csmem.h ../csmem.cpp
    ../data_structures/array.h ../data_structures/array.cpp
    )

target_include_directories(memtest PRIVATE .. ../data_structures) 
