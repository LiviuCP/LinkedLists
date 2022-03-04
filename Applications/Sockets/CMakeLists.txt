cmake_minimum_required(VERSION 3.5)

project(Sockets LANGUAGES C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(NetworkServer
    servermain.c
)

add_executable(NetworkClient
    clientmain.c
)

target_link_libraries(NetworkClient PRIVATE LinkedListsLib)
target_link_libraries(NetworkClient PRIVATE Utils)
