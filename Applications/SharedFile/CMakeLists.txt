project(SharedFile LANGUAGES C)

add_executable(FileProducer
    producermain.c
)

add_executable(FileConsumer
    consumermain.c
)

target_link_libraries(FileProducer PRIVATE Utils)
target_link_libraries(FileConsumer PRIVATE LinkedListsLib)
target_link_libraries(FileConsumer PRIVATE Utils)
