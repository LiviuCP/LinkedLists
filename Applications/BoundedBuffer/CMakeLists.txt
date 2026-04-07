project(BoundedBuffer LANGUAGES CXX)

add_executable(${PROJECT_NAME}
    buffermain.cpp
    boundedbuffer.cpp
    buffermanipulation.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE LinkedListsLib)
target_link_libraries(${PROJECT_NAME} PRIVATE Utils)

if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
endif()
