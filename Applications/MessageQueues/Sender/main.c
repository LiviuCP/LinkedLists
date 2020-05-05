#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>

#include "messagequeue.h"
#include "../../../LinkedListsLib/linkedlist.h"
#include "../../ManualListEntry/apputils.h"

void addElementsToList(List* list, const size_t* priorities, const size_t elementsCount);
void createQueueFile(void);
void sendToQueue(const size_t* priorities, const size_t *priorityTypes, const size_t elementsCount);

int main()
{
    const size_t priorities[] = {5, 2, 104, 205, 1200, 1099};
    const size_t priorityTypes[] = {1, 1, 2, 2, 3, 3};

    createQueueFile();

    List* senderList = createList();
    addElementsToList(senderList, priorities, 6);

    printf("The sender list has following elements:\n\n");
    printList(senderList);

    printf("Sending list element priorities and matching priority types to a message queue\n\n");
    sendToQueue(priorities, priorityTypes, 6);
    printf("\nDone\n");

    deleteList(senderList, deleteObject);
    senderList = NULL;

    return 0;
}

void addElementsToList(List* list, const size_t* priorities, const size_t elementsCount)
{
    if (list != NULL && priorities != NULL && elementsCount > 0)
    {
        for (size_t index = 0; index < elementsCount; ++index)
        {
            createAndAppendToList(list, priorities[index]);
        }
    }
}

void createQueueFile()
{
    FILE* queueFile = fopen(QUEUE_FILENAME, "a+");
    if (queueFile == NULL)
    {
        perror("Error in opening queue file");
        exit(-1);
    }
    else
    {
        fclose(queueFile);
    }
}

void sendToQueue(const size_t* priorities, const size_t* priorityTypes, const size_t elementsCount)
{
    const key_t queueKey = ftok(QUEUE_FILENAME, BASE_ID);
    if (queueKey < 0)
    {
        perror("The queue key could not be obtained\n");
        exit(-1);
    }

    const int queueId = msgget(queueKey, 0666 | IPC_CREAT);
    if (queueId < 0)
    {
        perror("The queue id could not be obtained\n");
        exit(-1);
    }

    for (size_t index = 0; index < elementsCount; ++index)
    {
        QueueMessage currentMessage;
        currentMessage.priority = priorities[index];
        currentMessage.priorityType = priorityTypes[index];
        msgsnd(queueId, &currentMessage, sizeof(currentMessage), IPC_NOWAIT);

        printf("Sent list element priority %d of type %d\n", (int)currentMessage.priority, (int)currentMessage.priorityType);
    }
}
