#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "../../LinkedListsLib/linkedlist.h"
#include "../ManualListEntry/apputils.h"

#define WRITE_SIDE 1
#define READ_SIDE 0
#define LIST_SIZE 5

int main()
{
    int fileDescriptors[2];
    char buffer[256];
    memset(buffer, '\0', sizeof(buffer));

    if (pipe(fileDescriptors) < 0)
    {
        perror("An error occurred when creating unnamed pipe\n");
        exit(-1);
    }
    pid_t childId = fork();

    if (childId < 0)
    {
        perror("An error occurred when spawning child process\n");
        exit(-1);
    }

    if (childId == 0)                                                                                       /* child */
    {
        close(fileDescriptors[WRITE_SIDE]);

        sleep(1); // for synchronization purposes (so parent has the chance to send the data)
        printf("[PID: %d] Receiving priorities from pipe...\n\n", getpid());

        if (read(fileDescriptors[READ_SIDE], buffer, sizeof(buffer)) > 0)
        {
            printf("[PID: %d] Done\n\n", getpid());
            List* receiverList = createEmptyList();

            if (receiverList != NULL)
            {
                size_t* startAddress = (size_t*)buffer;
                for (size_t index = 0; index < LIST_SIZE; ++index)
                {
                    createAndAppendToList(receiverList, *(startAddress + index));
                }

                sortAscendingByPriority(receiverList);

                printf("[PID: %d] The list has following elements after sorting:\n\n", getpid());
                printList(receiverList);
                printf("\n");

                deleteList(receiverList, deleteObject);
                receiverList = NULL;
            }
            else
            {
                fprintf(stderr, "[PID: %d] Unable to create list: no memory allocated\n", getpid());
                _exit(-1);
            }
        }
        else
        {
            fprintf(stderr, "[PID: %d] Nothing read or error occurred", getpid());
        }

        close(fileDescriptors[READ_SIDE]);
        _exit(0);
    }
    else                                                                                                    /* parent */
    {
        close(fileDescriptors[READ_SIDE]);
        const size_t priorities[] = {5, 2, 7, 3, 4};

        List* senderList = createListFromPrioritiesArray(priorities, LIST_SIZE);

        if (senderList != NULL)
        {
            printf("[PID: %d] The list has following elements:\n\n", getpid());
            printList(senderList);
            deleteList(senderList, deleteObject);
            senderList = NULL;
        }
        else
        {
            fprintf(stderr, "[PID: %d] Unable to create list: no memory allocated\n\n", getpid());
        }

        printf("\n");

        printf("[PID: %d] Sending priorities to pipe...\n\n", getpid());

        size_t* startAddress = (size_t*)buffer;
        for (size_t index = 0; index < LIST_SIZE; ++index)
        {
            *(startAddress + index) = priorities[index];
        }

        write(fileDescriptors[WRITE_SIDE], buffer, sizeof(buffer));

        printf("[PID: %d] Done\n\n", getpid());
        printf("------------------------------------------------------\n\n");

        close(fileDescriptors[WRITE_SIDE]);
        wait(NULL);
        exit(0);
    }

    return 0; // just for the sake of standards (int type function), does not return anything
}
