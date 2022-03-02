cmake_minimum_required(VERSION 3.5)

project(LinkedLists LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(Applications)
add_subdirectory(LinkedListsLib)
add_subdirectory(Collections)
add_subdirectory(Utils)

find_package(QT NAMES Qt5 Qt6 COMPONENTS Test QUIET)

if(QT_FOUND)
    add_subdirectory(Tests)
endif()
