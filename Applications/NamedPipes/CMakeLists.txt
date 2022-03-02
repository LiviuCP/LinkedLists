cmake_minimum_required(VERSION 3.5)

project(NamedPipes LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(../ManualListEntry)

add_executable(PipeSender
    sendermain.c
    ../ManualListEntry/apputils.c
)

add_executable(PipeReceiver
    receivermain.c
    ../ManualListEntry/apputils.c
)

target_link_libraries(PipeSender PRIVATE LinkedListsLib)
target_link_libraries(PipeSender PRIVATE Utils)

target_link_libraries(PipeReceiver PRIVATE LinkedListsLib)
target_link_libraries(PipeReceiver PRIVATE Utils)
