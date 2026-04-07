project(ManualListEntry LANGUAGES C)

add_executable(${PROJECT_NAME}
    listentrymain.c
)

target_link_libraries(${PROJECT_NAME} PRIVATE LinkedListsLib)
target_link_libraries(${PROJECT_NAME} PRIVATE Utils)
