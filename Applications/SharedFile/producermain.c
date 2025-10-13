#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "listelement.h"
#include "codeutils.h"

#define BUFFER_SIZE 256

static const char* dataFile = "/tmp/listdata";
static const size_t nrOfPriorities = 5;
static const Priority priorities[] = {6, 2, 7, 3, 4};
static const char* endMessage = "Sorry mate, this is all I've got!";

void writeDataToFile(const char* dataFile, const char* buffer, size_t bufferSize);

int main()
{
    char buffer[BUFFER_SIZE];
    setNChars(buffer, '\0', BUFFER_SIZE);

    Priority* writeAddress = (Priority*)buffer;
    *writeAddress++ = nrOfPriorities;

    for (size_t index = 0; index < nrOfPriorities; ++index)
    {
        *writeAddress++ = priorities[index];
    }

    copyNCharsToString((char*)writeAddress, endMessage, strlen(endMessage));
    writeDataToFile(dataFile, buffer, (sizeof(nrOfPriorities) + sizeof(priorities) + strlen(endMessage) + 1));

    return 0;
}

void writeDataToFile(const char* dataFile, const char* buffer, size_t bufferSize)
{
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();

    int fileDescriptor;
    if ((fileDescriptor = open(dataFile, O_RDWR | O_CREAT, 0666)) < 0)
    {
        perror("File opening failed");
        exit(-1);
    }

    if (fcntl(fileDescriptor, F_SETLK, &lock) < 0)
    {
        perror("Failed to get file lock");
        exit(-1);
    }
    else
    {
        write(fileDescriptor, buffer, bufferSize);
        printf("Data successfully written to file...\n");
    }

    lock.l_type = F_UNLCK;
    if (fcntl(fileDescriptor, F_SETLK, &lock) < 0)
    {
        perror("Failed to unlock the file");
        exit(-1);
    }

    close(fileDescriptor);
}
