project(Collections LANGUAGES C)

include_directories(
    ../LinkedListsLib
    ../Utils
)

add_library(${PROJECT_NAME} ${LIB_TYPE}
    hashtable.c
    priorityqueue.c
    stack.c
)

target_link_libraries(${PROJECT_NAME} PRIVATE LinkedListsLib)
target_link_libraries(${PROJECT_NAME} PRIVATE Utils)
