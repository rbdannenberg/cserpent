cmake_minimum_required(VERSION 3.29)
project(output)
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wno-return-type -Wplacement-new=0)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)

# normal set of include paths:
# runtime library
add_subdirectory(../../../../src/runtime build)
add_executable(easyglobal
        easyglobal.cpp
        easyglobal.h
        global_vars.cpp
)
target_link_libraries(easyglobal runtime)
target_include_directories(easyglobal PUBLIC ../../../../src/runtime ../../../../src/gc)
