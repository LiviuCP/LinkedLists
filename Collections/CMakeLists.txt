cmake_minimum_required(VERSION 3.5)

project(Collections LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(
    ../LinkedListsLib
    ../Utils
)

add_library(${PROJECT_NAME} SHARED
    collections.cpp
    hashtable.c
    priorityqueue.c
    stack.c
)

target_link_libraries(${PROJECT_NAME} PRIVATE LinkedListsLib)
target_link_libraries(${PROJECT_NAME} PRIVATE Utils)

target_compile_definitions(${PROJECT_NAME} PRIVATE COLLECTIONS_LIBRARY)
