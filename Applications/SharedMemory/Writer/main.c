#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>

#define BYTES_COUNT 512
#define PERMISSIONS 0644
#define READ_ACCESS_TIME 15

static const char* dataFile = "/listdata";
static const char* semaphoreName = "listSemaphore";

static const size_t nrOfPriorities = 5;
static const size_t priorities[] = {6, 2, 7, 3, 4};
static const char* endMessage = "Sorry mate, this is all I've got!";

int main()
{
    int fd = shm_open(dataFile, O_RDWR | O_CREAT, PERMISSIONS);
    if (fd < 0)
    {
        perror("Shared memory segment opening error");
    }

    ftruncate(fd, BYTES_COUNT);

    void* sharedMemory = mmap(NULL, BYTES_COUNT, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (sharedMemory == NULL)
    {
        perror("Error in getting memory segment");
        exit(-1);
    }

    printf("Shared memory address: %p [0..%d]\n", sharedMemory, BYTES_COUNT - 1);
    printf("Backing file: /dev/shm%s\n", dataFile );

    sem_t* semaphore = sem_open(semaphoreName, O_CREAT, PERMISSIONS, 0);

    if (semaphore == NULL)
    {
        perror("Semaphore opening error");
        exit(-1);
    }

    size_t* writeAddress = (size_t*)sharedMemory;
    *writeAddress++ = nrOfPriorities;

    for (size_t index = 0; index < nrOfPriorities; ++index)
    {
        *writeAddress++ = priorities[index];
    }

    strcpy((char*)writeAddress, endMessage);

    if (sem_post(semaphore) < 0)
    {
        perror("Error in incrementing semaphore");
        exit(-1);
    }

    sleep(READ_ACCESS_TIME);

    munmap(sharedMemory, BYTES_COUNT);
    close(fd);
    sem_close(semaphore);
    shm_unlink(dataFile);

    return 0;
}
