#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>

#include "listprintutils.h"

#define BYTES_COUNT 512
#define PERMISSIONS 0644

static const char* dataFile = "/listdata";
static const char* semaphoreName = "listSemaphore";

int main()
{
    int fd = shm_open(dataFile, O_RDWR, PERMISSIONS);
    if (fd < 0)
    {
        perror("Error in getting file descriptor");
        exit(-1);
    }

    void* sharedMemory = mmap(NULL, BYTES_COUNT, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (sharedMemory == NULL)
    {
        perror("Segment access error");
        exit(-1);
    }

    sem_t* semaphore = sem_open(semaphoreName, O_CREAT, PERMISSIONS, 0);
    if (semaphore == NULL)
    {
        perror("Semaphore opening error");
        exit(-1);
    }

    List* list = NULL;

    if (!sem_wait(semaphore))
    {
        Priority* readAddress = (Priority*)sharedMemory;
        printf("Gained access to shared memory. Checking payload data size...\n");
        sleep(1);
        size_t payloadSize = *readAddress++;
        printf("Done\n\n");

        if (payloadSize > 0)
        {
            list = createListFromPrioritiesArray(readAddress, payloadSize, NULL);
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
                deleteList(list, deleteObjectPayload);
                list = NULL;
            }
            else
            {
                printf("Unable to create reader list: no memory allocated\n");
                exit(-1);
            }
        }
        else
        {
            printf("No payload data available, cannot create list\n");
            exit(-1);
        }

        sem_post(semaphore);
    }

    munmap(sharedMemory, BYTES_COUNT);
    close(fd);
    sem_close(semaphore);
    unlink(dataFile);

    return 0;
}
