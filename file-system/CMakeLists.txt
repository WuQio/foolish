cmake_minimum_required(VERSION 3.6)
project(file_system)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp OS.h test.cpp)
add_executable(file_system ${SOURCE_FILES})