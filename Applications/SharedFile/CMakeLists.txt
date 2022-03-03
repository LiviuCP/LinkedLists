cmake_minimum_required(VERSION 3.5)

project(SharedFile LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(FileProducer
    producermain.c
)

add_executable(FileConsumer
    consumermain.c
)

target_link_libraries(FileConsumer PRIVATE LinkedListsLib)
target_link_libraries(FileConsumer PRIVATE Utils)
