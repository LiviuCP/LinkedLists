#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "../../../LinkedListsLib/linkedlist.h"
#include "../../ManualListEntry/apputils.h"

void fillListFromPipe(List* list, const char* pipeName, size_t* nrOfBytes, size_t* nrOfElements);

int main()
{
    List* receiverList = createList();
    const char* fifoName = "/tmp/linkedlistpipe";

    printf("Reading bytes from the pipe. Please wait...\n\n");

    sleep(1);

    size_t nrOfBytes = 0;
    size_t nrOfElements = 0;

    fillListFromPipe(receiverList, fifoName, &nrOfBytes, &nrOfElements);

    printf("Done! %d bytes read from pipe. %d elements appended to list\n\n", (int)nrOfBytes, (int)nrOfElements);

    sortAscendingByPriority(receiverList);

    printf("The receiver list has following elements after sorting:\n\n");
    printList(receiverList);

    deleteList(receiverList, deleteObject);
    receiverList = NULL;
    return 0;
}

void fillListFromPipe(List* list, const char* pipeName, size_t* nrOfBytes, size_t* nrOfElements)
{
    const int fDescriptor = open(pipeName, O_RDONLY);

    *nrOfBytes = 0;
    *nrOfElements = 0;

    if (fDescriptor < 0)
    {
        perror("Error! Named pipe cannot be opened for reading\n");
        exit(-1);
    }

    while(1)
    {
        size_t currentValue;
        ssize_t bytesRead = read(fDescriptor, &currentValue, sizeof(size_t));

        if (bytesRead == sizeof(size_t))
        {
            createAndAppendToList(list, currentValue);
            ++(*nrOfElements);
        }
        else if (bytesRead == 0)
        {
            break;
        }

        *nrOfBytes += (size_t)bytesRead;
    }

    close(fDescriptor);
    unlink(pipeName);
}
