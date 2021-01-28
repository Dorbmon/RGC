cmake_minimum_required(VERSION 3.12)
project(RGC C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
add_executable(RGC
        GC.h GC.cpp)
