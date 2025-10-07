cmake_minimum_required(VERSION 3.5)

project(Asynchronous LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(StdAsync
    asyncmain.cpp
    asyncutils.cpp
)

add_executable(StdFuturePromise
    futurepromisemain.cpp
    asyncutils.cpp
)

target_link_libraries(StdAsync PRIVATE LinkedListsLib)
target_link_libraries(StdAsync PRIVATE Utils)

target_link_libraries(StdFuturePromise PRIVATE LinkedListsLib)
target_link_libraries(StdFuturePromise PRIVATE Utils)

if(UNIX AND NOT APPLE)
    target_link_libraries(StdAsync PRIVATE pthread)
    target_link_libraries(StdFuturePromise PRIVATE pthread)
endif()
