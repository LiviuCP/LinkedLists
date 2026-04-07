project(Sockets LANGUAGES C)

add_executable(NetworkServer
    servermain.c
)

add_executable(NetworkClient
    clientmain.c
)

target_link_libraries(NetworkServer PRIVATE Utils)
target_link_libraries(NetworkClient PRIVATE LinkedListsLib)

target_link_libraries(NetworkClient PRIVATE Utils)
