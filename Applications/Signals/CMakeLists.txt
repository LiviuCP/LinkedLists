cmake_minimum_required(VERSION 3.5)

project(Signals LANGUAGES C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_compile_options(-D_XOPEN_SOURCE=700)

add_executable(${PROJECT_NAME}
    signalsmain.c
)

target_link_libraries(${PROJECT_NAME} PRIVATE LinkedListsLib)
target_link_libraries(${PROJECT_NAME} PRIVATE Utils)
