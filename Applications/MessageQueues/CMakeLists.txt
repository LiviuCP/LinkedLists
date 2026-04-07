project(MessageQueues LANGUAGES C)

add_executable(QueueSender
    sendermain.c
)

add_executable(QueueReceiver
    receivermain.c
)

target_link_libraries(QueueSender PRIVATE LinkedListsLib)
target_link_libraries(QueueSender PRIVATE Utils)

target_link_libraries(QueueReceiver PRIVATE LinkedListsLib)
target_link_libraries(QueueReceiver PRIVATE Utils)
