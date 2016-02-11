cmake_minimum_required(VERSION 3.3)
project(shell)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.c shell.h debug.h shell.c)
add_executable(shell ${SOURCE_FILES})