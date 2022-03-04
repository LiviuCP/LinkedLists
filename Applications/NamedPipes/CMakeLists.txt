cmake_minimum_required(VERSION 3.5)

project(NamedPipes LANGUAGES C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(PipeSender
    sendermain.c
)

add_executable(PipeReceiver
    receivermain.c
)

target_link_libraries(PipeSender PRIVATE LinkedListsLib)
target_link_libraries(PipeSender PRIVATE Utils)

target_link_libraries(PipeReceiver PRIVATE LinkedListsLib)
target_link_libraries(PipeReceiver PRIVATE Utils)
