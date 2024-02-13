cmake_minimum_required(VERSION 3.16)
project(sum_teqc)

set(CMAKE_CXX_STANDARD 17)

add_executable(sum_teqc
    sum_teqc.cpp
    )

target_link_libraries(sum_teqc)