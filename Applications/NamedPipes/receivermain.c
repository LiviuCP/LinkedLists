#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "listprintutils.h"

void fillListFromPipe(List* list, const char* pipeName, size_t* nrOfBytes, size_t* nrOfElements);

int main()
{
    List* receiverList = createEmptyList(NULL);

    if (receiverList != NULL)
    {
        const char* fifoName = "/tmp/linkedlistpipe";

        printf("We will read priorities from a named pipe and use them for creating and appending new elements to list. Please wait...\n\n");

        sleep(1);

        size_t nrOfBytes = 0;
        size_t nrOfElements = 0;

        fillListFromPipe(receiverList, fifoName, &nrOfBytes, &nrOfElements);

        printf("\n%d bytes read from pipe. %d elements appended to list\n\n", (int)nrOfBytes, (int)nrOfElements);

        sortAscendingByPriority(receiverList);

        printf("The receiver list has following elements after sorting:\n\n");
        printList(receiverList);

        deleteList(receiverList, deleteObjectPayload);
        receiverList = NULL;
    }
    else
    {
        printf("Unable to create receiver list: no memory allocated\n");
        exit(-1);
    }

    return 0;
}

void fillListFromPipe(List* list, const char* pipeName, size_t* nrOfBytes, size_t* nrOfElements)
{
    printf("Opening pipe for reading\n");
    sleep(2);
    const int fDescriptor = open(pipeName, O_RDONLY);

    if (fDescriptor < 0)
    {
        perror("Error! Named pipe cannot be opened for reading\n");
        exit(-1);
    }
    else
    {
        printf("Done opening pipe for reading\n");
    }

    *nrOfBytes = 0;
    *nrOfElements = 0;

    sleep(2);

    printf("\nReading from pipe, creating and appending to list\n");
    sleep(1);

    while(1)
    {
        Priority currentValue;
        ssize_t readBytesCount = read(fDescriptor, &currentValue, sizeof(Priority));

        if (readBytesCount == sizeof(Priority))
        {
            createAndAppendToList(list, currentValue);
            ++(*nrOfElements);
        }
        else if (readBytesCount == 0)
        {
            break;
        }

        *nrOfBytes += (size_t)readBytesCount;
    }

    printf("Done reading from pipe, creating and appending\n");

    close(fDescriptor);
    unlink(pipeName);
}
