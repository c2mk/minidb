cmake_minimum_required(VERSION 3.20)

project(dbms)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(dbms
    src/main.cpp
    src/shell.cpp
    src/command.cpp
    src/dispatcher.cpp
)

target_include_directories(dbms PRIVATE include)
# cmake -G "MinGW Makefiles" -B build