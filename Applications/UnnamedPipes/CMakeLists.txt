project(UnnamedPipes LANGUAGES C)

add_executable(${PROJECT_NAME}
    pipesmain.c
)

target_link_libraries(${PROJECT_NAME} PRIVATE LinkedListsLib)
target_link_libraries(${PROJECT_NAME} PRIVATE Utils)
