cmake_minimum_required(VERSION 3.20)

project(dbms)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(dbms_lib

    src/catalog.cpp
    src/dispatcher.cpp
    src/executor.cpp
    src/result.cpp
    src/shell.cpp

    src/parser/parser.cpp
    src/parser/tokenizer.cpp

    src/storage/table.cpp
    src/storage/types.cpp
)
target_include_directories(dbms_lib PUBLIC include)

add_executable(dbms
    src/main.cpp
)
target_link_libraries(dbms PRIVATE dbms_lib)

add_executable(parser_debug
    debug/parser_debug.cpp
 )
target_link_libraries(parser_debug PRIVATE dbms_lib)

add_executable(storage_debug
     debug/storage_debug.cpp
)
target_link_libraries(storage_debug PRIVATE dbms_lib)

add_executable(executor_debug
 debug/executor_debug.cpp
 )
target_link_libraries(executor_debug PRIVATE dbms_lib)

add_executable(integration_debug
 debug/integration_debug.cpp
 )
target_link_libraries(integration_debug PRIVATE dbms_lib)

# cmake -G "MinGW Makefiles" -B build