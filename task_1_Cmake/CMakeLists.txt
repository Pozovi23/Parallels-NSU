cmake_minimum_required(VERSION 3.1)

add_executable(task_1_Cmake task_1_Cmake.cpp)
target_compile_definitions(task_1_Cmake PUBLIC TYPENAME=double)