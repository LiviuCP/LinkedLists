cmake_minimum_required(VERSION 3.5)

project(Applications LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(
    ../LinkedListsLib
    ../Utils
)

add_subdirectory(ManualListEntry)

if (UNIX)
    add_subdirectory(Asynchronous)
    add_subdirectory(BoundedBuffer)
    add_subdirectory(Signals)
    add_subdirectory(UnnamedPipes)
    add_subdirectory(MessageQueues)
    add_subdirectory(NamedPipes)
    add_subdirectory(SharedFile)
    add_subdirectory(SharedMemory)
    add_subdirectory(Sockets)
endif()
