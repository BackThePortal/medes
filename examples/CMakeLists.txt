include(FetchContent)
cmake_minimum_required(VERSION 4.0)
project(medes_examples)

set(CMAKE_CXX_STANDARD 23)

find_package(medes)

add_executable(
        medes_examples
        basic.cpp
)
