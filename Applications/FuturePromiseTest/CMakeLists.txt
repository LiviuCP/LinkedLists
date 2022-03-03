cmake_minimum_required(VERSION 3.5)

project(FuturePromise LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(../ASyncTest)

add_executable(${PROJECT_NAME}
    main.cpp
    ../ASyncTest/asyncutils.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE LinkedListsLib)
target_link_libraries(${PROJECT_NAME} PRIVATE Utils)

if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
endif()
