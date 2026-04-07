project(SharedMemory LANGUAGES C)

add_executable(SharedMemoryReader
    readermain.c
)

add_executable(SharedMemoryWriter
    writermain.c
)

target_link_libraries(SharedMemoryReader PRIVATE LinkedListsLib)
target_link_libraries(SharedMemoryReader PRIVATE Utils)
target_link_libraries(SharedMemoryWriter PRIVATE Utils)


if(UNIX AND NOT APPLE)
    target_link_libraries(SharedMemoryReader PRIVATE rt)
    target_link_libraries(SharedMemoryReader PRIVATE pthread)
    target_link_libraries(SharedMemoryWriter PRIVATE rt)
    target_link_libraries(SharedMemoryWriter PRIVATE pthread)
endif()
