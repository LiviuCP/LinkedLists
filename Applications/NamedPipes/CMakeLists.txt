project(NamedPipes LANGUAGES C)

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
