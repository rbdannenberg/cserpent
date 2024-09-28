# normal set of include paths:
set(CS_INCLUDES src/runtime src/runtime/data_structures src/runtime/builtin_functions src/gc src/runtime/globals)

list(TRANSFORM CS_INCLUDES PREPEND ../../)
message(STATUS "after prepending ../../: CS_INCLUDES is ${CS_INCLUDES}") 

# runtime_benchmarks - runs benchmarks using main() in main_benchmark.cpp
add_executable(matrix_mult_benchmark matrix_mult.cpp ../runner/runner.cpp)
target_include_directories(matrix_mult_benchmark PUBLIC ${CS_INCLUDES})
target_link_libraries(matrix_mult_benchmark runtime) 

add_executable(nqueen_benchmark nqueen.cpp ../runner/runner.cpp) 
target_include_directories(matrix_mult_benchmark PUBLIC ${CS_INCLUDES})   
target_link_libraries(matrix_mult_benchmark runtime) 

add_executable(binary_trees_benchmark binary_trees.cpp ../runner/runner.cpp)
target_include_directories(binary_trees_benchmark PUBLIC ${CS_INCLUDES})  
target_link_libraries(binary_trees_benchmark runtime) 

