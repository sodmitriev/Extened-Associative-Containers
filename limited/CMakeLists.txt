cmake_minimum_required(VERSION 3.10)
project(Util)

set(CMAKE_CXX_STANDARD 17)

if (NOT NO_TEST)
    enable_testing()
endif()

add_subdirectory(Tests)
