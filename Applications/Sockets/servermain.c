#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#define PORT_NUMBER         9801
#define MAX_CONNECTS        4
#define BUFFER_SIZE         512
#define NR_OF_REQUESTS      2

static const size_t nrOfAvailablePriorities = 10;
static const size_t prioritiesList[10] = {5, 12, 4, 6, 1, 2, 3, 8, 7, 14};

void setSocket(const int* fileDescriptor);

int main()
{
    const int fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (fileDescriptor < 0)
    {
        perror("socket");
        exit(-1);
    }

    setSocket(&fileDescriptor);

    printf("Server is listening on port %d for clients...\n\n", (int)PORT_NUMBER);

    while (1) {
        struct sockaddr_in clientAddress;
        unsigned int len = sizeof(clientAddress);

        int clientFileDescriptor = accept(fileDescriptor, (struct sockaddr*) &clientAddress, &len);
        if (clientFileDescriptor < 0)
        {
            perror("Connection accept error");
            continue;
        }

        char buffer[BUFFER_SIZE + 1];
        size_t* bufferAddress = (size_t*)buffer;

        for (size_t requestNr = 0; requestNr < NR_OF_REQUESTS; ++requestNr)
        {
            memset(buffer, '\0', sizeof(buffer));

            printf("Client request received. Reading client request data...\n");
            ssize_t count = read(clientFileDescriptor, buffer, sizeof (buffer));

            if (count >= (ssize_t)sizeof(size_t))
            {
                if (*bufferAddress > 0)
                {
                    const size_t nrOfPrioritiesToSend = *bufferAddress <= nrOfAvailablePriorities ? *bufferAddress : nrOfAvailablePriorities;
                    printf("Client will get first %d priorities from list\n", (int)nrOfPrioritiesToSend);

                    for (size_t index = 0; index < nrOfPrioritiesToSend; ++index)
                    {
                        printf("Writing priority %d into buffer\n", (int)prioritiesList[index]);
                        *(bufferAddress + index) = prioritiesList[index];
                    }
                }
                else
                {
                    *bufferAddress = nrOfAvailablePriorities;
                    printf("Client only reequires to know how many priorities are available. The number is %d\n", (int)nrOfAvailablePriorities);
                }

                printf("Sending requested data to client...\n");
                write(clientFileDescriptor, buffer, sizeof(buffer));
                printf("Done\n\n");
            }
        }

        close(clientFileDescriptor);
    }

    return 0; // not used as return point, kept only for the sake of standards (providing a return value to int function())
}

void setSocket(const int* fileDescriptor)
{
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(PORT_NUMBER);

    if (bind(*fileDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
    {
        perror("Error when binding socket address");
        exit(-1);
    }

    if (listen(*fileDescriptor, MAX_CONNECTS) < 0)
    {
        perror("Connection listening error");
        exit(-1);
    }
}
