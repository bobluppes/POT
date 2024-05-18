cmake_minimum_required(VERSION 3.11...3.28)

project(emd
        VERSION 0.1.0
        DESCRIPTION "Earth Mover's Distance Solver"
        LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()
add_subdirectory(test)

add_library(emd SHARED EMD_wrapper.cpp)