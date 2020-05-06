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

    if (childId == 0)
    {
        close(fileDescriptors[WRITE_SIDE]);

        printf("Receiving priorities from pipe...\n\n");

        if (read(fileDescriptors[READ_SIDE], buffer, sizeof(buffer)) > 0)
        {
            printf("Done\n\n");
            List* receiverList = createEmptyList();

            size_t* startAddress = (size_t*)buffer;
            for (size_t index = 0; index < LIST_SIZE; ++index)
            {
                createAndAppendToList(receiverList, *(startAddress + index));
            }

            sortAscendingByPriority(receiverList);

            printf("The receiver list has following elements after sorting:\n\n");
            printList(receiverList);
            printf("\n");

            deleteList(receiverList, deleteObject);
            receiverList = NULL;
        }
        else
        {
            fprintf(stderr, "Nothing read or error occurred");
        }

        close(fileDescriptors[READ_SIDE]);
        _exit(0);
    }
    else
    {
        close(fileDescriptors[READ_SIDE]);
        const size_t priorities[] = {5, 2, 7, 3, 4};

        List* senderList = createListFromPrioritiesArray(priorities, LIST_SIZE);

        printf("The sender list has following elements:\n\n");
        printList(senderList);
        printf("\n");

        printf("Sending priorities to pipe...\n\n");

        size_t* startAddress = (size_t*)buffer;
        for (size_t index = 0; index < LIST_SIZE; ++index)
        {
            *(startAddress + index) = priorities[index];
        }

        write(fileDescriptors[WRITE_SIDE], buffer, sizeof(buffer));

        printf("Done\n\n");
        printf("------------------------------------------------------\n\n");

        deleteList(senderList, deleteObject);
        senderList = NULL;

        close(fileDescriptors[WRITE_SIDE]);
        wait(NULL);
        exit(0);
    }

    return 0; // just for the sake of standards (int type function), does not return anything
}
