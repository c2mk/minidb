cmake_minimum_required(VERSION 3.20)

project(dbms)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(dbms
    src/main.cpp
    src/shell.cpp
    src/dispatcher.cpp
    src/parser/parser.cpp
    src/parser/tokenizer.cpp
    src/storage/types.cpp
    src/storage/table.cpp
    # main-test.cpp
)

target_include_directories(dbms PUBLIC include)
# cmake -G "MinGW Makefiles" -B build