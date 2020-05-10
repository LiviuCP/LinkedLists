#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "../../../LinkedListsLib/linkedlist.h"
#include "../../ManualListEntry/apputils.h"

#define BUFFER_SIZE 256

static const char* dataFile = "/tmp/listdata";

void readDataFromFile(const char* dataFile, char* buffer, size_t* bytesNumber);

int main() {
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

    size_t readBytesNumber = 0;
    printf("Reading data from file...\n");
    sleep(1);
    readDataFromFile(dataFile, buffer, &readBytesNumber);
    printf("Done\n\n");

    List* list = NULL;

    if (readBytesNumber >= sizeof(size_t))
    {
        size_t* readAddress = (size_t*)buffer;

        const size_t payloadSize = *readAddress++;

        if (payloadSize > 0)
        {
            list = createListFromPrioritiesArray(readAddress, payloadSize);
            if (list != NULL)
            {
                sortAscendingByPriority(list);
                sleep(1);
                printf("The list has been created. After sorting it has following content:\n\n");
                printList(list);
                readAddress += payloadSize;
                sleep(1);
                printf("\nAdditional message from sender: \n");
                sleep(1);
                printf("%s\n\n", (char*)readAddress);
                sleep(1);
                deleteList(list, deleteObject);
                list = NULL;
            }
            else
            {
                fprintf(stderr, "Unable to create consumer list: no memory allocated\n");
                exit(-1);
            }
        }
        else
        {
            fprintf(stderr, "No payload data available, cannot create list\n");
            exit(-1);
        }
    }
    else
    {
        fprintf(stderr, "Insufficient data, no valid header available\n");
        exit(-1);
    }

    return 0;
}

void readDataFromFile(const char* dataFile, char* buffer, size_t* bytesNumber)
{
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();

    int fileDescriptor;
    if ((fileDescriptor = open(dataFile, O_RDONLY)) < 0)
    {
        perror("File opening failed");
        exit(-1);
    }

    fcntl(fileDescriptor, F_GETLK, &lock);
    if (lock.l_type != F_UNLCK)
    {
        perror("Write lock is still active");
        exit(-1);
    }

    lock.l_type = F_RDLCK;
    if (fcntl(fileDescriptor, F_SETLK, &lock) < 0)
    {
        perror("Failed to get read-only file lock");
        exit(-1);
    }

    char c;
    size_t writeIndex = 0;
    while (read(fileDescriptor, &c, 1) > 0)
    {
        buffer[writeIndex] = c;
        ++writeIndex;
    }
    *bytesNumber = writeIndex;

    lock.l_type = F_UNLCK;
    if (fcntl(fileDescriptor, F_SETLK, &lock) < 0)
    {
        perror("Failed to unlock the file");
        exit(-1);
    }

    close(fileDescriptor);
}
