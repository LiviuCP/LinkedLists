cmake_minimum_required(VERSION 3.5)

project(Sockets LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(../ManualListEntry)

add_executable(NetworkServer
    servermain.c
)

add_executable(NetworkClient
    clientmain.c
    ../ManualListEntry/apputils.c
)

target_link_libraries(NetworkClient PRIVATE LinkedListsLib)
target_link_libraries(NetworkClient PRIVATE Utils)
