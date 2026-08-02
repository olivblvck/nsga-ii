cmake_minimum_required(VERSION 3.20)

project(nsga_ii LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(nsga_ii
        main.cpp
        RandomGen.h
        main.cpp
        RandomGen.h
        main.cpp
)

set_target_properties(nsga_ii PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
