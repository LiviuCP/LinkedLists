project(LinkedListsLib LANGUAGES C)

include_directories(../Utils)

add_library(${PROJECT_NAME} ${LIB_TYPE}
    linkedlist.c
    listelement.c
    listelementspool.c
    listelementspoolproxy.c
    sort.c
    listprintutils.c
)

target_link_libraries(${PROJECT_NAME} PRIVATE Utils)

if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
endif()
