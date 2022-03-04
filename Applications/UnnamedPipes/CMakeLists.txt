cmake_minimum_required(VERSION 3.5)

project(UnnamedPipes LANGUAGES C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(${PROJECT_NAME}
    pipesmain.c
)

target_link_libraries(${PROJECT_NAME} PRIVATE LinkedListsLib)
target_link_libraries(${PROJECT_NAME} PRIVATE Utils)
