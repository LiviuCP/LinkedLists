cmake_minimum_required(VERSION 3.5)

project(BoundedBuffer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(${PROJECT_NAME}
    buffermain.cpp
    boundedbuffer.cpp
    buffermanipulation.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE LinkedListsLib)
target_link_libraries(${PROJECT_NAME} PRIVATE Utils)

if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
endif()
