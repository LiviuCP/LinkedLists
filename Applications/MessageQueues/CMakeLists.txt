cmake_minimum_required(VERSION 3.5)

project(MessageQueues LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(QueueSender
    sendermain.c
)

add_executable(QueueReceiver
    receivermain.c
)

target_link_libraries(QueueSender PRIVATE LinkedListsLib)
target_link_libraries(QueueSender PRIVATE Utils)

target_link_libraries(QueueReceiver PRIVATE LinkedListsLib)
target_link_libraries(QueueReceiver PRIVATE Utils)
