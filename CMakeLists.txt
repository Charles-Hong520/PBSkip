cmake_minimum_required(VERSION 3.10)

project(MyProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Collect all source files
set(SOURCES 
    src/sequential/person.pbs.cpp 
    src/sequential/test.cpp
)

add_executable(test ${SOURCES})
