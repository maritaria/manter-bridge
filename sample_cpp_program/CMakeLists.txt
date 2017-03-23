cmake_minimum_required(VERSION 3.6)
project(sample_cpp_program)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(sample_cpp_program ${SOURCE_FILES})