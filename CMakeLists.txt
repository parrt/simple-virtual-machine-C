cmake_minimum_required(VERSION 3.1)
project(simple_virtual_machine_C)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    src/vm.c
    src/vm.h
    src/vmtest.c)

add_executable(simple_virtual_machine_C ${SOURCE_FILES})