cmake_minimum_required(VERSION 3.29)
project(csruntime)

set(CMAKE_CXX_STANDARD 17)
# suppress warnings about type_error (NOT A FULL SOLUTION):
add_compile_options(-Wno-return-type -Wplacement-new=0) 
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)

# normal set of include paths:
set(CSRT_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} data_structures builtin_functions
                  ${CMAKE_CURRENT_SOURCE_DIR}/../gc globals)

# cserpent executable was here previously - was never used so removed it

# cserpent runtime library
add_library(runtime STATIC
        any.cpp
        any.h
        any_utils.cpp
        any_utils.h
        op_overload.cpp
        op_overload.h
        builtin_functions/builtin_functions.cpp
        builtin_functions/builtin_functions.h
        data_structures/array.cpp
        data_structures/array.h
        csmem.h
        csmem.cpp
        obj.h
        obj.cpp
        data_structures/array.h
        data_structures/array.cpp
        data_structures/dict.cpp
        data_structures/dict.h
        data_structures/csstring.h
        data_structures/csstring.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../gc/gc.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../gc/gc.cpp
        heap_obj.cpp
        heap_obj.h
        data_structures/symbol.cpp
        data_structures/symbol.h
        runtime.cpp
        runtime.h
)
target_include_directories(runtime PUBLIC ${CSRT_INCLUDES}) 

