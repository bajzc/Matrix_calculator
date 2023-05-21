cmake_minimum_required(VERSION 3.22)
project(test-demo C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(test)

add_executable(Matrix_calculator
        test/demo.c)
