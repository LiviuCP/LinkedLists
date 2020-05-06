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

#define PORT_NUMBER         9801
#define MAX_CONNECTS        4
#define BUFFER_SIZE         512
#define HOSTNAME            "localhost"

static const size_t requestedNrOfElementPriorities = 5; // hard-coded for the moment, might change in a future commit

void establishServerSocketConnection(const int* fileDescriptor);

int main()
{
    const int fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (fileDescriptor < 0)
    {
        perror("Error when creating socket file descriptor");
        exit(-1);
    }

    printf("Connecting to server...\n");
    establishServerSocketConnection(&fileDescriptor);
    printf("Connection established\n\nRequesting a number of %d list element priorities...\n", (int)requestedNrOfElementPriorities);

    if (write(fileDescriptor, &requestedNrOfElementPriorities, sizeof(size_t)))
    {
        char buffer[BUFFER_SIZE+1];
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
            for (size_t index = 0; index < requestedNrOfElementPriorities; ++index)
            {
                if (*(startAddress + index) > 0)
                {
                    createAndAppendToList(list, *(startAddress + index));
                    printf("Created and appended element with priority: %d\n", (int)(*(startAddress + index)));
                }
                else
                {
                    printf("Could not create and append element, priority is 0\n");
                }
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
    }

    close(fileDescriptor);

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
