cmake_minimum_required(VERSION 3.5)

project(ManualListEntry LANGUAGES C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(${PROJECT_NAME}
    listentrymain.c
)

target_link_libraries(${PROJECT_NAME} PRIVATE LinkedListsLib)
target_link_libraries(${PROJECT_NAME} PRIVATE Utils)
