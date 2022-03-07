cmake_minimum_required(VERSION 3.5)

project(Utils LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(${PROJECT_NAME} ${LIB_TYPE}
    utils.cpp
    codeutils.c
    testobjects.c
    bitoperations.c
)

target_compile_definitions(${PROJECT_NAME} PRIVATE UTILS_LIBRARY)
