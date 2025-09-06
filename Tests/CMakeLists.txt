cmake_minimum_required(VERSION 3.5)

project(Tests LANGUAGES CXX)

find_package(QT NAMES Qt5 Qt6 COMPONENTS Test REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

include_directories(
    ../LinkedListsLib
    ../Collections
    ../Utils
)

add_executable(BitOperationsTests tst_bitoperationstests.cpp)
add_executable(CodeUtilsTests tst_codeutilstests.cpp)
add_executable(HashTableTests tst_hashtabletests.cpp)
add_executable(LinkedListTests tst_linkedlisttests.cpp)
add_executable(ListElementTests tst_listelementtests.cpp)
add_executable(PriorityQueueTests tst_priorityqueuetests.cpp)
add_executable(StackTests tst_stacktests.cpp)

add_test(NAME BitOperationsTests COMMAND BitOperationsTests)
add_test(NAME CodeUtilsTests COMMAND CodeUtilsTests)
add_test(NAME HashTableTests COMMAND HashTableTests)
add_test(NAME LinkedListTests COMMAND LinkedListTests)
add_test(NAME ListElementTests COMMAND ListElementTests)
add_test(NAME PriorityQueueTests COMMAND PriorityQueueTests)
add_test(NAME StackTests COMMAND StackTests)

target_link_libraries(BitOperationsTests PRIVATE Qt${QT_VERSION_MAJOR}::Test)
target_link_libraries(BitOperationsTests PRIVATE Utils)

target_link_libraries(CodeUtilsTests PRIVATE Qt${QT_VERSION_MAJOR}::Test)
target_link_libraries(CodeUtilsTests PRIVATE Utils)

target_link_libraries(HashTableTests PRIVATE Qt${QT_VERSION_MAJOR}::Test)
target_link_libraries(HashTableTests PRIVATE Collections)
target_link_libraries(HashTableTests PRIVATE LinkedListsLib)
target_link_libraries(HashTableTests PRIVATE Utils)

target_link_libraries(LinkedListTests PRIVATE Qt${QT_VERSION_MAJOR}::Test)
target_link_libraries(LinkedListTests PRIVATE LinkedListsLib)
target_link_libraries(LinkedListTests PRIVATE Utils)

target_link_libraries(ListElementTests PRIVATE Qt${QT_VERSION_MAJOR}::Test)
target_link_libraries(ListElementTests PRIVATE LinkedListsLib)
target_link_libraries(ListElementTests PRIVATE Utils)

target_link_libraries(PriorityQueueTests PRIVATE Qt${QT_VERSION_MAJOR}::Test)
target_link_libraries(PriorityQueueTests PRIVATE Collections)
target_link_libraries(PriorityQueueTests PRIVATE LinkedListsLib)
target_link_libraries(PriorityQueueTests PRIVATE Utils)

target_link_libraries(StackTests PRIVATE Qt${QT_VERSION_MAJOR}::Test)
target_link_libraries(StackTests PRIVATE Collections)
target_link_libraries(StackTests PRIVATE LinkedListsLib)
target_link_libraries(StackTests PRIVATE Utils)
