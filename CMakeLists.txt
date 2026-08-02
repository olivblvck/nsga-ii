cmake_minimum_required(VERSION 3.20)

project(nsga_ii LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(nsga_ii
main.cpp
RandomGen.h
)

target_compile_definitions(nsga_ii PRIVATE
PROJECT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)
