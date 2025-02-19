project(subtask_3_parallel_2)

cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

add_executable(subtask_3_parallel_2 subtask_3_parallel_2.cpp)