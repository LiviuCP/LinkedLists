#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>

#include "../../../LinkedListsLib/linkedlist.h"
#include "../../ManualListEntry/apputils.h"
#include "../../../Utils/codeutils.h"

#define PORT_NUMBER         9801
#define MAX_CONNECTS        4
#define BUFFER_SIZE         512
#define HOSTNAME            "localhost"

static const size_t availabilityRequestCode = 0; // used for querying server about the maximum number of retrievable priorities

void establishServerSocketConnection(const int* fileDescriptor);
size_t retrieveRequestedNrOfPriorities(void);

int main()
{
    char buffer[BUFFER_SIZE+1];
    const int fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (fileDescriptor < 0)
    {
        perror("Error when creating socket file descriptor");
        exit(-1);
    }

    const size_t nrOfUserRequestedPriorities = retrieveRequestedNrOfPriorities();

    if (nrOfUserRequestedPriorities != 0)
    {
        size_t availableCount = 0;
        size_t actuallyRequestedCount = 0;

        printf("\nConnecting to server...\n");
        establishServerSocketConnection(&fileDescriptor);
        printf("Connection established\n\n");

        printf("Checking data availability\n");

        sleep(1);
        if (write(fileDescriptor, &availabilityRequestCode, sizeof(size_t)))
        {
            memset(buffer, '\0', sizeof (buffer));

            size_t* startAddress = (size_t*)buffer;

            if (read(fileDescriptor, buffer, sizeof (buffer)))
            {
                availableCount = *startAddress;
            }
        }

        printf("Done\n\n");

        if (availableCount == 0)
        {
            printf("No data is available. List cannot be created\n");
        }
        else
        {
            if (nrOfUserRequestedPriorities > availableCount)
            {
                printf("You requested more data than available. Number of requested entries will be limited to maximum available.\n\n");
            }

            actuallyRequestedCount = nrOfUserRequestedPriorities <= availableCount ? nrOfUserRequestedPriorities : availableCount;

            printf("Requesting a number of %d list element priorities...\n", (int)actuallyRequestedCount);

            if (write(fileDescriptor, &actuallyRequestedCount, sizeof(size_t)))
            {
                memset(buffer, '\0', sizeof (buffer));

                size_t* startAddress = (size_t*)buffer;

                if (read(fileDescriptor, buffer, sizeof (buffer)))
                {
                    sleep(1);
                    printf("Response received from server\n\n");
                    sleep(1);
                    printf("Using received priorities to create list:\n\n");
                    sleep(1);
                    List* list = createEmptyList();
                    if (list != NULL)
                    {
                        for (size_t index = 0; index < actuallyRequestedCount; ++index)
                        {
                            createAndAppendToList(list, *(startAddress + index));
                            printf("Created and appended element with priority: %d\n", (int)(*(startAddress + index)));
                        }

                        printf("\nSorting list...\n");
                        sleep(1);
                        sortAscendingByPriority(list);
                        printf("Done\n");

                        printf("\nThe list has following elements after sorting:\n\n");
                        printList(list);

                        deleteList(list, deleteObject);
                        list = NULL;
                    }
                    else
                    {
                        fprintf(stderr, "Unable to create client list: no memory allocated\n");
                        exit(-1);
                    }
                }
                else
                {
                    fprintf(stderr, "The request could not be completed\n");
                }
            }
            else
            {
                fprintf(stderr, "The request could not be completed\n");
            }

            close(fileDescriptor);
        }
    }

    return 0;
}

void establishServerSocketConnection(const int* fileDescriptor)
{
    struct hostent* host = gethostbyname(HOSTNAME);

    if (!host)
    {
        perror("Error when getting host by name");
        exit(-1);
    }

    if (host->h_addrtype != AF_INET)
    {
        perror("Bad address family error");
        exit(-1);
    }

    struct sockaddr_in socketAddress;
    memset(&socketAddress, 0, sizeof(socketAddress));
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_addr.s_addr = ((struct in_addr*) host->h_addr_list[0])->s_addr;
    socketAddress.sin_port = htons(PORT_NUMBER);

    if (connect(*fileDescriptor, (struct sockaddr*) &socketAddress, sizeof(socketAddress)) < 0)
    {
        perror("Connection error");
        exit(-1);
    }
}

size_t retrieveRequestedNrOfPriorities()
{
    size_t nrOfRequestedPriorities;

    for (;;)
    {
        printf("Enter the requested number of list element priorities (0 to exit): ");
        if (!readUnsignedLong(&nrOfRequestedPriorities))
        {
            system("clear");
            printf("Invalid input! Please try again\n\n");
        }
        else
        {
            break;
        }
    }

    return nrOfRequestedPriorities;
}
