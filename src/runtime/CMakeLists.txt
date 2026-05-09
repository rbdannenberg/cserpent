cmake_minimum_required(VERSION 3.29)
project(csruntime)

set(CMAKE_CXX_STANDARD 17)
# suppress warnings about type_error (NOT A FULL SOLUTION):
add_compile_options(-Wno-return-type -Wplacement-new=0) 
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)

# normal set of include paths:
set(CSRT_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../gc)

# cserpent executable was here previously - was never used so removed it

# cserpent runtime library
add_library(runtime STATIC
        any.cpp any.h
        any_utils.cpp any_utils.h
        op_overload.cpp op_overload.h
        builtin_functions.cpp builtin_functions.h
        array.cpp array.h
        csmem.cpp csmem.h
        obj.cpp obj.h
        array.cpp array.h
        dict.cpp dict.h
        csstring.cpp csstring.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../gc/gc.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../gc/gc.cpp
        heap_obj.cpp heap_obj.h
        symbol.cpp symbol.h 
        symbol_table.cpp symbol_table.h 
        runtime.cpp runtime.h
)
target_include_directories(runtime PUBLIC ${CSRT_INCLUDES}) 

