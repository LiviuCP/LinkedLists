#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "../../../LinkedListsLib/linkedlist.h"
#include "../../ManualListEntry/apputils.h"

void addElementsToList(List* list, const size_t* priorities, const size_t elementsCount);
size_t makePipeAndSend(const char *pipeName, const size_t* priorities, const size_t elementsCount);

int main()
{
    const size_t priorities[] = {5, 2, 7, 3, 4};
    const char* fifoName = "/tmp/linkedlistpipe";

    List* senderList = createEmptyList();
    addElementsToList(senderList, priorities, 5);

    printf("The sender list has following elements:\n\n");
    printList(senderList);

    printf("Now we will send the priorities through a named pipe. Waiting for receiver to respond...\n\n");

    size_t bytesSent = makePipeAndSend(fifoName, priorities, 5);

    printf("Done! %d bytes sent through the named pipe\n", (int)bytesSent);

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

size_t makePipeAndSend(const char* pipeName, const size_t* priorities, const size_t elementsCount)
{
    size_t result = 0;

    if (priorities != NULL && elementsCount > 0)
    {
        const size_t nrOfBytes = elementsCount * sizeof (size_t);

        mkfifo(pipeName, 0666);

        int fDescriptor = open(pipeName, O_CREAT | O_WRONLY);

        if (fDescriptor < 0)
        {
            perror("Error! Named pipe cannot be opened for writing\n");
            exit(-1);
        }

        write(fDescriptor, priorities, nrOfBytes);

        close(fDescriptor);
        unlink(pipeName);
        result = nrOfBytes;
    }

    return result;
}

