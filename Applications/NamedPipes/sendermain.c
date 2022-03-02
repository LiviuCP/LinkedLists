#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "linkedlist.h"
#include "apputils.h"

size_t makePipeAndSend(const char *pipeName, const size_t* priorities, const size_t elementsCount);

int main()
{
    const size_t priorities[] = {5, 2, 7, 3, 4};
    const char* fifoName = "/tmp/linkedlistpipe";

    List* senderList = createListFromPrioritiesArray(priorities, 5);

    if (senderList != NULL)
    {
        printf("The sender list has following elements:\n\n");
        printList(senderList);
        deleteList(senderList, deleteObjectPayload);
        senderList = NULL;
    }
    else
    {
        fprintf(stderr, "Unable to create sender list: no memory allocated\n\n");
    }

    printf("Now we will send the priorities through a named pipe.\n\n");

    size_t bytesSent = makePipeAndSend(fifoName, priorities, 5);

    printf("\n%d bytes sent through the named pipe\n", (int)bytesSent);

    return 0;
}

size_t makePipeAndSend(const char* pipeName, const size_t* priorities, const size_t elementsCount)
{
    size_t result = 0;

    if (priorities != NULL && elementsCount > 0)
    {
        const size_t nrOfBytes = elementsCount * sizeof (size_t);

        printf("Creating named pipe\n");
        mkfifo(pipeName, 0666);
        printf("Done creating pipe\n");

        sleep(1);

        printf("\nOpening pipe for writing (open function returns once the receiver opens the pipe for reading)\n");
        int fDescriptor = open(pipeName, O_CREAT | O_WRONLY);

        if (fDescriptor < 0)
        {
            perror("Error! Named pipe cannot be opened for writing\n");
            exit(-1);
        }
        else
        {
            printf("Pipe opened for writing\n");
        }

        sleep(1);

        printf("\nWriting to pipe\n");
        write(fDescriptor, priorities, nrOfBytes);
        printf("Done writing to pipe\n");

        close(fDescriptor);
        unlink(pipeName);
        result = nrOfBytes;
    }

    return result;
}

