project(Asynchronous LANGUAGES CXX)

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
