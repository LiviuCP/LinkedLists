cmake_minimum_required(VERSION 3.5)

project(LinkedListsLib LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(../Utils)

add_library(${PROJECT_NAME} ${LIB_TYPE}
    linkedlistslib.cpp
    linkedlist.c
    sort.c
    listprintutils.c
)

target_link_libraries(${PROJECT_NAME} PRIVATE Utils)

if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE LINKEDLISTSLIB_LIBRARY)
