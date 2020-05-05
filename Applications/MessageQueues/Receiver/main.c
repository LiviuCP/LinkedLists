#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>

#include "messagequeue.h"
#include "../../../LinkedListsLib/linkedlist.h"
#include "../../ManualListEntry/apputils.h"

void fillListFromQueue(List* list, const size_t expectedElementsNr, const size_t* priorityTypes);

int main()
{
    List* receiverList = createList();
    const size_t expectedElementsNr = 6;
    const size_t expectedPriorityTypes[] = {3, 2, 1, 3, 2, 1};

    printf("Filling list elements from message queue\n\n");
    fillListFromQueue(receiverList, expectedElementsNr, expectedPriorityTypes);
    printf("\nDone!\n\n");

    sortAscendingByPriority(receiverList);

    printf("The receiver list has following elements after sorting:\n\n");
    printList(receiverList);

    deleteList(receiverList, deleteObject);
    receiverList = NULL;

    return 0;
}

void fillListFromQueue(List* list, const size_t expectedElementsNr, const size_t* priorityTypes)
{
    if (list != NULL && priorityTypes != NULL)
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

        for (size_t index = 0; index < expectedElementsNr; ++index)
        {
            QueueMessage currentMessage;

            if (msgrcv(queueId, &currentMessage, sizeof(currentMessage), (long)priorityTypes[index], MSG_NOERROR | IPC_NOWAIT) < 0)
            {
                fprintf(stderr, "Issues with receiving message\n");
            }

            printf("Received list element priority %d of type %d\n", (int)currentMessage.priority, (int)currentMessage.priorityType);

            createAndAppendToList(list, currentMessage.priority);
        }

        if (msgctl(queueId, IPC_RMID, NULL) < 0)
        {
            perror("There was an issue with removing the queue\n");
            exit(-1);
        }
    }
}
