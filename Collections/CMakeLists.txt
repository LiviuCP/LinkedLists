project(Collections LANGUAGES C CXX)

include_directories(
    ../LinkedListsLib
    ../Utils
)

add_library(${PROJECT_NAME} ${LIB_TYPE}
    collections.cpp
    hashtable.c
    priorityqueue.c
    stack.c
)

target_link_libraries(${PROJECT_NAME} PRIVATE LinkedListsLib)
target_link_libraries(${PROJECT_NAME} PRIVATE Utils)

target_compile_definitions(${PROJECT_NAME} PRIVATE COLLECTIONS_LIBRARY)
